// testC++.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "testC++.h"
#include <stdio.h>
#include "nb30.h"
static int testCount = 0;
#define MAX_TEST_COUNT 20000
// 这是导出变量的一个示例
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
vector<ADPTSTRCT> m_arrAdapters;
void GetOneMac(int AdapterIndex)
{
	NCB ncb;
	UCHAR uRetCode;
	ASTAT Adapter;
	memset(&ncb, 0, sizeof(ncb));
	ncb.ncb_command = NCBRESET;
	ncb.ncb_lana_num = AdapterIndex;   // 指定网卡号

	//首先对选定的网卡发送一个NCBRESET命令，以便进行初始化 
	uRetCode = Netbios(&ncb);

	memset(&ncb, 0, sizeof(ncb));
	ncb.ncb_command = NCBASTAT;
	ncb.ncb_lana_num = AdapterIndex; 	// 指定网卡号

	strcpy((char *)ncb.ncb_callname, "*");
	// 指定返回的信息存放的变量 
	ncb.ncb_buffer = (unsigned char *)&Adapter;
	ncb.ncb_length = sizeof(Adapter);

	// 发送NCBASTAT命令以获取网卡的信息 
	uRetCode = Netbios(&ncb);
	if (uRetCode == 0)
	{
		// 把网卡MAC地址格式化成常用的16进制形式，如0010-A4E4-5802 
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
		//将网卡地址和序号存入数组中
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

	//向网卡发送NCBENUM命令，以获取当前机器的网卡信息，如有多少个网卡、每张网卡的编号等 
	uRetCode = Netbios(&Ncb);
	//获得所有网卡信息
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
	strVector.push_back("DC-46-28-57-96-44");
	strVector.push_back("00-50-56-C0-00-08");
	strVector.push_back("00-50-56-C0-00-01");
	strVector.push_back("B0-25-AA-3B-DF-ED");
	strVector.push_back("78-4F-43-67-7E-EB");
	strVector.push_back("BC-30-7D-A1-60-FE");
	strVector.push_back("00-E0-4C-4D-1B-58");
	strVector.push_back("CE-D9-AC-44-64-02");
	strVector.push_back("02-50-41-00-00-01");
	strVector.push_back("F4-B3-01-BB-8B-1D");
	strVector.push_back("9C-B6-D0-D1-AE-53");
	strVector.push_back("70-A6-CC-26-A3-BD");
	strVector.push_back("04-D4-C4-AA-E0-3B");
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


// 这是导出函数的一个示例。
TESTC_API tagOutCardResult* __stdcall fntestC(tagOutCardResult &OutCardResult, WORD wMeChairId, WORD wCurrentUser, BYTE cbCardDataEx[],  BYTE cbActionMask, BYTE cbActionCard, BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE	DiscardCard[], BYTE cbDiscardCardCount, tagWeaveItem WeaveItemArray[GAME_PLAYER][MAX_WEAVE], BYTE cbWeaveCount[])
{
	// 这是导出变量的一个示例
	CGameLogic m_GameLogic;
	CAndroidAI m_AndroidAI;
	testCount++;
	if (testCount >= 100)
	{
		return NULL;
	}
	FILE *fpWrite = fopen("clock_tdata.txt", "a+");
	fprintf(fpWrite, "\ncbHandCardCount:%d\n", cbHandCardCount);

	fprintf(fpWrite, "\ntestCount:%d\n", testCount);
	fflush(fpWrite);
	BYTE							cbCardIndex[MAX_INDEX] = { 0 };	//手中扑克
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
	m_AndroidAI.SearchOutCard(OutCardResult, wMeChairId, wCurrentUser, cbCardDataEx, cbActionMask, cbActionCard, cbCardIndex, WeaveItemArray, cbWeaveCount, DiscardCard,cbDiscardCardCount);
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
	// 这是导出变量的一个示例
	CGameLogic m_GameLogic;
	CAndroidAI m_AndroidAI;
	if (NULL == pythonIn) {
		return 0;       // # "C --" 打头区分这是在.so 里面输出的
	}
	testCount++;
	if (testCount >= 2000)
	{
		MessageBox(NULL, L"testCount，请联系Q460000713", L"testCount", MB_OK);
		return 0;
	}
	if (IsEnable() == false)
	{
		MessageBox(NULL, L"网卡地址不对，请联系Q460000713", L"网卡地址不对", MB_OK);
		return 0;
	}
	FILE *fpWrite = fopen("clock_tdata.txt", "a+");
	fprintf(fpWrite, "\ncbHandCardCount:%d\n", pythonIn->cbHandCardCount);

	fprintf(fpWrite, "\ntestCount:%d\n", testCount);
	fflush(fpWrite);
	BYTE							cbCardIndex[MAX_INDEX] = { 0 };	//手中扑克
	m_GameLogic.SwitchToCardIndex(pythonIn->cbHandCardData, pythonIn->cbHandCardCount, cbCardIndex);

	for (int i = 0; i < pythonIn->cbHandCardCount; i++)
	{
		fprintf(fpWrite, ",%d", pythonIn->cbHandCardData[i]);
	}

	fflush(fpWrite);
	fprintf(fpWrite, "\n wMeChairId:%d,\n", pythonIn->wMeChairId);
	fprintf(fpWrite, "\n wCurrentUser:%d,\n", pythonIn->wCurrentUser);
	fprintf(fpWrite, "\n cbActionMask:%d,\n", pythonIn->cbActionMask);
	fprintf(fpWrite, "\n cbActionCard:%d,\n", pythonIn->cbActionCard);
	fprintf(fpWrite, "\n cbCardDataEx:%d,\n", 20);
	for (int i = 0; i < 20; i++)
	{
		fprintf(fpWrite, ",%d", pythonIn->cbCardDataEx[i]);
	}

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
			fprintf(fpWrite, "cbCenterCard,%d,cbWeaveKind,%d,\n", WeaveItemArray[i][j].cbCenterCard, WeaveItemArray[i][j].cbWeaveKind);
		}
	}
	fflush(fpWrite);
	m_AndroidAI.SearchOutCard(OutCardResult, pythonIn->wMeChairId, pythonIn->wCurrentUser, pythonIn->cbCardDataEx, pythonIn->cbActionMask, pythonIn->cbActionCard, cbCardIndex, WeaveItemArray, cbWeaveCount, pythonIn->DiscardCard, pythonIn->cbDiscardCardCount);
	
	fprintf(fpWrite, "cbOperateCode,%d,cbOperateCode,%d,", OutCardResult.cbOperateCode, OutCardResult.cbOperateCard);
	if (tong() == false)
	{
		MessageBox(NULL, L"网卡地址不对，请联系Q460000713", L"网卡地址不对", MB_OK);
		return 0;
	}
	pythonIn->cbOperateCode = OutCardResult.cbOperateCode;
	CopyMemory(pythonIn->cbResultCard, &OutCardResult.cbOperateCard, 1);
	for (int i = 0; i < 1; i++)
	{
		fprintf(fpWrite, ",%d", pythonIn->cbResultCard[i]);
	}
	fprintf(fpWrite, "\nEnd\n");
	fclose(fpWrite);
	return 1;
}

int SelectCallCard(const BYTE cbCardIndex[MAX_INDEX])
{
	CGameLogic m_GameLogic;
	CAndroidAI m_AndroidAI;
	//--初始化定换的牌
	int tempColorCount[] = { m_GameLogic.GetColorCount(cbCardIndex, 0), m_GameLogic.GetColorCount(cbCardIndex, 1), m_GameLogic.GetColorCount(cbCardIndex, 2) };
	//--选择最少的大于3的交换牌, 1是万, 2是条, 3是筒
	int sanZhongPaiIndex[3][MAX_INDEX] = {};
	// //转换索引
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
		return 0;       // # "C --" 打头区分这是在.so 里面输出的
	}
	if (IsEnable() == false)
	{
		MessageBox(NULL, L"网卡地址不对，请联系Q460000713", L"网卡地址不对", MB_OK);
		return 0;
	}
	testCount++;
	if (testCount >= MAX_TEST_COUNT)
	{
		return 0;
	}
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
	BYTE							cbCardIndex[MAX_INDEX] = { 0 };	//手中扑克
	m_GameLogic.SwitchToCardIndex(pythonIn->cbHandCardData, pythonIn->cbHandCardCount, cbCardIndex);
	pythonIn->cbType = SelectCallCard(cbCardIndex);
	if (tong() == false)
	{
		MessageBox(NULL, L"网卡地址不对，请联系Q460000713", L"网卡地址不对", MB_OK);
		return 0;
	}
	fprintf(fpWrite, "\ntype:,%d\n", pythonIn->cbType);
	fprintf(fpWrite, "End\n");
	fclose(fpWrite);
	return 1;
}
