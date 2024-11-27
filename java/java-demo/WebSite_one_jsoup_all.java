

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.net.MalformedURLException;
import java.net.SocketTimeoutException;
import java.net.URL;
import java.security.cert.CertificateException;
import java.security.cert.X509Certificate;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Properties;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.net.ssl.HostnameVerifier;
import javax.net.ssl.HttpsURLConnection;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSession;
import javax.net.ssl.TrustManager;
import javax.net.ssl.X509TrustManager;

import org.apache.commons.io.FileUtils;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Node;
import org.jsoup.nodes.TextNode;
import org.jsoup.select.Elements;

import my.Internet.entity.Book;
import my.Internet.entity.Chapter;
import my.Internet.utils.MessageUtils;

class website_param {
	String encode; // 编码
	String cssBookTitle; // 标题 css
	String cssAllChapterHtml; // 章节列表css
	String cssEachChapterContent; // 每个章节内容css
	int startpos;
	String url;
	boolean isGenerateChapterName;

	public website_param(String encode, String cssBookTitle, String cssAllChapterHtml, String cssEachChapterContent,
			int startpos, boolean isGenerateChapterName) {
		this.encode = encode;
		this.cssBookTitle = cssBookTitle;
		this.cssAllChapterHtml = cssAllChapterHtml;
		this.cssEachChapterContent = cssEachChapterContent;
		this.startpos = startpos;
		this.isGenerateChapterName = isGenerateChapterName;
	}

	static HashMap<String, website_param> web_map = new HashMap<String, website_param>();
	static {
		web_map.put("www.xxx.cc",
				new website_param("utf-8", "h1", "div[id='list'] dl dd", "div[id='content']", 12, true));


	}

	public static website_param getWebParam(String url) {
		try {

			URL weburl = new URL(url);
			String host = weburl.getHost();
			website_param wb = web_map.get(host);
			if (wb != null) {
				wb.url = url;
			}
			return wb;
		} catch (MalformedURLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return null;
		}

	}
}

public class WebSite_one_jsoup_all {

	private static String listUrl = "";
	public static String ENCODE = "utf-8";

	// 是否手动写入章节标题
	public static boolean isGenerateChapterName = false;

	public static boolean isdebug = false;

	// 获取book 标题
	private static String cssBookTitle = "";

	// 获取章节列表所在节点
	private static String cssAllChapterHtml = ""; // 节点a 的所在地方 text() href

	// 章节内容
	private static String cssEachChapterContent = "";

	private static HashMap<String, String> paramMap = new HashMap<String, String>();
	static int startpos = 0;

	// 初始化代码
	static {
		paramMap.put("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
		// paramMap.put("Accept-Encoding", "gzip, deflate");
		paramMap.put("Accept-Language", "zh-CN,zh;q=0.8,en-US;q=0.5,en;q=0.3");
		// paramMap.put("Cache-Control", "max-age=0");
		paramMap.put("Connection", "keep-alive");
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
					.headers(paramMap).get();
			Elements e_title = doc.select(cssBookTitle);
			book.setBookname(e_title.text());
			Elements e_list = doc.select(cssAllChapterHtml);
			int start = 0;
			for (Element e : e_list) {
				if (start < startpos) {
					start++;
					continue;
				}
				Chapter c = new Chapter();

				Element a = e.getElementsByTag("a").first();
				String name = a.text().trim();
				if (isGenerateChapterName) {
					String[] names = name.split(" ");
					if (names.length > 1) {
						name = "";
						for (int i = 1; i < names.length; i++) {
							name += names[i];
						}
					} else {
						name = names[0];
					}

					start += 1;
					name = "第" + (start - startpos) + "章 " + name.trim();
				}

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

	static String filterContent(String s) {
		String[] str = { "xxx",};
		for (String t : str) {
			s = s.replaceAll(t, "");
		}
		return s;
	}

	public static String getChpaterContent(String url) throws Exception {
		StringBuffer result = new StringBuffer();
		Document doc = Jsoup.connect(url).headers(paramMap).get();// default 30 seconds
		doc.outputSettings().prettyPrint(true);
		Elements e_content = doc.select(cssEachChapterContent);

		for (Element e : e_content) {
			for (Node n : e.childNodes()) {
				if (n.nodeName() == "p" ) {
					Element t = (Element) n;
					result.append(filterContent(t.text()) + "\r\n\r\n");
				}
				if (n.nodeName() == "#text") {
					TextNode t = (TextNode) n;
					result.append(filterContent(t.text()) + "\r\n\r\n");
				}
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
		File fileName = new File("E:\\xxx\\xxx\\" + book.getBookname() + ".txt");
		File fileproperties = new File("E:\\xxx\\xxx\\" + book.getBookname() + ".properties");
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

				String content = "";
				try {

					content = getChpaterContent(c.getUrl());
				} catch (SocketTimeoutException e) {
					// TODO: handle exception
					MessageUtils.info("超时,延时2s", true);
					delay(2000);
					i--;
					continue;
				}

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

	public static void setUp(String url) {
		website_param wb = website_param.getWebParam(url);
		if (wb == null) {
			System.err.println("web param not found!");
			return;
		}
		cssBookTitle = wb.cssBookTitle;
		cssAllChapterHtml = wb.cssAllChapterHtml;
		cssEachChapterContent = wb.cssEachChapterContent;
		startpos = wb.startpos;
		isGenerateChapterName = wb.isGenerateChapterName;

		startUp(wb.url);
	}

	public static void main(String[] args) throws Exception {
		ignore_https.ignoreSsl();

		setUp("url");
	}
}

class ignore_https {
	private static void trustAllHttpsCertificates() throws Exception {
		TrustManager[] trustAllCerts = new TrustManager[1];
		TrustManager tm = new miTM();
		trustAllCerts[0] = tm;
		SSLContext sc = SSLContext.getInstance("SSL");
		sc.init(null, trustAllCerts, null);
		HttpsURLConnection.setDefaultSSLSocketFactory(sc.getSocketFactory());
	}

	private static class miTM implements TrustManager, X509TrustManager {
		public X509Certificate[] getAcceptedIssuers() {
			return null;
		}

		public boolean isServerTrusted(X509Certificate[] certs) {
			return true;
		}

		public boolean isClientTrusted(X509Certificate[] certs) {
			return true;
		}

		public void checkServerTrusted(X509Certificate[] certs, String authType) throws CertificateException {
			return;
		}

		public void checkClientTrusted(X509Certificate[] certs, String authType) throws CertificateException {
			return;
		}
	}

	/**
	 * 忽略HTTPS请求的SSL证书，必须在openConnection之前调用
	 * 
	 * @throws Exception
	 */
	public static void ignoreSsl() throws Exception {
		HostnameVerifier hv = new HostnameVerifier() {
			public boolean verify(String urlHostName, SSLSession session) {
				System.out.println("Warning: URL Host: " + urlHostName + " vs. " + session.getPeerHost());
				return true;
			}
		};
		trustAllHttpsCertificates();
		HttpsURLConnection.setDefaultHostnameVerifier(hv);
	}

	/**
	 * 忽略 https 证书验证错误问题
	 */
	public static void trustAllHosts() {
		final String TAG = "trustAllHosts";
		// Create a trust manager that does not validate certificate chains
		TrustManager[] trustAllCerts = new TrustManager[] { new X509TrustManager() {
			public java.security.cert.X509Certificate[] getAcceptedIssuers() {
				return new java.security.cert.X509Certificate[] {};
			}

			public void checkClientTrusted(X509Certificate[] chain, String authType) throws CertificateException {

			}

			public void checkServerTrusted(X509Certificate[] chain, String authType) throws CertificateException {

			}
		} };
		// Install the all-trusting trust manager
		try {
			SSLContext sc = SSLContext.getInstance("TLS");
			sc.init(null, trustAllCerts, new java.security.SecureRandom());
			HttpsURLConnection.setDefaultSSLSocketFactory(sc.getSocketFactory());
		} catch (Exception e) {

		}
	}
}
