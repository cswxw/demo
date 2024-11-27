/********************************************************************
 * author: waponx
 * email : 18826419224@163.com
 * time  : 10.11.2015
 * ******************************************************************/

#ifndef _MAIL_LOG_H
#define _MAIL_LOG_H


#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <time.h>


namespace Mail {
	bool OpenLogFile(int &log) {
		if ( (log = open("./mail.log", O_WRONLY | O_APPEND)) == -1) {
			printf("open log file error!\n");
			printf("please, make sure have a file called mail.log in the path\n");

			return false;
		}
		return true;
	}

	std::string GetTime() {
		time_t tt = time(NULL);

		return std::string(ctime(&tt));
	}
	void log(const char *str, size_t) {
		static int log = -1;
		if (log < 0) {
			if (!OpenLogFile(log)) {
				return ;
			}
		}
		std::string res(GetTime());
		res =  res + str + "\n";
		write (log, res.c_str(), res.length());
	}

}	// namespace Mail

#endif	// _MAIL_LOG_H 
