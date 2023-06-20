#include "stdafx.h"
#include "cwritelog.h"
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#include <string>
#include <iostream>
using namespace std;
cwritelog::cwritelog()
{
	InitializeCriticalSection(&cs_log);
}


cwritelog::~cwritelog()
{
}

void cwritelog::LogV(const char *pszFmt, va_list argp)
{
	struct tm *now;
	struct timeb tb;

	if (NULL == pszFmt || 0 == pszFmt[0]) return;
	vsnprintf(logstr, MAXLINSIZE, pszFmt, argp);
	ftime(&tb);
	now = localtime(&tb.time);
	sprintf(datestr, "%04d-%02d-%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
	sprintf(timestr, "%02d:%02d:%02d", now->tm_hour, now->tm_min, now->tm_sec);
	sprintf(mss, "%03d:", tb.millitm);
	printf("%s %s.%s %s", datestr, timestr, mss, logstr);
	FILE *flog = fopen(logfilename1, "a");
	if (NULL != flog) {
		fprintf(flog, "%s %s.%s %s\n", datestr, timestr, mss, logstr);
		if (ftell(flog) > MAXLOGSIZE) {
			fclose(flog);
			if (rename(logfilename1, logfilename2)) {
				remove(logfilename2);
				rename(logfilename1, logfilename2);
			}
		}
		else {
			fclose(flog);
		}
	}
}

void cwritelog::log(const char *pszFmt, ...)
{
	va_list argp;

	Lock(&cs_log);
	va_start(argp, pszFmt);
	LogV(pszFmt, argp);
	va_end(argp);
	Unlock(&cs_log);
}


void cwritelog::log(BYTE cards[], BYTE cardCount)
{
	string str;
	for (int i = 0; i < cardCount; i++)
	{
		str += to_string(cards[i])+",";
	}
	log(str.c_str());
}

void cwritelog::Lock(CRITICAL_SECTION *l)
{
	EnterCriticalSection(l);
}

void cwritelog::Unlock(CRITICAL_SECTION *l)
{
	LeaveCriticalSection(l);
}
