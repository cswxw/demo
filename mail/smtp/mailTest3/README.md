# SMTPMail
这是一个简单的基于SMTP协议的邮件发送类
目前无法还添加附件，只能发送文本。

在打开程序之前，你应该先在程序的目录下新建一个名为mail.log的文件
该文件为当前程序的日志文件

在main.cc文件中有使用的示例：

1）在SMTP的构造函数中：
	username：	替换为你计算机的用户名
	password：	替换为你计算机用户对应的密码
	serverdomain:	替换为你使用的smtp服务器的域名,例如：smtp.qq.com


2）在SendEmail函数只中：
	targetaddr:	邮件接收方的地址
	text：		邮件发送的正文
