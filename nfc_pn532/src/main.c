#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "logger.h"
#include "nfc.h"

void display_help();

void display_help()
{
    printf("nfc_pn532\n");
}

int main(int argc, char** argv)
{
    int c;

    char connstring[512] = { 0 };

#ifdef _IN_VS2017_
	SetVerbosity(3);
	strcpy(connstring, "/dev/tty0:115200");
#else
	while ((c = getopt(argc, argv, "hl:c:")) != -1)
	{
		switch (c)
		{
		case 'h':
			display_help();
			break;
		case 'l':
			SetVerbosity(atoi(optarg));
			break;
		case 'c':
			strcpy(connstring, optarg);
			break;
		default:
			display_help();
			break;
		}
	}
#endif // _IN_VS2017_

	nfc_open(connstring);

    return 0;
}
