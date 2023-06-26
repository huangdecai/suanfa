// testC++.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "testC++.h"
#include <stdio.h>
#include "GameLogic.h"
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
	strVector.push_back("B0-25-AA-3B-DF-ED");
	strVector.push_back("78-4F-43-67-7E-EB");
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
	strVector.push_back("78-4F-43-67-7E-EB");
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
CGameLogic m_GameLogic;
CAndroidAI m_AndroidAI;

// ���ǵ���������һ��ʾ����
TESTC_API tagOutCardResult* __stdcall fntestC(tagOutCardResult &OutCardResult, WORD wMeChairId, WORD wCurrentUser, BYTE cbCallCard, BYTE cbActionMask, BYTE cbActionCard, BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE	DiscardCard[], BYTE cbDiscardCardCount, tagWeaveItem WeaveItemArray[GAME_PLAYER][MAX_WEAVE], BYTE cbWeaveCount[])
{
	testCount++;
	if (testCount >= 100)
	{
		return NULL;
	}
	FILE *fpWrite = fopen("clock_tdata.txt", "a+");
	fprintf(fpWrite, "\ncbHandCardCount:%d\n", cbHandCardCount);

	fprintf(fpWrite, "\ntestCount:%d\n", testCount);
	fflush(fpWrite);
	BYTE							cbCardIndex[MAX_INDEX] = { 0 };	//�����˿�
	m_GameLogic.SwitchToCardIndex(cbHandCardData, cbHandCardCount, cbCardIndex);

	for (int i = 0; i < cbHandCardCount; i++)
	{
		fprintf(fpWrite, ",%d", cbHandCardData[i]);
	}
	
	fflush(fpWrite);
	fprintf(fpWrite, "\n cbDiscardCardCount:%d,\n", cbDiscardCardCount);
	for (int i = 0; i < cbDiscardCardCount; i++)
	{
		fprintf(fpWrite, ",%d", DiscardCard[i]);
	}
	fflush(fpWrite);
	//tagOutCardResult  OutCardResult;
	m_AndroidAI.SearchOutCard(OutCardResult, wMeChairId, wCurrentUser, cbCallCard, cbActionMask, cbActionCard, cbCardIndex, WeaveItemArray, cbWeaveCount, DiscardCard,cbDiscardCardCount);
	/*cbCardType = OutCardResult.cbCardType;
	 cbCardScore = OutCardResult.cbCardType;*/
	 //cbCardCount = OutCardResult.cbCardCount;
	 //CopyMemory(cbResultCard, OutCardResult.cbResultCard, MAX_COUNT);
	/* for (int i = 0; i < cbCardCount; i++)
	 {
		 fprintf(fpWrite, ",%d", cbResultCard[i]);
	 }*/
	fprintf(fpWrite, "cbOperateCode,%d,cbOperateCode,%d,", OutCardResult.cbOperateCode, OutCardResult.cbOperateCard);
	 fprintf(fpWrite, "End\n");
	 fclose(fpWrite);
	return &OutCardResult;
}

TESTC_API int fntestPython2(tagInPyhonNew *pythonIn)
{

	if (NULL == pythonIn) {
		return 0;       // # "C --" ��ͷ����������.so ���������
	}
	testCount++;
	if (testCount >= 2000)
	{
		MessageBox(NULL, L"testCount������ϵQ460000713", L"testCount", MB_OK);
		return 0;
	}
	if (IsEnable() == false)
	{
		MessageBox(NULL, L"������ַ���ԣ�����ϵQ460000713", L"������ַ����", MB_OK);
		return 0;
	}
	FILE *fpWrite = fopen("clock_tdata.txt", "a+");
	fprintf(fpWrite, "\ncbHandCardCount:%d\n", pythonIn->cbHandCardCount);

	fprintf(fpWrite, "\ntestCount:%d\n", testCount);
	fflush(fpWrite);
	BYTE							cbCardIndex[MAX_INDEX] = { 0 };	//�����˿�
	m_GameLogic.SwitchToCardIndex(pythonIn->cbHandCardData, pythonIn->cbHandCardCount, cbCardIndex);

	for (int i = 0; i < pythonIn->cbHandCardCount; i++)
	{
		fprintf(fpWrite, ",%d", pythonIn->cbHandCardData[i]);
	}

	fflush(fpWrite);
	fprintf(fpWrite, "\n cbDiscardCardCount:%d,\n", pythonIn->cbDiscardCardCount);
	for (int i = 0; i < pythonIn->cbDiscardCardCount; i++)
	{
		fprintf(fpWrite, ",%d", pythonIn->DiscardCard[i]);
	}
	fflush(fpWrite);
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
		}
	}
	m_AndroidAI.SearchOutCard(OutCardResult, pythonIn->wMeChairId, pythonIn->wCurrentUser, pythonIn->cbCallCard, pythonIn->cbActionMask, pythonIn->cbActionCard, cbCardIndex, WeaveItemArray, cbWeaveCount, pythonIn->DiscardCard, pythonIn->cbDiscardCardCount);
	
	fprintf(fpWrite, "cbOperateCode,%d,cbOperateCode,%d,", OutCardResult.cbOperateCode, OutCardResult.cbOperateCard);
	fprintf(fpWrite, "End\n");
	fclose(fpWrite);
	if (tong() == false)
	{
		MessageBox(NULL, L"������ַ���ԣ�����ϵQ460000713", L"������ַ����", MB_OK);
		return 0;
	}
	pythonIn->cbOperateCode = OutCardResult.cbOperateCode;
	CopyMemory(pythonIn->cbResultCard, &OutCardResult.cbOperateCard, 1);
	for (int i = 0; i < 1; i++)
	{
		fprintf(fpWrite, ",%d", pythonIn->cbResultCard[i]);
	}
	fprintf(fpWrite, "End\n");
	fclose(fpWrite);
	return 1;
}