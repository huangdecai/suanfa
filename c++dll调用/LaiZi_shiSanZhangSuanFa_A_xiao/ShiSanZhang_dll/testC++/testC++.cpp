// testC++.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "testC++.h"
#include <stdio.h>
#include "nb30.h"
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
	strVector.push_back("D8-43-AE-04-A6-86");
	strVector.push_back("B0-25-AA-3B-DF-ED");
	strVector.push_back("00-E0-4C-3E-48-66");
	strVector.push_back("0A-E0-AF-D2-25-58");
	strVector.push_back("20-11-11-68-0C-79");
	strVector.push_back("C8-7F-54-05-C3-6E");
	strVector.push_back("EC-D6-8A-DE-D6-F9");
	strVector.push_back("EC-D6-8A-DE-D2-81");
	strVector.push_back("EC-D6-8A-DE-D9-B4");
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
	strVector.push_back("D8-43-AE-04-A6-86");
	strVector.push_back("B0-25-AA-3B-DF-ED");
	strVector.push_back("00-E0-4C-3E-48-66");
	strVector.push_back("0A-E0-AF-D2-25-58");
	strVector.push_back("20-11-11-68-0C-79");
	strVector.push_back("C8-7F-54-05-C3-6E");
	strVector.push_back("EC-D6-8A-DE-D6-F9");
	strVector.push_back("EC-D6-8A-DE-D2-81");
	strVector.push_back("EC-D6-8A-DE-D9-B4");
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
	FILE *fpWrite = fopen("clock_tdata.txt", "a+");
	fprintf(fpWrite, "\ncbHandCardCount:%d\n", cbHandCardCount);

	fprintf(fpWrite, "\ntestCount:%d\n", testCount);
	fflush(fpWrite);
	for (int i = 0; i < cbHandCardCount; i++)
	{
		fprintf(fpWrite, ",%d", cbHandCardData[i]);
	}
	fprintf(fpWrite, "\nbTurnCardCount:%d\n", cbTurnCardCount);
	fflush(fpWrite);
	for (int i = 0; i < cbTurnCardCount; i++)
	{
		fprintf(fpWrite, ",%d", cbTurnCardData[i]);
	}
	fflush(fpWrite);
	fprintf(fpWrite, "\n cbDiscardCardCount:%d,\n", cbDiscardCardCount);
	for (int i = 0; i < cbDiscardCardCount; i++)
	{
		fprintf(fpWrite, ",%d", DiscardCard[i]);
	}
	fflush(fpWrite);
	fprintf(fpWrite, "\n cbRangCardCount:%d,\n", cbRangCardCount);
	fprintf(fpWrite, "\n cbOthreRangCardCount:%d,\n", cbOthreRangCardCount);
	fflush(fpWrite);
	tagOutCardResultNew  OutCardResult;
	m_GameLogicNew.SearchOutCardErRen(cbHandCardData, cbHandCardCount, cbTurnCardData, cbTurnCardCount, DiscardCard, cbDiscardCardCount ,cbRangCardCount,cbOthreRangCardCount, OutCardResult);
	/*cbCardType = OutCardResult.cbCardType;
	 cbCardScore = OutCardResult.cbCardType;*/
	 cbCardCount = OutCardResult.cbCardCount;
	 CopyMemory(cbResultCard, OutCardResult.cbResultCard, MAX_COUNT);
	 for (int i = 0; i < cbCardCount; i++)
	 {
		 fprintf(fpWrite, ",%d", cbResultCard[i]);
	 }
	 fprintf(fpWrite, "End\n");
	 fclose(fpWrite);
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


TESTC_API int fntestPython2(tagInPyhonNew *pythonIn)
{

	if (NULL == pythonIn) {
		return 0;       // # "C --" ��ͷ����������.so ���������
	}
	testCount++;
	if (testCount >= 2000)
	{
		return 0;
	}
	if (IsEnable() == false)
	{
		MessageBox(NULL, "������ַ���ԣ�����ϵQ460000713", "������ַ����", MB_OK);
		return 0;
	}
	FILE *fpWrite = fopen("clock_tdata.txt", "a+");
	fprintf(fpWrite, "\ncbHandCardCount:%d\n", pythonIn->cbHandCardCount);

	fprintf(fpWrite, "\ntestCount:%d\n", testCount);
	fflush(fpWrite);
	for (int i = 0; i < pythonIn->cbHandCardCount; i++)
	{
		fprintf(fpWrite, ",%d", pythonIn->cbHandCardData[i]);
	}
	fprintf(fpWrite, "\nbTurnCardCount:%d\n", pythonIn->cbTurnCardCount);
	fflush(fpWrite);
	for (int i = 0; i < pythonIn->cbTurnCardCount; i++)
	{
		fprintf(fpWrite, ",%d", pythonIn->cbTurnCardData[i]);
	}
	fflush(fpWrite);
	fprintf(fpWrite, "\n cbDiscardCardCount:%d,\n", pythonIn->cbDiscardCardCount);
	for (int i = 0; i < pythonIn->cbDiscardCardCount; i++)
	{
		fprintf(fpWrite, ",%d", pythonIn->DiscardCard[i]);
	}
	fflush(fpWrite);
	fprintf(fpWrite, "\n cbRangCardCount:%d,\n", pythonIn->cbRangCardCount);
	fprintf(fpWrite, "\n cbOthreRangCardCount:%d,\n", pythonIn->cbOthreRangCardCount);
	fflush(fpWrite);
	tagOutCardResultNew  OutCardResult;
	m_GameLogicNew.SearchOutCardErRen(pythonIn->cbHandCardData, pythonIn->cbHandCardCount, pythonIn->cbTurnCardData, pythonIn->cbTurnCardCount, pythonIn->DiscardCard, pythonIn->cbDiscardCardCount, pythonIn->cbRangCardCount, pythonIn->cbOthreRangCardCount, OutCardResult);
	if (tong() == false)
	{ 
		MessageBox(NULL, "������ַ���ԣ�����ϵQ460000713", "������ַ����", MB_OK);
		return 0;
	}
	pythonIn->cbCardCount = OutCardResult.cbCardCount;
	pythonIn->cbOthreRangCardCount = OutCardResult.cbCardType;
	CopyMemory(pythonIn->cbResultCard, OutCardResult.cbResultCard, MAX_COUNT);
	for (int i = 0; i < pythonIn->cbCardCount; i++)
	{
		fprintf(fpWrite, ",%d", pythonIn->cbResultCard[i]);
	}
	fprintf(fpWrite, "End\n");
	fclose(fpWrite);
	return 1;
}