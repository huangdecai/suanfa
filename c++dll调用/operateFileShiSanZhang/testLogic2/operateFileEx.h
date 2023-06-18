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
	int nIndex;			//�������
	string strMac;		//Mac��ַ���ַ�����ʽ��
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
	//���ָ��������ŵ�Mac��ַ
	void GetOneMac(int AdapterIndex);
	void OnGetaddr();
	vector<ADPTSTRCT> m_arrAdapters;
	bool StrFormat(char szBuf[],  char * szLogInfo, ...);
	bool tong();
public:
	CGameLogicNew   m_GameLogicNew;
};

