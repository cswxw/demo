/********************************************************************
 * author: waponx
 * email : 18826419224@163.com
 * time  : 10.11.2015
 * ******************************************************************/

#ifndef MAIL_H
#define MAIL_H

#include <memory>
#include <netdb.h>
#include <string>

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName &);\
	void operator = (const TypeName &)
#endif	// DISALLOW_COPY_AND_ASSIGN

namespace Mail {
class SMTPMail{
public :
	typedef std::string String;
	typedef struct addrinfo AddrType;
	//  @param: 
	//  uesrname: 发送邮件方的用户名
	//  password: 发送邮件方的密码
	//  host: 邮件服务器的主机名
	explicit SMTPMail(const String &username,
			const String &password, 
			const String &host = String(""));
	~SMTPMail();
	//	发送一封邮件
	//	@param: 
	//	to: 接收方的邮件地址或用户名
	//	text: 邮件的正文
	void SendEmail(const String &to, const String &text);
	String GetHost();
	void SetHost(const String &host);
private:
	DISALLOW_COPY_AND_ASSIGN(SMTPMail);
	AddrType *InitAddrInfoFilter();
	bool Socket(AddrType *res);
	bool Login();
	void Send(const String &msg);
	String Recv();
private :
	bool _islogin;
	socklen_t _reuseaddr;	//make the address can be reuse
	int _fd;	//	文件描述符
	AddrType *_paddrfilter;		//返回的地址信息的过滤器
	String _username;
	String _password;
	String _host;
	
};
}	// namespace Mail

#endif	// MAIL_H
