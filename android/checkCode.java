/*
* 判断设备 是否使用代理上网
* */
private boolean isWifiProxy(Context context) {

	final boolean IS_ICS_OR_LATER = Build.VERSION.SDK_INT >= Build.VERSION_CODES.ICE_CREAM_SANDWICH;

	String proxyAddress;

	int proxyPort;

	if (IS_ICS_OR_LATER) {

		proxyAddress = System.getProperty("http.proxyHost");

		String portStr = System.getProperty("http.proxyPort");

		proxyPort = Integer.parseInt((portStr != null ? portStr : "-1"));

	} else {

		proxyAddress = android.net.Proxy.getHost(context);

		proxyPort = android.net.Proxy.getPort(context);

	}

	return (!TextUtils.isEmpty(proxyAddress)) && (proxyPort != -1);
}

//栈检测xposed
public static boolean isInstallXposed(){
	try{
		throw new Exception("hook");
	}catch(Exception e){
		for(StackTraceElement className : e.getStackTrace()) {
			if(className.getClassName().contains("de.robv.android.xposed")){
				return true;
			}
			return false;
		}
	}
}


