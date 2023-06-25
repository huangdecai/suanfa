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
#include "GameLogicNew.h"
#include <time.h>
CGameLogicNew  m_GameLogicNew;
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
//tagInPyhonNew pythonIn = {};
//CopyMemory(pythonIn.cbHandCardData, tempCard, sizeof(tempCard));
//pythonIn.cbHandCardCount = sizeof(tempCard);
//CopyMemory(pythonIn.cbTurnCardData, cbTurnCardData, sizeof(cbTurnCardData));
//pythonIn.cbTurnCardCount = cbTurnCardCount;
//CopyMemory(pythonIn.cbDiscardCard, cbDiscardCard, sizeof(cbDiscardCard));
//pythonIn.cbDiscardCardCount = cbDiscardCardCount;
//CopyMemory(pythonIn.cbOtherDiscardCard, cbOtherDisscard, sizeof(cbOtherDisscard));
//pythonIn.cbOtherDiscardCardCount = cbOtherDisscardCount;
//CopyMemory(pythonIn.cbMaxCard, maxCard, sizeof(maxCard));
//CopyMemory(pythonIn.cbCardDataEx, cbCardDataEx, sizeof(cbCardDataEx));
//int tempScore2 = dynamic_call(&pythonIn);
// 动态调用DLL库
int dynamic_call(tagInPyhonNew *pythonIn)
{
    typedef int(*AddFunc)(tagInPyhonNew *pythonIn);

	HMODULE module = LoadLibrary(L"testC++.dll");
	if (module == NULL)
	{
		cout << "加载DemoDLL.dll动态库失败" << endl;
		return -1;
	}
	BYTE							cbResultCard[MAX_COUNT] = {};
	AddFunc add = (AddFunc)GetProcAddress(module, "fntestPython2");
	return add(pythonIn);
}

void TestGameLogic()
{
	
	BYTE	cbHandCardData[NORMAL_COUNT] = { 0 };	//手上扑克
	//BYTE    tempCard[] = { 0x1C, 0x12, 0x02, 0x38, 0x27, 0x05, 0x23, 0x03, 0x19, 0x28, 0x37, 0x26, 0x25 };
	BYTE    tempCard[] = { 14, 1, 1, 1, 13, 12, 12, 11, 11, 9, 7, 7, 6, 5, 5, 5 };
	//BYTE    tempCard[] = { 2, 2, 2, 13, 10, 8, 8, 7, 7, 5 };

	tagOutCardResultNew  OutCardResult;
	BYTE cbTurnCardData[] = { 12 };// { S6, 6, 5, 5, 4, 4 };
	int cbTurnCardCount =   sizeof(cbTurnCardData);
	BYTE cbDiscardCard[] = { 1, 1, 6, 6, 6, 5, 11, 11, 11, 7 };
	BYTE cbDiscardCardCount =  sizeof(cbDiscardCard);
	BYTE cbOtherDisscard[] = { 5, 5, 8, 8, 8, 6 };
	BYTE cbOtherDisscardCount = sizeof(cbOtherDisscard);
	BYTE cbCardDataEx[MAX_COUNT] = { 0, 0, 3, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	BYTE maxCard[MAX_COUNT] = { 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	WORD wBankerUser = 0;
	WORD wUndersideUser = (wBankerUser + 1) % GAME_PLAYER;
	WORD wUpsideUser = (wUndersideUser + 1) % GAME_PLAYER;
	m_GameLogicNew.SetBanker(wBankerUser);

	m_GameLogicNew.SetUserCard(wBankerUser, tempCard, sizeof(tempCard));

	tagOutCardResultNew CardTypeResult1;
	CardTypeResult1.cbCardCount = 5;
	BYTE    tempCard1[] = {  4, 4 };
	BYTE    tempCard2[] = {  3, 3 };
	BYTE    tempCard3[] = {  3, 4 };
	CopyMemory(CardTypeResult1.cbResultCard, tempCard1, CardTypeResult1.cbCardCount);
	float score1 = m_GameLogicNew.GetCardTypeScore(CardTypeResult1);
	CopyMemory(CardTypeResult1.cbResultCard, tempCard2, CardTypeResult1.cbCardCount);
	float score2 = m_GameLogicNew.GetCardTypeScore(CardTypeResult1);
	CopyMemory(CardTypeResult1.cbResultCard, tempCard3, CardTypeResult1.cbCardCount);
	float score3 = m_GameLogicNew.GetCardTypeScore(CardTypeResult1);

	/*clock_t start, fihst;
	start = clock();*/

	int cbCurrentLandScore = 0;
	int tempScore = m_GameLogicNew.LandScoreErRen(tempCard, sizeof(tempCard), cbCurrentLandScore);
	//int a = 4;
	////
	BYTE    tempCardEX[3] = { tempCard[0],tempCard[1],tempCard[2] };
	//int turnCount = m_GameLogicNew.CalCardTurnCount(tempCardEX, sizeof(tempCardEX), tempCard, sizeof(tempCard), NULL, 0,1);
	for (int i = 0; i < 1000;i++)
	{
		clock_t start, fihst;
		start = clock();
		BYTE bCardData[DISPATCH_COUNT] = { 0 };
		m_GameLogicNew.RandCardList(bCardData, DISPATCH_COUNT);
		//BYTE    tempCard[] = { 15,14,2,1,13,12,12,10,10,9,9,8,8,8,7,6,6,5,5,5 };
		//CopyMemory(tempCard, bCardData, sizeof(tempCard));
		m_GameLogicNew.SearchOutCardErRen(tempCard, sizeof(tempCard), cbTurnCardData, cbTurnCardCount, cbDiscardCard, cbDiscardCardCount, cbOtherDisscard, cbOtherDisscardCount, cbCardDataEx, maxCard, OutCardResult);
		if (OutCardResult.cbCardCount>0)
		{
			int a = 4;
		}
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

