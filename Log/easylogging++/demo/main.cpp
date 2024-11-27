#include "easylogging++.h"  

INITIALIZE_EASYLOGGINGPP

int main(int argc, char** argv)
{
	el::Configurations conf("my_log.conf");
	el::Loggers::reconfigureAllLoggers(conf);

	LOG(TRACE) << "***** trace log  *****";
	LOG(DEBUG) << "***** debug log  *****";
	LOG(ERROR) << "***** error log  *****";
	LOG(WARNING) << "***** warning log  *****";
	LOG(INFO) << "***** info log  *****";

	system("pause");
	return 0;
}
/*
#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

int main(int argv, char* argc[]) {
LOG(INFO) << "My first info log using default logger";
return 0;
}
*/