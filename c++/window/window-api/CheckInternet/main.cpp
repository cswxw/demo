#include <wininet.h >
#include <Sensapi.h>
#pragma comment(lib,"Wininet.lib")
#pragma comment(lib, "Sensapi.lib")
int checkInternet() {

	//IsNetworkAlive
	DWORD dw=0;
	BOOL isConnect = ::IsNetworkAlive(&dw); //���Ƿ��������ϣ�vm������
	if (isConnect)
		cout << "IsNetworkAlive����" << endl;
	else {
		cout << "IsNetworkAliveδ����" << endl;
	}
			

	//InternetGetConnectedState
	DWORD dw2=0;
	BOOL ret = InternetGetConnectedState(&dw2, 0);
	if (ret)
		cout << "InternetGetConnectedState����" << endl;
	else {
		cout << "InternetGetConnectedStateδ����" << endl;

	}
			

	//InternetCheckConnection
	BOOL bConnected = InternetCheckConnection(_T("http://www.baidu.com"), FLAG_ICC_FORCE_CONNECTION, 0);
	if (bConnected)
		cout << "InternetCheckConnection����" << endl;
	else {
		cout << "InternetCheckConnectionδ����" << endl;

	}
			
	/*
	//��ϵͳ��֧���������
	BOOL bReachable= IsDestinationReachable(_T("http://www.baidu.com.com"), NULL);
	if (bReachable) {
		printf("IsDestinationReachable ����\n");
	}
	else {
		printf("IsDestinationReachable δ���� %d\n", GetLastError());
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