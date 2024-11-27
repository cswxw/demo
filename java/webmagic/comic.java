package com.debug.dmzj;

import java.io.File;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.URLDecoder;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.script.Invocable;
import javax.script.ScriptEngine;
import javax.script.ScriptEngineManager;

import org.apache.commons.io.FileUtils;
import org.apache.commons.lang3.StringUtils;

import com.alibaba.fastjson.JSONArray;
import com.debug.config.WebSite;

import us.codecraft.webmagic.Page;
import us.codecraft.webmagic.Request;
import us.codecraft.webmagic.Site;
import us.codecraft.webmagic.Spider;
import us.codecraft.webmagic.SpiderListener;
import us.codecraft.webmagic.processor.PageProcessor;
import us.codecraft.webmagic.utils.UrlUtils;



public class DMZJ implements PageProcessor ,SpiderListener{
	private Site site = Site.me()// .setHttpProxy(new HttpHost("127.0.0.1",8888))
			.setRetryTimes(2).setSleepTime(1000).setUseGzip(true).setUserAgent(
					"Mozilla/5.0 (Linux; Android 7.0; PLUS Build/NRD90M) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/61.0.3163.98 Mobile Safari/537.36")
			.setCharset("UTF-8");
			
	public  static  final   String PAGETYPE = "pageType";
	
	private static Spider mainSpider;

	
	String[] grep = {};
	String g_title="";
	enum PAGE{
		HOME,
		EACH,
		IMG
	}
	static ArrayList<String> errList = new ArrayList<>();
	public void process(Page page) {
		// TODO Auto-generated method stub
		List<Request> target = page.getTargetRequests();
		Request curRequest = page.getRequest();
		PAGE p = (PAGE)curRequest.getExtra(PAGETYPE);
		if (p == null || p == PAGE.HOME) {
			//��ȡ�����б��ַ
			List<String> urls = page.getHtml().xpath("//div[@class='cartoon_online_border']/ul//a/@href").all();
			List<String> urlNames=page.getHtml().xpath("//div[@class='cartoon_online_border']/ul//a/@text()").all();
			if (urls.size() == urlNames.size()) {
		        for (String s : urls) {
		            if (StringUtils.isBlank(s) || s.equals("#") || s.startsWith("javascript:")) {
		                continue;
		            }
		            s = UrlUtils.canonicalizeUrl(s, page.getUrl().toString());
		            Request r = new Request(s);
		            //cookies
		            Map<String, String> cookies = curRequest.getCookies();
		            for (String key : cookies.keySet()) { 
		              r.addCookie(key, cookies.get(key));
		            } 
		            
		            r.putExtra(PAGETYPE, PAGE.EACH);
		            target.add(r);
		            
		            //break;
		        }
			}else {
				System.err.println("��ȡ�б����!");
			}		
			
			
		}else if(p == PAGE.EACH){
			//��ÿ���б�ҳ���ȡ����ͼƬ��ַ
			String next_page = page.getHtml().regex("next_chapter_pages.*?\"(.*?)\"").get().trim().replaceAll("\\\\", "");
			String title = page.getHtml().regex("g_comic_name.*?\"(.*?)\"").get().trim();
			String chapterName = page.getHtml().regex("g_chapter_name.*?\"(.*?)\"").get().trim();
			int g_max_pic_count = Integer.valueOf(page.getHtml().regex("g_max_pic_count.*?=(.*?);").get().trim());
			String saveFloder = WebSite.DMZJ.getSavePath() 
					+ "\\" + title 
					+ "\\" + chapterName;
			
			//��ȡ����ͼƬ���ƣ���������׺
			//String img1 = page.getHtml().regex("pages(.*?)'").get().trim();
			
			
			//js ��������
			String params =page.getHtml().regex("p\\}(.*?)\\)\\)").get().trim();
			params = params +")";
			List<String > imgnames =paseUrl(params);
			
			if(imgnames.size() != g_max_pic_count) {
				System.out.println("�쳣��ͼƬ��������");
				System.exit(-1);;
			}
			
	
			
			//System.out.println(page.getHtml().get());
			for(int i=0;i < imgnames.size();i++) {
				String s=imgnames.get(i);
				String imgName = (i+1)+ s.substring(s.lastIndexOf("."));
				//String imgName = s.substring(s.lastIndexOf("/")+1);
				String savePath = saveFloder
						+ "\\" + imgName;
				
				if(!new File(savePath).exists()) {
					s = "https://images.dmzj.com/"+s;

		            if (StringUtils.isBlank(s) || s.equals("#") || s.startsWith("javascript:")) {
		                continue;
		            }
		            s = UrlUtils.canonicalizeUrl(s, page.getUrl().toString());
		            Request r = new Request(s);
		            
		            //cookies
//		            Map<String, String> cookies = curRequest.getCookies();
//		            for (String key : cookies.keySet()) {
//		            	System.out.println("key:"+key+" value:"+cookies.get(key));
//		              r.addCookie(key, cookies.get(key));
//		            } 
//		            
//		            //cookies
//		            Map<String, String> headers = curRequest.getHeaders();
//		            for (String key : headers.keySet()) {
//		            	System.out.println("key:"+key+" value:"+headers.get(key));
//		              r.addCookie(key, headers.get(key));
//		            } 
		            
//		            String str = cookies.get("history_CookieR");
//		            JSONObject  json =JSONObject.parseObject(str);
//		            String comicId=(String) json.get("comicId");//����id
//		            String chapterId=(String) json.get("chapterId");//��id
//		            String pagenum=(String) json.get("page");//�ڼ�ҳ
//		            String time=(String) json.get("time");//�ۿ�ʱ��
		            //r.addCookie("host", "images.dmzj.com");
		            r.addHeader("Referer", curRequest.getUrl());
		            
		            r.putExtra("chapterName", chapterName); //����½�����
		            r.putExtra("title", title); //����½�����
		            r.putExtra("savePath", savePath);
		            r.putExtra(PAGETYPE, PAGE.IMG);
		            target.add(r);
				}else {
					System.out.println("[INFO][beofre download][path] " + savePath + "�Ѵ���!");
				}
				

			}
			if(g_max_pic_count == 0) {
				System.err.println("ͼƬ�б�ʧ��!");
				System.exit(-1);
			}


			
			
		}else if(p == PAGE.IMG) {

			String url = page.getRequest().getUrl();
			String charset = page.getCharset();
			try {
				url = URLDecoder.decode(url,charset);
			} catch (UnsupportedEncodingException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				System.exit(-2);
			}

			String savePath =(String) curRequest.getExtra("savePath");
			File f =new File(savePath);
			if(!f.exists()) {
				try {
					FileUtils.writeByteArrayToFile(f,page.getBytes());
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
					System.exit(-1);
				}
			}else {
				System.out.println("[INFO][after download][path] " + savePath + "�Ѵ���!");
			}
			
		}
		page.setSkip(true); //Ĭ�ϲ�����pipline
	}


	public void onSuccess(Request request) {
		// TODO Auto-generated method stub
		//ÿ�����سɹ��ص�
	
	}


	public void onError(Request request) {
		// TODO Auto-generated method stub
		errList.add(request.getUrl());
	}

	public Site getSite() {
		return site;
	}
	
	public static void main(String[] args) {
		List<String> urls= new ArrayList<String>();

		urls.add("xxx");

		mainSpider = Spider.create(new DMZJ())
				.thread(5);
		for(String url:urls) {
			if (url!=null) {
				mainSpider.addUrl(url);	
			}
		}
		mainSpider.run();
		

		while (true) {
			if (mainSpider.getStatus().equals(Spider.Status.Stopped)) {
				System.out.println("�������!");
				break;
			}
			try {
				Thread.sleep(3000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		if(errList.size()>0) {
			System.err.println("[Error] list:");	
			System.err.println(errList);			
		}else {
			System.out.println("[Info ] all success download");
		}

	}
	public static boolean isNumeric(String str)
	{
		if(str == null || str.length() == 0) return false;
		for (int i = 0; i < str.length(); i++)
		{
			//System.out.println(str.charAt(i));
			if (!Character.isDigit(str.charAt(i)))
			{
				return false;
			}
			}
		return true;
	}
	/**
	 * ����������ʽ�ж��ַ����Ƿ�������
	 * @param str
	 * @return
	 */
	public boolean isNumeric2(String str){
		Pattern pattern = Pattern.compile("[0-9]*");
		Matcher isNum = pattern.matcher(str);
		if( !isNum.matches() ){
		return false;
		}
		return true;
	}
	
	static String internal_parse(int c,int a_50) {
		StringBuffer string = new StringBuffer();
		if(c< a_50) {
			return "";
		}else {
			String r1=internal_parse(c/a_50,a_50);
			c = c % a_50;
			if(c>35) {
				string.append((char)c + 29);
			}else {
				string.append((char)36);
			}
			return r1+string.toString();
					
		}
	}
	
	
	public List<String> paseUrl(String p,
			int a,int c, //50
			String k, //page
			int   e, //0
			String d   //{}
			){
		List<String> res= new ArrayList<String>();
		Map<String, String> d1= new HashMap<>();
		String [] k1=k.split("\\|");
		
		while(c-- > 0) {
			if(k1.length > c) {
				d1.put(internal_parse(c, a), k1[c]);
			}
		}
		k=d1.get(e);
		//TODO
		
		
		
		return res;
	}
	
	
    public static List<String> paseUrl(String params){
        ScriptEngineManager manager = new ScriptEngineManager();
        ScriptEngine engine = manager.getEngineByName("javascript");
        try{
            engine.eval(
          "		function abc(p, a, c, k, e, d) {\r\n" + 
          //...
          "			return p\r\n" + 
          "		};\r\n" + 
          "		function abcd(){\r\n" + 
          "			return abc" +params+";\r\n" + 
          "		}\r\n" + 
          "		abcd();"
            		
            		);
            if (engine instanceof Invocable) {
                Invocable in = (Invocable) engine;
                String res= (String) in.invokeFunction("abcd");
                res=res.substring(res.indexOf("["),res.lastIndexOf("]")+1);
                JSONArray array= JSONArray.parseArray(res);
                List<String> reList = array.toJavaList(String.class);
                //System.out.println(res);
                return reList;
                
             }
            }catch(Exception e){
            	e.printStackTrace();
            }
        return null;
    }
}
