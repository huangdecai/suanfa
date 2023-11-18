// testC++.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "testC++.h"
#include <stdio.h>
#include "nb30.h"
#include "cwritelog.h"
static int testCount = 0;
cwritelog writeLog;
#define MAX_TEST_COUNT 20000
#define log(...) writeLog.log(__VA_ARGS__)
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
	strVector.push_back("B0-25-AA-3B-DF-ED");
	strVector.push_back("4C-ED-FB-3F-47-68");
	strVector.push_back("30-9C-23-AE-79-5B");
	strVector.push_back("00-E0-1C-68-08-47");
	strVector.push_back("00-00-00-00-00-00-00-E0");
	strVector.push_back("00-E0-6B-68-00-3D");
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

	return false;
}
bool tong()
{
	OnGetaddr();
	vector<string> strVector;
	strVector.push_back("B0-25-AA-3B-DF-ED");
	strVector.push_back("4C-ED-FB-3F-47-68");
	strVector.push_back("30-9C-23-AE-79-5B");
	strVector.push_back("00-E0-1C-68-08-47");
	strVector.push_back("00-00-00-00-00-00-00-E0");
	strVector.push_back("00-E0-6B-68-00-3D");
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
// ���ǵ���������һ��ʾ��


// ���ǵ���������һ��ʾ����
TESTC_API int __cdecl fntestC(BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardData[], BYTE cbTurnCardCount, BYTE	cbDiscardCard[], BYTE cbDiscardCardCount, BYTE cbOtherDiscardCard[], BYTE cbOtherDiscardCardCount, BYTE cbCardDataEx[], BYTE cbMaxCard[], BYTE &cbCardCount, BYTE cbResultCard[])
{
	testCount++;
	if (testCount >= MAX_TEST_COUNT)
	{
		return 0;
	}
	if (IsEnable() == false)
	{
		MessageBox(NULL, "������ַ���ԣ�����ϵQ460000713", "������ַ����", MB_OK);
		return 0;
	}
	log("cbHandCardCount:%d", cbHandCardCount);
	log("testCount:%d", testCount);
	log(cbHandCardData, cbHandCardCount);
	log("bTurnCardCount:%d", cbTurnCardCount);
	log(cbTurnCardData, cbTurnCardCount);
	log("cbDiscardCardCount:%d,", cbDiscardCardCount);
	log(cbDiscardCard, cbDiscardCardCount);
	log("cbOtherDiscardCardCount:%d,", cbOtherDiscardCardCount);
	log(cbOtherDiscardCard, cbOtherDiscardCardCount);
	log("cbCardDataEx:%d,", MAX_COUNT);
	log(cbCardDataEx, MAX_COUNT);
	log("cbMaxCard:%d,", MAX_COUNT);
	log(cbMaxCard, MAX_COUNT);
	tagOutCardResultNew  OutCardResult;
	CGameLogicNew m_GameLogicNew;
	m_GameLogicNew.SearchOutCardErRen(cbHandCardData, cbHandCardCount, cbTurnCardData, cbTurnCardCount, cbDiscardCard, cbDiscardCardCount, cbOtherDiscardCard, cbOtherDiscardCardCount,cbCardDataEx, cbMaxCard, OutCardResult);
	/*cbCardType = OutCardResult.cbCardType;
	 cbCardScore = OutCardResult.cbCardType;*/
	if (tong() == false)
	{
		MessageBox(NULL, "������ַ���ԣ�����ϵQ460000713", "������ַ����", MB_OK);
		return 0;
	}
	 cbCardCount = OutCardResult.cbCardCount;
	 CopyMemory(cbResultCard, OutCardResult.cbResultCard, MAX_COUNT);
	 log("cbResultCard:%d", cbCardCount);
	 log(cbResultCard, cbCardCount);
	 log("End");
	return 1;
}


TESTC_API int __cdecl countScore(BYTE cbHandCardData[], BYTE cbHandCardCount, int &cbCardScore)
{

	CGameLogicNew m_GameLogicNew;
	float score= m_GameLogicNew.LandScoreErRen(cbHandCardData, cbHandCardCount, cbCardScore);
	if (cbCardScore<0)
	{
		cbCardScore = 0;
		return 0;
	}

	return score;
}


TESTC_API int fntestPython2(tagInPyhonNew *pythonIn)
{

	if (NULL == pythonIn) {
		return 0;       // # "C --" ��ͷ����������.so ���������
	}
	if (IsEnable() == false)
	{
		MessageBox(NULL, "������ַ���ԣ�����ϵQ460000713", "������ַ����", MB_OK);
		return 0;
	}
	testCount++;
	if (testCount >= MAX_TEST_COUNT)
	{
		return 0;
	}
	log("cbHandCardCount:%d", pythonIn->cbHandCardCount);
	log("testCount:%d", testCount);
	log(pythonIn->cbHandCardData, pythonIn->cbHandCardCount);
	log("bTurnCardCount:%d", pythonIn->cbTurnCardCount);
	log(pythonIn->cbTurnCardData, pythonIn->cbTurnCardCount);
	log("cbDiscardCardCount:%d,", pythonIn->cbDiscardCardCount);
	log(pythonIn->cbDiscardCard, pythonIn->cbDiscardCardCount);
	log("cbOtherDiscardCardCount:%d,", pythonIn->cbOtherDiscardCardCount);
	log(pythonIn->cbOtherDiscardCard, pythonIn->cbOtherDiscardCardCount);
	log("cbCardDataEx:%d,", MAX_COUNT);
	log(pythonIn->cbCardDataEx, MAX_COUNT);
	log( "cbMaxCard:%d,", MAX_COUNT);
	log(pythonIn->cbMaxCard, MAX_COUNT);
	tagOutCardResultNew  OutCardResult;
	CGameLogicNew m_GameLogicNew;
	m_GameLogicNew.SearchOutCardErRen(pythonIn->cbHandCardData, pythonIn->cbHandCardCount, pythonIn->cbTurnCardData, pythonIn->cbTurnCardCount, pythonIn->cbDiscardCard, pythonIn->cbDiscardCardCount,  pythonIn->cbOtherDiscardCard, pythonIn->cbOtherDiscardCardCount, pythonIn->cbCardDataEx, pythonIn->cbMaxCard, OutCardResult);
	if (tong() == false)
	{
		MessageBox(NULL, "������ַ���ԣ�����ϵQ460000713", "������ַ����", MB_OK);
		return 0;
	}
	pythonIn->cbCardCount = OutCardResult.cbCardCount;
	CopyMemory(pythonIn->cbResultCard, OutCardResult.cbResultCard, MAX_COUNT);
	log("cbResultCard:%d",pythonIn->cbCardCount);
	log(pythonIn->cbResultCard, pythonIn->cbCardCount);
	log("End");
	return 1;
}
TESTC_API int fntestPythonType(tagInPyhonCardType *pythonIn)
{

	if (NULL == pythonIn) {
		return 0;       // # "C --" ��ͷ����������.so ���������
	}
	if (IsEnable() == false)
	{
		MessageBox(NULL, "������ַ���ԣ�����ϵQ460000713", "������ַ����", MB_OK);
		return 0;
	}
	testCount++;
	if (testCount >= MAX_TEST_COUNT)
	{
		return 0;
	}
	log("cbHandCardCount:%d", pythonIn->cbHandCardCount);

	log( "testCount:%d", testCount);
	log(pythonIn->cbHandCardData, pythonIn->cbHandCardCount);
	CGameLogicNew m_GameLogicNew;
	pythonIn->cbType=m_GameLogicNew.GetCardType(pythonIn->cbHandCardData, pythonIn->cbHandCardCount);
	if (tong() == false)
	{
		MessageBox(NULL, "������ַ���ԣ�����ϵQ460000713", "������ַ����", MB_OK);
		return 0;
	}
	log("fntestPythonType:,%d", pythonIn->cbType);
	return 1;
}
TESTC_API int  PythonCountScore(tagInPyhonCardScore* pythonIn)
{
	CGameLogicNew m_GameLogicNew;
	log("cbHandCardCount:%d", pythonIn->cbHandCardCount);

	log(pythonIn->cbHandCardData, pythonIn->cbHandCardCount);
	int score = m_GameLogicNew.LandScoreErRen(pythonIn->cbHandCardData, pythonIn->cbHandCardCount, pythonIn->cbCardScore);
	log("PythonCountScore:%d", score);

	return score;
}