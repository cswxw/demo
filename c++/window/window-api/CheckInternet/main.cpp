#include <wininet.h >
#include <Sensapi.h>
#pragma comment(lib,"Wininet.lib")
#pragma comment(lib, "Sensapi.lib")
int checkInternet() {

	//IsNetworkAlive
	DWORD dw=0;
	BOOL isConnect = ::IsNetworkAlive(&dw); //总是返回连接上，vm网卡？
	if (isConnect)
		cout << "IsNetworkAlive连接" << endl;
	else {
		cout << "IsNetworkAlive未连接" << endl;
	}
			

	//InternetGetConnectedState
	DWORD dw2=0;
	BOOL ret = InternetGetConnectedState(&dw2, 0);
	if (ret)
		cout << "InternetGetConnectedState连接" << endl;
	else {
		cout << "InternetGetConnectedState未连接" << endl;

	}
			

	//InternetCheckConnection
	BOOL bConnected = InternetCheckConnection(_T("http://www.baidu.com"), FLAG_ICC_FORCE_CONNECTION, 0);
	if (bConnected)
		cout << "InternetCheckConnection连接" << endl;
	else {
		cout << "InternetCheckConnection未连接" << endl;

	}
			
	/*
	//该系统不支持这个功能
	BOOL bReachable= IsDestinationReachable(_T("http://www.baidu.com.com"), NULL);
	if (bReachable) {
		printf("IsDestinationReachable 连接\n");
	}
	else {
		printf("IsDestinationReachable 未连接 %d\n", GetLastError());
	}
	*/
	if(isConnect == TRUE 
		&& ret == TRUE
		&& bConnected == TRUE
		//&& bReachable == TRUE
		)
		return 1;
	return 0;
}