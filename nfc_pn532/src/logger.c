#include "logger.h"
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <memory.h>


static int g_verbosity = 0;
static const int MAX_LOG_LENGTH	= 4 * 1024;


void SetVerbosity(int verbosity)
{
    g_verbosity = verbosity;
}

void AddLogLine(int verbosity, const char* fmt, ...)
{
	if (g_verbosity >= verbosity)
	{
    	char buf[MAX_LOG_LENGTH];
		va_list args;
    	va_start(args, fmt);
    	vsnprintf(buf, MAX_LOG_LENGTH - 10, fmt, args);
    	va_end(args);

        struct tm* ptr;
        time_t ti;
        ti = time(NULL);
        ptr = localtime(&ti);

		char timebuf[1024] = {0};
		sprintf(timebuf, "[ %4d/%02d/%02d %02d:%02d:%02d ]",
                ptr->tm_year + 1900,
                ptr->tm_mon + 1,
                ptr->tm_mday,
                ptr->tm_hour,
                ptr->tm_min,
                ptr->tm_sec); 
		char namebuf[512] = {0};
		sprintf(namebuf, "./%4d%02d%02d.log", 
                ptr->tm_year + 1900, 
                ptr->tm_mon + 1,
                ptr->tm_mday);

        char print_buf[MAX_LOG_LENGTH];
		memset(print_buf, 0, MAX_LOG_LENGTH);
		
		strcat(print_buf, timebuf);
		
		switch(verbosity)
		{
		case System:
			strcat(print_buf, "[ SYS ] ");
			break;
		case Error:
			strcat(print_buf, "[ ERR ] ");
			break;
        case Warning:
			strcat(print_buf, "[ WRN ] ");
			break;
		case Debug:
			strcat(print_buf, "[ DBG ] ");
			break;
		case Info:
			strcat(print_buf, "[ INF ] ");
			break;
		}
        
		strcat(print_buf, buf);
		strcat(print_buf, "\n");

		FILE *fs = fopen(namebuf, "a");
		fwrite(print_buf, sizeof(char), strlen(print_buf) + 1, fs);

        printf("%s\n", print_buf);
	}
}
