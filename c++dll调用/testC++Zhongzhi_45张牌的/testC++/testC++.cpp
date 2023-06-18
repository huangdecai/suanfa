// testC++.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "testC++.h"
#include <stdio.h>
static int testCount = 0;
// 这是导出变量的一个示例

CGameLogicNew m_GameLogicNew;
// 这是导出函数的一个示例。
TESTC_API tagOutCardResultNew* __cdecl fntestC(BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardData[], BYTE cbTurnCardCount, BYTE	DiscardCard[], BYTE cbDiscardCardCount, BYTE cbRangCardCount, BYTE cbOthreRangCardCount, BYTE &cbCardCount, BYTE cbResultCard[])
{
	testCount++;
	if (testCount >= 200)
	{
		tagOutCardResultNew  OutCardResult;
		return NULL;
	}
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
	return &OutCardResult;
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
		return 0;       // # "C --" 打头区分这是在.so 里面输出的
	}
	testCount++;
	if (testCount >= 200)
	{
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
	pythonIn->cbCardCount = OutCardResult.cbCardCount;
	CopyMemory(pythonIn->cbResultCard, OutCardResult.cbResultCard, MAX_COUNT);
	for (int i = 0; i < pythonIn->cbCardCount; i++)
	{
		fprintf(fpWrite, ",%d", pythonIn->cbResultCard[i]);
	}
	fprintf(fpWrite, "End\n");
	fclose(fpWrite);
	return 1;
}