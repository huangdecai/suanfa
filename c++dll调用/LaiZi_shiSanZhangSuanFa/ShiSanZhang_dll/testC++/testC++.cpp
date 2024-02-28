// testC++.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "testC++.h"
#include <stdio.h>
#include "nb30.h"
#include <iostream>
#include <string>
#include "cwritelog.h"
cwritelog writeLog;
#define MAX_TEST_COUNT 20000
#define log(...) writeLog.log(__VA_ARGS__)
using namespace std;
static int testCount = 0;
// ���ǵ���������һ��ʾ��
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
vector<ADPTSTRCT> m_arrAdapters;
void GetOneMac(int AdapterIndex)
{
	NCB ncb;
	UCHAR uRetCode;
	ASTAT Adapter;
	memset(&ncb, 0, sizeof(ncb));
	ncb.ncb_command = NCBRESET;
	ncb.ncb_lana_num = AdapterIndex;   // ָ��������

	//���ȶ�ѡ������������һ��NCBRESET����Ա���г�ʼ�� 
	uRetCode = Netbios(&ncb);

	memset(&ncb, 0, sizeof(ncb));
	ncb.ncb_command = NCBASTAT;
	ncb.ncb_lana_num = AdapterIndex; 	// ָ��������

	strcpy((char *)ncb.ncb_callname, "*");
	// ָ�����ص���Ϣ��ŵı��� 
	ncb.ncb_buffer = (unsigned char *)&Adapter;
	ncb.ncb_length = sizeof(Adapter);

	// ����NCBASTAT�����Ի�ȡ��������Ϣ 
	uRetCode = Netbios(&ncb);
	if (uRetCode == 0)
	{
		// ������MAC��ַ��ʽ���ɳ��õ�16������ʽ����0010-A4E4-5802 
		string strMacAddr;

		char szLog[1024] = {};
		sprintf(szLog, "%02X-%02X-%02X-%02X-%02X-%02X",
			Adapter.adapt.adapter_address[0],
			Adapter.adapt.adapter_address[1],
			Adapter.adapt.adapter_address[2],
			Adapter.adapt.adapter_address[3],
			Adapter.adapt.adapter_address[4],
			Adapter.adapt.adapter_address[5]);
		strMacAddr = szLog;
		//��������ַ����Ŵ���������
		ADPTSTRCT AdptSt;
		AdptSt.nIndex = AdapterIndex;
		AdptSt.strMac = strMacAddr;
		m_arrAdapters.push_back(AdptSt);
	}

	string szStr = m_arrAdapters.at(0).strMac;
}

void OnGetaddr()
{
	NCB Ncb;
	UCHAR uRetCode;
	LANA_ENUM lenum;
	int i = 0;

	memset(&Ncb, 0, sizeof(Ncb));
	Ncb.ncb_command = NCBENUM;
	Ncb.ncb_buffer = (UCHAR *)&lenum;
	Ncb.ncb_length = sizeof(lenum);

	//����������NCBENUM����Ի�ȡ��ǰ������������Ϣ�����ж��ٸ�������ÿ�������ı�ŵ� 
	uRetCode = Netbios(&Ncb);
	//�������������Ϣ
	for (i = 0; i < lenum.length; i++)
	{
		GetOneMac(lenum.lana[i]);
	}
}
bool IsEnable()
{
	OnGetaddr();
	vector<string> strVector;
	strVector.push_back("04-42-1A-97-C9-35");
	strVector.push_back("B0-25-AA-3B-DF-ED");
	strVector.push_back("00-E0-4C-3E-48-66");
	strVector.push_back("0A-E0-AF-D2-25-58");
	strVector.push_back("20-11-11-68-0C-79");
	strVector.push_back("00-16-3E-03-3A-D5");
	strVector.push_back("D8-43-AE-04-A6-86");
	strVector.push_back("FA-16-3E-D6-2E-F4");
	strVector.push_back("7C-83-34-BA-22-DC");
	strVector.push_back("72-A6-CC-A4-47-96");
	strVector.push_back("70-A6-CC-A4-47-97");
	strVector.push_back("70-A6-CC-A4-47-9A");
	strVector.push_back("70-A6-CC-A4-47-96");
	strVector.push_back("62-FB-AA-22-5D-96");
	strVector.push_back("04-D9-C8-B5-AC-BB");
	strVector.push_back("DC-46-28-57-96-44");
	string strError;
	for (int i = 0; i < strVector.size(); i++)
	{
		string szStr;
		for (int j = 0; j < m_arrAdapters.size(); j++)
		{
			szStr = m_arrAdapters.at(j).strMac;
			if (szStr == strVector[i])
			{
				return true;
			}
		}
		
	}
	MessageBox(NULL, "������ַ���ԣ�����ϵQ460000713", strError.c_str(), MB_OK);
	//
	return false;
}

bool StrFormat(char szBuf[], char * szLogInfo, ...)
{

	va_list arglist;
	va_start(arglist, szLogInfo);
	int n = sizeof(szBuf);
	int m = sizeof(char);

	vsprintf_s(szBuf, CountArray(szBuf) - 1, szLogInfo, arglist);
	va_end(arglist);
	return true;
}
CGameLogicNew m_GameLogicNew;
// ���ǵ���������һ��ʾ����
TESTC_API int __cdecl fntestC(BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardData[], BYTE cbTurnCardCount, BYTE	DiscardCard[], BYTE cbDiscardCardCount, BYTE cbRangCardCount, BYTE cbOthreRangCardCount, BYTE &cbCardCount, BYTE cbResultCard[])
{
	testCount++;
	/*if (testCount >= 20000)
	{
	tagOutCardResultNew  OutCardResult;
	return NULL;
	}*/
	log("ncbHandCardCount:%d", cbHandCardCount);
	log("\ntestCount:%d", testCount);
	log(cbHandCardData, cbHandCardCount);
	log("nbTurnCardCount:%d", cbTurnCardCount);
	log(cbTurnCardData, cbTurnCardCount);
	log("cbDiscardCardCount:%d", cbDiscardCardCount);
	log(DiscardCard, cbDiscardCardCount);
	log("cbRangCardCount:%d", cbRangCardCount);
	log("cbOthreRangCardCount:%d", cbOthreRangCardCount);
	tagOutCardResultNew  OutCardResult;
	m_GameLogicNew.SearchOutCardErRen(cbHandCardData, cbHandCardCount, cbTurnCardData, cbTurnCardCount, DiscardCard, cbDiscardCardCount ,cbRangCardCount,cbOthreRangCardCount, OutCardResult);
	/*cbCardType = OutCardResult.cbCardType;
	 cbCardScore = OutCardResult.cbCardType;*/
	 cbCardCount = OutCardResult.cbCardCount;
	 CopyMemory(cbResultCard, OutCardResult.cbResultCard, MAX_COUNT);
	 log(cbResultCard, cbCardCount);
	 log("End");
	return 1;
}


TESTC_API int __cdecl countScore(BYTE cbHandCardData[], BYTE cbHandCardCount, int &cbCardScore)
{

	float score= m_GameLogicNew.LandScoreErRen(cbHandCardData, cbHandCardCount, cbCardScore);
	if (cbCardScore<0)
	{
		cbCardScore = 0;
		return 0;
	}

	return score;
}


TESTC_API int  fntestPython2(tagInPyhonNew *pythonIn)
{

	if (NULL == pythonIn) {
		return 0;       // # "C --" ��ͷ����������.so ���������
	}
	if (testCount == 0)
	{
		cout << testCount << endl;
		log("testCount:%d", testCount);
		if (IsEnable() == false)
		{
			MessageBox(NULL, "������ַ���ԣ�����ϵQ460000713", "������ַ����", MB_OK);
			return 0;
		}
	}
	testCount++;
	log("cbHandCardCount:%d", pythonIn->cbHandCardCount);
	log("testCount:%d", testCount);
	log(pythonIn->cbHandCardData, pythonIn->cbHandCardCount);
	log("bTurnCardCount:%d", pythonIn->cbTurnCardCount);
	log(pythonIn->cbTurnCardData, pythonIn->cbTurnCardCount);
	log("cbDiscardCardCount:%d,", pythonIn->cbDiscardCardCount);
	tagOutCardResultNew  OutCardResult;
	m_GameLogicNew.SearchOutCardErRen(pythonIn->cbHandCardData, pythonIn->cbHandCardCount, pythonIn->cbTurnCardData, pythonIn->cbTurnCardCount, pythonIn->DiscardCard, pythonIn->cbDiscardCardCount, pythonIn->cbRangCardCount, pythonIn->cbOthreRangCardCount, OutCardResult);

	pythonIn->cbCardCount = OutCardResult.cbCardCount;
	pythonIn->cbOthreRangCardCount = OutCardResult.cbCardType;
	CopyMemory(pythonIn->cbResultCard, OutCardResult.cbResultCard, MAX_COUNT);
	log("cbResultCard:%d", pythonIn->cbCardCount);
	log(pythonIn->cbResultCard, pythonIn->cbCardCount);
	log("End");
	return 1;
}