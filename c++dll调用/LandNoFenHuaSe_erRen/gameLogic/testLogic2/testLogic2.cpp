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
	//BYTE    tempCard[] = { 2, 13, 12, 10, 8, 8, 5, 5 };
	BYTE    tempCard[] = { 15, 14, 8,8,9, 9, 7, 7, 7, 7 };
	tagOutCardResultNew  OutCardResult;
	BYTE cbTurnCardData[] = { 6, 6, 6, 5 };// { S6, 6, 5, 5, 4, 4 };
	int cbTurnCardCount =   sizeof(cbTurnCardData);
	BYTE cbDiscardCard[] = { 1, 13, 12, 11, 10, 9, 2, 2, 5, 5, 5, 10 };
	BYTE cbDiscardCardCount =   sizeof(cbDiscardCard);
	BYTE cbOtherDisscard[] = { 6, 6, 6, 5 };
	BYTE cbOtherDisscardCount =   sizeof(cbOtherDisscard);
	BYTE cbCardDataEx[MAX_COUNT] = { 0, 1, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	BYTE maxCard[MAX_COUNT] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	tagSearchCardResult pSearchCardResult = {};
	//m_GameLogicNew.SearchOutCard(tempCard, sizeof(tempCard), cbTurnCardData, 0, &pSearchCardResult);
	tagOutCardResultNew CardTypeResult1;
	BYTE    tempCard1[] = {  1, 1,1,1,6 };
	CardTypeResult1.cbCardCount = sizeof(tempCard1);
	CopyMemory(CardTypeResult1.cbResultCard, tempCard1, CardTypeResult1.cbCardCount);
	float score1 = m_GameLogicNew.GetCardTypeScore(CardTypeResult1);
	tagOutCardResultNew CardTypeResult2;
	BYTE    tempCard2[] = { 10, 10, 10, 12, 12 };
	CardTypeResult2.cbCardCount = sizeof(tempCard2);
	CopyMemory(CardTypeResult2.cbResultCard, tempCard2, CardTypeResult2.cbCardCount);
	float score2 = m_GameLogicNew.GetCardTypeScore(CardTypeResult2);
	int cbCurrentLandScore = 0;
	int tempScore = m_GameLogicNew.LandScoreErRen(tempCard, sizeof(tempCard), cbCurrentLandScore);
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
		if (duration>200)
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

