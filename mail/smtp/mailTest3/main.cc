/********************************************************************
 * author: waponx
 * email : 18826419224@163.com
 * time  : 10.11.2015
 * ******************************************************************/

#include "smtpmail.h"

#include <string>

int main() {
	using Mail::SMTPMail;
	SMTPMail *sm = new SMTPMail(SMTPMail::String("username"),
			SMTPMail::String("password"),
			SMTPMail::String("smtp.qq.com"));

	sm->SendEmail("targetaddr", "text");
	delete sm;

	return 0;
}
