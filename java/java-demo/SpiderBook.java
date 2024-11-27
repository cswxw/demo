package my.Internet.webSite;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.net.SocketTimeoutException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Properties;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.commons.io.FileUtils;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.TextNode;
import org.jsoup.select.Elements;

import my.Internet.entity.Book;
import my.Internet.entity.Chapter;
import my.Internet.utils.MessageUtils;

public class WebSite_one_jsoup {
	private static String listUrl = "";
	public static String ENCODE = "utf-8";

	// 是否手动写入章节标题
	public static boolean isWriteChapterName = false;

	public static boolean isdebug = false;

	// 获取book 标题
	private static String cssBookTitle = "h1";

	// 获取章节列表所在节点
	private static String cssAllChapterHtml = "div[id='list'] dl dd"; //节点a 的所在地方 text() href

	// 章节内容
	private static String cssEachChapterContent = "div[id='content']";
 
	private static HashMap<String, String> paramMap = new HashMap<String, String>();
	static int startpos=0;
	// 初始化代码
	static {
		paramMap.put("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
		// paramMap.put("Accept-Encoding", "gzip, deflate");
		paramMap.put("Accept-Language", "zh-CN,zh;q=0.8,en-US;q=0.5,en;q=0.3");
		// paramMap.put("Cache-Control", "max-age=0");
		paramMap.put("Connection", "keep-alive");
		// paramMap.put("Host", "www.biquge.com.tw");
		// paramMap.put("Cookie", "__cfduid=da5ad855a98af031f8e951cc97c9979dd1497787578;
		// a8919_pages=49; a8919_times=1");
		paramMap.put("User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:50.0) Gecko/20100101 Firefox/50.0");
		// paramMap.put("If-Modified-Since", "Sat, 08 Apr 2017 23:14:15 GMT");
		// paramMap.put("Upgrade-Insecure-Requests", "1");
	}

	public static Book getBook() {
		Book book = new Book();
		ArrayList<Chapter> chapters = new ArrayList<Chapter>();
		try {
			Document doc = Jsoup.connect(listUrl)
					// .userAgent("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.31 (KHTML,
					// like Gecko) Chrome/26.0.1410.64 Safari/537.31")
					.headers(paramMap).get();
			Elements e_title = doc.select(cssBookTitle);
			book.setBookname(e_title.text());
			Elements e_list = doc.select(cssAllChapterHtml);
			int start = 0;
			for (Element e : e_list) {
				if(start < startpos) {
					start++;
					continue;      
				}
				Chapter c = new Chapter();

				Element a = e.getElementsByTag("a").first();
				String name=a.text().trim();
				
				String[] names = name.split(" ");
				if(names.length>1) {
					name = "";
					for(int i=1;i<names.length;i++) {
						name += names[i];
					}
				}else {
					name = names[0];
				}
			
				
				start +=1;
				name = "第" +(start) + "章 " + name.trim();
				c.setName(name);
				c.setUrl(a.absUrl("href"));
				chapters.add(c);
			}
			book.setChapters(chapters);

		} catch (SocketTimeoutException e) {
			// TODO: handle exception
			MessageUtils.error("获取book超时");
			return null;
		} catch (Exception e) {
			MessageUtils.error("获取book异常", e);
			e.printStackTrace();
			return null;
		}

		return book;
	}

	public static String getChpaterContent(String url) throws Exception {
		StringBuffer result = new StringBuffer();
		Document doc = Jsoup.connect(url)
				.headers(paramMap).get();//default 30 seconds
		doc.outputSettings().prettyPrint(true); 
		Elements e_content = doc.select(cssEachChapterContent);
		
		for(Element e:e_content) {
			for(TextNode t:e.textNodes()) {
				result.append(t.text()+"\r\n\r\n");
			}
		}
		return result.toString();
	}

	public static void delay(int millis) {
		try {
			Thread.sleep(millis);
		} catch (Exception e) {
			// TODO: handle exception
			e.printStackTrace();
		}
	}

	public static void debugInfo(Object o) {
		if (isdebug) {
			System.out.println(o);
		}
	}

	public static String getTitle(String title) {
		title = title.replace("&#13;", "").trim();
		Matcher m = Pattern.compile("章节名称[\\S\\s](.*?)字数").matcher(title);
		if (m.find()) {
			return m.group(1);
		}

		return title;
	}

	public static void startUp(String url) {

		listUrl = url;
		if (listUrl.equals("")) {
			System.out.println("请先设置网址");
			return;
		}

		Book book = getBook();
		if (book == null) {
			System.out.println("未获取到book!");
			return;
		}
		File fileName = new File("X:\\xxxtemp\\xxx001\\" + book.getBookname() + ".txt");
		File fileproperties = new File("X:\\xxxtemp\\conf\\" + book.getBookname() + ".properties");
		Properties pro = new Properties();
		try {
			FileUtils.write(fileproperties, "", ENCODE, true);
			pro.load(new FileInputStream(fileproperties));
		} catch (Exception e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		int hasDownload = 0;
		MessageUtils.compareUrl(pro, url);
		pro.setProperty("indexUrl", listUrl);
		String hs = pro.getProperty("hasDownload");
		if (hs != null && !hs.equals("")) {
			hasDownload = Integer.parseInt(hs);
		}
		Chapter c = new Chapter();
		List<Chapter> chapterList = book.getChapters();
		try {
			for (int i = 0; i < chapterList.size(); i++) {
				if (i < hasDownload) {
					continue;
				}
				c = chapterList.get(i);

				System.out.print("正在下载..." + c.getName());
				String content = getChpaterContent(c.getUrl());

				if (!content.equals("")) {
					
					FileUtils.write(fileName, "\r\n" + c.getName() + "\r\n", ENCODE, true);
				
					FileUtils.write(fileName, content, ENCODE, true);
					System.out.println("\t已下载 " + c.getName());
					hasDownload++;
					pro.setProperty("hasDownload", hasDownload + "");
				} else {
					System.out.println("未获取到章节内容!");
					break;
				}
				if (isdebug) {
					break;
				}
			}
		} catch (Exception e) {
			// TODO Auto-generated catch block
			System.out.println("异常:" + c.getName() + " 地址:" + c.getUrl());
			e.printStackTrace();
		} finally {
			System.out.println(book.getBookname() + " 下载完成!");
			System.out.println("");
			try {
				pro.store(new FileOutputStream(fileproperties), "save to file properties");
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}

	}

	public static void main(String[] args) {
		startUp("");
}
