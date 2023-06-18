#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "GameLogicNew.h"
#include "nb30.h"
using namespace std;
typedef struct _ASTAT_
{
	ADAPTER_STATUS adapt;
	NAME_BUFFER    NameBuff[30];
}ASTAT, *PASTAT;
typedef struct AdapterStrct
{
	int nIndex;			//网卡编号
	string strMac;		//Mac地址（字符串形式）
}ADPTSTRCT;
class operateFileEx
{
public:
	void operateFile(string fileName);

	void WriteFile(string &fileName, vector<string> &allStr);

	string operateBinaryFileTxt(string fileName);
	bool IsEnable();
	operateFileEx();
	virtual ~operateFileEx();
	//获得指定网卡序号的Mac地址
	void GetOneMac(int AdapterIndex);
	void OnGetaddr();
	vector<ADPTSTRCT> m_arrAdapters;
	bool StrFormat(char szBuf[],  char * szLogInfo, ...);
	bool tong();
public:
	CGameLogicNew   m_GameLogicNew;
};

