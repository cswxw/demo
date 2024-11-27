#include <Windows.h>
int main(int argc, char *argv[])
{
	HANDLE hOut;


	hOut = GetStdHandle(STD_OUTPUT_HANDLE);


	SetConsoleTextAttribute(hOut,
		FOREGROUND_RED |
		FOREGROUND_GREEN);
	printf("This text is yellow.\n");


	SetConsoleTextAttribute(hOut,
		FOREGROUND_GREEN |
		FOREGROUND_BLUE);
	printf("This text is cyan.\n");


	SetConsoleTextAttribute(hOut,
		FOREGROUND_BLUE |
		FOREGROUND_RED);
	printf("This text is magenta.\n");


	SetConsoleTextAttribute(hOut,
		FOREGROUND_RED |
		FOREGROUND_GREEN |
		FOREGROUND_BLUE
		//| FOREGROUND_INTENSITY
		);
	printf("This text is white.\n");


	SetConsoleTextAttribute(hOut,
		BACKGROUND_GREEN |
		BACKGROUND_BLUE 
		//| BACKGROUND_INTENSITY
		);
	printf("Cyan    \n");
	return 0;
	
}


