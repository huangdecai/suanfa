// testLogic2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <random>
#include <iomanip>
#include <chrono>
#include <string>
#include <sstream>
using namespace std;
#include "GameLogic.h"
#include "AndroidAI.h"
#include <time.h>
CGameLogic m_GameLogic;
CAndroidAI m_AndroidAI;
#include <objbase.h>
#include <map>
#include <ctime>
#include<sys/timeb.h>
int SelectCallCard(const BYTE cbCardIndex[MAX_INDEX]);
static int getRandEx(int nMin, int nMax)
{
	if (nMax <= nMin)
		return nMin;
	static std::default_random_engine generator(time(0));
	std::uniform_int_distribution<int> dis(nMin, nMax);
	return dis(generator);
}

void TestGameLogic()
{

	BYTE	cbHandCardData[MAX_COUNT] = { 0 };	//手上扑克
	//BYTE    tempCard[] = { 0x09 };
	//BYTE    tempCard[] = { };
	//BYTE    tempCard[] = { 0x2B, 0x0B, 0x05, 0x1D, 0x0D, 0x2D, 0x32, 0x33, 0x26, 0x16, 0x34, 0x14, 0x04 };
	//BYTE    tempCard[] = { 0x3C, 0x32, 0x02, 0x3A, 0x09, 0x04, 0x24, 0x03, 0x1D, 0x1B, 0x1A, 0x17, 0x13 };
	//BYTE    tempCard[] = { 0x1C, 0x12, 0x02, 0x38, 0x27, 0x05, 0x23, 0x03, 0x19, 0x28, 0x37, 0x26, 0x25 };

	
	

	/*clock_t start, fihst;
	start = clock();*/

	
	
	
	tagWeaveItem WeaveItemArray[GAME_PLAYER][MAX_WEAVE] = {};
	byte tmpCardData[] = { 49, 49, 49, 0 };
	WeaveItemArray[0][0].cbCenterCard = tmpCardData[0];
	WeaveItemArray[0][0].cbPublicCard = tmpCardData[0];
	WeaveItemArray[0][0].cbWeaveKind = WIK_PENG ;
	WeaveItemArray[0][0].wProvideUser = 1;
	CopyMemory(WeaveItemArray[0][0].cbCardData, tmpCardData, sizeof(tmpCardData));

	byte tmpCardData2[] = { 54, 54, 54, 0 };
	WeaveItemArray[0][1].cbCenterCard = tmpCardData2[0];
	WeaveItemArray[0][1].cbPublicCard = tmpCardData2[0];
	WeaveItemArray[0][1].cbWeaveKind = WIK_PENG;
	WeaveItemArray[0][1].wProvideUser = 1;
	CopyMemory(WeaveItemArray[0][1].cbCardData, tmpCardData2, sizeof(tmpCardData2));
	byte tmpCardData3[] = { 55, 55, 55, 0 };
	WeaveItemArray[0][2].cbCenterCard = tmpCardData3[0];
	WeaveItemArray[0][2].cbPublicCard = tmpCardData3[0];
	WeaveItemArray[0][2].cbWeaveKind = WIK_PENG;
	WeaveItemArray[0][2].wProvideUser = 1;
	CopyMemory(WeaveItemArray[0][2].cbCardData, tmpCardData3, sizeof(tmpCardData3));
	byte tmpCardData4[] = { 39, 39, 39, 0 };
	WeaveItemArray[0][3].cbCenterCard = tmpCardData4[0];
	WeaveItemArray[0][3].cbPublicCard = tmpCardData4[0];
	WeaveItemArray[0][3].cbWeaveKind = WIK_PENG;
	WeaveItemArray[0][3].wProvideUser = 1;
	CopyMemory(WeaveItemArray[0][3].cbCardData, tmpCardData4, sizeof(tmpCardData4));
	BYTE cbWeaveCount[MAX_WEAVE] = { 0,0,0,0 };
	BYTE cbCardIndex[MAX_INDEX] = { 0 };	//手中扑克
	//二项听测试手牌
	//BYTE    tempCard[] = { 0x22,0x23,0x23,0x27,0x02,0x03,0x04,0x11,0x11,0x11,0x15,0x16,0x16,0x17 };


	BYTE    tempCard[] = { 0x19,0x19,0x18,0x14,0x13,0x11,0x11,0x11,0x09,0x09,0x07,0x06,0x04,0x03 };
	BYTE cbActionCard = 0x18;
	BYTE cbActionMask = 0;// 0;// WIK_CHI_HU;
		//BYTE    tempCard[] = { 4, 38, 8, 56, 10, 9, 11, 43, 12, 44, 60, 1, 33 };

		//BYTE    tempCard[] = { 0x09 };
		//BYTE    tempCard[] = { 0x02 };
		CopyMemory(cbHandCardData, tempCard, sizeof(tempCard));
	BYTE cbTurnCardData[] = { 0x37 };
	int cbTurnCardCount = 0;
	int cbRangCardCount = 0;
	int cbOthreRangCardCount = 0;
	BYTE cbCardDataEx[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	BYTE cbDiscardCard[] = { 0 };
	m_GameLogic.SwitchToCardIndex(cbHandCardData, sizeof(tempCard), cbCardIndex);
	int que = SelectCallCard(cbCardIndex);
	CChiHuRight chr;
	//int m_cbUserAction = m_GameLogic.AnalyseChiHuCard(cbCardIndex, WeaveItemArray[0], cbWeaveCount[0], cbActionCard, chr);

	for (int i = 0; i < 1000;i++)
	{
		clock_t start, fihst;
		start = clock();
		BYTE bCardData[MAX_REPERTORY] = { 0 };
		m_GameLogic.RandCardList(bCardData, MAX_REPERTORY);
		/*	CopyMemory(cbHandCardData, bCardData, sizeof(cbHandCardData));
			ZeroMemory(cbCardIndex, sizeof(cbCardIndex));
			m_GameLogic.SwitchToCardIndex(cbHandCardData, sizeof(cbHandCardData), cbCardIndex);
			cbActionCard = cbHandCardData[MAX_COUNT - 1];*/
		//胡牌判断
		tagOutCardResult OutCardResult = {};
		bool reulst = m_AndroidAI.SearchOutCard(OutCardResult, 0, 0, cbCardDataEx, cbActionMask, cbActionCard, cbCardIndex, WeaveItemArray, cbWeaveCount, cbDiscardCard, sizeof(cbDiscardCard));
		if (OutCardResult.cbOperateCard==0)
		{
			int a = 4;
		}
		

		fihst = clock();
		double duration = (double)(fihst - start);
		cout << duration << ":" << i << endl;
		if (duration>2000)
		{
			int a=4;
			cout << duration <<":"<<i<< endl;
		}
	}
	
	
	//CString str;
	//str.Format(L"time:%f", duration);
	//CTraceService::TraceString(str, TraceLevel_Exception);

}
int SelectCallCard(const BYTE cbCardIndex[MAX_INDEX])
{
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
			for (int j = (i) * 9; j < MAX_INDEX - MAX_HUA_COUNT; j++)
			{
				if (j < (i + 1) * 9) {
					sanZhongPaiIndex[i][j] = cbCardIndex[j];
					if (cbCardIndex[j] == 4)
					{
						bExistSiNum[i] = 1;
					}
				}
				else {
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
	for (int i = 0; i < 3; i++)
	{
		if ((minScore > sanZhongPaiScore[i] || (tempColorCount[tempSelectIndex] - tempColorCount[i]) >= 2) && bExistSiNum[i] != 1)
		{
			minScore = sanZhongPaiScore[i];
			tempSelectIndex = i;
		}
	}
	return tempSelectIndex;
}
int _tmain(int argc, _TCHAR* argv[])
{
	TestGameLogic(); 
	return 0;
}

