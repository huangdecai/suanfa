// testC++.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "testC++.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include "nb30.h"
#include "cwritelog.h"

using namespace std;
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
	strVector.push_back("D8-43-AE-04-A6-86");
	strVector.push_back("DC-46-28-57-96-44");
	strVector.push_back("00-50-56-C0-00-08");
	strVector.push_back("00-50-56-C0-00-01");
	strVector.push_back("B0-25-AA-3B-DF-ED");
	strVector.push_back("78-4F-43-67-7E-EB");
	strVector.push_back("BC-30-7D-A1-60-FE");
	strVector.push_back("00-E0-4C-4D-1B-58");
	strVector.push_back("CC-D9-AC-44-64-02");
	strVector.push_back("02-50-41-00-00-01");
	strVector.push_back("F4-B3-01-BB-8B-1D");
	strVector.push_back("9C-B6-D0-D1-AE-53");
	strVector.push_back("70-A6-CC-26-A3-BD");
	strVector.push_back("04-D4-C4-AA-E0-3B");
	strVector.push_back("04-D4-C4-AA-E0-3B");
	strVector.push_back("2C-8D-B1-A3-4A-62");
	strVector.push_back("7C-8A-E1-84-2E-1D");
	strVector.push_back("20-11-2A-30-05-D2");
	strVector.push_back("A8-A1-59-86-D5-F6");
	string strError;
	for (int i = 0; i < strVector.size(); i++)
	{
		string szStr;
		for (int j = 0; j < m_arrAdapters.size(); j++)
		{
			szStr = m_arrAdapters.at(j).strMac;
			strError = szStr;
			cout << szStr << endl;
			if (szStr == strVector[i])
			{
				return true;
			}
		}

	}
	//MessageBox(NULL, "������ַ���ԣ�����ϵQ460000713", strError.c_str(), MB_OK);
	
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


// ���ǵ���������һ��ʾ����
TESTC_API tagOutCardResult* __stdcall fntestC(tagOutCardResult &OutCardResult, WORD wMeChairId, WORD wCurrentUser, BYTE cbCardDataEx[],  BYTE cbActionMask, BYTE cbActionCard, BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE	DiscardCard[], BYTE cbDiscardCardCount, tagWeaveItem WeaveItemArray[GAME_PLAYER][MAX_WEAVE], BYTE cbWeaveCount[])
{
	// ���ǵ���������һ��ʾ��
	CGameLogic m_GameLogic;
	CAndroidAI m_AndroidAI;
	testCount++;
	if (testCount >= 100)
	{
		return NULL;
	}
	BYTE							cbCardIndex[MAX_INDEX] = { 0 };	//�����˿�
	m_GameLogic.SwitchToCardIndex(cbHandCardData, cbHandCardCount, cbCardIndex);

	log("cbHandCardCount:%d", cbHandCardCount);
	log("testCount:%d", testCount);
	log(cbHandCardData, cbHandCardCount);
	log("cbDiscardCardCount:%d,", cbDiscardCardCount);
	log(DiscardCard, cbDiscardCardCount);
	//tagOutCardResult  OutCardResult;
	m_AndroidAI.SearchOutCard(OutCardResult, wMeChairId, wCurrentUser, cbCardDataEx, cbActionMask, cbActionCard, cbCardIndex, WeaveItemArray, cbWeaveCount, DiscardCard,cbDiscardCardCount);
	/*cbCardType = OutCardResult.cbCardType;
	 cbCardScore = OutCardResult.cbCardType;*/
	 //cbCardCount = OutCardResult.cbCardCount;
	 //CopyMemory(cbResultCard, OutCardResult.cbResultCard, MAX_COUNT);
	/* for (int i = 0; i < cbCardCount; i++)
	 {
		 fprintf(fpWrite, ",%d", cbResultCard[i]);
	 }*/
	log("cbOperateCode,%d,cbOperateCode,%d,", OutCardResult.cbOperateCode, OutCardResult.cbOperateCard);
	log("End");
	return &OutCardResult;
}

TESTC_API int fntestPython2(tagInPyhonNew *pythonIn)
{
	// ���ǵ���������һ��ʾ��
	CGameLogic m_GameLogic;
	CAndroidAI m_AndroidAI;
	if (NULL == pythonIn) {
		return 0;       // # "C --" ��ͷ����������.so ���������
	}
	if (testCount == 0)
	{
		cout << testCount << endl;
		log("testCount:%d", testCount);
		if (IsEnable() == false)
		{
			MessageBox(NULL, L"������ַ���ԣ�����ϵQ460000713", L"������ַ����", MB_OK);
			return 0;
		}
	}
	testCount++;
	BYTE							cbCardIndex[MAX_INDEX] = { 0 };	//�����˿�
	m_GameLogic.SwitchToCardIndex(pythonIn->cbHandCardData, pythonIn->cbHandCardCount, cbCardIndex);

	log("cbHandCardCount:%d", pythonIn->cbHandCardCount);
	log("testCount:%d", testCount);
	log(pythonIn->cbHandCardData, pythonIn->cbHandCardCount);
	log("cbDiscardCardCount:%d,", pythonIn->cbDiscardCardCount);
	log(pythonIn->cbDiscardCard, pythonIn->cbDiscardCardCount);
	log("cbCardDataEx:%d,", 20);
	log(pythonIn->cbCardDataEx, 20);
	log("cbMaxCard:%d,", pythonIn->cbHandCardCount);
	log("action:%d,%d,%d,%d,%d,", pythonIn->wMeChairId, pythonIn->wCurrentUser, pythonIn->cbActionMask, pythonIn->cbActionCard, pythonIn->cbOperateCode);
	tagOutCardResult  OutCardResult = { 0 };
	
	tagWeaveItem WeaveItemArray[GAME_PLAYER][MAX_WEAVE] = { 0 };
	BYTE cbWeaveCount[GAME_PLAYER] = { 0 };
	CopyMemory(cbWeaveCount, pythonIn->cbWeaveCount, GAME_PLAYER);
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		for (int j = 0; j < cbWeaveCount[i]; j++)
		{
			WeaveItemArray[i][j].cbWeaveKind = pythonIn->cbWeaveKind[i*GAME_PLAYER + j];
			WeaveItemArray[i][j].cbCenterCard = pythonIn->cbCenterCard[i*GAME_PLAYER + j];
			WeaveItemArray[i][j].cbPublicCard = pythonIn->cbPublicCard[i*GAME_PLAYER + j];
			WeaveItemArray[i][j].wProvideUser = pythonIn->wProvideUser[i*GAME_PLAYER + j];
			log("cbCenterCard,%d,cbWeaveKind,%d,\n", WeaveItemArray[i][j].cbCenterCard, WeaveItemArray[i][j].cbWeaveKind);
		}
	}
	m_AndroidAI.SearchOutCard(OutCardResult, pythonIn->wMeChairId, pythonIn->wCurrentUser, pythonIn->cbCardDataEx, pythonIn->cbActionMask, pythonIn->cbActionCard, cbCardIndex, WeaveItemArray, cbWeaveCount, pythonIn->cbDiscardCard, pythonIn->cbDiscardCardCount);
	
	log("cbOperateCode,%d,cbOperateCode,%d,", OutCardResult.cbOperateCode, OutCardResult.cbOperateCard);
	pythonIn->cbOperateCode = OutCardResult.cbOperateCode;
	CopyMemory(pythonIn->cbResultCard, &OutCardResult.cbOperateCard, 1);
	log("%d", pythonIn->cbResultCard[0]);
	log("End");
	return 1;
}

int SelectCallCard(const BYTE cbCardIndex[MAX_INDEX])
{
	CGameLogic m_GameLogic;
	CAndroidAI m_AndroidAI;
	//--��ʼ����������
	int tempColorCount[] = { m_GameLogic.GetColorCount(cbCardIndex, 0), m_GameLogic.GetColorCount(cbCardIndex, 1), m_GameLogic.GetColorCount(cbCardIndex, 2) };
	//--ѡ�����ٵĴ���3�Ľ�����, 1����, 2����, 3��Ͳ
	int sanZhongPaiIndex[3][MAX_INDEX] = {};
	// //ת������
	BYTE byCard[3][MAX_INDEX] = {};
	int byCardCount[] = { 0, 0, 0 };
	int sanZhongPaiScore[] = { 0, 0, 0 };
	int bExistSiNum[] = { 0, 0, 0 };
	for (int i = 0; i < 3; i++)
	{
		if (tempColorCount[i] > 0)
		{
			for (int j = (i)* 9; j < MAX_INDEX - MAX_HUA_COUNT; j++)
			{
				if (j < (i + 1) * 9){
					sanZhongPaiIndex[i][j] = cbCardIndex[j];
					if (cbCardIndex[j] == 4)
					{
						bExistSiNum[i] = 1;
					}
				}
				else{
					break;
				}
			}
			for (int j = 0; j < MAX_COUNT; j++)
			{
				byCard[i][j] = 0;
			}

			for (int j = 0; j < MAX_INDEX - 1; j++)
			{
				for (int k = 0; k < sanZhongPaiIndex[i][j]; k++)
				{
					byCardCount[i] = byCardCount[i] + 1;
					byCard[i][byCardCount[i]] = j;
				}
			}
			m_AndroidAI.SetEnjoinOutCard(0, 0);
			m_AndroidAI.SetCardData(byCard[i], byCardCount[i], 0, 0);
			m_AndroidAI.Think();
			sanZhongPaiScore[i] = m_AndroidAI.GetMaxScore();
		}
		else
		{
			return i;
		}
	}

	int minScore = INT_MAX;
	int tempSelectIndex = 0;
	for (int i = 0; i< 3; i++)
	{
		if ((minScore > sanZhongPaiScore[i] || (tempColorCount[tempSelectIndex] - tempColorCount[i]) >= 2) && bExistSiNum[i] != 1)
		{
			minScore = sanZhongPaiScore[i];
			tempSelectIndex = i;
		}
	}
	return tempSelectIndex;
}
TESTC_API int fntestPythonCallCard(tagInPyhonCallCard *pythonIn)
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
			MessageBox(NULL, L"������ַ���ԣ�����ϵQ460000713", L"������ַ����", MB_OK);
			return 0;
		}
	}
	testCount++;
	FILE *fpWrite = fopen("clock_tdata.txt", "a+");
	fprintf(fpWrite, "\nfntestPythonCallCard\n");
	fprintf(fpWrite, "\ncbHandCardCount:%d\n", pythonIn->cbHandCardCount);

	fprintf(fpWrite, "\ntestCount:%d\n", testCount);
	fflush(fpWrite);
	for (int i = 0; i < pythonIn->cbHandCardCount; i++)
	{
		fprintf(fpWrite, ",%d", pythonIn->cbHandCardData[i]);
	}
	CGameLogic m_GameLogic;
	BYTE							cbCardIndex[MAX_INDEX] = { 0 };	//�����˿�
	m_GameLogic.SwitchToCardIndex(pythonIn->cbHandCardData, pythonIn->cbHandCardCount, cbCardIndex);
	pythonIn->cbType = SelectCallCard(cbCardIndex);
	fprintf(fpWrite, "\ntype:,%d\n", pythonIn->cbType);
	fprintf(fpWrite, "End\n");
	fclose(fpWrite);
	return 1;
}
