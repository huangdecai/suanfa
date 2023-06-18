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

	
	tagOutCardResult OutCardResult = {};
	tagWeaveItem WeaveItemArray[GAME_PLAYER][MAX_WEAVE] = {};
	WeaveItemArray[0][0].cbCenterCard = 33;
	WeaveItemArray[0][0].cbPublicCard = 33;
	WeaveItemArray[0][0].cbWeaveKind = WIK_PENG ;
	WeaveItemArray[0][0].wProvideUser = 1;
	WeaveItemArray[0][1].cbCenterCard = 0x12;
	WeaveItemArray[0][1].cbPublicCard = 0x12;
	WeaveItemArray[0][1].cbWeaveKind = WIK_PENG;
	WeaveItemArray[0][1].wProvideUser = 1;
	WeaveItemArray[0][2].cbCenterCard = 0x13;
	WeaveItemArray[0][2].cbPublicCard = 0x13;
	WeaveItemArray[0][2].cbWeaveKind = WIK_GANG;
	WeaveItemArray[0][2].wProvideUser = 1;
	BYTE cbWeaveCount[MAX_WEAVE] = { 0,0,0,0 };
	BYTE cbCardIndex[MAX_INDEX] = { 0 };	//手中扑克
	BYTE    tempCard[] = { 1, 2, 3, 4, 5, 6, 7, 7, 7, 17, 17, 20, 21 };
	BYTE cbActionCard = 17;
	BYTE cbActionMask = WIK_PENG;// 0;// WIK_CHI_HU;
		//BYTE    tempCard[] = { 4, 38, 8, 56, 10, 9, 11, 43, 12, 44, 60, 1, 33 };

		//BYTE    tempCard[] = { 0x09 };
		//BYTE    tempCard[] = { 0x02 };
		CopyMemory(cbHandCardData, tempCard, sizeof(tempCard));
	BYTE cbTurnCardData[] = { 0x37 };
	int cbTurnCardCount = 0;
	int cbRangCardCount = 0;
	int cbOthreRangCardCount = 0;
	BYTE cbDiscardCard[] = { 0x01 };
	m_GameLogic.SwitchToCardIndex(cbHandCardData, sizeof(tempCard), cbCardIndex);

	for (int i = 0; i < 500;i++)
	{
		clock_t start, fihst;
		start = clock();
		////BYTE    tempCard[] = { 0x2d, 0x0d, 0x1b, 0x0b, 0x2a, 0x0a, 0x19, 0x37, 0x17, 0x36, 0x26, 0x33, 0x03, };
		//CopyMemory(tempCard, bCardData, sizeof(tempCard));
		BYTE	cbCardIndex[MAX_INDEX] = { 0 };
		m_GameLogic.SwitchToCardIndex(cbHandCardData, sizeof(tempCard), cbCardIndex);
		//胡牌判断
		CChiHuRight chr;
		int m_cbUserAction=m_GameLogic.AnalyseChiHuCard(cbCardIndex, WeaveItemArray[0], cbWeaveCount[0], cbActionCard, chr);
		m_AndroidAI.SearchOutCard(OutCardResult, 0, 1, 0x43, cbActionMask, cbActionCard, cbCardIndex, WeaveItemArray, cbWeaveCount, cbDiscardCard, 0);
		fihst = clock();
		double duration = (double)(fihst - start);
		if (duration>2000)
		{
			int a=4;
			cout << duration << endl;
		}
	}
	
	
	//CString str;
	//str.Format(L"time:%f", duration);
	//CTraceService::TraceString(str, TraceLevel_Exception);

}

int _tmain(int argc, _TCHAR* argv[])
{
	TestGameLogic(); 
	return 0;
}

