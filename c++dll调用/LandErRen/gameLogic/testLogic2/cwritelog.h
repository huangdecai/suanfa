#pragma once
#define MAXLOGSIZE 20000000
#define MAXLINSIZE 16000
#include <process.h>
class cwritelog
{
public:
	cwritelog();
	~cwritelog();
	void log(){

	}
	void Lock(CRITICAL_SECTION *l);
	void Unlock(CRITICAL_SECTION *l);
	void LogV(const char *pszFmt, va_list argp);
	void log(const char *pszFmt, ...);
	void log(BYTE cards[], BYTE cardCount);
	char *logfilename1 = "clock_tdata.txt";
	char *logfilename2 = "clock_tdata2.txt";
	char logstr[MAXLINSIZE + 1];
	char datestr[16];
	char timestr[16];
	char mss[4];
	CRITICAL_SECTION cs_log;
	
};

