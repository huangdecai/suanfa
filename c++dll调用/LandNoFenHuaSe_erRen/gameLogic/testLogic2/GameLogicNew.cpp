#include "StdAfx.h"
#include "GameLogicNew.h"
#include <iostream>
using namespace std;
#include<thread>
#include<numeric>
#include<functional>
//////////////////////////////////////////////////////////////////////////
//静态变量
//#define ASSERT(f)    ((void)0)
//#define VERIFY(f)          ((void)(f))
////索引变量
const BYTE cbIndexCount=5;
static int tableRe[100] = { 0 };
//扑克数据
const BYTE	CGameLogicNew::m_cbCardData[FULL_COUNT]=
{
	//0x01,     0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	//0x11,     0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	//0x21,     0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	//     0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
	0x01, 0x02, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,	//方块 A - K
	0x01, 0x02, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,	//梅花 A - K
	0x01, 0x02, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,	//红桃 A - K
	0x01, 0x02, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,	//黑桃 A - K
	14, 15
};


//////////////////////////////////////////////////////////////////////////

void CGameLogicNew::SetDiscardCard(BYTE cbCardData[], BYTE cbCardCount, BYTE cbOtherCardData[], BYTE cbOtherCardCount)
{
	ZeroMemory(m_cbDiscardCard, sizeof(m_cbDiscardCard));
	CopyMemory(m_cbDiscardCard , cbCardData, cbCardCount);
	m_cbDiscardCardCount = cbCardCount;
	ZeroMemory(m_cbOtherDiscardCard, sizeof(m_cbOtherDiscardCard));
	CopyMemory(m_cbOtherDiscardCard, cbOtherCardData, cbOtherCardCount);
	m_cbOtherDiscardCount = cbOtherCardCount;
	int count = cbCardCount;
	for (int i = 0; i < cbOtherCardCount; i++)
	{
		m_cbDiscardCard[count++] = cbOtherCardData[i];
	}
	m_cbDiscardCardCount += cbOtherCardCount;
	int a=4;
}

void CGameLogicNew::ReSetData()
{
	ZeroMemory(m_cbDiscardCard, sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbOtherDiscardCard, sizeof(m_cbOtherDiscardCard));
	ZeroMemory(m_cbUserCardCount, sizeof(m_cbUserCardCount));
	ZeroMemory(m_cbMaxCard, sizeof(m_cbMaxCard));
	ZeroMemory(m_cbCardDataEx, sizeof(m_cbCardDataEx));
	ZeroMemory(m_cbDiscardCard, sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbOtherDiscardCard, sizeof(m_cbOtherDiscardCard));
	ZeroMemory(m_cbAllCardData, sizeof(m_cbAllCardData));
	m_cbOtherDiscardCount = 0;
	m_cbDiscardCardCount = 0;
	m_cbFirstCard = 0;
	m_bHavePass = false;
	
}

//构造函数
CGameLogicNew::CGameLogicNew()
{
	ReSetData();
}

//析构函数
CGameLogicNew::~CGameLogicNew()
{
}



//获取类型
BYTE CGameLogicNew::GetCardType(const BYTE cbCardData[], BYTE cbCardCount)
{
	//构造扑克
	BYTE cbTempCardData[FULL_COUNT] = { 0 };
	CopyMemory(cbTempCardData, cbCardData, sizeof(BYTE)*cbCardCount);
	//大小排序
	SortCardList(cbTempCardData, cbCardCount, ST_ORDER);
	//简单牌型
	switch (cbCardCount)
	{
	case 0:	//空牌
		{
			return CT_ERROR;
		}
	case 1: //单牌
		{
			return CT_SINGLE;
		}
	case 2:	//对牌火箭
		{
			//牌型判断
		if ((cbTempCardData[0] == 15) && (cbTempCardData[1] == 14)) return CT_MISSILE_CARD;
		if (GetCardLogicValue(cbTempCardData[0]) == GetCardLogicValue(cbTempCardData[1])) return CT_DOUBLE;
			
			return CT_ERROR;
		}
	}

	//分析扑克
	tagAnalyseResultNew AnalyseResult;
	AnalysebCardData(cbTempCardData, cbCardCount, AnalyseResult);

	//四牌判断
	if (AnalyseResult.cbBlockCount[3]>0)
	{
		//牌型判断
		if ((AnalyseResult.cbBlockCount[3]==1)&&(cbCardCount==4)) return CT_BOMB_CARD;
		if ((AnalyseResult.cbBlockCount[3]==1)&&(cbCardCount==6)) return CT_FOUR_TAKE_ONE;
		//if ((AnalyseResult.cbBlockCount[3] == 1) && (cbCardCount == 7)) return CT_FOUR_TAKE_THREE;
		if ((AnalyseResult.cbBlockCount[3]==1)&&(cbCardCount==8)&&(AnalyseResult.cbBlockCount[1]==2)) return CT_FOUR_TAKE_TWO;
		if ((AnalyseResult.cbBlockCount[3] == 1) && (cbCardCount == 5)) return CT_THREE_TAKE_TWO;
		return CT_ERROR;
	}

	//三牌判断
	if (AnalyseResult.cbBlockCount[2]>0)
	{
		//连牌判断
		if (AnalyseResult.cbBlockCount[2]>1)
		{
			//变量定义
			BYTE cbCardData=AnalyseResult.cbCardData[2][0];
			BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

			//错误过虑
			if (cbFirstLogicValue>=15) return CT_ERROR;
			if (G_THREE_TAKE_TWO_EX)
			{
				 //连牌判断
				tagSearchCardResult tmpSearchCardResult = {};
				SearchLineCardType(cbTempCardData, cbCardCount, 0, 3, (cbCardCount / 5), &tmpSearchCardResult);
				if ((tmpSearchCardResult.cbCardCount[0] * 5 / 3) == cbCardCount )
					return CT_THREE_TAKE_TWO;
			}
				
			//连牌判断
			for (BYTE i=1;i<AnalyseResult.cbBlockCount[2];i++)
			{
				BYTE cbCardData=AnalyseResult.cbCardData[2][i*3];
				if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) return CT_ERROR;
			}
		}
		else if( cbCardCount == 3 ) return CT_THREE;

		//牌形判断
		if (AnalyseResult.cbBlockCount[2]*3==cbCardCount) return CT_THREE_LINE;
		if (AnalyseResult.cbBlockCount[2]*4==cbCardCount) return CT_THREE_TAKE_ONE;
		if ((AnalyseResult.cbBlockCount[2]*5==cbCardCount)&&(AnalyseResult.cbBlockCount[1]==AnalyseResult.cbBlockCount[2])) return CT_THREE_TAKE_TWO;
		if (G_THREE_TAKE_TWO_DAN) 
		{
			if
				(((AnalyseResult.cbBlockCount[2] * 5) == cbCardCount &&
				((AnalyseResult.cbBlockCount[0]) / 2 + AnalyseResult.cbBlockCount[1]) ==
				AnalyseResult.cbBlockCount[2]))
				{
					return CT_THREE_TAKE_TWO;

			}
		}
			
		return CT_ERROR;
	}

	//两张类型
	if (AnalyseResult.cbBlockCount[1]>=3)
	{
		//变量定义
		BYTE cbCardData=AnalyseResult.cbCardData[1][0];
		BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

		//错误过虑
		if (cbFirstLogicValue>=15) return CT_ERROR;

		//连牌判断
		for (BYTE i=1;i<AnalyseResult.cbBlockCount[1];i++)
		{
			BYTE cbCardData=AnalyseResult.cbCardData[1][i*2];
			if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) return CT_ERROR;
		}

		//二连判断
		if ((AnalyseResult.cbBlockCount[1]*2)==cbCardCount) return CT_DOUBLE_LINE;

		return CT_ERROR;
	}

	//单张判断
	if ((AnalyseResult.cbBlockCount[0]>=5)&&(AnalyseResult.cbBlockCount[0]==cbCardCount))
	{
		//变量定义
		BYTE cbCardData=AnalyseResult.cbCardData[0][0];
		BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

		//错误过虑
		if (cbFirstLogicValue>=15) return CT_ERROR;

		//连牌判断
		for (BYTE i=1;i<AnalyseResult.cbBlockCount[0];i++)
		{
			BYTE cbCardData=AnalyseResult.cbCardData[0][i];
			if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) return CT_ERROR;
		}

		return CT_SINGLE_LINE;
	}

	return CT_ERROR;
}

//排列扑克
VOID CGameLogicNew::SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType)
{
	//数目过虑
	if (cbCardCount==0) return;
	if (cbSortType==ST_CUSTOM) return;

	//转换数值
	BYTE cbSortValue[FULL_COUNT];
	for (BYTE i=0;i<cbCardCount;i++) cbSortValue[i]=GetCardLogicValue(cbCardData[i]);	

	//排序操作
	bool bSorted=true;
	BYTE cbSwitchData=0,cbLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<cbLast;i++)
		{
			bool bEnable = false;
			if (cbSortType == ST_ORDER)
			{
				bEnable = cbSortValue[i]<cbSortValue[i + 1];
			}
			else
			{
				bEnable = cbSortValue[i]>cbSortValue[i + 1];
			}
			if ((bEnable) ||
				((cbSortValue[i] == cbSortValue[i + 1]) && (cbCardData[i] < cbCardData[i + 1])))
			{
				//设置标志
				bSorted=false;

				//扑克数据
				cbSwitchData=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbSwitchData;

				//排序权位
				cbSwitchData=cbSortValue[i];
				cbSortValue[i]=cbSortValue[i+1];
				cbSortValue[i+1]=cbSwitchData;
			}	
		}
		cbLast--;
	} while(bSorted==false);

	//数目排序
	if (cbSortType==ST_COUNT)
	{
		//变量定义
		BYTE cbCardIndex=0;

		//分析扑克
		tagAnalyseResultNew AnalyseResult;
		AnalysebCardData(&cbCardData[cbCardIndex],cbCardCount-cbCardIndex,AnalyseResult);

		//提取扑克
		for (BYTE i=0;i<CountArray(AnalyseResult.cbBlockCount);i++)
		{
			//拷贝扑克
			BYTE cbIndex=CountArray(AnalyseResult.cbBlockCount)-i-1;
			CopyMemory(&cbCardData[cbCardIndex],AnalyseResult.cbCardData[cbIndex],AnalyseResult.cbBlockCount[cbIndex]*(cbIndex+1)*sizeof(BYTE));

			//设置索引
			cbCardIndex+=AnalyseResult.cbBlockCount[cbIndex]*(cbIndex+1)*sizeof(BYTE);
		}
	}

	return;
}

//混乱扑克
VOID CGameLogicNew::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//混乱准备
	BYTE cbCardData[CountArray(m_cbCardData)];
	CopyMemory(cbCardData,m_cbCardData,sizeof(m_cbCardData));

	//混乱扑克
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(cbBufferCount-cbRandCount);
		cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
		cbCardData[cbPosition]=cbCardData[cbBufferCount-cbRandCount];
	} while (cbRandCount<cbBufferCount);

	return;
}

int CGameLogicNew::GetACardCount(const BYTE cbCardBuffer[], const BYTE cbBufferCount, BYTE cbCard)
{
	int count = 0;
	for (int i = 0; i < cbBufferCount; i++)
	{
		if (GetCardLogicValue(cbCardBuffer[i]) == GetCardLogicValue(cbCard))
		{
			count++;
		}
	}
	return count;
}

//删除扑克
bool CGameLogicNew::RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount)
{
	//检验数据
	ASSERT(cbRemoveCount<=cbCardCount);

	//定义变量
	BYTE cbDeleteCount = 0, cbTempCardData[FULL_COUNT] = { 0 };
	if (cbCardCount>CountArray(cbTempCardData)) return false;
	CopyMemory(cbTempCardData,cbCardData,cbCardCount*sizeof(cbCardData[0]));

	//置零扑克
	for (BYTE i=0;i<cbRemoveCount;i++)
	{
		for (BYTE j=0;j<cbCardCount;j++)
		{
			if (cbRemoveCard[i]==cbTempCardData[j])
			{
				cbDeleteCount++;
				cbTempCardData[j]=0;
				break;
			}
		}
	}
	if (cbDeleteCount!=cbRemoveCount) return false;

	//清理扑克
	BYTE cbCardPos=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbTempCardData[i]!=0) cbCardData[cbCardPos++]=cbTempCardData[i];
	}
	for (int i = cbCardPos; i < cbCardCount; i++)
	{
		cbCardData[cbCardPos++] = 0;
	}

	return true;
}

int CGameLogicNew::ClearReLogicValue(BYTE cbCardData[], BYTE cbCardCount)
{
	SortCardList(cbCardData, cbCardCount, ST_ORDER);
	BYTE tmpCardData[FULL_COUNT] = { 0 };
	int  count = 0;
	tmpCardData[count++] = cbCardData[0];
	for (int i = 1; i < cbCardCount;i++)
	{
		if (GetCardLogicValue(cbCardData[i]) != GetCardLogicValue(tmpCardData[count-1]))
		{
			tmpCardData[count++] = cbCardData[i];
		}
	}
	for (int i = count; i < cbCardCount; i++)
	{
		cbCardData[i] = 0;
	}
	CopyMemory(cbCardData, tmpCardData, count);
	return count;
}

//有效判断
bool CGameLogicNew::IsValidCard(BYTE cbCardData)
{
	//获取属性
	BYTE cbCardColor=GetCardColor(cbCardData);
	BYTE cbCardValue=GetCardValue(cbCardData);

	//有效判断
	if ((cbCardData==14)||(cbCardData==15)) return true;
	if ((cbCardColor<=0x30)&&(cbCardValue>=0x01)&&(cbCardValue<=0x0D)) return true;

	return false;
}

//逻辑数值
BYTE CGameLogicNew::GetCardLogicValue(BYTE cbCardData)
{
	//扑克属性
	BYTE cbCardColor=GetCardColor(cbCardData);
	BYTE cbCardValue=GetCardValue(cbCardData);

	//转换数值
	if (cbCardValue == 15 || cbCardValue == 14) return cbCardValue + 2;
	return (cbCardValue<=2)?(cbCardValue+13):cbCardValue;
}

//对比扑克
bool CGameLogicNew::CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount)
{
	//获取类型
	BYTE cbNextType=GetCardType(cbNextCard,cbNextCount);
	BYTE cbFirstType=GetCardType(cbFirstCard,cbFirstCount);

	//类型判断
	if (cbNextType==CT_ERROR) return false;
	if (cbNextType==CT_MISSILE_CARD) return true;

	//炸弹判断
	if ((cbFirstType!=CT_BOMB_CARD)&&(cbNextType==CT_BOMB_CARD)) return true;
	if ((cbFirstType==CT_BOMB_CARD)&&(cbNextType!=CT_BOMB_CARD)) return false;

	//规则判断
	if ((cbFirstType!=cbNextType)||(cbFirstCount!=cbNextCount)) return false;

	//开始对比
	switch (cbNextType)
	{
	case CT_SINGLE:
	case CT_DOUBLE:
	case CT_THREE:
	case CT_SINGLE_LINE:
	case CT_DOUBLE_LINE:
	case CT_THREE_LINE:
	case CT_BOMB_CARD:
		{
			//获取数值
			BYTE cbNextLogicValue=GetCardLogicValue(cbNextCard[0]);
			BYTE cbFirstLogicValue=GetCardLogicValue(cbFirstCard[0]);

			//对比扑克
			return cbNextLogicValue>cbFirstLogicValue;
		}
	case CT_THREE_TAKE_ONE:
	case CT_THREE_TAKE_TWO:
		{
			//分析扑克
			tagAnalyseResultNew NextResult;
			tagAnalyseResultNew FirstResult;
			AnalysebCardData(cbNextCard,cbNextCount,NextResult);
			AnalysebCardData(cbFirstCard,cbFirstCount,FirstResult);

			//获取数值
			BYTE cbNextLogicValue=GetCardLogicValue(NextResult.cbCardData[2][0]);
			BYTE cbFirstLogicValue=GetCardLogicValue(FirstResult.cbCardData[2][0]);

			//对比扑克
			return cbNextLogicValue>cbFirstLogicValue;
		}
	case CT_FOUR_TAKE_ONE:
	case CT_FOUR_TAKE_TWO:
	//case CT_FOUR_TAKE_THREE:
		{
			//分析扑克
			tagAnalyseResultNew NextResult;
			tagAnalyseResultNew FirstResult;
			AnalysebCardData(cbNextCard,cbNextCount,NextResult);
			AnalysebCardData(cbFirstCard,cbFirstCount,FirstResult);

			//获取数值
			BYTE cbNextLogicValue=GetCardLogicValue(NextResult.cbCardData[3][0]);
			BYTE cbFirstLogicValue=GetCardLogicValue(FirstResult.cbCardData[3][0]);

			//对比扑克
			return cbNextLogicValue>cbFirstLogicValue;
		}
	}
	
	return false;
}

//出牌搜索
BYTE CGameLogicNew::SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagSearchCardResult *pSearchCardResult, bool bNoSearchBomb)
{
	//设置结果
	ASSERT(pSearchCardResult != NULL);
	if (pSearchCardResult == NULL) return 0;

	ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));

	//变量定义
	BYTE cbResultCount = 0;
	tagSearchCardResult tmpSearchCardResult = {};

	//构造扑克
	BYTE cbCardData[FULL_COUNT];
	BYTE cbCardCount = cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//排列扑克
	SortCardList(cbCardData, cbCardCount, ST_ORDER);

	//获取类型
	BYTE cbTurnOutType = GetCardType(cbTurnCardData, cbTurnCardCount);

	//出牌分析
	switch (cbTurnOutType)
	{
	case CT_ERROR:					//错误类型
	{
		//提取各种牌型一组
		ASSERT(pSearchCardResult);
		if (!pSearchCardResult) return 0;

		//是否一手出完
		int tmpType = GetCardType(cbCardData, cbCardCount);
		if ((tmpType != CT_ERROR && (tmpType != CT_THREE_TAKE_ONE&&tmpType != CT_FOUR_TAKE_ONE)) || (tmpType == CT_THREE_TAKE_ONE&&cbCardCount == 4))
		{
			pSearchCardResult->cbCardCount[cbResultCount] = cbCardCount;
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], cbCardData,
				sizeof(BYTE)*cbCardCount);
			cbResultCount++;
		}

		//如果最小牌不是单牌，则提取
		BYTE cbSameCount = 0;
		if (cbCardCount > 1 && GetCardValue(cbCardData[cbCardCount - 1]) == GetCardValue(cbCardData[cbCardCount - 2]))
		{
			cbSameCount = 1;
			pSearchCardResult->cbResultCard[cbResultCount][0] = cbCardData[cbCardCount - 1];
			BYTE cbCardValue = GetCardValue(cbCardData[cbCardCount - 1]);
			for (int i = cbCardCount - 2; i >= 0; i--)
			{
				if (GetCardValue(cbCardData[i]) == cbCardValue)
				{
					pSearchCardResult->cbResultCard[cbResultCount][cbSameCount++] = cbCardData[i];
				}
				else break;
			}

			pSearchCardResult->cbCardCount[cbResultCount] = cbSameCount;
			cbResultCount++;
		}

		//单牌
		BYTE cbTmpCount = 0;
		if (cbSameCount != 1)
		{
			cbTmpCount = SearchSameCard(cbCardData, cbCardCount, 0, 1, &tmpSearchCardResult);
			if (cbTmpCount > 0)
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0]);
				cbResultCount++;
			}
		}

		//对牌
		if (cbSameCount != 2)
		{
			cbTmpCount = SearchSameCard(cbCardData, cbCardCount, 0, 2, &tmpSearchCardResult);
			if (cbTmpCount > 0)
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0]);
				cbResultCount++;
			}
		}

		//三条
		if (cbSameCount != 3)
		{
			cbTmpCount = SearchSameCard(cbCardData, cbCardCount, 0, 3, &tmpSearchCardResult);
			if (cbTmpCount > 0)
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0]);
				cbResultCount++;
			}
		}

		//三带一单
			cbTmpCount = SearchTakeCardType(cbCardData, cbCardCount, 0, 3, 1, &tmpSearchCardResult);
		if (cbTmpCount > 0)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[0],
				sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0]);
			cbResultCount++;
		}

		//三带一对
		cbTmpCount = SearchTakeCardType(cbCardData, cbCardCount, 0, 3, 2, &tmpSearchCardResult);
		if (cbTmpCount > 0)
		{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0]);
				cbResultCount++;
			
		}

		//单连
		cbTmpCount = SearchLineCardType(cbCardData, cbCardCount, 0, 1, 0, &tmpSearchCardResult);
		if (cbTmpCount > 0)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[0],
				sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0]);
			cbResultCount++;
		}

		//连对
		cbTmpCount = SearchLineCardType(cbCardData, cbCardCount, 0, 2, 0, &tmpSearchCardResult);
		if (cbTmpCount > 0)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[0],
				sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0]);
			cbResultCount++;
		}

		//三连
		cbTmpCount = SearchLineCardType(cbCardData, cbCardCount, 0, 3, 0, &tmpSearchCardResult);
		if (cbTmpCount > 0)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[0],
				sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0]);
			cbResultCount++;
		}

		////飞机
		for (int i = 2; i <= 5;i++)
		{
			cbTmpCount = SearchThreeTwoLine(cbCardData, cbCardCount, 0, i, 1, &tmpSearchCardResult);
			if (cbTmpCount > 0)
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0]);
				cbResultCount++;
			}
			else
			{
				break;
			}
		}
		for (int i = 2; i < 5; i++)
		{
			cbTmpCount = SearchThreeTwoLine(cbCardData, cbCardCount, 0, i, 2, &tmpSearchCardResult);
			if (cbTmpCount > 0)
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0]);
				cbResultCount++;
			}
			else
			{
				break;
			}
		}
		//炸弹
		if (cbSameCount != 4)
		{
			cbTmpCount = SearchSameCard(cbCardData, cbCardCount, 0, 4, &tmpSearchCardResult);
			if (cbTmpCount > 0)
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0]);
				cbResultCount++;
			}
		}
		//四带1
		cbTmpCount = SearchTakeCardType(cbCardData, cbCardCount, 0, 4, 2, &tmpSearchCardResult);
		if (cbTmpCount > 0)
		{
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0]);
				cbResultCount++;
			}

		}
		//四带2
		cbTmpCount = SearchTakeCardType(cbCardData, cbCardCount, 0, 4, 4, &tmpSearchCardResult);
		if (cbTmpCount > 0)
		{
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0]);
				cbResultCount++;
			}

		}

		

		//搜索火箭
		if ((cbCardCount >= 2) && (cbCardData[0] == 15) && (cbCardData[1] == 14))
		{
			//设置结果
			pSearchCardResult->cbCardCount[cbResultCount] = 2;
			pSearchCardResult->cbResultCard[cbResultCount][0] = cbCardData[0];
			pSearchCardResult->cbResultCard[cbResultCount][1] = cbCardData[1];

			cbResultCount++;
		}

		pSearchCardResult->cbSearchCount = cbResultCount;
		return cbResultCount;
	}
	case CT_SINGLE:					//单牌类型
	case CT_DOUBLE:					//对牌类型
	case CT_THREE:					//三条类型
	{
		//变量定义
		BYTE cbReferCard = cbTurnCardData[0];
		BYTE cbSameCount = 1;
		if (cbTurnOutType == CT_DOUBLE) cbSameCount = 2;
		else if (cbTurnOutType == CT_THREE) cbSameCount = 3;

		//搜索相同牌
		cbResultCount = SearchSameCard(cbCardData, cbCardCount, cbReferCard, cbSameCount, pSearchCardResult);

		break;
	}
	case CT_SINGLE_LINE:		//单连类型
	case CT_DOUBLE_LINE:		//对连类型
	case CT_THREE_LINE:				//三连类型
	{
		//变量定义
		BYTE cbBlockCount = 1;
		if (cbTurnOutType == CT_DOUBLE_LINE) cbBlockCount = 2;
		else if (cbTurnOutType == CT_THREE_LINE) cbBlockCount = 3;

		BYTE cbLineCount = cbTurnCardCount / cbBlockCount;

		//搜索边牌
		cbResultCount = SearchLineCardType(cbCardData, cbCardCount, cbTurnCardData[0], cbBlockCount, cbLineCount, pSearchCardResult);

		break;
	}
	case CT_THREE_TAKE_ONE:	//三带一单
	case CT_THREE_TAKE_TWO:	//三带一对
	{
		//效验牌数
		if (cbCardCount < cbTurnCardCount) break;
		BYTE cbTakeCardCount = cbTurnOutType == CT_THREE_TAKE_ONE ? 1 : 2;
		//如果是三带一或三带二
		if (cbTurnCardCount == 4 || cbTurnCardCount == 5)
		{
			//搜索三带牌型
			cbResultCount = SearchTakeCardType(cbCardData, cbCardCount, cbTurnCardData[2], 3, cbTakeCardCount, pSearchCardResult);
		}
		else
		{
			BYTE cbLineCount = 2;
			if (cbTurnOutType == CT_THREE_TAKE_ONE)
			{
				cbLineCount = cbTurnCardCount / 4;
			}
			else{
				cbLineCount = cbTurnCardCount / 5;
			}

			cbResultCount = SearchThreeTwoLine(cbCardData, cbCardCount, cbTurnCardData[2], cbLineCount, cbTakeCardCount, pSearchCardResult);
		}

		break;
	}
	case CT_FOUR_TAKE_ONE:		//四带两单
	case CT_FOUR_TAKE_TWO:		//四带两双
	{
		BYTE cbTakeCount = cbTurnOutType == CT_FOUR_TAKE_ONE ? 1 : 2;

		BYTE cbTmpTurnCard[MAX_COUNT] = {};
		CopyMemory(cbTmpTurnCard, cbTurnCardData, sizeof(BYTE)*cbTurnCardCount);
		SortOutCardList(cbTmpTurnCard, cbTurnCardCount);

		//搜索带牌
		cbResultCount = SearchTakeCardType(cbCardData, cbCardCount, cbTmpTurnCard[0], 4, cbTakeCount, pSearchCardResult);

		break;
	}
	}
	if (bNoSearchBomb==false)
	{
		//搜索炸弹
		if ((cbCardCount >= 4) && (cbTurnOutType != CT_MISSILE_CARD))
		{
			//变量定义
			BYTE cbReferCard = 0;
			if (cbTurnOutType == CT_BOMB_CARD) cbReferCard = cbTurnCardData[0];

			//搜索炸弹
			BYTE cbTmpResultCount = SearchSameCard(cbCardData, cbCardCount, cbReferCard, 4, &tmpSearchCardResult);
			for (BYTE i = 0; i < cbTmpResultCount; i++)
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i]);
				cbResultCount++;
			}
		}

		//搜索火箭
		if (cbTurnOutType != CT_MISSILE_CARD && (cbCardCount >= 2) && (cbCardData[0] == 15) && (cbCardData[1] == 14))
		{
			//设置结果
			pSearchCardResult->cbCardCount[cbResultCount] = 2;
			pSearchCardResult->cbResultCard[cbResultCount][0] = cbCardData[0];
			pSearchCardResult->cbResultCard[cbResultCount][1] = cbCardData[1];

			cbResultCount++;
		}
	}
	

	pSearchCardResult->cbSearchCount = cbResultCount;
	return cbResultCount;
}



float CGameLogicNew::GetHandScore(vector<tagOutCardResultNew> &CardTypeResult, int minTypeCount,bool bJiaoFen)
{
	float score = 0;
	int cbHandCardCount = 0;
	int typeCount[CT_TYPE_COUNT] = { 0 };
	int shunIndex = 0;
	bool bHave2 = false;
	int bHaveWang = 0;
	bool bHaveBoomb = false;
	vector<int> vecShunZiCount;
	for (int i = 0; i < CardTypeResult.size(); i++)
	{
		if (CardTypeResult[i].cbCardType == CT_SINGLE)
		{
			if (GetCardLogicValue(CardTypeResult[i].cbResultCard[0]) == 17 && bJiaoFen)
			{
				bHaveWang = 1;
				score += 10;
			}
		}
		else if (CardTypeResult[i].cbCardType >= CT_FOUR_TAKE_ONE)
		{
			bHaveBoomb = true;
		}
	}
	for (int i = 0; i < CardTypeResult.size(); i++)
	{
		float tempScore = GetCardTypeScore(CardTypeResult[i]);
		CardTypeResult[i].cbCardScore = tempScore;
		score += tempScore;
		typeCount[CardTypeResult[i].cbCardType]++;
		if (CardTypeResult[i].cbCardType==CT_SINGLE_LINE)
		{
			shunIndex = i;
			vecShunZiCount.push_back(CardTypeResult[i].cbCardCount);
		}
		else if (CardTypeResult[i].cbCardType == CT_SINGLE)
		{
			if (GetCardLogicValue(CardTypeResult[i].cbResultCard[0]) == 15 && bJiaoFen)
			{
				score += 5;
				if (bHaveWang==1)
				{
					score += 3;
				}
				else if (bHaveWang == 2)
				{
					score += 5;
				}
				bHave2 = true;
			}
		}
		if (CardTypeResult[i].cbCardType == CT_MISSILE_CARD)
		{   
				score += 50;
				bHaveWang = 2;
		}
		else if (CardTypeResult[i].cbCardType == CT_DOUBLE)
		{   //为了解决压对12,12，手上牌1,1,13,13,12，12，8，6，6上对1还是上对13的问题
			if (bJiaoFen)
			{
				if (GetCardLogicValue(CardTypeResult[i].cbResultCard[0]) == 14)
				{
					score += 5;
				}
				else if (GetCardLogicValue(CardTypeResult[i].cbResultCard[0]) == 15)
				{
					score += 7;
					if (bHaveWang == 2 )
					{
						score += 20;
					}
					else if (bHaveWang == 2 )
					{
						score += 10;
					}
				}
			}
			
		}
		else if (CardTypeResult[i].cbCardType == CT_THREE || CardTypeResult[i].cbCardType == CT_THREE_TAKE_ONE || CardTypeResult[i].cbCardType == CT_THREE_TAKE_TWO)
		{   //为了解决压对12,12，手上牌1,1,13,13,12，12，8，6，6上对1还是上对13的问题
			if (bJiaoFen)
			{
				if (GetCardLogicValue(CardTypeResult[i].cbResultCard[0]) == 14)
				{
					score += 8;
				}
				else if (GetCardLogicValue(CardTypeResult[i].cbResultCard[0]) == 15)
				{
					score += 10;
					if (bHaveWang == 2 )
					{
						score += 30;
					}
					else if (bHaveWang == 2 )
					{
						score += 20;
					}
				}
			}
			
		}
		else if (m_bTrunMode&& CardTypeResult[i].cbCardType == CT_SINGLE)
		{   
			if (IsJueDuiDaPai(CardTypeResult[i].cbResultCard, CardTypeResult[i].cbCardCount, CardTypeResult[i].cbCardType) == JUE_DUI_DA)
			{
				score += 10;
			}
		}
	}
	if (CardTypeResult.size() == 4 && typeCount[CT_SINGLE] == 2 && typeCount[CT_SINGLE_LINE]==1)
	{
		
		for (int i = 0; i < CardTypeResult.size(); i++)
		{
			if (CardTypeResult[i].cbCardType == CT_SINGLE)
			{
				if (IsJueDuiDaPai(CardTypeResult[i].cbResultCard, CardTypeResult[i].cbCardCount, CardTypeResult[i].cbCardType) == JUE_DUI_DA)
				{
					score += 10;
					if (CardTypeResult[shunIndex].cbCardCount >= 7)
					{
						score += (CardTypeResult[shunIndex].cbCardCount-7)*5;
					}
					break;
				}
			}

		}
	
	}
	if (CardTypeResult.size() == 4 && typeCount[CT_DOUBLE] == 2 && typeCount[CT_SINGLE_LINE] == 1)
	{

		for (int i = 0; i < CardTypeResult.size(); i++)
		{
			if (CardTypeResult[i].cbCardType == CT_DOUBLE)
			{
				if (IsJueDuiDaPai(CardTypeResult[i].cbResultCard, CardTypeResult[i].cbCardCount, CardTypeResult[i].cbCardType) == JUE_DUI_DA)
				{
					score += 10;
					if (CardTypeResult[shunIndex].cbCardCount >= 7)
					{
						score += (CardTypeResult[shunIndex].cbCardCount - 7) * 5;
					}
					break;
				}
			}

		}

	}
	if (typeCount[CT_SINGLE] <= 1 && (typeCount[CT_DOUBLE_LINE] + typeCount[CT_SINGLE] + typeCount[CT_DOUBLE]) == CardTypeResult.size())
	{

		for (int i = 0; i < CardTypeResult.size(); i++)
		{
			if (CardTypeResult[i].cbCardType == CT_DOUBLE_LINE)
			{
				if (IsJueDuiDaPai(CardTypeResult[i].cbResultCard, 2, CT_DOUBLE)==JUE_DUI_DA)
				{
					score += 7;
				}
			}

		}

	}

	if (CardTypeResult.size() == 2 && typeCount[CT_THREE_TAKE_TWO] == 1 && typeCount[CT_DOUBLE] == 1)
	{
		// 1, 13, 8, 8, 8, 4, 4,
		for (int i = 0; i < CardTypeResult.size(); i++)
		{
			if (CardTypeResult[i].cbCardType == CT_THREE_TAKE_TWO)
			{
				if (CardTypeResult[i].cbCardCount==5)
				{
					if (GetCardLogicValue(CardTypeResult[i].cbResultCard[4]) != GetCardLogicValue(CardTypeResult[i].cbResultCard[3]))
					{
						score += 7;
						break;
					}
					else{
						if (GetCardLogicValue(CardTypeResult[i].cbResultCard[4]) == GetCardLogicValue(CardTypeResult[i].cbResultCard[3]))
						{
							//三拖牌大过连对，优先三拖
							if (GetCardLogicValue(CardTypeResult[i].cbResultCard[0])>GetCardLogicValue(CardTypeResult[1].cbResultCard[0]) && (GetCardLogicValue(CardTypeResult[i].cbResultCard[4]) + 1) == GetCardLogicValue(CardTypeResult[1].cbResultCard[0]))
							{
								score += 7;
								break;
							}
							
						}
					}
				}
			}

		}

	}
	if (vecShunZiCount.size() == 2 && vecShunZiCount[0] == vecShunZiCount[1])
	{
		score += 2;
	}
	if (bJiaoFen&&bHave2&&bHaveBoomb==false)
	{
		score -= 30;
	}
	
	//如果最小牌型数小于10的话,应该适当加分
	score = score - CardTypeResult.size() * 10;
	
	return score;
}

float  CGameLogicNew::GetCardTypeScore(tagOutCardResultNew& CardTypeResult)
{
	//-- 2. maxCard：牌型大小的牌值，单张和对子就是自身，顺子和连对是最大那张牌，三带是有三张的那个牌，这个很容易理解，相同的牌型比较大小，就是比较maxCard
	BYTE cbCardData[MAX_COUNT] = { 0 };
	int  cbCardCount = CardTypeResult.cbCardCount;
	CopyMemory(cbCardData, CardTypeResult.cbResultCard, cbCardCount);
	int type = GetCardType(cbCardData, cbCardCount);

	if (type == CT_ERROR)
	{
		return 0;
	}
	else if (type == CT_SINGLE)
	{
		return  (float)(GetCardLogicValue(cbCardData[0]) - 10);
	}
	else if (type == CT_DOUBLE)
	{
		float score = (float)GetCardLogicValue(cbCardData[0]) / 100;
		float tempscore = (float)(GetCardLogicValue(cbCardData[0]) - 6);
		return score+tempscore ;
	}
	else if (type == CT_THREE)
	{
		return ((float)GetCardLogicValue(cbCardData[0]) - 3 + 1) / 2 ;
	}
	else if (type == CT_SINGLE_LINE)
	{
		float tempscore = (float)GetCardLogicValue(cbCardData[0]) - 10 + 1;
		return  tempscore + ((float)(cbCardCount - 5) * 0.6f);
	}
	else if (type == CT_DOUBLE_LINE)
	{
		float tempscore = (float)GetCardLogicValue(cbCardData[0]) - 10 + 1;
		return  tempscore;
	}
	else if (type == CT_THREE_LINE)
	{
		return (((float)GetCardLogicValue(cbCardData[0]) - 3 + 7) / 2) + (float)((cbCardCount - 3) * 1);
	}
	else if (type == CT_THREE_TAKE_TWO || type == CT_THREE_TAKE_ONE)
	{
		if (cbCardCount <= 5)
		{
			int cardCount = 0;
			if (type == CT_THREE_TAKE_ONE)
				cardCount = cbCardCount / 4;
			else
				cardCount = (cbCardCount / 5) * 2;

			float score = 0;
			vector<int> TwoCardLogic;
			for (int i = 0; i < cardCount; i++)
			{
				float logic = GetCardLogicValue(cbCardData[cbCardCount - i - 1]);
				float tempscore = 0.0f;
			/*	if (m_bTrunMode && (GetCardLogicValue(cbCardData[cbCardCount - i - 1])>11))
				{
					tempscore = (float)(GetCardLogicValue(cbCardData[cbCardCount - i - 1]) - 10);
				}*/
				score = score + tempscore + (float)(logic / 100);
				TwoCardLogic.push_back(logic);
			}
			if (TwoCardLogic.size()>=2 && TwoCardLogic[1] == TwoCardLogic[0])
			{
				score = score + 0.12f;
			}

			return ((float)GetCardLogicValue(cbCardData[0]) - 3 + 1) / 2 +  score;
		}
		else
		{
			int cardCount = 0;
			if (type == CT_THREE_TAKE_ONE)
				cardCount = cbCardCount / 4;
			else
				cardCount = (cbCardCount / 5) * 2;

			float score = 0;
			vector<int> TwoCardLogic;
			for (int i = 0; i < cardCount; i++)
			{
				float logic = GetCardLogicValue(cbCardData[cbCardCount - i - 1]);
				score = score + (float)(logic / 100);
				TwoCardLogic.push_back(logic);
			}
			if (TwoCardLogic.size() >= 2&&TwoCardLogic[1] == TwoCardLogic[0])
			{
				score = score + 0.12f;
			}

			return ((float)GetCardLogicValue(cbCardData[0]) - 3 + 7) + 5 + score;
		}

	}

	else if (type == CT_BOMB_CARD )
	{
		return (float)GetCardLogicValue(cbCardData[0]) -3+7+9;

	}
	else if ( type == CT_MISSILE_CARD)
	{
		return (float)GetCardLogicValue(cbCardData[0]) + 7 + 20;

	}
	else if (type == CT_FOUR_TAKE_TWO )
	{
		int tempCardCount = 4;
		float score = 0;
		vector<int> TwoCardLogic;
		for (int i = 0; i < tempCardCount; i++)
		{
			float logic = GetCardLogicValue(cbCardData[cbCardCount - i - 1]);
			score = score + (float)(logic / 100);
			TwoCardLogic.push_back(logic);
		}
		if (TwoCardLogic[1] == TwoCardLogic[0])
		{
			score = score + 0.12f;
		}
		return ((float)GetCardLogicValue(cbCardData[0]) - 3 + 1) / 2 + 9+score;
	}
	else if (type == CT_FOUR_TAKE_ONE )
	{
		int tempCardCount =2;
		float score = 0;
		vector<int> TwoCardLogic;
		for (int i = 0; i < tempCardCount; i++)
		{
			float logic = GetCardLogicValue(cbCardData[cbCardCount - i - 1]);
			score = score + (float)(logic / 100);
			TwoCardLogic.push_back(logic);
		}
		if (TwoCardLogic[1] == TwoCardLogic[0])
		{
			score = score + 0.12f;
		}
		return((float)GetCardLogicValue(cbCardData[0]) - 3 + 1) / 2 + 7 + score;
	}

	return (float)GetCardLogicValue(cbCardData[0]) - 10;
}

int CGameLogicNew::RemoveTypeCard(tagOutCardTypeResultNew & CardTypeResult, BYTE * cbTmpHReminCardData, int &iLeftCardCount)
{
	for (int i = CardTypeResult.cbCardTypeCount-1; i >=0; i--)
	{
		if (RemoveCard(CardTypeResult.cbCardData[i], CardTypeResult.cbEachHandCardCount[i], cbTmpHReminCardData, iLeftCardCount))
		{
			iLeftCardCount -= CardTypeResult.cbEachHandCardCount[i];
		}
	}	
	return iLeftCardCount;
}

void CGameLogicNew::GetMissileCardResult(BYTE * cbHandCardData,BYTE cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	if (cbHandCardCount < 2)
	{
		return;
	}
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);
	if ((cbTmpCardData[0] == 15) && (cbTmpCardData[1] == 14))
	{
		BYTE Index = CardTypeResult[CT_MISSILE_CARD].cbCardTypeCount;
		CardTypeResult[CT_MISSILE_CARD].cbCardType = CT_MISSILE_CARD;
		CardTypeResult[CT_MISSILE_CARD].cbCardData[Index][0] = cbTmpCardData[0];
		CardTypeResult[CT_MISSILE_CARD].cbCardData[Index][1] = cbTmpCardData[1];
		CardTypeResult[CT_MISSILE_CARD].cbEachHandCardCount[Index] = 2;
		CardTypeResult[CT_MISSILE_CARD].cbCardTypeCount++;
	}
}

void CGameLogicNew::GetFourTakeOneResult(BYTE * cbHandCardData,  BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	if (cbHandCardCount < 6)
	{
		return;
	}
	//恢复扑克，防止分析时改变扑克
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

	tagSearchCardResult SameCardResult = {};
	BYTE cbSameCardResultCount = SearchSameCard(cbTmpCardData, cbHandCardCount, 0, 4, &SameCardResult);
	for (int i = 0; i < SameCardResult.cbSearchCount; i++)
	{
		//移除四条
		BYTE cbRemainCard[MAX_COUNT];
		BYTE cbRemainCardCount = cbHandCardCount - SameCardResult.cbCardCount[i];
		CopyMemory(cbRemainCard, cbTmpCardData, cbHandCardCount*sizeof(BYTE));
		RemoveCard(SameCardResult.cbResultCard[i], SameCardResult.cbCardCount[i], cbRemainCard, cbHandCardCount);
		//单牌组合
		BYTE cbComCard[MAX_COLS];
		BYTE cbComResCard[MAX_RESULT_COUNT][MAX_COLS];
		int cbComResLen = 0;
		//单牌组合
		Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCard, 2, cbRemainCardCount, 2);
		for (int j = 0; j < cbComResLen; ++j)
		{

			BYTE Index = CardTypeResult[CT_FOUR_TAKE_ONE].cbCardTypeCount;
			CardTypeResult[CT_FOUR_TAKE_ONE].cbCardType = CT_FOUR_TAKE_ONE;
			CopyMemory(CardTypeResult[CT_FOUR_TAKE_ONE].cbCardData[Index], SameCardResult.cbResultCard[i], SameCardResult.cbCardCount[i]);
			CardTypeResult[CT_FOUR_TAKE_ONE].cbCardData[Index][4] = cbComResCard[j][0];
			CardTypeResult[CT_FOUR_TAKE_ONE].cbCardData[Index][4 + 1] = cbComResCard[j][1];
			CardTypeResult[CT_FOUR_TAKE_ONE].cbEachHandCardCount[Index] = 6;
			CardTypeResult[CT_FOUR_TAKE_ONE].cbCardTypeCount++;

		}
	}
}

void CGameLogicNew::GetFourTakeTwoResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	if (cbHandCardCount < 8 || CardTypeResult[CT_DOUBLE].cbCardTypeCount<2)
	{
		return;
	}
	//恢复扑克，防止分析时改变扑克
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

	tagSearchCardResult SameCardResult = {};
	BYTE cbSameCardResultCount = SearchSameCard(cbTmpCardData, cbHandCardCount, 0, 4, &SameCardResult);
	for (int i = 0; i < SameCardResult.cbSearchCount; i++)
	{
		//移除四条
		BYTE cbRemainCard[MAX_COUNT];
		BYTE cbRemainCardCount = 0;
		for (int j = 0; j < CardTypeResult[CT_DOUBLE].cbCardTypeCount; j++)
		{
			if (SameCardResult.cbResultCard[i][0] != CardTypeResult[CT_DOUBLE].cbCardData[j][0])
			{
				cbRemainCard[cbRemainCardCount++] = CardTypeResult[CT_DOUBLE].cbCardData[j][0];
				cbRemainCard[cbRemainCardCount++] = CardTypeResult[CT_DOUBLE].cbCardData[j][1];
			}
		}
		SortCardList(cbRemainCard, cbRemainCardCount, ST_ORDER);
		BYTE Index;
		//单牌组合
		BYTE cbComCard[MAX_COLS];
		BYTE cbComResCard[MAX_RESULT_COUNT][MAX_COLS] = {};
		int cbNeedCardCount = 4;
		int cbComResLen = 0;

		//单牌组合
		Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCard, cbNeedCardCount, cbRemainCardCount, cbNeedCardCount);
		for (int j = 0; j < cbComResLen; ++j)
		{
			if (isAllDoubleType(cbComResCard[j], cbNeedCardCount) == false)
			{
				continue;
			}
			Index = CardTypeResult[CT_FOUR_TAKE_TWO].cbCardTypeCount;
			CardTypeResult[CT_FOUR_TAKE_TWO].cbCardType = CT_FOUR_TAKE_TWO;
			CopyMemory(CardTypeResult[CT_FOUR_TAKE_TWO].cbCardData[Index], SameCardResult.cbResultCard[i], SameCardResult.cbCardCount[i]);
			CardTypeResult[CT_FOUR_TAKE_TWO].cbCardData[Index][4] = cbComResCard[j][0];
			CardTypeResult[CT_FOUR_TAKE_TWO].cbCardData[Index][4 + 1] = cbComResCard[j][1];
			CardTypeResult[CT_FOUR_TAKE_TWO].cbCardData[Index][4 + 2] = cbComResCard[j][2];
			CardTypeResult[CT_FOUR_TAKE_TWO].cbCardData[Index][4 + 3] = cbComResCard[j][3];
			CardTypeResult[CT_FOUR_TAKE_TWO].cbEachHandCardCount[Index] = 8;
			CardTypeResult[CT_FOUR_TAKE_TWO].cbCardTypeCount++;

		}
	}
}

void CGameLogicNew::GetThreeResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	if (cbHandCardCount < 3)
	{
		return;
	}
	//恢复扑克，防止分析时改变扑克
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);
	//搜索同张
	tagSearchCardResult SameCardResult = {};
	BYTE cbSameCardResultCount = SearchSameCard(cbTmpCardData, cbHandCardCount, 0, 3, &SameCardResult);
	for (BYTE i = 0; i < SameCardResult.cbSearchCount; i++)
	{
		BYTE Index = CardTypeResult[CT_THREE].cbCardTypeCount;
		CardTypeResult[CT_THREE].cbCardType = CT_THREE;
		CopyMemory(CardTypeResult[CT_THREE].cbCardData[Index], SameCardResult.cbResultCard[i], SameCardResult.cbCardCount[i]);
		CardTypeResult[CT_THREE].cbEachHandCardCount[Index] = SameCardResult.cbCardCount[i];
		CardTypeResult[CT_THREE].cbCardTypeCount++;

	}
}

void CGameLogicNew::GetDoubleResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	if (cbHandCardCount < 2)
	{
		return;
	}
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);
	//搜索同张
	tagSearchCardResult SameCardResult = {};
	BYTE cbSameCardResultCount = SearchSameCard(cbTmpCardData, cbHandCardCount, 0, 2, &SameCardResult);
	for (BYTE i = 0; i < SameCardResult.cbSearchCount; i++)
	{
		BYTE Index = CardTypeResult[CT_DOUBLE].cbCardTypeCount;
		CardTypeResult[CT_DOUBLE].cbCardType = CT_DOUBLE;
		CopyMemory(CardTypeResult[CT_DOUBLE].cbCardData[Index], SameCardResult.cbResultCard[i], SameCardResult.cbCardCount[i]);
		CardTypeResult[CT_DOUBLE].cbEachHandCardCount[Index] = SameCardResult.cbCardCount[i];
		CardTypeResult[CT_DOUBLE].cbCardTypeCount++;

	}
}

void CGameLogicNew::GetSingleResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);
	for (BYTE i = 0; i < cbHandCardCount; ++i)
	{
		//if (GetACardCount(cbHandCardData,cbHandCardCount,cbHandCardData[i])==1)
		{
			BYTE Index = CardTypeResult[CT_SINGLE].cbCardTypeCount;
			CardTypeResult[CT_SINGLE].cbCardType = CT_SINGLE;
			CardTypeResult[CT_SINGLE].cbCardData[Index][0] = cbTmpCardData[i];
			CardTypeResult[CT_SINGLE].cbEachHandCardCount[Index] = 1;
			CardTypeResult[CT_SINGLE].cbCardTypeCount++;
		}
	}
}

void CGameLogicNew::GetBombCardResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	if (cbHandCardCount<4)
	{
		return;
	}
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);
	BYTE cbFourCardData[MAX_COUNT];
	BYTE cbFourCardCount = 0;
/*	if (cbHandCardCount >= 2 && 15 == cbTmpCardData[0] && 14 == cbTmpCardData[1])
	{
		BYTE Index = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount;
		CardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD;
		CardTypeResult[CT_BOMB_CARD].cbCardData[Index][0] = cbTmpCardData[0];
		CardTypeResult[CT_BOMB_CARD].cbCardData[Index][1] = cbTmpCardData[1];
		CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index] = 2;
		CardTypeResult[CT_BOMB_CARD].cbCardTypeCount++;
		GetAllBomCard(cbTmpCardData + 2, cbHandCardCount - 2, cbFourCardData, cbFourCardCount);
	}
	else*/ GetAllBomCard(cbTmpCardData, cbHandCardCount, cbFourCardData, cbFourCardCount);
	for (BYTE i = 0; i < cbFourCardCount; i += 4)
	{
		BYTE Index = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount;
		CardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD;
		CardTypeResult[CT_BOMB_CARD].cbCardData[Index][0] = cbFourCardData[i];
		CardTypeResult[CT_BOMB_CARD].cbCardData[Index][1] = cbFourCardData[i + 1];
		CardTypeResult[CT_BOMB_CARD].cbCardData[Index][2] = cbFourCardData[i + 2];
		CardTypeResult[CT_BOMB_CARD].cbCardData[Index][3] = cbFourCardData[i + 3];
		CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index] = 4;
		CardTypeResult[CT_BOMB_CARD].cbCardTypeCount++;

	}
}

void CGameLogicNew::GetThreeTakeTwoCardResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	if (cbHandCardCount < 5)
	{
		return;
	}
	//恢复扑克，防止分析时改变扑克
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

	for (int i = 0; i < CardTypeResult[CT_THREE].cbCardTypeCount; i++)
	{
		//移除四条
		BYTE cbRemainCard[MAX_COUNT];
		BYTE cbRemainCardCount = cbHandCardCount - CardTypeResult[CT_THREE].cbEachHandCardCount[i];
		CopyMemory(cbRemainCard, cbTmpCardData, cbHandCardCount*sizeof(BYTE));
		RemoveCard(CardTypeResult[CT_THREE].cbCardData[i], CardTypeResult[CT_THREE].cbEachHandCardCount[i], cbRemainCard, cbHandCardCount);
		//单牌组合
		BYTE cbComCard[MAX_COLS];
		BYTE cbComResCard[MAX_RESULT_COUNT][MAX_COLS];
		int cbComResLen = 0;
		BYTE cbNeedCardCount = 2;
		//单牌组合
		Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCard, cbNeedCardCount, cbRemainCardCount, cbNeedCardCount);
		for (int j = 0; j < cbComResLen; ++j)
		{
			if (isAllDoubleType(cbComResCard[j], cbNeedCardCount) == false)
			{
				continue;
			}
			BYTE Index = CardTypeResult[CT_THREE_TAKE_TWO].cbCardTypeCount;
			CardTypeResult[CT_THREE_TAKE_TWO].cbCardType = CT_THREE_TAKE_TWO;
			CopyMemory(CardTypeResult[CT_THREE_TAKE_TWO].cbCardData[Index], CardTypeResult[CT_THREE].cbCardData[i], CardTypeResult[CT_THREE].cbEachHandCardCount[i]);
			CardTypeResult[CT_THREE_TAKE_TWO].cbCardData[Index][3] = cbComResCard[j][0];
			CardTypeResult[CT_THREE_TAKE_TWO].cbCardData[Index][4] = cbComResCard[j][1];
			CardTypeResult[CT_THREE_TAKE_TWO].cbEachHandCardCount[Index] = 5;
			CardTypeResult[CT_THREE_TAKE_TWO].cbCardTypeCount++;

		}
	}
	for (int i = 0; i < CardTypeResult[CT_THREE_LINE].cbCardTypeCount; i++)
	{
		//移除四条
		BYTE cbRemainCard[MAX_COUNT];
		BYTE cbRemainCardCount = cbHandCardCount - CardTypeResult[CT_THREE_LINE].cbEachHandCardCount[i];
		CopyMemory(cbRemainCard, cbTmpCardData, cbHandCardCount*sizeof(BYTE));
		RemoveCard(CardTypeResult[CT_THREE_LINE].cbCardData[i], CardTypeResult[CT_THREE_LINE].cbEachHandCardCount[i], cbRemainCard, cbHandCardCount);
		//单牌组合
		BYTE cbComCard[MAX_COLS];
		BYTE cbComResCard[MAX_RESULT_COUNT][MAX_COLS];
		int cbComResLen = 0;
		BYTE cbNeedCardCount = (CardTypeResult[CT_THREE_LINE].cbEachHandCardCount[i] / 3)*2;
		//单牌组合
		Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCard, cbNeedCardCount, cbRemainCardCount, cbNeedCardCount);
		for (int j = 0; j < cbComResLen; ++j)
		{
			if (isAllDoubleType(cbComResCard[j], cbNeedCardCount) == false)
			{
				continue;
			}
			int tmpCount = CardTypeResult[CT_THREE_LINE].cbEachHandCardCount[i];
			BYTE Index = CardTypeResult[CT_THREE_TAKE_TWO].cbCardTypeCount;
			CardTypeResult[CT_THREE_TAKE_TWO].cbCardType = CT_THREE_TAKE_TWO;
			CopyMemory(CardTypeResult[CT_THREE_TAKE_TWO].cbCardData[Index], CardTypeResult[CT_THREE_LINE].cbCardData[i], tmpCount);
			CopyMemory(CardTypeResult[CT_THREE_TAKE_TWO].cbCardData[Index] + tmpCount, cbComResCard[j], cbNeedCardCount);
			CardTypeResult[CT_THREE_TAKE_TWO].cbEachHandCardCount[Index] = tmpCount + cbNeedCardCount;
			CardTypeResult[CT_THREE_TAKE_TWO].cbCardTypeCount++;
		}
	}
}

void CGameLogicNew::GetThreeTakeOneCardResult(BYTE * cbHandCardData,  BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	if (cbHandCardCount < 4)
	{
		return;
	}
	//恢复扑克，防止分析时改变扑克
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

	for (int i = 0; i < CardTypeResult[CT_THREE].cbCardTypeCount; i++)
	{
		//移除四条
		BYTE cbRemainCard[MAX_COUNT];
		BYTE cbRemainCardCount = cbHandCardCount - CardTypeResult[CT_THREE].cbEachHandCardCount[i];
		CopyMemory(cbRemainCard, cbTmpCardData, cbHandCardCount*sizeof(BYTE));
		RemoveCard(CardTypeResult[CT_THREE].cbCardData[i], CardTypeResult[CT_THREE].cbEachHandCardCount[i], cbRemainCard, cbHandCardCount);
		////单牌组合
		for (int j = 0; j < CardTypeResult[CT_THREE].cbCardTypeCount; j++)
		{
			if (RemoveCard(&CardTypeResult[CT_THREE].cbCardData[i][j], 1, cbRemainCard, cbRemainCardCount))
			{
				cbRemainCardCount--;
			}
		}
		cbRemainCardCount = ClearReLogicValue(cbRemainCard, cbRemainCardCount);
		for (int j = 0; j < cbRemainCardCount; ++j)
		{
			BYTE Index = CardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount;
			CardTypeResult[CT_THREE_TAKE_ONE].cbCardType = CT_THREE_TAKE_ONE;
			CopyMemory(CardTypeResult[CT_THREE_TAKE_ONE].cbCardData[Index], CardTypeResult[CT_THREE].cbCardData[i], CardTypeResult[CT_THREE].cbEachHandCardCount[i]);
			CardTypeResult[CT_THREE_TAKE_ONE].cbCardData[Index][3] = cbRemainCard[j];
			CardTypeResult[CT_THREE_TAKE_ONE].cbEachHandCardCount[Index] = 4;
			CardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount++;

		}
	}
	for (int i = 0; i < CardTypeResult[CT_THREE_LINE].cbCardTypeCount; i++)
	{
		//移除四条
		BYTE cbRemainCard[MAX_COUNT];
		BYTE cbRemainCardCount = cbHandCardCount - CardTypeResult[CT_THREE_LINE].cbEachHandCardCount[i];
		CopyMemory(cbRemainCard, cbTmpCardData, cbHandCardCount*sizeof(BYTE));
		RemoveCard(CardTypeResult[CT_THREE_LINE].cbCardData[i], CardTypeResult[CT_THREE_LINE].cbEachHandCardCount[i], cbRemainCard, cbHandCardCount);
		////单牌组合
		for (int j = 0; j < CardTypeResult[CT_THREE_LINE].cbCardTypeCount; j++)
		{
			if (RemoveCard(&CardTypeResult[CT_THREE_LINE].cbCardData[i][j], 1, cbRemainCard, cbRemainCardCount))
			{
				cbRemainCardCount--;
			}
		}
		//单牌组合
		BYTE cbComCard[MAX_COLS];
		BYTE cbComResCard[MAX_RESULT_COUNT][MAX_COLS];
		int cbComResLen = 0;
		BYTE needCardCount = CardTypeResult[CT_THREE_LINE].cbEachHandCardCount[i]/3;
		//单牌组合
		Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCard, needCardCount, cbRemainCardCount, needCardCount);
		for (int j = 0; j < cbComResLen; ++j)
		{
			int tmpCount = CardTypeResult[CT_THREE_LINE].cbEachHandCardCount[i];
			BYTE Index = CardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount;
			CardTypeResult[CT_THREE_TAKE_ONE].cbCardType = CT_THREE_TAKE_ONE;
			CopyMemory(CardTypeResult[CT_THREE_TAKE_ONE].cbCardData[Index], CardTypeResult[CT_THREE_LINE].cbCardData[i], tmpCount);
			CopyMemory(CardTypeResult[CT_THREE_TAKE_ONE].cbCardData[Index] + tmpCount, cbComResCard[j], needCardCount);
			CardTypeResult[CT_THREE_TAKE_ONE].cbEachHandCardCount[Index] = tmpCount + needCardCount;
			CardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount++;
		}
	}
}

void CGameLogicNew::GetThreeLineCardResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	if (cbHandCardCount < 6)
	{
		return;
	}
	//三连类型
	//对连类型
	//恢复扑克，防止分析时改变扑克
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

	//变量定义
	int cbBlockCount = 3;
	//搜索边牌
	tagSearchCardResult tmpSearchCardResult = {};
	//搜索边牌
	for (int i = 2; i < 7; i++)
	{
		SearchLineCardType(cbTmpCardData, cbHandCardCount, 0, cbBlockCount, i, &tmpSearchCardResult);
		if (tmpSearchCardResult.cbSearchCount > 0)
		{
			//保存数据
			for (int j = 0; j < tmpSearchCardResult.cbSearchCount; j++)
			{
				int Index = CardTypeResult[CT_THREE_LINE].cbCardTypeCount;
				CardTypeResult[CT_THREE_LINE].cbCardType = CT_THREE_LINE;
				CopyMemory(CardTypeResult[CT_THREE_LINE].cbCardData[Index], tmpSearchCardResult.cbResultCard[j], tmpSearchCardResult.cbCardCount[j]);
				CardTypeResult[CT_THREE_LINE].cbEachHandCardCount[Index] = tmpSearchCardResult.cbCardCount[j];
				CardTypeResult[CT_THREE_LINE].cbCardTypeCount++;
			}
		}
		else
		{
			break;
		}
	}
}

void CGameLogicNew::GetDoubleLineCardResult(BYTE *cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	if (cbHandCardCount < 6)
	{
		return;
	}
	//对连类型
		//恢复扑克，防止分析时改变扑克
		BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
		CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

		 //变量定义
		int cbBlockCount = 2;
		//搜索边牌
		tagSearchCardResult tmpSearchCardResult = {};
			//搜索边牌
		for (int i = 3; i < 11; i++)
		{
			SearchLineCardType(cbTmpCardData, cbHandCardCount, 0, cbBlockCount, i, &tmpSearchCardResult);
			if (tmpSearchCardResult.cbSearchCount > 0)
			{
				 //保存数据
				for (int j = 0; j < tmpSearchCardResult.cbSearchCount; j++)
				{
					int Index = CardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount;
					CardTypeResult[CT_DOUBLE_LINE].cbCardType = CT_DOUBLE_LINE;
					CopyMemory(CardTypeResult[CT_DOUBLE_LINE].cbCardData[Index], tmpSearchCardResult.cbResultCard[j], tmpSearchCardResult.cbCardCount[j]);
					CardTypeResult[CT_DOUBLE_LINE].cbEachHandCardCount[Index] = tmpSearchCardResult.cbCardCount[j];
					CardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount++;
				}
			}
			else
			{
				break;
			}
		}
}

void CGameLogicNew::GetSingleLineCardResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	if (cbHandCardCount < 5)
	{
		return;
	}
	 //变量定义
	int cbBlockCount = 1;
	//恢复扑克，防止分析时改变扑克
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	int   tmpCardCount = 0;
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

	//去除2和王
	for (BYTE i = 0; i < cbHandCardCount; ++i)
	{
		if (GetCardLogicValue(cbTmpCardData[i]) <15)
		{
			cbTmpCardData[tmpCardCount++] = cbHandCardData[i];
		}
	}
		 //搜索边牌
		 tagSearchCardResult tmpSearchCardResult = {};
		   int i = 5;
			while (i <= 12)
			{
				SearchLineCardType(cbTmpCardData, tmpCardCount, 0, cbBlockCount, i, &tmpSearchCardResult);
				if (tmpSearchCardResult.cbSearchCount > 0)
				{
					//策略优化, 如果有顺子有重叠数据, 先让其他不重叠
					//保存数据
					//保存数据
					for (int j = 0; j < tmpSearchCardResult.cbSearchCount; j++)
					{
						int Index = CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount;
						CardTypeResult[CT_SINGLE_LINE].cbCardType = CT_SINGLE_LINE;
						CopyMemory(CardTypeResult[CT_SINGLE_LINE].cbCardData[Index], tmpSearchCardResult.cbResultCard[j], tmpSearchCardResult.cbCardCount[j]);
						CardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[Index] = tmpSearchCardResult.cbCardCount[j];
						CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount++;
						
					}
				}
				else
				{
					break;
				}
				i++;

			}
			
	
}


//
//void com(vector<int> &candidates, int start, int sum, int target, vector<int> &path,  vector<vector<int> > &res)
//{
//	if (sum > target)//超出目标值 退出
//		return;
//	if (sum == target)// 找到一种解
//	{
//			res.push_back(path);
//		
//		return;
//	}
//
//	for (int i = start; i < candidates.size(); i++)
//	{
//		path.push_back(i);//存放当前值
//		com(candidates, i + 1, sum + candidates[i], target, path, res);
//		path.pop_back();//回溯
//
//	}
//
//}
void FillCombinatory(vector<tagOutCardResultNew> &tempVecSearch, vector<vector<tagOutCardResultNew> > &resultAllVec)
{
	bool bSuccess = true;
	bool tableRe[100] = { false };
	for (int j = 0; j < tempVecSearch.size(); j++)
	{
		for (int i = 0; i < tempVecSearch[j].cbCardCount; i++)
		{
			if (tableRe[tempVecSearch[j].cbResultCard[i]] != true)
			{
				tableRe[tempVecSearch[j].cbResultCard[i]] = true;
			}
			else
			{
				return;
			}
		}
	}
	resultAllVec.push_back(tempVecSearch);

}
void com(vector<int> &candidates, int start, int sum, int target, int path[], int &pathCount, vector<tagOutCardResultNew>& resultVec, vector<vector<tagOutCardResultNew> > &resultAllVec)
{
	if (sum > target)//超出目标值 退出
		return;
	if (pathCount >= 2)
	{
		bool tableRe[80] = { false };
		for (int j = 0; j < pathCount; j++)
		{
			for (int i = 0; i < resultVec[path[j]].cbCardCount; i++)
			{
				if (tableRe[resultVec[path[j]].cbResultCard[i]] != true)
				{
					tableRe[resultVec[path[j]].cbResultCard[i]] = true;
				}
				else
				{
					return;
				}
			}
		}
	}
	if (sum == target)// 找到一种解
	{
		vector<tagOutCardResultNew> tempVecSearch;
		for (int i = 0; i < pathCount; i++)
		{
			tempVecSearch.push_back(resultVec[path[i]]);
		}
		resultAllVec.push_back(tempVecSearch);
		//FillCombinatory(tempVecSearch, resultAllVec);
		return;
	}
	
	for (int i = start; i < candidates.size(); i++)
	{
		path[pathCount++] = i;//存放当前值
		com(candidates, i + 1, sum + candidates[i], target, path, pathCount, resultVec, resultAllVec);
		path[pathCount--] = 0;//回溯

	}

}
void comThread(vector<int> &candidates, int start, int end, int sum, int target, int path[],int &pathCount, vector<tagOutCardResultNew>& resultVec, vector<vector<tagOutCardResultNew> > &resultAllVec)
{
	if (sum > target)//超出目标值 退出
		return;
	if (pathCount >= 2)
	{
		int tableRe2[100] = { 0 };
		for (int j = 0; j < pathCount; j++)
		{
			for (int i = 0; i < resultVec[path[j]].cbCardCount; i++)
			{
				BYTE tempIndex = resultVec[path[j]].cbResultCard[i];
				tableRe2[tempIndex] ++;
				if (tableRe2[tempIndex]>tableRe[tempIndex])
				{
					return;
				}
			}
		}

	}
	if (sum == target)// 找到一种解
	{
		vector<tagOutCardResultNew> tempVecSearch;
		for (int i = 0; i < pathCount; i++)
		{
			tempVecSearch.push_back(resultVec[path[i]]);
		}
		resultAllVec.push_back(tempVecSearch);
		//FillCombinatory(tempVecSearch, resultAllVec);
		return;
	}
	//if (res.size() > 5000000)
	//{
	//	//达到6百万时,内存会暴掉,这里以5百万会分界线,几乎也不影响一手好牌的组合
	//	return;
	//}

	for (int i = start; i < candidates.size(); i++)
	{
		if (pathCount == 0 && i >= end)
		{
			break;
		}
		path[pathCount++]=i;//存放当前值
		comThread(candidates, i + 1, end, sum + candidates[i], target, path, pathCount, resultVec, resultAllVec);
		path[pathCount--] = 0;//回溯

	}

}
struct accumulate_block{
	void operator () (vector<int> &candidates, int start, int end, int sum, int target,  vector<tagOutCardResultNew>& resultVec, vector<vector<tagOutCardResultNew> > &resultAllVec){
		int   vecTempIndex[MAX_COUNT] = { 0 };
		int   pathCount = 0;
		comThread(candidates, start, end, 0, target, vecTempIndex, pathCount, resultVec, resultAllVec);
	}
};

int multithreadSort(vector<int> &vecHandCardCount, int sum, vector<tagOutCardResultNew>& resultVec, vector<vector<vector<tagOutCardResultNew> >> &resultAllVec){
	unsigned long const min_per_thread = 10;//每个线程的子序列大小  
	unsigned long const max_threads = 4;
	int num_threads = vecHandCardCount.size();
	if (num_threads > 4)
	{
		num_threads = 4;
	}
	vector<thread> threads(num_threads);

	for (int i = 0; i < num_threads; i++){//这里只分配子线程的任务序列,主线程也会执行一个排序任务
		int start = 0;
		int end = 0;
		if (i == 0)
		{
			start = 0;
			end = 1;
		}
		else if (i == 1)
		{
			start = 1;
			end = 2;
		}
		else if (i == 2)
		{
			start = 2;
			end = 3;
		}
		else if (i >= 3)
		{
			start = 3;
			end = vecHandCardCount.size();
		}
		threads[i] = thread(accumulate_block(), std::ref(vecHandCardCount), start, end, 0, sum, std::ref(resultVec), std::ref(resultAllVec[i]));//每个子线程的子序列分配  
	}
	std::for_each(threads.begin(), threads.end(), std::mem_fn(&thread::join));//等待子线程完成  
	return 0;

}
int CGameLogicNew::FindCardKindMinNum(BYTE const cbHandCardData[], BYTE const cbHandCardCount, tagOutCardTypeResultNew CardTypeResult[CT_TYPE_COUNT], vector<tagOutCardResultNew>   &CardResult, float &MinTypeScore, bool & bBiYing, bool bSearchBiYing)
{
	
	ZeroMemory(CardTypeResult, sizeof(tagOutCardTypeResultNew) * CT_TYPE_COUNT);
	if (cbHandCardCount==1)
	{

		tagOutCardResultNew OutCardResult;
		OutCardResult.cbCardCount = 1;
		OutCardResult.cbCardType = CT_SINGLE;
		OutCardResult.cbResultCard[0] = cbHandCardData[0];
		CardResult.push_back(OutCardResult);
		MinTypeScore = GetCardTypeScore(OutCardResult);
		BYTE Index = CardTypeResult[CT_SINGLE].cbCardTypeCount;
		CardTypeResult[CT_SINGLE].cbCardType = CT_SINGLE;
		CardTypeResult[CT_SINGLE].cbCardData[Index][0] = cbHandCardData[0];
		CardTypeResult[CT_SINGLE].cbEachHandCardCount[Index] = 1;
		CardTypeResult[CT_SINGLE].cbCardTypeCount++;
		return 1;
	}
	ZeroMemory(tableRe, sizeof(tableRe) );
	for (int i=0;i<cbHandCardCount;i++)
	{
		tableRe[cbHandCardData[i]]++;
	}
	BYTE cbTmpHReminCardData[MAX_COUNT] = { 0 };
	//保留扑克，防止分析时改变扑克
	BYTE cbReserveCardData[MAX_COUNT];
	BYTE cbReserveCardCount=cbHandCardCount;
	CopyMemory(cbReserveCardData, cbHandCardData, cbHandCardCount);
	//火箭类型
	GetMissileCardResult(cbReserveCardData, cbReserveCardCount, CardTypeResult);
	BYTE MISSILE_CARD[] = { 14, 15 };
	if (RemoveCard(MISSILE_CARD, sizeof(MISSILE_CARD), cbReserveCardData, cbReserveCardCount))
	{
		cbReserveCardCount -= sizeof(MISSILE_CARD);
	}
	int iLeftCardCount = cbReserveCardCount;
	SortCardList(cbReserveCardData, cbReserveCardCount, ST_ORDER);
	CopyMemory(cbTmpHReminCardData, cbReserveCardData, cbReserveCardCount);
	//炸弹类型
	GetBombCardResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
	//iLeftCardCount = RemoveTypeCard(CardTypeResult[CT_BOMB_CARD], cbTmpHReminCardData, iLeftCardCount);
	//	//--跑得快里面炸弹类型价值比较高, 所以需要将炸弹先移出来
	//CopyMemory(cbReserveCardData, cbTmpHReminCardData, iLeftCardCount);
	//cbReserveCardCount = iLeftCardCount;

	//三连类型
	GetThreeLineCardResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
	//三条类型
	//if (cbHandCardCount == 3)
	{
		GetThreeResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
	}
	//双连类型
	GetDoubleLineCardResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
	//三带一单
	{
		GetThreeTakeOneCardResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
	}
	//三带一对
	GetThreeTakeTwoCardResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
	
	//单连类型
	GetSingleLineCardResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
	//对牌类型
	GetDoubleResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
	//单牌类型
	GetSingleResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
	
	//if (iLeftCardCount<=10)
	{
		//四带两单
		if (CT_FOUR_TAKE_ONE>0)
		{
			GetFourTakeOneResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
		}
		//四带两对
		if (CT_FOUR_TAKE_TWO>0)
		{
			GetFourTakeTwoResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
		}
	}

	////单牌类型
	//CopyMemory(cbTmpHReminCardData, cbReserveCardData, cbReserveCardCount);
	//iLeftCardCount = cbReserveCardCount;
	//RemoveTypeCard(CardTypeResult[CT_SINGLE_LINE], cbTmpHReminCardData, iLeftCardCount);
	//RemoveTypeCard(CardTypeResult[CT_DOUBLE], cbTmpHReminCardData, iLeftCardCount);
	//RemoveTypeCard(CardTypeResult[CT_SINGLE], cbTmpHReminCardData, iLeftCardCount);
	//GetDoubleResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
	//GetSingleResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);

	//CopyMemory(cbTmpHReminCardData, cbReserveCardData, cbReserveCardCount);
	//iLeftCardCount = cbReserveCardCount;
	//RemoveTypeCard(CardTypeResult[CT_SINGLE_LINE], cbTmpHReminCardData, iLeftCardCount);
	//RemoveTypeCard(CardTypeResult[CT_THREE_LINE], cbTmpHReminCardData, iLeftCardCount);
	//RemoveTypeCard(CardTypeResult[CT_SINGLE], cbTmpHReminCardData, iLeftCardCount);
	//GetSingleResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);

	//CopyMemory(cbTmpHReminCardData, cbReserveCardData, cbReserveCardCount);
	//iLeftCardCount = cbReserveCardCount;
	//RemoveTypeCard(CardTypeResult[CT_SINGLE_LINE], cbTmpHReminCardData, iLeftCardCount);
	//RemoveTypeCard(CardTypeResult[CT_THREE], cbTmpHReminCardData, iLeftCardCount);
	//RemoveTypeCard(CardTypeResult[CT_SINGLE], cbTmpHReminCardData, iLeftCardCount);
	//GetSingleResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);

	//BYTE tempSingle[MAX_COUNT] = { 0 };
	//BYTE tempSingleCount = 0;
	//for (BYTE i = 0; i < CardTypeResult[CT_SINGLE].cbCardTypeCount; ++i)
	//{
	//	for (BYTE j = 0; j < CardTypeResult[CT_SINGLE].cbEachHandCardCount[i]; j++)
	//	{
	//		tempSingle[tempSingleCount++] = CardTypeResult[CT_SINGLE].cbCardData[i][0];
	//	}
	//}
	////增加一条策略,单牌数少于等于5的,双牌的也加进去
	//for (BYTE i = 0; i < CardTypeResult[CT_DOUBLE].cbCardTypeCount; ++i)
	//{
	//	if (CardTypeResult[CT_SINGLE].cbCardTypeCount >= 17)
	//	{
	//		break;
	//	}
	//	for (BYTE j = 0; j < CardTypeResult[CT_DOUBLE].cbEachHandCardCount[i]; j++)
	//	{
	//		//if (GetACardCount(tempSingle, tempSingleCount, CardTypeResult[CT_DOUBLE].cbCardData[i][j]) == 0)
	//		{
	//			BYTE Index = CardTypeResult[CT_SINGLE].cbCardTypeCount;
	//			CardTypeResult[CT_SINGLE].cbCardType = CT_SINGLE;
	//			CardTypeResult[CT_SINGLE].cbCardData[Index][0] = CardTypeResult[CT_DOUBLE].cbCardData[i][j];
	//			CardTypeResult[CT_SINGLE].cbEachHandCardCount[Index] = 1;
	//			CardTypeResult[CT_SINGLE].cbCardTypeCount++;
	//		}
	//	}
	//	
	//}
	clearSingleReResult(CardTypeResult);

	vector<int>   vecHandCardCount;
	vector<tagOutCardResultNew> resultVec;
	for (int i = CT_TYPE_COUNT-1; i >=0; i--)
	{
		if (CardTypeResult[i].cbCardType > 0)
		{
			for (int j = 0; j < CardTypeResult[i].cbCardTypeCount; j++)
			{
				tagOutCardResultNew typeResult;
				ZeroMemory(&typeResult, sizeof(typeResult));
				typeResult.cbCardType = CardTypeResult[i].cbCardType;
				typeResult.cbCardCount = CardTypeResult[i].cbEachHandCardCount[j];
				CopyMemory(typeResult.cbResultCard, CardTypeResult[i].cbCardData[j], typeResult.cbCardCount);
				resultVec.push_back(typeResult);
				vecHandCardCount.push_back(typeResult.cbCardCount);
			}
		}
	}
	
	//////////////////////////////////////非多线程版本
	#define NO_CONDITION_THREAD 1
#ifdef NO_CONDITION_THREAD0
	int   vecTempIndex[MAX_COUNT] = { 0 };
	int   pathCount = 0;
	int MinTypeCount = INT_MAX;
	int index = -1;
	vector<vector<tagOutCardResultNew> > resultAllVec;
	com(vecHandCardCount, 0, 0, cbHandCardCount, vecTempIndex, pathCount, resultVec, resultAllVec);
	for (int i = 0; i < resultAllVec.size(); i++)
	{
		int tempScore = GetHandScore(resultAllVec.at(i), MinTypeCount);
		if (resultAllVec.at(i).size() < MinTypeCount || ((resultAllVec.at(i).size() == MinTypeCount && tempScore > MinTypeScore)))
		{
			MinTypeScore = GetHandScore(resultAllVec.at(i), MinTypeCount);
			index = i;
			MinTypeCount = resultAllVec.at(i).size();
		}
		if (cbHandCardCount < 18 && bSearchBiYing == true && isBiYing(resultAllVec[i]))
		{
			if (bBiYing != true || (bBiYing && resultAllVec[i].size() <= MinTypeCount && tempScore>MinTypeScore))
			{
				bBiYing = true;
				MinTypeScore = tempScore;
				index = i;
				MinTypeCount = resultAllVec[i].size();
			}

	}

	}
	if (resultAllVec.size() > 0)
	{
		CardResult = resultAllVec[index];
		sort(CardResult.begin(), CardResult.end(), [this](tagOutCardResultNew & first, tagOutCardResultNew& second)
		{ if (first.cbCardType > second.cbCardType)
		{
			return true;
		}
		if (first.cbCardType == second.cbCardType)
		{
			if (GetCardLogicValue(first.cbResultCard[0]) > GetCardLogicValue(second.cbResultCard[0]))
			{
				return true;
			}

		}
		return false; });
	}
	/////////////////////////////////////////////////////多线程版本/////////////////////////////////////
#else
	vector<vector<vector<tagOutCardResultNew> >> resultAllVec(4);
	multithreadSort(vecHandCardCount, cbHandCardCount,  resultVec, resultAllVec);
	int MinTypeCount = INT_MAX;
	int index_x = -1;
	int index_y = -1;
	for (int j = 0; j < 4;j++)
	{
		for (int i = 0; i < resultAllVec[j].size(); i++)
		{
			float tempScore = GetHandScore(resultAllVec[j].at(i), MinTypeCount);
			if (resultAllVec[j].at(i).size() < MinTypeCount || ((resultAllVec[j].at(i).size() == MinTypeCount && tempScore > MinTypeScore)))
			{
				MinTypeScore = GetHandScore(resultAllVec[j].at(i), MinTypeCount);
				index_x = j;
				index_y = i;
				MinTypeCount = resultAllVec[j].at(i).size();
			}
			if (cbHandCardCount < 18 && bSearchBiYing == true && isBiYing(resultAllVec[j][i]))
			{
				if (bBiYing != true || (bBiYing && resultAllVec[j][i].size() <= MinTypeCount && tempScore>MinTypeScore))
				{
					bBiYing = true;
					MinTypeScore = tempScore;
					index_x = j;
					index_y = i;
					MinTypeCount = resultAllVec[j][i].size();
				}

			}

		}
	}
	if (index_x != -1 && index_y!=-1)
	{
		
		CardResult = resultAllVec[index_x][index_y];
		sort(CardResult.begin(), CardResult.end(), [this](tagOutCardResultNew & first, tagOutCardResultNew& second)
		{ 
			if (first.cbCardType > second.cbCardType)
			{
				return true;
			}
			if (first.cbCardType == second.cbCardType)
			{
				if (GetCardLogicValue(first.cbResultCard[0]) > GetCardLogicValue(second.cbResultCard[0]))
				{
					return true;
				}

			}
			return false;
		});
	}
#endif // SDK_CONDITION
	/////////////////////////////////////////////////////多线程版本/////////////////////////////////////
	return MinTypeCount;

	return 0;
}



void CGameLogicNew::clearSingleReResult(tagOutCardTypeResultNew * CardTypeResult)
{
	vector<BYTE> tempSingleCard;
	BYTE sinlgeCount = 0;
	for (BYTE i = 0; i < CardTypeResult[CT_SINGLE].cbCardTypeCount; ++i)
	{
		for (BYTE j = 0; j < CardTypeResult[CT_SINGLE].cbEachHandCardCount[i]; j++)
		{
			tempSingleCard.push_back(CardTypeResult[CT_SINGLE].cbCardData[i][0]);
		}
	}

	sort(tempSingleCard.begin(), tempSingleCard.end(), [](BYTE first, BYTE second)  {
		return first > second;
	});  // 排序

	int ArrayIndex = unique(tempSingleCard.begin(), tempSingleCard.end())-tempSingleCard.begin() ;   // 去重
	ZeroMemory(&CardTypeResult[CT_SINGLE], sizeof(CardTypeResult[CT_SINGLE]));
	for (int i = 0; i < ArrayIndex; i++)
	{
		BYTE Index = CardTypeResult[CT_SINGLE].cbCardTypeCount;
		CardTypeResult[CT_SINGLE].cbCardType = CT_SINGLE;
		CardTypeResult[CT_SINGLE].cbCardData[Index][0] = tempSingleCard[i];
		CardTypeResult[CT_SINGLE].cbEachHandCardCount[Index] = 1;
		CardTypeResult[CT_SINGLE].cbCardTypeCount++;

	}
}


void CGameLogicNew::FillCombinatory(vector<tagOutCardResultNew> &tempVecSearch, BYTE * cbTmpHReminCardData, BYTE const cbHandCardCount, vector<vector<tagOutCardResultNew> > &resultAllVec)
{
	bool bSuccess = true;
	for (int j = 0; j < tempVecSearch.size(); j++)
	{
		if (RemoveCard(tempVecSearch.at(j).cbResultCard, tempVecSearch.at(j).cbCardCount, cbTmpHReminCardData, cbHandCardCount) == false)
		{
			bSuccess = false;
			break;
		}
	}
	if (bSuccess)
	{
		resultAllVec.push_back(tempVecSearch);
	}
}


VOID CGameLogicNew::SortOutCardList(BYTE cbCardData[], BYTE cbCardCount)
{
	//获取牌型
	BYTE cbCardType = GetCardType(cbCardData, cbCardCount);

	if (cbCardType == CT_THREE_TAKE_ONE || cbCardType == CT_THREE_TAKE_TWO)
	{
		//分析牌
		tagAnalyseResultNew AnalyseResult = {};
		AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

		cbCardCount = AnalyseResult.cbBlockCount[2] * 3;
		CopyMemory(cbCardData, AnalyseResult.cbCardData[2], sizeof(BYTE)*cbCardCount);
		for (int i = CountArray(AnalyseResult.cbBlockCount) - 1; i >= 0; i--)
		{
			if (i == 2) continue;

			if (AnalyseResult.cbBlockCount[i] > 0)
			{
				CopyMemory(&cbCardData[cbCardCount], AnalyseResult.cbCardData[i],
					sizeof(BYTE)*(i + 1)*AnalyseResult.cbBlockCount[i]);
				cbCardCount += (i + 1)*AnalyseResult.cbBlockCount[i];
			}
		}
	}
	else if (cbCardType == CT_FOUR_TAKE_ONE || cbCardType == CT_FOUR_TAKE_TWO)
	{
		//分析牌
		tagAnalyseResultNew AnalyseResult = {};
		AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

		cbCardCount = AnalyseResult.cbBlockCount[3] * 4;
		CopyMemory(cbCardData, AnalyseResult.cbCardData[3], sizeof(BYTE)*cbCardCount);
		for (int i = CountArray(AnalyseResult.cbBlockCount) - 1; i >= 0; i--)
		{
			if (i == 3) continue;

			if (AnalyseResult.cbBlockCount[i] > 0)
			{
				CopyMemory(&cbCardData[cbCardCount], AnalyseResult.cbCardData[i],
					sizeof(BYTE)*(i + 1)*AnalyseResult.cbBlockCount[i]);
				cbCardCount += (i + 1)*AnalyseResult.cbBlockCount[i];
			}
		}
	}

	return;
}

BYTE CGameLogicNew::SearchSameCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCardCount, tagSearchCardResult *pSearchCardResult)
{
	//设置结果
	if (pSearchCardResult)
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//构造扑克
	BYTE cbCardData[FULL_COUNT];
	BYTE cbCardCount = cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//排列扑克
	SortCardList(cbCardData, cbCardCount, ST_ORDER);

	//分析扑克
	tagAnalyseResultNew AnalyseResult = {};
	AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

	BYTE cbReferLogicValue = cbReferCard == 0 ? 0 : GetCardLogicValue(cbReferCard);
	BYTE cbBlockIndex = cbSameCardCount - 1;
	do
	{
		for (BYTE i = 0; i < AnalyseResult.cbBlockCount[cbBlockIndex]; i++)
		{
			BYTE cbIndex = (AnalyseResult.cbBlockCount[cbBlockIndex] - i - 1)*(cbBlockIndex + 1);
			if (GetCardLogicValue(AnalyseResult.cbCardData[cbBlockIndex][cbIndex]) > cbReferLogicValue)
			{
				if (pSearchCardResult == NULL) return 1;

				ASSERT(cbResultCount < CountArray(pSearchCardResult->cbCardCount));

				//复制扑克
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], &AnalyseResult.cbCardData[cbBlockIndex][cbIndex],
					cbSameCardCount*sizeof(BYTE));
				pSearchCardResult->cbCardCount[cbResultCount] = cbSameCardCount;

				cbResultCount++;
			}
		}

		cbBlockIndex++;
	} while (cbBlockIndex < CountArray(AnalyseResult.cbBlockCount));

	if (pSearchCardResult)
		pSearchCardResult->cbSearchCount = cbResultCount;
	return cbResultCount;
}

BYTE CGameLogicNew::SearchTakeCardType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCount, BYTE cbTakeCardCount, tagSearchCardResult *pSearchCardResult)
{
	//设置结果
	if (pSearchCardResult)
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//效验
	if (cbSameCount != 3 && cbSameCount != 4)
		return cbResultCount;
	//长度判断
	if (cbSameCount == 4 && cbHandCardCount < cbSameCount + cbTakeCardCount)
		return cbResultCount;

	//构造扑克
	BYTE cbCardData[FULL_COUNT] = {};
	BYTE cbCardCount = cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//排列扑克
	SortCardList(cbCardData, cbCardCount, ST_ORDER);

	//搜索同张
	tagSearchCardResult tmpSearchResult = {};
	BYTE cbTmpResultCount = SearchSameCard(cbCardData, cbCardCount, cbReferCard, cbSameCount, &tmpSearchResult);

	if (cbTmpResultCount > 0)
	{
		//提取带牌
		for (BYTE i = 0; i < cbTmpResultCount; i++)
		{
			BYTE cbRemainCard[FULL_COUNT];
			BYTE cbRemainCardCount = cbHandCardCount - tmpSearchResult.cbCardCount[i];
			BYTE cbNeedCardCount =  cbTakeCardCount;
			if (cbRemainCardCount < cbNeedCardCount)
			{
				continue;
			}
			
			CopyMemory(cbRemainCard, cbHandCardData, cbHandCardCount*sizeof(BYTE));
			RemoveCard(tmpSearchResult.cbResultCard[i], tmpSearchResult.cbCardCount[i], cbRemainCard, cbHandCardCount);
			for (int j = 0; j < tmpSearchResult.cbCardCount[i]; j++)
			{
				if (RemoveCard(&tmpSearchResult.cbResultCard[i][j], 1, cbRemainCard, cbRemainCardCount))
				{
					cbRemainCardCount--;
				}
			}
			if (cbTakeCardCount>=2)
			{
				//不能带两王
				SortCardList(cbRemainCard, cbRemainCardCount, ST_ORDER);
				if (cbRemainCard[0] == 15 && cbRemainCard[1] == 14)
				{
					BYTE tempCard[2] = { 15, 14 };
					if (RemoveCard(tempCard, sizeof(tempCard), cbRemainCard, cbRemainCardCount))
					{
						cbRemainCardCount-=2;
					}
				}
			}

			//单牌组合
			BYTE cbComCard[MAX_COLS];
			BYTE cbComResCard[MAX_RESULT_COUNT][MAX_COLS];
			int cbComResLen = 0;
			//单牌组合
			if (cbTakeCardCount == 1)
			{
				cbRemainCardCount = ClearReLogicValue(cbRemainCard, cbRemainCardCount);
			}
			Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCard, cbNeedCardCount, cbRemainCardCount, cbNeedCardCount);
			for (int j = 0; j < cbComResLen; ++j)
			{
				if (((cbSameCount == 3 && cbTakeCardCount == 2) || (cbSameCount == 4 && cbTakeCardCount == 4)) && isAllDoubleType(cbComResCard[j], cbNeedCardCount) == false)
				{
					continue;
				}
				 cbResultCount = pSearchCardResult->cbSearchCount++;
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchResult.cbResultCard[i],
					tmpSearchResult.cbCardCount[i]);
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount] + tmpSearchResult.cbCardCount[i], cbComResCard[j],
					cbNeedCardCount);
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchResult.cbCardCount[i] + cbNeedCardCount;

			}
		}
	}

	return	pSearchCardResult->cbSearchCount;
}



int CGameLogicNew::YouXianDaNengShouHuiCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew>& vecMinTypeCardResult, tagOutCardResultNew & OutCardResult, bool &bZhiJieChu)
{
	int typeCount[CT_TYPE_COUNT] = { 0 };
	int xiangDuiMaxIndex[CT_TYPE_COUNT] = { 0 };
	int juDuiMaxIndex[CT_TYPE_COUNT] = { 0 };
	int NoXiangDuiDaPaiMaxCount = 0;
	vector<vector<int>> MaxIndexSet(CT_TYPE_COUNT);
	vector<int> allMaxCardIndex;
	int DoubleReMaxCount = 0;
	int bombCount = 0;
	vector <int> singleLogic;
	vector <int> doubleLogic;
	for (int i = 0; i < vecMinTypeCardResult.size(); i++)
	{
		int type = vecMinTypeCardResult[i].cbCardType;
		typeCount[type]++;
		bool bExistMax = false;
		BYTE tmpTurnCard[MAX_COUNT] = { 0 };
		int tmpTurnCardCount = vecMinTypeCardResult[i].cbCardCount;
		CopyMemory(tmpTurnCard, vecMinTypeCardResult[i].cbResultCard, vecMinTypeCardResult[i].cbCardCount);
		bool bCheck = CheckBombPercent(cbHandCardData, cbHandCardCount, m_cbDiscardCard, m_cbDiscardCardCount);
	/*	if (cbHandCardCount >= m_cbCardTypeCount)
		{
			bExistMax = SearchOtherHandCardThan(tmpTurnCard, tmpTurnCardCount, true);
		}
		else*/
		{
			bCheck = true;
			if ((type == CT_DOUBLE || type == CT_SINGLE))
			{
				bCheck = true;
				if (type == CT_DOUBLE)
				{
					doubleLogic.push_back(GetCardLogicValue(tmpTurnCard[0]));
				}
				else if (type == CT_SINGLE)
				{
					singleLogic.push_back(GetCardLogicValue(tmpTurnCard[0]));
				}
			}
			bExistMax = SearchOtherHandCardThan(tmpTurnCard, tmpTurnCardCount, bCheck);
		}
		if (type == CT_DOUBLE && bExistMax && juDuiMaxIndex[type] <= 0 && vecMinTypeCardResult.size()>2 && vecMinTypeCardResult[vecMinTypeCardResult.size()-1].cbCardType==CT_SINGLE)
		{
			//--优化, 从连对中找出最大对子
			BYTE maxTrunCardLogic = GetCardLogicValue(tmpTurnCard[0]);
			int doubleMaxIndex =- 1;
			for (int j = vecMinTypeCardResult.size()-1; j >=0; j--)
			{
				if (vecMinTypeCardResult[j].cbCardType == CT_DOUBLE_LINE)
				{
					if (GetCardLogicValue(vecMinTypeCardResult[j].cbResultCard[0]) > maxTrunCardLogic)
					{
						ZeroMemory(tmpTurnCard, sizeof(tmpTurnCard));
						tmpTurnCard[0] = vecMinTypeCardResult[j].cbResultCard[0];
						tmpTurnCard[1] = vecMinTypeCardResult[j].cbResultCard[1];
						doubleMaxIndex = j;
						break;
					}
				}
				else if ( vecMinTypeCardResult[j].cbCardType == CT_THREE_TAKE_TWO )
				{
					if (GetCardLogicValue(vecMinTypeCardResult[j].cbResultCard[0]) > maxTrunCardLogic)
					{
						ZeroMemory(tmpTurnCard, sizeof(tmpTurnCard));
						tmpTurnCard[0] = vecMinTypeCardResult[j].cbResultCard[0];
						tmpTurnCard[1] = vecMinTypeCardResult[j].cbResultCard[1];
						doubleMaxIndex = j;
						break;
					}
				}
			}
			bool bExistDoubleMax = false;
			if (cbHandCardCount >= m_cbCardTypeCount)
				{
					bExistDoubleMax = SearchOtherHandCardThan(tmpTurnCard, tmpTurnCardCount, true);
				}
				else{
					bExistDoubleMax = SearchOtherHandCardThan(tmpTurnCard, tmpTurnCardCount, bCheck);
				}
				if ((bExistDoubleMax != true) /*|| GetCardLogicValue(tmpTurnCard[0])==14*/)
				{
					DoubleReMaxCount++;
					if (SearchOtherHandCardSame(tmpTurnCard, tmpTurnCardCount, type) == false)
					{
						juDuiMaxIndex[type]++;
					}
					else
					{
						xiangDuiMaxIndex[type]++;
					}
					if (MaxIndexSet[type].size() == 0)
					{
						MaxIndexSet[type].push_back(doubleMaxIndex);
						for (int j = 0; j < vecMinTypeCardResult.size(); j++)
						{
							if (doubleMaxIndex != j && type == vecMinTypeCardResult[j].cbCardType)
							{
								MaxIndexSet[type].push_back(j);
							}
						}
					}
				}
		}
		if (bExistMax != true)
		{
			
			
			if (SearchOtherHandCardSame(tmpTurnCard, tmpTurnCardCount, type) == false)
			{
				if ((type == CT_THREE_TAKE_ONE || type == CT_THREE_TAKE_TWO) && tmpTurnCardCount>5)
				{
					xiangDuiMaxIndex[type]++;
				}
				else{
					juDuiMaxIndex[type]++;
				}
			}
			else
			{
				xiangDuiMaxIndex[type]++;
			}
		/*	if (type == CT_FOUR_TAKE_ONE || type == CT_FOUR_TAKE_ONE)
			{
				continue;
			}*/
			allMaxCardIndex.push_back(i);
			if (MaxIndexSet[type].size() == 0)
			{
				MaxIndexSet[type].push_back(i);
				for (int j = 0; j < vecMinTypeCardResult.size(); j++)
				{
					if (i != j && type == vecMinTypeCardResult[j].cbCardType)
					{
						MaxIndexSet[type].push_back(j);
					}
				}
			}
		}
		else
		{
			if (vecMinTypeCardResult[i].cbCardCount>NoXiangDuiDaPaiMaxCount)
			{
				NoXiangDuiDaPaiMaxCount = vecMinTypeCardResult[i].cbCardCount;
			}
		}

	/*	if (type >= CT_BOMB_CARD)
		{
			bombCount++;
		}*/
	}
	int  maxTypeCount = 0;
	int   resultIndex = -1;
	for (int i = 0; i < CT_TYPE_COUNT; i++)
	{
		maxTypeCount += (juDuiMaxIndex[i] + xiangDuiMaxIndex[i]);
	}
	maxTypeCount -= DoubleReMaxCount;
	if ((maxTypeCount + bombCount) >= vecMinTypeCardResult.size() - 1)
	{
		bZhiJieChu = true;
	}
	if (bZhiJieChu==false)
	{
		int tempAllCount = 0;
		for (int i = 0; i < allMaxCardIndex.size(); i++)
		{
			tempAllCount += vecMinTypeCardResult[allMaxCardIndex[i]].cbCardCount;
		}
		if ((tempAllCount + NoXiangDuiDaPaiMaxCount) >= m_cbUserCardCount[0])
		{
			bZhiJieChu = true;
		}
	}
	if (bZhiJieChu)
	{
		if (juDuiMaxIndex[CT_SINGLE] == 1 && juDuiMaxIndex[CT_FOUR_TAKE_ONE]==1&&m_cbUserCardCount[1]!=1)
		{
			for (int i = 0; i < vecMinTypeCardResult.size(); i++)
			{
				int type = vecMinTypeCardResult[i].cbCardType;
				if (type == CT_SINGLE)
				{
					resultIndex = i;
					break;
				}
			}
		}
		else if (juDuiMaxIndex[CT_DOUBLE] == 1 && juDuiMaxIndex[CT_FOUR_TAKE_TWO] == 1 && m_cbUserCardCount[1] != 2)
		{
			for (int i = 0; i < vecMinTypeCardResult.size(); i++)
			{
				int type = vecMinTypeCardResult[i].cbCardType;
				if (type == CT_DOUBLE)
				{
					resultIndex = i;
					break;
				}
			}
		}
		else if (allMaxCardIndex.size() > 0)
		{
			resultIndex = allMaxCardIndex[allMaxCardIndex.size() - 1];
			int maxTypeCount = vecMinTypeCardResult[resultIndex].cbCardCount;
			for (int i = 0; i < allMaxCardIndex.size(); i++)
			{
				if ((vecMinTypeCardResult[allMaxCardIndex[i]].cbCardType != CT_MISSILE_CARD) && vecMinTypeCardResult[allMaxCardIndex[i]].cbCardCount>maxTypeCount)
				{
					maxTypeCount = vecMinTypeCardResult[allMaxCardIndex[i]].cbCardCount;
					resultIndex = allMaxCardIndex[i];
				}
				else if ((vecMinTypeCardResult[resultIndex].cbCardType == CT_FOUR_TAKE_ONE || vecMinTypeCardResult[resultIndex].cbCardType == CT_FOUR_TAKE_TWO) && (vecMinTypeCardResult[allMaxCardIndex[i]].cbCardType < CT_FOUR_TAKE_ONE))
				{
					maxTypeCount = vecMinTypeCardResult[allMaxCardIndex[i]].cbCardCount;
					resultIndex = allMaxCardIndex[i];
				}
			}
		}
	}
	bool danShuangNengShouNO = false;
	if (resultIndex == -1)
	{
		int tempTypeCount = 0;
		for (int i = 0; i < CT_TYPE_COUNT; i++)
		{
			int tempSize = MaxIndexSet[i].size();
			if (i == CT_DOUBLE&&cbHandCardCount >= m_cbCardTypeCount&&tempSize == 2 && (doubleLogic.size()>1)&&(doubleLogic[1] >= 11))
			{
				danShuangNengShouNO = true;
				continue;
			}
			else if (i == CT_SINGLE&&cbHandCardCount >= m_cbCardTypeCount&&tempSize == 2 && (singleLogic.size()>1)&&(singleLogic[1] >= 11))
			{
				danShuangNengShouNO = true;
				continue;
			}
			if (juDuiMaxIndex[i] > 0 && ((typeCount[i] > 1) || (i == CT_DOUBLE && (typeCount[i] + DoubleReMaxCount)>1)) && tempSize > 0)
			{
				
				tempTypeCount = typeCount[i];
				resultIndex = getMostCountIndex(MaxIndexSet, i, vecMinTypeCardResult);
				if (i == CT_DOUBLE)
				{
					if ((juDuiMaxIndex[CT_SINGLE] > 0) && (juDuiMaxIndex[CT_DOUBLE] > 0) && (tempSize>=3) && (typeCount[CT_SINGLE] + typeCount[CT_DOUBLE]) == vecMinTypeCardResult.size())
					{
						resultIndex = MaxIndexSet[i][tempSize - 2];
					}
				}
				
				break;
			}
		}
		//
		if (resultIndex==-1)
		{
			for (int i = 0; i < CT_TYPE_COUNT; i++)
			{
				int tempSize = MaxIndexSet[i].size();
				if (xiangDuiMaxIndex[i] > 0 && typeCount[i] > 1 && tempSize > 0)
				{
					resultIndex = getMostCountIndex(MaxIndexSet, i, vecMinTypeCardResult);
					break;
				}

			}
		}
		else
		{   	//绝对大牌只有两个,比相对大牌手数少时,相对大牌的先出
			if (tempTypeCount<=2)
			{
				for (int i = 0; i < CT_TYPE_COUNT; i++)
				{
					int tempSize = MaxIndexSet[i].size();
					if (xiangDuiMaxIndex[i] > 0 && typeCount[i] > 1 && typeCount[i] > tempTypeCount && tempSize > 0)
					{
						resultIndex = getMostCountIndex(MaxIndexSet, i, vecMinTypeCardResult);
						break;
					}

				}
			}
		}
	
	}
	//
	if (resultIndex == -1)
	{
		if (((maxTypeCount + bombCount) >= vecMinTypeCardResult.size() - 2) && xiangDuiMaxIndex[CT_SINGLE_LINE] == 0)
		{
			for (int i = 0; i < vecMinTypeCardResult.size(); i++)
			{
				int type = vecMinTypeCardResult[i].cbCardType;
				if (CT_SINGLE_LINE == type&&vecMinTypeCardResult[i].cbCardCount >= 7)
				{
					resultIndex = i;
					break;
				}
			}
		}
		if (((maxTypeCount + bombCount) >= vecMinTypeCardResult.size() - 2) && xiangDuiMaxIndex[CT_DOUBLE_LINE] == 0)
		{
			for (int i = 0; i < vecMinTypeCardResult.size(); i++)
			{
				int type = vecMinTypeCardResult[i].cbCardType;
				if (CT_DOUBLE_LINE == type&&vecMinTypeCardResult[i].cbCardCount >= 6)
				{
					resultIndex = i;
					break;
				}
			}
		}
	}
	
	if (resultIndex == -1)
	{

		resultIndex = DanShuangNengShouHuiLaiCheck(vecMinTypeCardResult, OutCardResult);
		if (OutCardResult.cbCardCount > 0)
		{
			return resultIndex;
		}
		
		if (resultIndex == -1&&m_cbUserCardCount[1]==2)
		{
			if ((juDuiMaxIndex[CT_SINGLE] > 0)  && (typeCount[CT_SINGLE] + typeCount[CT_DOUBLE]) == vecMinTypeCardResult.size())
			{
				for (int i = 0; i < vecMinTypeCardResult.size(); i++)
				{
					int type = vecMinTypeCardResult[i].cbCardType;
					if (type == CT_DOUBLE)
					{
						resultIndex = i;
						OutCardResult.cbCardCount = 1;
						OutCardResult.cbCardType = CT_SINGLE;
						CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[resultIndex].cbResultCard, 1);
						break;
					}
				}
				
				return resultIndex;
			}
		}
		
	}
	
	if (resultIndex!=-1)
	{
		OutCardResult.cbCardCount = vecMinTypeCardResult[resultIndex].cbCardCount;
		OutCardResult.cbCardType = vecMinTypeCardResult[resultIndex].cbCardType;
		CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[resultIndex].cbResultCard, OutCardResult.cbCardCount);
	}
	return resultIndex;
}

int CGameLogicNew::IsBiShengTurnCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew>& vecMinTypeCardResult, tagOutCardResultNew & OutCardResult, bool &bZhiJieChu)
{
	int typeCount[CT_TYPE_COUNT] = { 0 };
	int xiangDuiMaxIndex[CT_TYPE_COUNT] = { 0 };
	int juDuiMaxIndex[CT_TYPE_COUNT] = { 0 };
	int NoXiangDuiDaPaiMaxCount = 0;
	vector<vector<int>> MaxIndexSet(CT_TYPE_COUNT);
	vector<int> allMaxCardIndex;
	int DoubleReMaxCount = 0;
	int bombCount = 0;
	vector <int> singleLogic;
	vector <int> doubleLogic;
	for (int i = 0; i < vecMinTypeCardResult.size(); i++)
	{
		int type = vecMinTypeCardResult[i].cbCardType;
		typeCount[type]++;
		bool bExistMax = false;
		BYTE tmpTurnCard[MAX_COUNT] = { 0 };
		int tmpTurnCardCount = vecMinTypeCardResult[i].cbCardCount;
		CopyMemory(tmpTurnCard, vecMinTypeCardResult[i].cbResultCard, vecMinTypeCardResult[i].cbCardCount);
		bool bCheck = CheckBombPercent(cbHandCardData, cbHandCardCount, m_cbDiscardCard, m_cbDiscardCardCount);
		/*	if (cbHandCardCount >= m_cbCardTypeCount)
		{
		bExistMax = SearchOtherHandCardThan(tmpTurnCard, tmpTurnCardCount, true);
		}
		else*/
		{
			bCheck = true;
			if ((type == CT_DOUBLE || type == CT_SINGLE))
			{
				bCheck = true;
				if (type == CT_DOUBLE)
				{
					doubleLogic.push_back(GetCardLogicValue(tmpTurnCard[0]));
				}
				else if (type == CT_SINGLE)
				{
					singleLogic.push_back(GetCardLogicValue(tmpTurnCard[0]));
				}
			}
			if (m_cbUserCardCount[1]>12){
				bCheck = false;
			}
			bExistMax = SearchOtherHandCardThan(tmpTurnCard, tmpTurnCardCount, bCheck);
		}
		
		if (type == CT_DOUBLE && bExistMax && juDuiMaxIndex[type] <= 0 && vecMinTypeCardResult.size()>2 && vecMinTypeCardResult[vecMinTypeCardResult.size() - 1].cbCardType == CT_SINGLE)
		{
			//--优化, 从连对中找出最大对子
			BYTE maxTrunCardLogic = GetCardLogicValue(tmpTurnCard[0]);
			int doubleMaxIndex = -1;
			for (int j = vecMinTypeCardResult.size() - 1; j >= 0; j--)
			{
				if (vecMinTypeCardResult[j].cbCardType == CT_DOUBLE_LINE)
				{
					if (GetCardLogicValue(vecMinTypeCardResult[j].cbResultCard[0]) > maxTrunCardLogic)
					{
						ZeroMemory(tmpTurnCard, sizeof(tmpTurnCard));
						tmpTurnCard[0] = vecMinTypeCardResult[j].cbResultCard[0];
						tmpTurnCard[1] = vecMinTypeCardResult[j].cbResultCard[1];
						doubleMaxIndex = j;
						break;
					}
				}
				else if (vecMinTypeCardResult[j].cbCardType == CT_THREE_TAKE_TWO)
				{
					if (GetCardLogicValue(vecMinTypeCardResult[j].cbResultCard[0]) > maxTrunCardLogic)
					{
						ZeroMemory(tmpTurnCard, sizeof(tmpTurnCard));
						tmpTurnCard[0] = vecMinTypeCardResult[j].cbResultCard[0];
						tmpTurnCard[1] = vecMinTypeCardResult[j].cbResultCard[1];
						doubleMaxIndex = j;
						break;
					}
				}
			}
			bool bExistDoubleMax = false;
			if (cbHandCardCount >= m_cbCardTypeCount)
			{
				bExistDoubleMax = SearchOtherHandCardThan(tmpTurnCard, tmpTurnCardCount, true);
			}
			else{
				bExistDoubleMax = SearchOtherHandCardThan(tmpTurnCard, tmpTurnCardCount, bCheck);
			}
			if ((bExistDoubleMax != true) /*|| GetCardLogicValue(tmpTurnCard[0]) == 14*/)
			{
				DoubleReMaxCount++;
				if (SearchOtherHandCardSame(tmpTurnCard, tmpTurnCardCount, type) == false)
				{
					juDuiMaxIndex[type]++;
				}
				else
				{
					xiangDuiMaxIndex[type]++;
				}
				if (MaxIndexSet[type].size() == 0)
				{
					MaxIndexSet[type].push_back(doubleMaxIndex);
					for (int j = 0; j < vecMinTypeCardResult.size(); j++)
					{
						if (doubleMaxIndex != j && type == vecMinTypeCardResult[j].cbCardType)
						{
							MaxIndexSet[type].push_back(j);
						}
					}
				}
			}
		}
		if (bExistMax != true)
		{

			allMaxCardIndex.push_back(i);
			if (SearchOtherHandCardSame(tmpTurnCard, tmpTurnCardCount, type) == false)
			{
				juDuiMaxIndex[type]++;
			}
			else
			{
				xiangDuiMaxIndex[type]++;
			}
			if (MaxIndexSet[type].size() == 0)
			{
				MaxIndexSet[type].push_back(i);
				for (int j = 0; j < vecMinTypeCardResult.size(); j++)
				{
					if (i != j && type == vecMinTypeCardResult[j].cbCardType)
					{
						MaxIndexSet[type].push_back(j);
					}
				}
			}
		}
		else
		{
			if (vecMinTypeCardResult[i].cbCardCount>NoXiangDuiDaPaiMaxCount)
			{
				NoXiangDuiDaPaiMaxCount = vecMinTypeCardResult[i].cbCardCount;
			}
		}

		/*if (type == CT_BOMB_CARD)
		{
		bombCount++;
		}*/
	}
	int  maxTypeCount = 0;
	int   resultIndex = -1;
	for (int i = 0; i < CT_TYPE_COUNT; i++)
	{
		maxTypeCount += (juDuiMaxIndex[i] + xiangDuiMaxIndex[i]);
	}
	maxTypeCount -= DoubleReMaxCount;
	if ((maxTypeCount + bombCount) >= vecMinTypeCardResult.size() - 1)
	{
		bZhiJieChu = true;
		resultIndex = 0;
	}
	if (bZhiJieChu == false)
	{
		int tempAllCount = 0;
		for (int i = 0; i < allMaxCardIndex.size(); i++)
		{
			tempAllCount += vecMinTypeCardResult[allMaxCardIndex[i]].cbCardCount;
		}
		if ((tempAllCount + NoXiangDuiDaPaiMaxCount) >= (cbHandCardCount -m_cbBeiRangCount ))
		{
			bZhiJieChu = true;
			resultIndex = 0;
		}
	}

	
	//
	if (((maxTypeCount + bombCount) >= vecMinTypeCardResult.size() - 2) && xiangDuiMaxIndex[CT_SINGLE_LINE] == 0)
	{
		for (int i = 0; i < vecMinTypeCardResult.size(); i++)
		{
			int type = vecMinTypeCardResult[i].cbCardType;
			if (CT_SINGLE_LINE == type&&vecMinTypeCardResult[i].cbCardCount >= 7)
			{
				resultIndex = i;
				break;
			}
		}
	}
	if (((maxTypeCount + bombCount) >= vecMinTypeCardResult.size() - 2) && xiangDuiMaxIndex[CT_DOUBLE_LINE] == 0)
	{
		for (int i = 0; i < vecMinTypeCardResult.size(); i++)
		{
			int type = vecMinTypeCardResult[i].cbCardType;
			if (CT_DOUBLE_LINE == type&&vecMinTypeCardResult[i].cbCardCount >= 6)
			{
				resultIndex = i;
				break;
			}
		}
	}


	return resultIndex;
}

int CGameLogicNew::DanShuangNengShouHuiLaiCheck(vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew &OutCardResult)
{
	vector<BYTE> singleData;
	vector<BYTE> doubleData;
	int bombCount = 0;
	int typeCount[CT_TYPE_COUNT] = { 0 };
	int cbCardCount = 0;
	for (int i = 0; i < vecMinTypeCardResult.size(); i++)
	{
		int type = vecMinTypeCardResult[i].cbCardType;
		typeCount[type]++;
		cbCardCount += vecMinTypeCardResult[i].cbCardCount;
		if (type == CT_SINGLE)
		{
			singleData.push_back(vecMinTypeCardResult[i].cbResultCard[0]);
		}
		if (type == CT_DOUBLE)
		{
			doubleData.push_back(vecMinTypeCardResult[i].cbResultCard[0]);
			doubleData.push_back(vecMinTypeCardResult[i].cbResultCard[1]);
		}
		if (type == CT_THREE_TAKE_ONE)
		{
			for (int k = 0; k < vecMinTypeCardResult[i].cbCardCount / 4; k++)
			{
				singleData.push_back(vecMinTypeCardResult[i].cbResultCard[vecMinTypeCardResult[i].cbCardCount - 1 - k]);
			}
		}
		if (type == CT_THREE_TAKE_TWO)
		{
			if (vecMinTypeCardResult[i].cbCardCount<=5)
			{
				for (int k = 0; k < vecMinTypeCardResult[i].cbCardCount / 5; k++)
				{
					BYTE data1 = vecMinTypeCardResult[i].cbResultCard[vecMinTypeCardResult[i].cbCardCount - 1 - 2 * k];
					BYTE data2 = vecMinTypeCardResult[i].cbResultCard[vecMinTypeCardResult[i].cbCardCount - 2 - 2 * k];
					if (GetCardLogicValue(data1) == GetCardLogicValue(data2))
					{
						doubleData.push_back(data1);
						doubleData.push_back(data2);
					}
					else{
						singleData.push_back(data1);
						singleData.push_back(data2);
					}
				}
			}
			
		}
	}

	if (singleData.size() > 1 )
	{
		sort(singleData.begin(), singleData.end(), [this](BYTE first, BYTE second)
		{
			if (GetCardLogicValue(first) < GetCardLogicValue(second))
			{
				return true;
			}
			return false;
		});
		bool juDuiDaPai = false;
		for (int i = 0; i < singleData.size(); i++)
		{
			BYTE tmpTurnCard[MAX_COUNT] = { 0 };
			tmpTurnCard[0] = { singleData[i] };
			int tmpTurnCardCount = 1;
			int reIndex=IsJueDuiDaPai(tmpTurnCard, tmpTurnCardCount, CT_SINGLE);
			if (reIndex >= XIANG_DUI_DA)
			{
				juDuiDaPai = true;
				OutCardResult.cbCardCount = 1;
				OutCardResult.cbCardType = CT_SINGLE;
				CopyMemory(OutCardResult.cbResultCard, &singleData[0], 1);
				break;
			}
		}
		if (cbCardCount == m_cbCardTypeCount&&(typeCount[CT_SINGLE_LINE] + typeCount[CT_THREE_TAKE_TWO]) == vecMinTypeCardResult.size())
		{
			ZeroMemory(&OutCardResult, sizeof(OutCardResult));
		}

	}


	if (doubleData.size() > 2)
	{
		sort(doubleData.begin(), doubleData.end(), [this](BYTE first, BYTE second)
		{
			if (GetCardLogicValue(first) < GetCardLogicValue(second))
			{
				return true;
			}
			return false;
		});
		bool juDuiDaPai = false;
		for (int i = 0; i < doubleData.size(); i += 2)
		{
			BYTE tmpTurnCard[MAX_COUNT] = { 0 };
			tmpTurnCard[0] = { doubleData[i] };
			tmpTurnCard[1] = { doubleData[i + 1] };
			int tmpTurnCardCount = 2;
			int reIndex = IsJueDuiDaPai(tmpTurnCard, tmpTurnCardCount, CT_DOUBLE);
			if (reIndex >= XIANG_DUI_DA)
			{
					juDuiDaPai = true;
					OutCardResult.cbCardCount = 2;
					OutCardResult.cbCardType = CT_DOUBLE;
					CopyMemory(OutCardResult.cbResultCard, &doubleData[0], 2);
					break;

			}
		}
		if (cbCardCount == m_cbCardTypeCount && (typeCount[CT_SINGLE] >1) && doubleData.size()==4)
		{
			if (GetCardLogicValue(doubleData[doubleData.size()-1])==14)
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));

			}
		}
	}

	if (OutCardResult.cbCardCount>0)
	{
		return 0;
	}
	return -1;
}

int CGameLogicNew::SearchMutilType(const BYTE cbHandCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew>& vecMinTypeCardResult, tagOutCardResultNew & OutCardResult)
{
	int MaxTypeCount = 0;
	int resultIndex = -1;
	int resultIndexType = -1;
	int cardIndexCount = -1;
	int maxCard = INT_MAX;
	int typeCount[CT_TYPE_COUNT] = { 0 };
	for (int i = 0; i < vecMinTypeCardResult.size(); i++)
	{
		int type = vecMinTypeCardResult[i].cbCardType;
		typeCount[type]++;
	}
	for (int i = 0; i < vecMinTypeCardResult.size(); i++)
	{
		int type = vecMinTypeCardResult[i].cbCardType;
		bool bSpecal = false;
		if ((type == CT_FOUR_TAKE_ONE || type == CT_FOUR_TAKE_TWO)&&vecMinTypeCardResult.size()<=2)
		{
			bSpecal = true;
		}
		if ((type>CT_SINGLE && type <= CT_THREE_TAKE_TWO) || bSpecal)
		{
			if (vecMinTypeCardResult[i].cbCardCount > cardIndexCount)
		    {
				cardIndexCount = vecMinTypeCardResult[i].cbCardCount;
				resultIndex = i;
				MaxTypeCount = typeCount[type];
				resultIndexType = vecMinTypeCardResult[i].cbCardType;
				maxCard = GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0]);
		    }
			else if (vecMinTypeCardResult[i].cbCardCount == cardIndexCount && typeCount[type] > MaxTypeCount)
			{
				cardIndexCount = vecMinTypeCardResult[i].cbCardCount;
				resultIndex = i;
				MaxTypeCount = typeCount[type];
				resultIndexType = vecMinTypeCardResult[i].cbCardType;
				maxCard = GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0]);
			}
			else if
				(vecMinTypeCardResult[i].cbCardCount == cardIndexCount && typeCount[type] == MaxTypeCount &&
				GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0]) < maxCard
				)
			{
				cardIndexCount = vecMinTypeCardResult[i].cbCardCount;
				resultIndex = i;
				MaxTypeCount = typeCount[type];
				resultIndexType = vecMinTypeCardResult[i].cbCardType;
				maxCard = GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0]);
			}
		
			else if (
				resultIndexType == CT_SINGLE_LINE && vecMinTypeCardResult[i].cbCardType == CT_SINGLE_LINE &&
				vecMinTypeCardResult[i].cbCardCount <= vecMinTypeCardResult[resultIndex].cbCardCount
				)
			{
				cardIndexCount = vecMinTypeCardResult[i].cbCardCount;
				resultIndex = i;
				MaxTypeCount = typeCount[type];
				resultIndexType = vecMinTypeCardResult[i].cbCardType;
				maxCard = GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0]);
			}
						 
		}
	}
	if (resultIndex==-1)
	{
		for (int i = 0; i < vecMinTypeCardResult.size(); i++)
		{
			int type = vecMinTypeCardResult[i].cbCardType;
			if (type== CT_SINGLE )
			{
				if
					(
					GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0]) < maxCard
					)
				{
					cardIndexCount = vecMinTypeCardResult[i].cbCardCount;
					resultIndex = i;
					MaxTypeCount = typeCount[type];
					resultIndexType = vecMinTypeCardResult[i].cbCardType;
					maxCard = GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0]);
				}
			}
		}
	}
	if (resultIndex != -1)
	{
		
			OutCardResult.cbCardCount = vecMinTypeCardResult[resultIndex].cbCardCount;
			CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[resultIndex].cbResultCard, OutCardResult.cbCardCount);
			int tmpType = vecMinTypeCardResult[resultIndex].cbCardType;
			bool bExist = false;
			for (int i = 0; i < vecMinTypeCardResult.size(); i++)
			{
				int type = vecMinTypeCardResult[i].cbCardType;
				if (typeCount[type] >= 1 && (tmpType >= CT_THREE_TAKE_ONE && tmpType <= CT_THREE_TAKE_TWO && type >= CT_THREE_TAKE_ONE && type <= CT_THREE_TAKE_TWO))
				{
					if (GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0]) < GetCardLogicValue(OutCardResult.cbResultCard[0]))
					{
						ZeroMemory(&OutCardResult, sizeof(OutCardResult));
						OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
						CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, OutCardResult.cbCardCount);
						bExist = true;
					}
				}

			}
			if (bExist == false && (tmpType == CT_THREE_TAKE_TWO || tmpType == CT_THREE_TAKE_ONE || tmpType == CT_DOUBLE))
			{
				if ((m_cbUserCardCount[0] -  OutCardResult.cbCardCount) <= 1)
				{
					bExist = true;
				}
			}
			if (bExist == false && (tmpType == CT_THREE_TAKE_TWO || tmpType == CT_THREE_TAKE_ONE || tmpType == CT_DOUBLE ) && GetCardLogicValue(vecMinTypeCardResult[resultIndex].cbResultCard[0]) >= 11 && typeCount[tmpType] == 1&&m_cbUserCardCount[1]!=1)
		   {
				
				for (int i = 0; i < CT_TYPE_COUNT; i++)
				{
					if (typeCount[i]>1)
					{
						for (int j = vecMinTypeCardResult.size()-1; j >=0; j--)
						{
							if (vecMinTypeCardResult[j].cbCardType==i)
							{
								ZeroMemory(&OutCardResult, sizeof(OutCardResult));
								OutCardResult.cbCardCount = vecMinTypeCardResult[j].cbCardCount;
								CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[j].cbResultCard, OutCardResult.cbCardCount);
								bExist = true;
								break;
							}
						}
						if (bExist)
						{
							break;
						}
					}

				}
		   }
			if (bExist == false && (tmpType == CT_DOUBLE_LINE) && (typeCount[CT_BOMB_CARD]>0 || GetCardLogicValue(vecMinTypeCardResult[resultIndex].cbResultCard[0]) ==14 ) && GetCardLogicValue(vecMinTypeCardResult[resultIndex].cbResultCard[0]) >= 11 && typeCount[tmpType] == 1)
			{

				for (int i = 0; i < CT_TYPE_COUNT; i++)
				{
					if (typeCount[i] > 1)
					{
						for (int j = vecMinTypeCardResult.size() - 1; j >= 0; j--)
						{
							if (vecMinTypeCardResult[j].cbCardType == i)
							{
								ZeroMemory(&OutCardResult, sizeof(OutCardResult));
								OutCardResult.cbCardCount = vecMinTypeCardResult[j].cbCardCount;
								CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[j].cbResultCard, OutCardResult.cbCardCount);
								bExist = true;
								break;
							}
						}
						if (bExist)
						{
							break;
						}
					}
				}
			}
			if (bExist == false && (tmpType == CT_SINGLE_LINE) && OutCardResult.cbCardCount>5 && (cbHandCardCount<m_cbCardTypeCount))
			{
					BYTE tmpTurnCard[MAX_COUNT] = { 0 };
				int tmpTurnCardCount = 1;
				CopyMemory(tmpTurnCard, &OutCardResult.cbResultCard[0],1);
				bool	bExistMax = SearchOtherHandCardThan(tmpTurnCard, tmpTurnCardCount, true);
				if (bExistMax==false)
				{
					OutCardResult.cbResultCard[0] = OutCardResult.cbResultCard[OutCardResult.cbCardCount - 1];
					OutCardResult.cbResultCard[OutCardResult.cbCardCount - 1] = 0;
					OutCardResult.cbCardCount -= 1;
					bExist = true;
					SortCardList(OutCardResult.cbResultCard, OutCardResult.cbCardCount, ST_ORDER);
				}
				
			}
			if (bExist == false && (tmpType == CT_THREE_TAKE_TWO || tmpType == CT_THREE_TAKE_ONE) && ((typeCount[CT_THREE_TAKE_TWO] + typeCount[CT_THREE_TAKE_ONE]) == vecMinTypeCardResult.size() && vecMinTypeCardResult.size() == 2))
			{
				//只剩下两个三拖时，出最大那个
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
				OutCardResult.cbCardCount = vecMinTypeCardResult[0].cbCardCount;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[0].cbResultCard, OutCardResult.cbCardCount);
				bExist = true;
			}
	}
	return resultIndex;
}

bool CGameLogicNew::isBiYing(vector<tagOutCardResultNew > & CardTypeResult)
{
	//--优先打能收回来的牌
	bool bExistJuiMax = true;
	int xiangDuiDaPaiCount = 0;
	int totalCount = 0;
	for (int i = 0; i < CardTypeResult.size(); i++)
	{
		int type = GetCardType(CardTypeResult[i].cbResultCard, CardTypeResult[i].cbCardCount);
		if (type != 0)
		{
			bool bExistMax = false;
			bool bExist = true;
			bExistMax = SearchOtherHandCardThan(CardTypeResult[i].cbResultCard, CardTypeResult[i].cbCardCount, false);
			if (bExistMax != true)
			{
				xiangDuiDaPaiCount = xiangDuiDaPaiCount + 1;
				totalCount += CardTypeResult[i].cbCardCount;
				if (totalCount >= m_cbUserCardCount[0])
				{
					return true;
				}
			}
		}
	}

	if (xiangDuiDaPaiCount > 0 && (xiangDuiDaPaiCount >= (CardTypeResult.size() - 1)))
		return true;

	return false;
}

//构造扑克
BYTE CGameLogicNew::MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex)
{
	return (cbColorIndex<<4)|(cbValueIndex+1);
}

//分析扑克
VOID CGameLogicNew::AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResultNew & AnalyseResult)
{
	//设置结果
	ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));

	//扑克分析
	for (BYTE i=0;i<cbCardCount;i++)
	{
		//变量定义
		BYTE cbSameCount=1,cbCardValueTemp=0;
		BYTE cbLogicValue=GetCardLogicValue(cbCardData[i]);

		//搜索同牌
		for (BYTE j=i+1;j<cbCardCount;j++)
		{
			//获取扑克
			if (GetCardLogicValue(cbCardData[j])!=cbLogicValue) break;

			//设置变量
			cbSameCount++;
		}

		//设置结果
		BYTE cbIndex=AnalyseResult.cbBlockCount[cbSameCount-1]++;
		for (BYTE j=0;j<cbSameCount;j++) AnalyseResult.cbCardData[cbSameCount-1][cbIndex*cbSameCount+j]=cbCardData[i+j];

		//设置索引
		i+=cbSameCount-1;
	}

	return;
}

//分析分布
VOID CGameLogicNew::AnalysebDistributing(const BYTE cbCardData[], BYTE cbCardCount, tagDistributingNew & Distributing)
{
	//设置变量
	ZeroMemory(&Distributing,sizeof(Distributing));

	//设置变量
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbCardData[i]==0) continue;

		//获取属性
		BYTE cbCardColor=GetCardColor(cbCardData[i]);
		BYTE cbCardValue=GetCardValue(cbCardData[i]);

		//分布信息
		Distributing.cbCardCount++;
		Distributing.cbDistributing[cbCardValue-1][cbIndexCount]++;
		Distributing.cbDistributing[cbCardValue-1][cbCardColor>>4]++;
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////以下为AI函数

//出牌搜索
bool CGameLogicNew::SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, WORD wOutCardUser, WORD wMeChairID, tagOutCardResultNew & OutCardResult)
{
	//玩家判断
	WORD wUndersideOfBanker = (m_wBankerUser+1)%GAME_PLAYER ;	//地主下家
	WORD wUpsideOfBanker = (wUndersideOfBanker+1)%GAME_PLAYER ;	//地主上家
	m_wMeChairID = wMeChairID;
	//初始变量
	ZeroMemory(&OutCardResult, sizeof(OutCardResult)) ;

	//先出牌
	if(cbTurnCardCount==0)
	{
		//地主出牌
		if(wMeChairID==m_wBankerUser) BankerOutCard(cbHandCardData, cbHandCardCount, OutCardResult) ;
		//地主下家
		else if(wMeChairID==wUndersideOfBanker) UndersideOfBankerOutCard(cbHandCardData, cbHandCardCount,wMeChairID,  OutCardResult) ;
		//地主上家
		else if(wMeChairID==wUpsideOfBanker) UpsideOfBankerOutCard(cbHandCardData, cbHandCardCount, wMeChairID, OutCardResult) ;
		//错误 ID
		else ASSERT(false) ;
	}
	//压牌
	else
	{	
		//地主出牌
		if(wMeChairID==m_wBankerUser) BankerOutCard(cbHandCardData, cbHandCardCount, wOutCardUser, cbTurnCardData, cbTurnCardCount, OutCardResult) ;
		//地主下家
		else if (wMeChairID == wUndersideOfBanker) UndersideOfBankerOutCard(cbHandCardData, cbHandCardCount, wMeChairID, wOutCardUser, cbTurnCardData, cbTurnCardCount, OutCardResult);
		//地主上家
		else if (wMeChairID == wUpsideOfBanker) UpsideOfBankerOutCard(cbHandCardData, cbHandCardCount, wMeChairID, wOutCardUser, cbTurnCardData, cbTurnCardCount, OutCardResult);
		//错误 ID
		else ASSERT(false) ;

	}
	return true ;
}

//分析炸弹
VOID CGameLogicNew::GetAllBomCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbBomCardData[], BYTE &cbBomCardCount, tagOutCardTypeResultNew   *CardTypeResult )
{
	BYTE cbTmpCardData[MAX_COUNT] ;
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount) ;

	//大小排序
	SortCardList(cbTmpCardData, cbHandCardCount, ST_ORDER);

	cbBomCardCount = 0 ;

	if(cbHandCardCount<4) return ;

	//双王炸弹
	//if(15==cbTmpCardData[0] && 14==cbTmpCardData[1])
	//{
	//	cbBomCardData[cbBomCardCount++] = cbTmpCardData[0] ;
	//	cbBomCardData[cbBomCardCount++] = cbTmpCardData[1] ;
	//}

	//扑克分析
	for (BYTE i=0;i<cbHandCardCount;i++)
	{
		//变量定义
		BYTE cbSameCount=1;
		BYTE cbLogicValue=GetCardLogicValue(cbTmpCardData[i]);

		//搜索同牌
		for (BYTE j=i+1;j<cbHandCardCount;j++)
		{
			//获取扑克
			if (GetCardLogicValue(cbTmpCardData[j])!=cbLogicValue) break;

			//设置变量
			cbSameCount++;
		}

		if(4==cbSameCount)
		{
			cbBomCardData[cbBomCardCount++] = cbTmpCardData[i] ;
			cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+1] ;
			cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+2] ;
			cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+3] ;
		}

		//设置索引
		i+=cbSameCount-1;
	}
}

//分析顺子
VOID CGameLogicNew::GetAllLineCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbLineCardData[], BYTE &cbLineCardCount, tagOutCardTypeResultNew   *CardTypeResult)
{
	BYTE cbTmpCard[MAX_COUNT] ;
	CopyMemory(cbTmpCard, cbHandCardData, cbHandCardCount) ;
	//大小排序
	SortCardList(cbTmpCard, cbHandCardCount, ST_ORDER) ;

	cbLineCardCount = 0 ;

	//数据校验
	if(cbHandCardCount<5) return ;

	BYTE cbFirstCard = 0 ;
	//去除2和王
	for(BYTE i=0 ; i<cbHandCardCount ; ++i)	if(GetCardLogicValue(cbTmpCard[i])<15)	{cbFirstCard = i ; break ;}

	BYTE cbSingleLineCard[12] ;
	BYTE cbSingleLineCount=0 ;
	BYTE cbLeftCardCount = cbHandCardCount ;
	bool bFindSingleLine = true ;

	//连牌判断
	while (cbLeftCardCount>=5 && bFindSingleLine)
	{
		cbSingleLineCount=1 ;
		bFindSingleLine = false ;
		BYTE cbLastCard = cbTmpCard[cbFirstCard] ;
		cbSingleLineCard[cbSingleLineCount-1] = cbTmpCard[cbFirstCard] ;
		for (BYTE i=cbFirstCard+1; i<cbLeftCardCount; i++)
		{
			BYTE cbCardData=cbTmpCard[i];

			//连续判断
			if (1!=(GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbCardData)) && GetCardValue(cbLastCard)!=GetCardValue(cbCardData)) 
			{
				cbLastCard = cbTmpCard[i] ;
				if(cbSingleLineCount<5) 
				{
					cbSingleLineCount = 1 ;
					cbSingleLineCard[cbSingleLineCount-1] = cbTmpCard[i] ;
					continue ;
				}
				else break ;
			}
			//同牌判断
			else if(GetCardValue(cbLastCard)!=GetCardValue(cbCardData))
			{
				cbLastCard = cbCardData ;
				cbSingleLineCard[cbSingleLineCount] = cbCardData ;
				++cbSingleLineCount ;
			}					
		}

		//保存数据
		if(cbSingleLineCount>=5)
		{
			RemoveCard(cbSingleLineCard, cbSingleLineCount, cbTmpCard, cbLeftCardCount) ;
			memcpy(cbLineCardData+cbLineCardCount , cbSingleLineCard, sizeof(BYTE)*cbSingleLineCount) ;
			cbLineCardCount += cbSingleLineCount ;
			cbLeftCardCount -= cbSingleLineCount ;
			bFindSingleLine = true ;
			if (CardTypeResult!=NULL)
			{
				int Index = CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount;
				CardTypeResult[CT_SINGLE_LINE].cbCardType = CT_SINGLE_LINE;
				CopyMemory(CardTypeResult[CT_SINGLE_LINE].cbCardData[Index], cbSingleLineCard, sizeof(BYTE)*cbSingleLineCount);
				CardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[Index] = cbSingleLineCount;
				CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount++;
			}
			
		}
	}
}

//分析三条
VOID CGameLogicNew::GetAllThreeCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbThreeCardData[], BYTE &cbThreeCardCount, tagOutCardTypeResultNew   *CardTypeResult)
{
	BYTE cbTmpCardData[MAX_COUNT] ;
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount) ;

	//大小排序
	SortCardList(cbTmpCardData, cbHandCardCount, ST_ORDER);

	cbThreeCardCount = 0 ;

	//扑克分析
	for (BYTE i=0;i<cbHandCardCount;i++)
	{
		//变量定义
		BYTE cbSameCount=1;
		BYTE cbLogicValue=GetCardLogicValue(cbTmpCardData[i]);

		//搜索同牌
		for (BYTE j=i+1;j<cbHandCardCount;j++)
		{
			//获取扑克
			if (GetCardLogicValue(cbTmpCardData[j])!=cbLogicValue) break;

			//设置变量
			cbSameCount++;
		}

		if(cbSameCount>=3)
		{
			cbThreeCardData[cbThreeCardCount++] = cbTmpCardData[i] ;
			cbThreeCardData[cbThreeCardCount++] = cbTmpCardData[i+1] ;
			cbThreeCardData[cbThreeCardCount++] = cbTmpCardData[i+2] ;	
		}

		//设置索引
		i+=cbSameCount-1;
	}
}

//分析对子
VOID CGameLogicNew::GetAllDoubleCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbDoubleCardData[], BYTE &cbDoubleCardCount, tagOutCardTypeResultNew   *CardTypeResult)
{
	BYTE cbTmpCardData[MAX_COUNT] ;
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount) ;

	//大小排序
	SortCardList(cbTmpCardData, cbHandCardCount, ST_ORDER);

	cbDoubleCardCount = 0 ;

	//扑克分析
	for (BYTE i=0;i<cbHandCardCount;i++)
	{
		//变量定义
		BYTE cbSameCount=1;
		BYTE cbLogicValue=GetCardLogicValue(cbTmpCardData[i]);

		//搜索同牌
		for (BYTE j=i+1;j<cbHandCardCount;j++)
		{
			//获取扑克
			if (GetCardLogicValue(cbTmpCardData[j])!=cbLogicValue) break;

			//设置变量
			cbSameCount++;
		}

		if(cbSameCount>=2)
		{
			cbDoubleCardData[cbDoubleCardCount++] = cbTmpCardData[i] ;
			cbDoubleCardData[cbDoubleCardCount++] = cbTmpCardData[i+1] ;
		}

		//设置索引
		i+=cbSameCount-1;
	}
}

//分析单牌
VOID CGameLogicNew::GetAllSingleCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbSingleCardData[], BYTE &cbSingleCardCount, tagOutCardTypeResultNew   *CardTypeResult)
{
	cbSingleCardCount =0 ;

	BYTE cbTmpCardData[MAX_COUNT] ;
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount) ;

	//大小排序
	SortCardList(cbTmpCardData, cbHandCardCount, ST_ORDER);

	//扑克分析
	for (BYTE i=0;i<cbHandCardCount;i++)
	{
		//变量定义
		BYTE cbSameCount=1;
		BYTE cbLogicValue=GetCardLogicValue(cbTmpCardData[i]);

		//搜索同牌
		for (BYTE j=i+1;j<cbHandCardCount;j++)
		{
			//获取扑克
			if (GetCardLogicValue(cbTmpCardData[j])!=cbLogicValue) break;

			//设置变量
			cbSameCount++;
		}

		if(cbSameCount==1)
		{
			cbSingleCardData[cbSingleCardCount++] = cbTmpCardData[i] ;
		}

		//设置索引
		i+=cbSameCount-1;
	}
}




/********************************************************************
函数名：Combination
参数：
cbCombineCardData：存储单个的组合结果
cbResComLen：已得到的组合长度，开始调用时此参数为0
cbResultCardData：存储所有的组合结果
cbResCardLen：cbResultCardData的第一下标的长度，组合结果的个数
cbSrcCardData：需要做组合的数据
cbSrcLen：cbSrcCardData的数据数目
cbCombineLen2，cbCombineLen1：组合的长度，开始调用时两者相等。
*********************************************************************/
//组合算法
VOID CGameLogicNew::Combination(BYTE cbCombineCardData[], int cbResComLen, BYTE cbResultCardData[MAX_RESULT_COUNT][MAX_COLS], int &cbResCardLen, BYTE cbSrcCardData[], BYTE cbCombineLen1, BYTE cbSrcLen, const BYTE cbCombineLen2)
{

	if( cbResComLen == cbCombineLen2 )
	{
		if (IsReCombination(cbResultCardData, cbCombineCardData, cbResCardLen, cbCombineLen2) == false)
		{
			CopyMemory(&cbResultCardData[cbResCardLen], cbCombineCardData, cbResComLen);
			++cbResCardLen;
		}
	}
	else
	{ 
		if(cbCombineLen1 >= 1 && cbSrcLen > 0 && (cbSrcLen+1) >= 0 ){
			cbCombineCardData[cbCombineLen2-cbCombineLen1] =  cbSrcCardData[0];
			++cbResComLen ;
			Combination(cbCombineCardData,cbResComLen, cbResultCardData, cbResCardLen, cbSrcCardData+1,cbCombineLen1-1, cbSrcLen-1, cbCombineLen2);

			--cbResComLen;
			Combination(cbCombineCardData,cbResComLen, cbResultCardData, cbResCardLen, cbSrcCardData+1,cbCombineLen1, cbSrcLen-1, cbCombineLen2);
		}
	}
}




bool CGameLogicNew::IsReCombination(BYTE cbResultCardData[][MAX_COLS], BYTE cbCardData[MAX_COLS], int cbResCardLen, int cbNeedCardCount)
{
	for (int i = 0; i < cbResCardLen; i++)
	{
		int count = 0;
		for (int k = 0; k < cbNeedCardCount; k++)
		{
			if (cbCardData[k] == cbResultCardData[i][k])
			{
				count++;
			}
		}
		if (count == cbNeedCardCount)
		{
			return true;
		}

	}
	return false;
}


//设置扑克
VOID CGameLogicNew::SetUserCard(WORD wChairID,const BYTE cbCardData[], BYTE cbCardCount)
{
	ZeroMemory(m_cbAllCardData[wChairID], cbCardCount);
	CopyMemory(m_cbAllCardData[wChairID], cbCardData, cbCardCount*sizeof(BYTE)) ;
	m_cbUserCardCount[wChairID] = cbCardCount ;

	//排列扑克
	SortCardList(m_cbAllCardData[wChairID], cbCardCount, ST_ORDER) ;
}

//设置底牌
VOID CGameLogicNew::SetBackCard(WORD wChairID, BYTE cbBackCardData[], BYTE cbCardCount)
{
	BYTE cbTmpCount = m_cbUserCardCount[wChairID] ;
	CopyMemory(m_cbAllCardData[wChairID]+cbTmpCount, cbBackCardData, cbCardCount*sizeof(BYTE)) ;
	m_cbUserCardCount[wChairID] += cbCardCount ;

	//排列扑克
	SortCardList(m_cbAllCardData[wChairID], m_cbUserCardCount[wChairID], ST_ORDER) ;
}

//设置庄家
VOID CGameLogicNew::SetBanker(WORD wBanker) 
{
	m_wBankerUser = wBanker ;
}
//删除扑克
VOID CGameLogicNew::RemoveUserCardData(WORD wChairID, BYTE cbRemoveCardData[], BYTE cbRemoveCardCount) 
{
	bool bSuccess = RemoveCard(cbRemoveCardData, cbRemoveCardCount, m_cbAllCardData[wChairID], m_cbUserCardCount[wChairID]) ;
	ASSERT(bSuccess) ;
	m_cbUserCardCount[wChairID] -= cbRemoveCardCount ;

}

BYTE CGameLogicNew::SearchLineCardType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbBlockCount, BYTE cbLineCount, tagSearchCardResult *pSearchCardResult)
{
	//设置结果
	if (pSearchCardResult)
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//定义变量
	BYTE cbLessLineCount = 0;
	if (cbLineCount == 0)
	{
		if (cbBlockCount == 1)
			cbLessLineCount = 5;
		else if (cbBlockCount == 2)
			cbLessLineCount = 3;
		else cbLessLineCount = 2;
	}
	else cbLessLineCount = cbLineCount;

	BYTE cbReferIndex = 2;
	if (cbReferCard != 0)
	{
		ASSERT(GetCardLogicValue(cbReferCard) - cbLessLineCount >= 2);
		cbReferIndex = GetCardLogicValue(cbReferCard) - cbLessLineCount + 1;
	}
	//超过A
	if (cbReferIndex + cbLessLineCount > 14) return cbResultCount;

	//长度判断
	if (cbHandCardCount < cbLessLineCount*cbBlockCount) return cbResultCount;

	//构造扑克
	BYTE cbCardData[FULL_COUNT] = { 0 };
	BYTE cbCardCount = cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//排列扑克
	SortCardList(cbCardData, cbCardCount, ST_ORDER);

	//分析扑克
	tagDistributingNew Distributing = {};
	AnalysebDistributing(cbCardData, cbCardCount, Distributing);
	CopyMemory(Distributing.cbDistributing[13], Distributing.cbDistributing[0], sizeof(Distributing.cbDistributing[0]));
	//搜索顺子
	BYTE cbTmpLinkCount = 0;
	BYTE cbValueIndex = 0;
	for (cbValueIndex = cbReferIndex; cbValueIndex < 14; cbValueIndex++)
	{
		//继续判断
		if (Distributing.cbDistributing[cbValueIndex][cbIndexCount] < cbBlockCount)
		{
			if (cbTmpLinkCount < cbLessLineCount)
			{
				cbTmpLinkCount = 0;
				continue;
			}
			else cbValueIndex--;
		}
		else
		{
			cbTmpLinkCount++;
			//寻找最长连
			if (cbLineCount == 0) continue;
		}

		if (cbTmpLinkCount >= cbLessLineCount)
		{
			if (pSearchCardResult == NULL) return 1;

			ASSERT(cbResultCount < CountArray(pSearchCardResult->cbCardCount));

			//复制扑克
			BYTE cbCount = 0;
			for (BYTE cbIndex = cbValueIndex + 1 - cbTmpLinkCount; cbIndex <= cbValueIndex; cbIndex++)
			{
				BYTE cbTmpCount = 0;
				for (BYTE cbColorIndex = 0; cbColorIndex < 4; cbColorIndex++)
				{
					for (BYTE cbColorCount = 0; cbColorCount < Distributing.cbDistributing[cbIndex][3 - cbColorIndex]; cbColorCount++)
					{
						if (cbIndex==13)
						{
							pSearchCardResult->cbResultCard[cbResultCount][cbCount++] = MakeCardData(0, 3 - cbColorIndex);
						}
						else{
							pSearchCardResult->cbResultCard[cbResultCount][cbCount++] = MakeCardData(cbIndex, 3 - cbColorIndex);
						}

						if (++cbTmpCount == cbBlockCount) break;
					}
					if (cbTmpCount == cbBlockCount) break;
				}
			}

			//设置变量
			pSearchCardResult->cbCardCount[cbResultCount] = cbCount;
			cbResultCount++;

			if (cbLineCount != 0)
			{
				cbTmpLinkCount--;
			}
			else
			{
				cbTmpLinkCount = 0;
			}
		}
	}

	
	for (int i = 0; i < cbResultCount;i++)
	{
		SortCardList(pSearchCardResult->cbResultCard[i], pSearchCardResult->cbCardCount[i], ST_ORDER);
	}
	if (pSearchCardResult)
		pSearchCardResult->cbSearchCount = cbResultCount;
	return cbResultCount;
}


bool CGameLogicNew::OutCardShengYuFenCheck(BYTE cbHandCardCount, const BYTE * cbHandCardData, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, WORD wOutCardUser, float MinTypeScoreVec, vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew & OutCardResult)
{
	tagSearchCardResult SearchCardResult;
	SearchOutCard(cbHandCardData, cbHandCardCount, cbTurnCardData, cbTurnCardCount, &SearchCardResult);
	int minTypeCount = 0;
	int resultIndex = -1;
	float MinTypeScore = INT_MIN;
	float BiShengScore = INT_MIN;
	vector<int> BiShengResult;
	vector<tagOutCardResultNew>   vecMinTypeCardResultBak;
	float tableScore[MAX_RESULT_COUNT] = { 0 };
	bool bExistBiYing = false;
	vector<int> vecIndexUnIncludeBomb ;
	vector<vector<tagOutCardResultNew>>  TempMinTypeCardResult(SearchCardResult.cbSearchCount);
	vector<BYTE> singleData;
	vector<BYTE> doubleData;
	vector<BYTE> BombData;
	bool bHaveBombType = false;
	for (int i = 0; i < vecMinTypeCardResult.size();i++)
	{
		if (vecMinTypeCardResult[i].cbCardType>=CT_FOUR_TAKE_ONE)
		{
			bHaveBombType = true;
		}
		if (vecMinTypeCardResult[i].cbCardType == CT_SINGLE)
		{
			singleData.push_back(vecMinTypeCardResult[i].cbResultCard[0]);
		}
		else if (vecMinTypeCardResult[i].cbCardType == CT_DOUBLE)
		{
			doubleData.push_back(vecMinTypeCardResult[i].cbResultCard[0]);
			doubleData.push_back(vecMinTypeCardResult[i].cbResultCard[1]);
		}
		else if (vecMinTypeCardResult[i].cbCardType == CT_BOMB_CARD)
		{
			BombData.push_back(vecMinTypeCardResult[i].cbResultCard[0]);
		}
		else if (vecMinTypeCardResult[i].cbCardType == CT_MISSILE_CARD)
		{
			BombData.push_back(vecMinTypeCardResult[i].cbResultCard[0]);
			BombData.push_back(vecMinTypeCardResult[i].cbResultCard[1]);
		}
	}
	bool bExistMaxIndex[MAX_RESULT_COUNT] = { 0 };
	for (int i = 0; i < SearchCardResult.cbSearchCount; i++)
	{

		BYTE tempType = GetCardType(SearchCardResult.cbResultCard[i], SearchCardResult.cbCardCount[i]);
		//不能拆炸弹处理
		int   tempCardCount = SearchCardResult.cbCardCount[i];
		bool bExistMax = SearchOtherHandCardThan(SearchCardResult.cbResultCard[i], SearchCardResult.cbCardCount[i], true);
		bExistMaxIndex[i] = bExistMax;
		BYTE cbReserveCardData[MAX_COUNT] = { 0 };
		BYTE cbReserveCardCount = cbHandCardCount;
		CopyMemory(cbReserveCardData, cbHandCardData, cbReserveCardCount);
		RemoveCard(
			SearchCardResult.cbResultCard[i],
			tempCardCount,
			cbReserveCardData,
			cbReserveCardCount
 			);
		cbReserveCardCount = cbReserveCardCount - tempCardCount;
	
		tagOutCardTypeResultNew tmpCardTypeResult[CT_TYPE_COUNT];
 		ZeroMemory(tmpCardTypeResult, sizeof(tmpCardTypeResult));

		if (cbReserveCardCount > 0)
		{
			int tempMinTypeCount = 0;
			float tempMinTypeScore = 0;
			bool biYing = false;
			tempMinTypeCount = FindCardKindMinNum(cbReserveCardData, cbReserveCardCount, tmpCardTypeResult, TempMinTypeCardResult[i], tempMinTypeScore, biYing, true);
			tableScore[i] = tempMinTypeScore;
			if (tempMinTypeScore > MinTypeScore)
			{
				{
					MinTypeScore = tempMinTypeScore;
					minTypeCount = tempMinTypeCount;
					resultIndex = i;
					vecMinTypeCardResultBak = TempMinTypeCardResult[i];
				}
			
			}
			tagOutCardResultNew  tempOutCardResult = {};
			bool zhiJieYing = false;
			int tempResultIndex = IsBiShengTurnCard(cbReserveCardData, cbReserveCardCount, TempMinTypeCardResult[i], tempOutCardResult, zhiJieYing);
			
			if ((tempResultIndex != -1) && bExistMax != true && tempType >= CT_BOMB_CARD)
			{
				BiShengResult.push_back(i);
			}

			if ((tempResultIndex != -1) && bExistMax != true && (tempMinTypeScore > BiShengScore) )
			{
				MinTypeScore = tempMinTypeScore;
				minTypeCount = tempMinTypeCount;
				BiShengScore = MinTypeScore;
				if (GetACardCount(BombData.data(), BombData.size(), SearchCardResult.cbResultCard[i][0])==0)
				{
					vecIndexUnIncludeBomb.push_back(i);
				}
				vecMinTypeCardResultBak = TempMinTypeCardResult[i];
				bExistBiYing = biYing;
				if (zhiJieYing )
				{
			/*		if (tempType == CT_THREE_TAKE_ONE || tempType == CT_THREE_TAKE_TWO)
					{

					}
					else*/
					{
						bExistBiYing = zhiJieYing;
					}
				}
				//break;
			}
			else if (biYing &&  bExistMax != true && (tempMinTypeScore > BiShengScore) )
			{
				MinTypeScore = tempMinTypeScore;
				minTypeCount = tempMinTypeCount;
				BiShengScore = MinTypeScore;
				if (GetACardCount(BombData.data(), BombData.size(), SearchCardResult.cbResultCard[i][0]) == 0)
				{
					vecIndexUnIncludeBomb.push_back(i);
				}
				vecMinTypeCardResultBak = TempMinTypeCardResult[i];
				bExistBiYing = biYing;
				if (zhiJieYing)
				{
					bExistBiYing = zhiJieYing;
				}
			}
			
			else if (tempMinTypeCount == 1 && bExistMax != true)
			{
				MinTypeScore = tempMinTypeScore;
				minTypeCount = tempMinTypeCount;
				resultIndex = i;
				vecMinTypeCardResultBak = TempMinTypeCardResult[i];
				break;
			}
			else if (m_cbUserCardCount[1] <= 2 && tempType == CT_SINGLE&&GetCardLogicValue(SearchCardResult.cbResultCard[i][0])==15)
			{
				MinTypeScore = tempMinTypeScore;
				minTypeCount = tempMinTypeCount;
				resultIndex = i;
				vecMinTypeCardResultBak = TempMinTypeCardResult[i];
			}
			else if (tempType == CT_SINGLE&&GetCardLogicValue(SearchCardResult.cbResultCard[i][0]) == 15 && WuDiCheck(cbReserveCardData, cbReserveCardCount, NULL, 0, OutCardResult))
			{
				MinTypeScore = tempMinTypeScore;
				minTypeCount = tempMinTypeCount;
				resultIndex = i;
				vecMinTypeCardResultBak = TempMinTypeCardResult[i];
				//假设2大
				if (GetACardCount(BombData.data(), BombData.size(), SearchCardResult.cbResultCard[i][0]) == 0)
				{
					vecIndexUnIncludeBomb.push_back(i);
					bExistBiYing = true;
				}
			}
			else if (tempType == CT_DOUBLE&&GetCardLogicValue(SearchCardResult.cbResultCard[i][0]) >= 13 && WuDiCheck(cbReserveCardData, cbReserveCardCount, NULL, 0, OutCardResult))
			{
				MinTypeScore = tempMinTypeScore;
				minTypeCount = tempMinTypeCount;
				resultIndex = i;
				vecMinTypeCardResultBak = TempMinTypeCardResult[i];
				//假设2大
				if (GetACardCount(BombData.data(), BombData.size(), SearchCardResult.cbResultCard[i][0]) == 0)
				{
					vecIndexUnIncludeBomb.push_back(i);
					bExistBiYing = true;
				}
			}

			
		
		}
	}
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	if ((resultIndex != -1))
	{
		 int tempCardType = GetCardType(SearchCardResult.cbResultCard[resultIndex], SearchCardResult.cbCardCount[resultIndex]);

		if (resultIndex != -1 && bExistBiYing&&vecIndexUnIncludeBomb.size() > 0)
		{
			int tempIndex = vecIndexUnIncludeBomb[vecIndexUnIncludeBomb.size() - 1];
			int reIndex = IsJueDuiDaPai(SearchCardResult.cbResultCard[tempIndex], SearchCardResult.cbCardCount[tempIndex], CT_SINGLE);
			if ((reIndex == XIANG_DUI_DA || reIndex == JUE_DUI_DA) && m_cbUserCardCount[1] != SearchCardResult.cbCardCount[resultIndex]&&bHaveBombType)
			{
				int a = 0;
			}
			else{
				resultIndex = vecIndexUnIncludeBomb[vecIndexUnIncludeBomb.size() - 1];
			}
		}
		else if (tempCardType < CT_BOMB_CARD&& BiShengResult.size()>0 && GetACardCount(BombData.data(), BombData.size(), SearchCardResult.cbResultCard[resultIndex][0]) > 0)
		{
			resultIndex = BiShengResult[0];
		}
		tempCardType = GetCardType(SearchCardResult.cbResultCard[resultIndex], SearchCardResult.cbCardCount[resultIndex]);
		if (tempCardType == CT_MISSILE_CARD&&bExistBiYing&&m_cbUserCardCount[1] >= 13)
		{
			resultIndex = -1;
		}
	}
	
	if ((resultIndex != -1) && bExistBiYing==false)
	{
		int tempCardType = GetCardType(SearchCardResult.cbResultCard[resultIndex], SearchCardResult.cbCardCount[resultIndex]);
		float tempCardScore = MinTypeScoreVec-tableScore[resultIndex];
		int vecMinTypeCardResultCount = vecMinTypeCardResult.size();
		int TempMinTypeCardResultCount = TempMinTypeCardResult[resultIndex].size();
		if (tempCardScore>20)
		{
			if (TempMinTypeCardResultCount - vecMinTypeCardResultCount>=1)
			{
				if (m_cbUserCardCount[1]>2)
				{
					resultIndex = -1;
				}
				else{

				}
			}
			if (tempCardType >= CT_BOMB_CARD && (m_cbUserCardCount[1] >= 10) || (m_cbUserCardCount[1]==1))
			{
				resultIndex = -1;
			}
		}
		else{
			if (tempCardType >= CT_BOMB_CARD)
			{
				
			}
			else
			{
				if (tempCardType==CT_SINGLE)
				{
					bExistBiYing= DanDingPaiCeLue(tempCardType, vecMinTypeCardResultBak, SearchCardResult, resultIndex, singleData, OutCardResult, bExistBiYing);

					if (bExistBiYing == false && vecMinTypeCardResultBak.size() == 2 && vecMinTypeCardResultBak[1].cbResultCard[0] == SearchCardResult.cbResultCard[resultIndex][0])
					{
						if (vecMinTypeCardResult[0].cbCardType == CT_SINGLE_LINE)
						{
							for (int i = vecMinTypeCardResultBak[0].cbCardCount - 1; i >= 0;i--)
							{
								if (GetCardLogicValue(vecMinTypeCardResultBak[0].cbResultCard[i])>GetCardLogicValue(vecMinTypeCardResultBak[1].cbResultCard[0]))
								{
									OutCardResult.cbCardCount = 1;
									OutCardResult.cbResultCard[0] = vecMinTypeCardResultBak[0].cbResultCard[i];
									return true;
								}
								
							}
						
						}
					}
					else if (bExistBiYing == false && vecMinTypeCardResultBak.size() == 1 && vecMinTypeCardResultBak[0].cbCardCount >3 )
					{
						if (vecMinTypeCardResult[0].cbCardType == CT_SINGLE_LINE)
						{
							if (GetCardLogicValue(vecMinTypeCardResultBak[0].cbResultCard[0]) > GetCardLogicValue(SearchCardResult.cbResultCard[resultIndex][0]))
							{
								OutCardResult.cbCardCount = 1;
								OutCardResult.cbResultCard[0] = vecMinTypeCardResultBak[0].cbResultCard[0];
								return true;
							}
						}
					}
				}else if (tempCardType==CT_DOUBLE)
				{
					bExistBiYing = ShuangDingPaiCeLue(tempCardType, vecMinTypeCardResultBak, SearchCardResult, resultIndex, doubleData, OutCardResult, bExistBiYing);
				}
				if (bExistBiYing&&OutCardResult.cbCardCount>0)
				{
					return bExistBiYing;
				}
					
			}
		}
		
	}

	if (resultIndex != -1)
	{
		{
			ZeroMemory(&OutCardResult, sizeof(OutCardResult));
			OutCardResult.cbCardCount = SearchCardResult.cbCardCount[resultIndex];
			CopyMemory(OutCardResult.cbResultCard, SearchCardResult.cbResultCard[resultIndex], OutCardResult.cbCardCount);
		}
		
	}
	return bExistBiYing;
}




bool CGameLogicNew::DanDingPaiCeLue(int tempCardType, vector<tagOutCardResultNew> &vecMinTypeCardResultBak, tagSearchCardResult &SearchCardResult, int resultIndex, vector<BYTE> &singleData, tagOutCardResultNew &OutCardResult, bool bExistBiYing)
{
	if (tempCardType == CT_SINGLE)
	{
		int count = 0;
		int index = 0;
		for (int i = 0; i < vecMinTypeCardResultBak.size(); i++)
		{
			if (vecMinTypeCardResultBak[i].cbCardType == CT_SINGLE)
			{
				count++;
				index = i;
			}
		}
		if (count == 1 && (GetCardLogicValue(SearchCardResult.cbResultCard[resultIndex][0]) != 14))
		{
			if (singleData.size() == 2 && GetCardValue(singleData[0]) == 2 && (GetCardLogicValue(singleData[1]) > GetCardLogicValue(SearchCardResult.cbResultCard[resultIndex][0])))
			{
				OutCardResult.cbCardCount = 1;
				CopyMemory(OutCardResult.cbResultCard, &singleData[1], OutCardResult.cbCardCount);
				return true;
			}
			/*else if ((vecMinTypeCardResultBak.size()==3)&&GetCardLogicValue(vecMinTypeCardResultBak[index].cbResultCard[0]) > GetCardLogicValue(SearchCardResult.cbResultCard[resultIndex][0]))
			{
			OutCardResult.cbCardCount = 1;
			CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResultBak[index].cbResultCard, OutCardResult.cbCardCount);
			return true;
			}*/
		}
		else if ( singleData.size() == 1)
		{
			if (IsJueDuiDaPai(&singleData[0], 1, CT_SINGLE)!=JUE_DUI_DA)
			{
				BYTE tmpTurnCard[MAX_COUNT] = { 0 };
				tmpTurnCard[0] = { SearchCardResult.cbResultCard[resultIndex][0] };
				int tmpTurnCardCount = 1;

				bool bExistMax = SearchOtherHandCardThan(tmpTurnCard, tmpTurnCardCount, true);


				if (bExistMax && (singleData.size() == 1) && (GetCardLogicValue(singleData[0]) > GetCardLogicValue(SearchCardResult.cbResultCard[resultIndex][0])))
				{
					OutCardResult.cbCardCount = 1;
					CopyMemory(OutCardResult.cbResultCard, &singleData[0], OutCardResult.cbCardCount);
					return true;
				}
			}
			
		}
		else if (singleData.size() == 2)
		{
			if (IsJueDuiDaPai(&singleData[0], 1, CT_SINGLE) == XIANG_DUI_DA)
			{
				if ((GetCardLogicValue(singleData[0]) > GetCardLogicValue(SearchCardResult.cbResultCard[resultIndex][0])))
				{
					OutCardResult.cbCardCount = 1;
					CopyMemory(OutCardResult.cbResultCard, &singleData[0], OutCardResult.cbCardCount);
					return true;
				}
				
			}
			/*else {
				OutCardResult.cbCardCount = 1;
				CopyMemory(OutCardResult.cbResultCard, &singleData[1], OutCardResult.cbCardCount);
				return true;
				}*/
		}
		else if (singleData.size() >= 3)
		{
			if(IsJueDuiDaPai(&singleData[0], 1, CT_SINGLE) == JUE_DUI_DA){
				if ((GetCardLogicValue(singleData[singleData.size() - 2]) > GetCardLogicValue(SearchCardResult.cbResultCard[resultIndex][0])))
				{
					OutCardResult.cbCardCount = 1;
					CopyMemory(OutCardResult.cbResultCard, &singleData[singleData.size() - 2], OutCardResult.cbCardCount);
				}
				return true;
			}
			else 
			{
				if ((GetCardLogicValue(singleData[1]) > GetCardLogicValue(SearchCardResult.cbResultCard[resultIndex][0])))
				{
					OutCardResult.cbCardCount = 1;
					CopyMemory(OutCardResult.cbResultCard, &singleData[1], OutCardResult.cbCardCount);
				}
			
				return true;
			}
			
		}
	}
	return false;
}

bool CGameLogicNew::ShuangDingPaiCeLue(int tempCardType, vector<tagOutCardResultNew> &vecMinTypeCardResultBak, tagSearchCardResult &SearchCardResult, int resultIndex, vector<BYTE> &singleData, tagOutCardResultNew &OutCardResult, bool bExistBiYing)
{
	if (tempCardType == CT_DOUBLE)
	{
		int count = 0;
		int index = 0;
		int maxDoubleLogic = 0;
		int maxDoubleIndex = 0;
		for (int i = 0; i < vecMinTypeCardResultBak.size(); i++)
		{
			if (vecMinTypeCardResultBak[i].cbCardType == CT_DOUBLE)
			{
				count++;
				index = i;
				if (GetCardLogicValue(vecMinTypeCardResultBak[i].cbResultCard[0])>maxDoubleLogic)
				{
					maxDoubleLogic = GetCardLogicValue(vecMinTypeCardResultBak[i].cbResultCard[0]);
					maxDoubleIndex = i;
				}
				

			}
		}
		if (count > 1 && (GetCardLogicValue(SearchCardResult.cbResultCard[resultIndex][0]) != 14))
		{
			if (singleData.size() == 4 && GetCardValue(singleData[0]) == 1 && (GetCardLogicValue(singleData[2]) > GetCardLogicValue(SearchCardResult.cbResultCard[resultIndex][0])))
			{
				OutCardResult.cbCardCount = 2;
				CopyMemory(OutCardResult.cbResultCard, &singleData[2], OutCardResult.cbCardCount);
				return true;
			}
			else if (GetCardLogicValue(vecMinTypeCardResultBak[index].cbResultCard[0]) > GetCardLogicValue(SearchCardResult.cbResultCard[resultIndex][0]))
			{
				OutCardResult.cbCardCount = 2;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResultBak[index].cbResultCard, OutCardResult.cbCardCount);
				return true;
			}
			else if (m_cbUserCardCount[1]==2)
			{
				OutCardResult.cbCardCount = 2;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResultBak[maxDoubleIndex].cbResultCard, OutCardResult.cbCardCount);
				return true;
			}
		}
		else if (count == 0 || (singleData.size() >= 1 && GetCardLogicValue(singleData[0])<=11))
		{
			if ((singleData.size() >= 1) && (GetCardLogicValue(singleData[0]) > GetCardLogicValue(SearchCardResult.cbResultCard[resultIndex][0])))
			{
				OutCardResult.cbCardCount = 2;
				CopyMemory(OutCardResult.cbResultCard, &singleData[0], OutCardResult.cbCardCount);
				return true;
			}
		}
	}
	return false;
}

bool CGameLogicNew::DoubleChaiFenCheck(vector<tagOutCardResultNew> &TempMinTypeCardResult, const BYTE * cbHandCardData, BYTE cbHandCardCount, WORD wOutCardUser)
{
	bool bExistMax = true;
	bool bXiangDuiDa = true;
	vector<tagCardIndex> vecSingleData;
	vector<tagCardIndex> vecDoubleData;
	for (int i = 0; i < TempMinTypeCardResult.size(); i++)
	{
		tagCardIndex  cardIndexSt;
		if (TempMinTypeCardResult[i].cbCardType == CT_SINGLE)
		{
			cardIndexSt.cbIndex = i;
			cardIndexSt.cbCardData = TempMinTypeCardResult[i].cbResultCard[0];
			vecSingleData.push_back(cardIndexSt);
		}
		if (TempMinTypeCardResult[i].cbCardType == CT_DOUBLE)
		{
			cardIndexSt.cbIndex = i;
			cardIndexSt.cbCardData = TempMinTypeCardResult[i].cbResultCard[0];
			vecDoubleData.push_back(cardIndexSt);
		}
	}
	bool bCheck = CheckBombPercent(cbHandCardData, cbHandCardCount, m_cbDiscardCard, m_cbDiscardCardCount);
	if (bCheck && vecDoubleData.size() == 1 && (vecDoubleData.size() * 2 >= vecSingleData.size()))
	{
		if (NoOutCardUserHandCardCountCheck(wOutCardUser, 3))
		{
			bExistMax = SearchOtherHandCardThan(&vecDoubleData[0].cbCardData, 1, true);
		}
		else
		{
			bExistMax = SearchOtherHandCardThan(&vecDoubleData[0].cbCardData, 1, false);
		}
		if (bExistMax == false)
		{
			bXiangDuiDa = SearchOtherHandCardSame(&vecDoubleData[0].cbCardData, 1, CT_SINGLE);
		}
	}
	if (bExistMax==false && bExistMax==false)
	{
		return true;
	}
	else{
		return false;
	}
}

bool CGameLogicNew::NoOutCardUserHandCardCountCheck(WORD wOutCardUser, BYTE cbCardCount, bool bXiaoYu, bool bYu, bool bOnlyDengYu)
{
	if (bXiaoYu)
	{
		if (m_wBankerUser != wOutCardUser&&m_cbUserCardCount[2] != 0)
		{
			if (bOnlyDengYu)
			{
				if (m_cbUserCardCount[m_wBankerUser] == cbCardCount)
				{
					return true;
				}
				else{
					return false;
				}
			}
			else if (m_cbUserCardCount[m_wBankerUser] <= cbCardCount)
			{
				return true;
			}
			else{
				return false;
			}
		}
		else
		{
			WORD wUndersideUser = (m_wBankerUser + 1) % GAME_PLAYER;
			WORD wUpsideUser = (wUndersideUser + 1) % GAME_PLAYER;

			if (m_cbUserCardCount[2] == 0)
			{
				wUpsideUser = wUndersideUser;
			}
			if (bOnlyDengYu)
			{
				if (m_cbUserCardCount[wUndersideUser] == cbCardCount||m_cbUserCardCount[wUpsideUser] == cbCardCount)
				{
					return true;
				}
				else{
					return false;
				}
			}
			if (bYu)
			{
				if (m_cbUserCardCount[wUndersideUser] <= cbCardCount&&m_cbUserCardCount[wUpsideUser] <= cbCardCount)
				{
					return true;
				}
			}
			else
			{
				if (m_cbUserCardCount[wUndersideUser] <= cbCardCount || m_cbUserCardCount[wUpsideUser] <= cbCardCount)
				{
					return true;
				}
			}
				return false;
		}
	}
	else
	{
		if (m_wBankerUser != wOutCardUser&&m_cbUserCardCount[2] != 0)
		{
			if (bOnlyDengYu)
			{
				if (m_cbUserCardCount[m_wBankerUser] == cbCardCount)
				{
					return true;
				}
				else{
					return false;
				}
			}
			if (m_cbUserCardCount[m_wBankerUser] >= cbCardCount)
			{
				return true;
			}
			else{
				return false;
			}
		}
		else
		{
			WORD wUndersideUser = (m_wBankerUser + 1) % GAME_PLAYER;
			WORD wUpsideUser = (wUndersideUser + 1) % GAME_PLAYER;

			if (m_cbUserCardCount[2] == 0)
			{
				wUpsideUser = wUndersideUser;
			}
			if (bOnlyDengYu)
			{
				if (m_cbUserCardCount[wUndersideUser] == cbCardCount||m_cbUserCardCount[wUpsideUser] == cbCardCount)
				{
					return true;
				}
				else{
					return false;
				}
			}
			if (bYu)
			{
				if (m_cbUserCardCount[wUndersideUser] >= cbCardCount&&m_cbUserCardCount[wUpsideUser] >= cbCardCount)
				{
					return true;
				}
			}
			else
			{
				if (m_cbUserCardCount[wUndersideUser] >= cbCardCount || m_cbUserCardCount[wUpsideUser] >= cbCardCount)
				{
					return true;
				}
			}

		}
	}

	return false;
}

vector<BYTE> CGameLogicNew::SearchOneOrTwoFromThreeTake(vector<tagOutCardResultNew> &vecMinTypeCardResult, BYTE cbOutCardType, tagOutCardResultNew &OutCardResult)
{
	vector<BYTE> vecResultData;
	for (int j = 0; j < vecMinTypeCardResult.size(); j++)
	{
		if (cbOutCardType == CT_SINGLE )
		{
			if (vecMinTypeCardResult[j].cbCardType == CT_THREE_TAKE_ONE)
			{
				for (int k = 0; k < vecMinTypeCardResult[j].cbCardCount / 4; k++)
				{
					vecResultData.push_back(vecMinTypeCardResult[j].cbResultCard[vecMinTypeCardResult[j].cbCardCount - 1 - k]);
				}
			}
			if (vecMinTypeCardResult[j].cbCardType == CT_THREE_TAKE_TWO)
			{
				for (int k = 0; k < vecMinTypeCardResult[j].cbCardCount / 5; k++)
				{
					BYTE firstCard = vecMinTypeCardResult[j].cbResultCard[vecMinTypeCardResult[j].cbCardCount - 1 - k];
					BYTE secondCard = vecMinTypeCardResult[j].cbResultCard[vecMinTypeCardResult[j].cbCardCount - 2 - k];
					if (GetCardLogicValue(firstCard) != GetCardLogicValue(secondCard))
					{
						vecResultData.push_back(firstCard);
						vecResultData.push_back(secondCard);
					}
				}
			}
		}
		if (cbOutCardType == CT_DOUBLE)
		{
			if (vecMinTypeCardResult[j].cbCardType == CT_THREE_TAKE_TWO)
			{
				for (int k = 0; k < vecMinTypeCardResult[j].cbCardCount / 5; k++)
				{
					BYTE firstCard = vecMinTypeCardResult[j].cbResultCard[vecMinTypeCardResult[j].cbCardCount - 1 - k];
					BYTE secondCard = vecMinTypeCardResult[j].cbResultCard[vecMinTypeCardResult[j].cbCardCount - 2 - k];
					if ( GetCardLogicValue(firstCard) == GetCardLogicValue(secondCard) )
					{
						vecResultData.push_back(firstCard);
						vecResultData.push_back(secondCard);
					}
				}
			}
		}

	}

	if (vecResultData.size()>0)
	{
		sort(vecResultData.begin(), vecResultData.end(), [this](BYTE first, BYTE second)
		{
			if (GetCardLogicValue(first) < GetCardLogicValue(second))
			{
				return true;
			}
			return false;
		});
		CopyMemory(OutCardResult.cbResultCard, &vecResultData[0], OutCardResult.cbCardCount);
	}
	
	return vecResultData;
}


//地主出牌（先出牌）
VOID CGameLogicNew::BankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagOutCardResultNew & OutCardResult)
{
	//零下标没用
	tagOutCardTypeResultNew CardTypeResult[CT_TYPE_COUNT];
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult));
	//初始变量
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	vector<tagOutCardResultNew>   vecMinTypeCardResult;
	float tempMinTypeScore = 0;
	bool biYing = false;
	int minTypeCount = FindCardKindMinNum(cbHandCardData, cbHandCardCount, CardTypeResult, vecMinTypeCardResult, tempMinTypeScore, biYing);
	if (FindMaxTypeTakeOneType(cbHandCardData, cbHandCardCount, 0,NULL, 0, vecMinTypeCardResult, OutCardResult))
	{
		FourTakeOneOrTwoFenChaiCheck(OutCardResult, vecMinTypeCardResult);
		return;
	}
	

	if (WuDiCheck(cbHandCardData, cbHandCardCount, NULL, 0, OutCardResult))
	{
		FourTakeOneOrTwoFenChaiCheck(OutCardResult, vecMinTypeCardResult);
		return;
	}

	bool bZhiJieChu = false;
	int outIndex = YouXianDaNengShouHuiCard(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult, bZhiJieChu);
	if (bZhiJieChu&&OutCardResult.cbCardCount>0)
	{
		FourTakeOneOrTwoFenChaiCheck(OutCardResult, vecMinTypeCardResult);
		return;
	}
	if (outIndex == -1)
	{
		outIndex = SearchMutilType(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult);
	}
	FourTakeOneOrTwoFenChaiCheck(OutCardResult, vecMinTypeCardResult);

	OutCardSpecialCheck(OutCardResult, vecMinTypeCardResult);

	WORD wUndersideUser = (m_wBankerUser + 1) % GAME_PLAYER;
	WORD wUpsideUser = (wUndersideUser + 1) % GAME_PLAYER;

	if ((m_cbUserCardCount[wUpsideUser] == CT_SINGLE || m_cbUserCardCount[wUndersideUser] == CT_SINGLE) &&  OutCardResult.cbCardCount == 1)
	{
		for (int i = vecMinTypeCardResult.size() - 1; i >0; i--)
		{
			if (vecMinTypeCardResult[i].cbCardType != CT_SINGLE)
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
				OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, OutCardResult.cbCardCount);
				break;
			}
		}
	}
	else if ((m_cbUserCardCount[wUpsideUser] == 2 || m_cbUserCardCount[wUndersideUser] == 2) && OutCardResult.cbCardCount==2)
	{
		for (int i = vecMinTypeCardResult.size()-1; i >=0; i--)
		{
			if (vecMinTypeCardResult[i].cbCardType != CT_DOUBLE)
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
				OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, OutCardResult.cbCardCount);
				break;
			}
		}
	}
	//出牌出倒数第二小的策略
	OutCardDaoShuDiErDaCheck(OutCardResult, vecMinTypeCardResult);

	
	if (OutCardResult.cbCardCount == 0)
	{
		for (int k = 0; k < CT_TYPE_COUNT; k++)
		{
			for (BYTE i = 0; i < CardTypeResult[k].cbCardTypeCount; ++i)
			{

				OutCardResult.cbCardCount = CardTypeResult[k].cbEachHandCardCount[i];
				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[k].cbCardData[i], CardTypeResult[k].cbEachHandCardCount[i]);
				break;
			}
		}
	}
	//当两个人中有其中一人手牌小于等于1了,那么地主必须拆牌来压了
	if (OutCardResult.cbCardCount == 1 && (m_cbUserCardCount[wUndersideUser] == 1))
	{
		OutCardResult.cbResultCard[0] = cbHandCardData[0];
	}

	return;
}

//地主出牌（后出牌）
VOID CGameLogicNew::BankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wOutCardUser, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResultNew & OutCardResult)
{
	//初始变量
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	//零下标没用
	tagOutCardTypeResultNew CardTypeResult[CT_TYPE_COUNT];
	ZeroMemory(&CardTypeResult, sizeof(CardTypeResult));

	vector<tagOutCardResultNew>   vecMinTypeCardResult;
	//出牌类型
	BYTE cbOutCardType = GetCardType(cbTurnCardData, cbTurnCardCount);
	float tempMinTypeScore = 0;
	bool biYing = false;
	int minTypeCount = FindCardKindMinNum(cbHandCardData, cbHandCardCount, CardTypeResult, vecMinTypeCardResult, tempMinTypeScore, biYing);

	if (FindMaxTypeTakeOneType(cbHandCardData, cbHandCardCount, cbOutCardType, cbTurnCardData, cbTurnCardCount, vecMinTypeCardResult, OutCardResult))
	{
		return;
	}
	if (minTypeCount == 1 && CompareCard(cbTurnCardData, cbHandCardData, cbTurnCardCount, cbHandCardCount))
	{
		OutCardResult.cbCardCount = vecMinTypeCardResult[0].cbCardCount;
		CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[0].cbResultCard, OutCardResult.cbCardCount);
		return;
	}


	if (WuDiCheck(cbHandCardData, cbHandCardCount, cbTurnCardData, cbTurnCardCount, OutCardResult))
	{
		return;
	}

	bool bExistBiYing = OutCardShengYuFenCheck(cbHandCardCount, cbHandCardData, cbTurnCardData, cbTurnCardCount, wOutCardUser, tempMinTypeScore, vecMinTypeCardResult, OutCardResult);
	if (bExistBiYing && OutCardResult.cbCardCount != 0)
	{
		return;
	}
	//最大牌
	BYTE cbTurnCardLogic = GetCardLogicValue(cbTurnCardData[0]);
	WORD wUndersideUser = (m_wBankerUser + 1) % GAME_PLAYER;
	WORD wUpsideUser = (wUndersideUser + 1) % GAME_PLAYER;
	if (cbOutCardType == CT_SINGLE && OutCardResult.cbCardCount == 0 && (m_cbUserCardCount[wUndersideUser] <= 1 || m_cbUserCardCount[wUpsideUser] <= 1))
	{
		if (OutCardResult.cbCardCount == 0)
		{
			for (int i = 0; i < CardTypeResult[CT_THREE].cbCardTypeCount; i++)
			{
				if (GetCardLogicValue(CardTypeResult[CT_THREE].cbCardData[i][0]) >= cbTurnCardLogic)
				{
					OutCardResult.cbCardCount = 1;
					CopyMemory(OutCardResult.cbResultCard, CardTypeResult[CT_THREE].cbCardData[i], OutCardResult.cbCardCount);
					break;
				}
			}
		}

	}
	if (cbOutCardType == CT_DOUBLE && OutCardResult.cbCardCount == 0 &&  (m_cbUserCardCount[wUndersideUser] <= 2 || m_cbUserCardCount[wUpsideUser] <= 2))
	{
		if (OutCardResult.cbCardCount == 0)
		{
			for (int i = 0; i < CardTypeResult[CT_THREE].cbCardTypeCount; i++)
			{
				if (GetCardLogicValue(CardTypeResult[CT_THREE].cbCardData[i][0]) > cbTurnCardLogic)
				{
					OutCardResult.cbCardCount = 2;
					CopyMemory(OutCardResult.cbResultCard, CardTypeResult[CT_THREE].cbCardData[i], OutCardResult.cbCardCount);
					break;
				}
			}
		}

	}
	return;
}

void CGameLogicNew::FourTakeOneOrTwoFenChaiCheck(tagOutCardResultNew &OutCardResult, vector<tagOutCardResultNew> &vecMinTypeCardResult)
{
	if ( vecMinTypeCardResult.size() <= 3)
	{
		BYTE tempCard[MAX_COUNT] = { 0 };
		BYTE tempCardCount = 0;
		
		for (int j = 0; j < vecMinTypeCardResult.size(); j++)
		{
			if (vecMinTypeCardResult[j].cbCardType == CT_FOUR_TAKE_ONE || vecMinTypeCardResult[j].cbCardType == CT_FOUR_TAKE_TWO)
			{
				CopyMemory(tempCard, vecMinTypeCardResult[j].cbResultCard, vecMinTypeCardResult[j].cbCardCount);
				tempCardCount = vecMinTypeCardResult[j].cbCardCount;
				break;
			}
		}
		BYTE tempType = GetCardType(tempCard, tempCardCount);

		if (tempCardCount>0)
		{
			bool bExistMax = true;
			bool bXiangDuiDa = true;
			BYTE cbNeedType = CT_SINGLE;
			if (tempType == CT_FOUR_TAKE_TWO)
			{
				cbNeedType = CT_DOUBLE;
			}
		
			for (int j = 0; j < vecMinTypeCardResult.size(); j++)
			{
				if (vecMinTypeCardResult[j].cbCardType == cbNeedType)
				{
					bExistMax = SearchOtherHandCardThan(vecMinTypeCardResult[j].cbResultCard, vecMinTypeCardResult[j].cbCardCount, true);
					if (bExistMax == false)
					{
						bXiangDuiDa = SearchOtherHandCardSame(vecMinTypeCardResult[j].cbResultCard, vecMinTypeCardResult[j].cbCardCount, vecMinTypeCardResult[j].cbCardType);
					}
					break;
				}
			}
			bool bExistBoomMaxE = false;
			bExistBoomMaxE = SearchOtherHandCardThan(tempCard, 4, false);
			if (bExistBoomMaxE == false)
			{
				if (bExistMax == false && bXiangDuiDa == false )
				{
					if (m_wMeChairID == m_wBankerUser)
					{
						WORD wUndersideUser = (m_wBankerUser + 1) % GAME_PLAYER;
						WORD wUpsideUser = (wUndersideUser + 1) % GAME_PLAYER;
						if (m_cbUserCardCount[wUndersideUser] > cbNeedType && m_cbUserCardCount[wUpsideUser] > cbNeedType)
						{
							ZeroMemory(&OutCardResult, sizeof(OutCardResult));
							OutCardResult.cbCardCount = cbNeedType;
							CopyMemory(OutCardResult.cbResultCard, tempCard + 4, OutCardResult.cbCardCount);
						}
					}
					else
					{
						if (m_cbUserCardCount[m_wBankerUser] > cbNeedType)
						{
							ZeroMemory(&OutCardResult, sizeof(OutCardResult));
							OutCardResult.cbCardCount = cbNeedType;
							CopyMemory(OutCardResult.cbResultCard, tempCard + 4, OutCardResult.cbCardCount);
						}
					}
				}
			}
		}
	}
}

void CGameLogicNew::OutCardDaoShuDiErDaCheck(tagOutCardResultNew &OutCardResult, vector<tagOutCardResultNew> &vecMinTypeCardResult)
{
	if (OutCardResult.cbCardCount == 1)
	{
		vector<BYTE> vecResultData;
		for (int i = vecMinTypeCardResult.size() - 1; i >= 0; i--)
		{
			if (vecMinTypeCardResult[i].cbCardType == CT_SINGLE)
			{
				vecResultData.push_back(vecMinTypeCardResult[i].cbResultCard[0]);
			}
		}
		if (vecResultData.size() >= 3 && GetCardLogicValue(vecResultData[1])<=14)
		{
			OutCardResult.cbCardCount = 1;
			CopyMemory(OutCardResult.cbResultCard, &vecResultData[1], OutCardResult.cbCardCount);
		}
	}
}



void CGameLogicNew::OutSingleOrDoubleMinCard(const BYTE * cbHandCardData, BYTE cbHandCardCount, vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew &OutCardResult, int type)
{
	vector<BYTE> vecResultData;
	int cardCount = 1;
	if (type == CT_DOUBLE)
	{
		cardCount = 2;
	}
	vecResultData = SearchOneOrTwoFromThreeTake(vecMinTypeCardResult, type, OutCardResult);
	for (int i = 0; i < vecMinTypeCardResult.size(); i++)
	{
		if (vecMinTypeCardResult[i].cbCardType == type)
		{
			for (int j = 0; j < cardCount; j++)
			{
				vecResultData.push_back(vecMinTypeCardResult[i].cbResultCard[j]);
			}
		}
		else if (vecMinTypeCardResult[i].cbCardType == CT_FOUR_TAKE_ONE)
		{
				if (vecMinTypeCardResult[i].cbResultCard[4] != vecMinTypeCardResult[i].cbResultCard[5] && type == CT_SINGLE)
				{
					vecResultData.push_back(vecMinTypeCardResult[i].cbResultCard[4]);
					vecResultData.push_back(vecMinTypeCardResult[i].cbResultCard[5]);
				}
				else if (vecMinTypeCardResult[i].cbResultCard[4] == vecMinTypeCardResult[i].cbResultCard[5] && type == CT_DOUBLE)
				{
					vecResultData.push_back(vecMinTypeCardResult[i].cbResultCard[4]);
					vecResultData.push_back(vecMinTypeCardResult[i].cbResultCard[5]);
				}
			
		}
		else if (vecMinTypeCardResult[i].cbCardType == CT_FOUR_TAKE_TWO)
		{
				vecResultData.push_back(vecMinTypeCardResult[i].cbResultCard[4]);
				vecResultData.push_back(vecMinTypeCardResult[i].cbResultCard[5]);
				vecResultData.push_back(vecMinTypeCardResult[i].cbResultCard[6]);
				vecResultData.push_back(vecMinTypeCardResult[i].cbResultCard[7]);
		}
		else if (vecMinTypeCardResult[i].cbCardType == CT_THREE_TAKE_TWO)
		{
			int tempCount = vecMinTypeCardResult[i].cbCardCount / 5;
			for (int j = 0; j < tempCount*2;j++)
			{
				vecResultData.push_back(vecMinTypeCardResult[i].cbResultCard[tempCount*3+j]);
			}
		}
			
	}
	// --防止单牌从三带一中出去
	if (type == CT_SINGLE)
	{
		vector<BYTE>::iterator itor = vecResultData.begin();
		while (itor != vecResultData.end())
		{
			if (GetACardCount(cbHandCardData, cbHandCardCount, *itor) >= 3)
			{
				itor = vecResultData.erase(itor);
			}
			else{
				itor++;
			}
		}
	}

	if (vecResultData.size() > 1)
	{
		sort(vecResultData.begin(), vecResultData.end(), [this](BYTE first, BYTE second)
		{
			if (GetCardLogicValue(first) < GetCardLogicValue(second))
			{
				return true;
			}
			return false;
		});
		CopyMemory(OutCardResult.cbResultCard, &vecResultData[0], OutCardResult.cbCardCount);
	}
}

bool CGameLogicNew::FindFirstType(const BYTE cbHandCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew> & vecMinTypeCardResult, tagOutCardResultNew &OutCardResult)
{
	if (m_cbFirstCard>0&&cbHandCardCount==10)
	{
		for (int i = 0; i < vecMinTypeCardResult.size();i++)
		{
			for (int j = 0; j < vecMinTypeCardResult[i].cbCardCount;j++)
			{
				if (vecMinTypeCardResult[i].cbResultCard[j]==GetCardLogicValue(m_cbFirstCard))
				{
					OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
					CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, vecMinTypeCardResult[i].cbCardCount);
					return true;
				}
			}
		}
	}
	return false;
}

void CGameLogicNew::OutCardSpecialCheck(tagOutCardResultNew &OutCardResult, vector<tagOutCardResultNew> vecMinTypeCardResult)
{
	BYTE  tempType = GetCardType(OutCardResult.cbResultCard, OutCardResult.cbCardCount);
	if (tempType == CT_DOUBLE )
	{
		SearchOneOrTwoFromThreeTake(vecMinTypeCardResult, tempType, OutCardResult);
	}
	/*else if (tempType == CT_THREE_TAKE_ONE || tempType == CT_THREE_TAKE_TWO)
	{
		SwitchOneOrTwoFromThreeTake(vecMinTypeCardResult, tempType, OutCardResult);
	}*/

}

bool CGameLogicNew::WuDiCheck(const BYTE * cbHandCardData, BYTE cbHandCardCount, const BYTE * cbTurnCardData, BYTE cbTurnCardCount, tagOutCardResultNew &OutCardResult, tagOutCardTypeResultNew *CardTypeResult)
{
	tagSearchCardResult SearchCardResult;
	{
		SearchOutCard(cbHandCardData, cbHandCardCount, cbTurnCardData, cbTurnCardCount, &SearchCardResult);
	}
	
	for (int i = 0; i < SearchCardResult.cbSearchCount; i++)
	{
		bool	bExistMax = SearchOtherHandCardThan(SearchCardResult.cbResultCard[i], SearchCardResult.cbCardCount[i], false);
		if (bExistMax == false)
		{
			if (CheckOutOneTypeWillWin(cbHandCardData, cbHandCardCount, SearchCardResult.cbResultCard[i], SearchCardResult.cbCardCount[i], OutCardResult))
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
				OutCardResult.cbCardCount = SearchCardResult.cbCardCount[i];
				CopyMemory(OutCardResult.cbResultCard, SearchCardResult.cbResultCard[i], OutCardResult.cbCardCount);
				return true;
			}
		}
		else if (SearchCardResult.cbCardCount[i] >= m_cbUserCardCount[0]){
			ZeroMemory(&OutCardResult, sizeof(OutCardResult));
			OutCardResult.cbCardCount = SearchCardResult.cbCardCount[i];
			CopyMemory(OutCardResult.cbResultCard, SearchCardResult.cbResultCard[i], OutCardResult.cbCardCount);
			return true;
		}
	}
	

	return false;
}

bool CGameLogicNew::CheckOutOneTypeWillWin(const BYTE * cbHandCardData, BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResultNew &OutCardResult)
{
	BYTE cbReserveCardData[MAX_COUNT] = { 0 };
	BYTE cbReserveCardCount = cbHandCardCount;
	CopyMemory(cbReserveCardData, cbHandCardData, cbReserveCardCount);
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	if (cbTurnCardCount>0)
	{
		RemoveCard(
			cbTurnCardData,
			cbTurnCardCount,
			cbReserveCardData,
			cbReserveCardCount
			);
	}
	cbReserveCardCount -= cbTurnCardCount;

	{
		BYTE tmpTurnCardData[MAX_COUNT] = { 0 };
		BYTE tmpTurnCardCount = 0;
		tagSearchCardResult SearchCardResult;
		SearchOutCard(cbReserveCardData, cbReserveCardCount, tmpTurnCardData, tmpTurnCardCount, &SearchCardResult);
		for (int i = 0; i < SearchCardResult.cbSearchCount; i++)
		{
			if (SearchCardResult.cbCardCount[i] >= (cbReserveCardCount-m_cbBeiRangCount ))
			{
				OutCardResult.cbCardCount = SearchCardResult.cbCardCount[i];
				CopyMemory(OutCardResult.cbResultCard, SearchCardResult.cbResultCard[i], OutCardResult.cbCardCount);
				return true;
			}
		}
	}
	
	return false;
}

//地主上家（先出牌）
VOID CGameLogicNew::UpsideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID, tagOutCardResultNew & OutCardResult)
{
	//零下标没用
	tagOutCardTypeResultNew CardTypeResult[CT_TYPE_COUNT];
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult));
	//初始变量
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	vector<tagOutCardResultNew>   vecMinTypeCardResult;
	float tempMinTypeScore = 0;
	bool biYing = false;
	int minTypeCount = FindCardKindMinNum(cbHandCardData, cbHandCardCount, CardTypeResult, vecMinTypeCardResult, tempMinTypeScore, biYing);
	if (FindMaxTypeTakeOneType(cbHandCardData, cbHandCardCount, 0,0, 0, vecMinTypeCardResult, OutCardResult))
	{
		FourTakeOneOrTwoFenChaiCheck(OutCardResult, vecMinTypeCardResult);
		return;
	}
	if (WuDiCheck(cbHandCardData, cbHandCardCount, NULL, 0, OutCardResult))
	{
		FourTakeOneOrTwoFenChaiCheck(OutCardResult, vecMinTypeCardResult);
		return;
	}

	bool bZhiJieChu = false;
	int outIndex = YouXianDaNengShouHuiCard(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult, bZhiJieChu);
	if (bZhiJieChu&&OutCardResult.cbCardCount>0)
	{
		FourTakeOneOrTwoFenChaiCheck(OutCardResult, vecMinTypeCardResult);
		return;
	}
	if (outIndex == -1 || (m_cbUserCardCount[m_wBankerUser] == 1 && OutCardResult.cbCardCount==1))
	{
		outIndex = SearchMutilType(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult);
	}
	FourTakeOneOrTwoFenChaiCheck(OutCardResult, vecMinTypeCardResult);
	OutCardSpecialCheck(OutCardResult, vecMinTypeCardResult);
	//出牌出倒数第二小的策略
	OutCardDaoShuDiErDaCheck(OutCardResult, vecMinTypeCardResult);
	if ((m_cbUserCardCount[m_wBankerUser] == 1 && OutCardResult.cbCardCount == 1))
	{
		OutCardResult.cbCardCount = 1;
		OutCardResult.cbResultCard[0] = cbHandCardData[0];
		return;
	}
		return ;
}

//地主上家（后出牌）
VOID CGameLogicNew::UpsideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID, WORD wOutCardUser, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResultNew & OutCardResult)
{

	//零下标没用
	tagOutCardTypeResultNew CardTypeResult[CT_TYPE_COUNT];
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult)) ;
	//初始变量
	ZeroMemory(&OutCardResult, sizeof(OutCardResult)) ;
	//出牌类型
	BYTE cbOutCardType = GetCardType(cbTurnCardData, cbTurnCardCount) ;
	BYTE cbBankerType = GetCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser]);

	BYTE cbTurnCardLogic = GetCardLogicValue(cbTurnCardData[0]);
	BYTE cbBankerLargestLogic = GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]);
	vector<tagOutCardResultNew>   vecMinTypeCardResult;
	float tempMinTypeScore = 0;
	bool biYing = false;
	int minTypeCount = FindCardKindMinNum(cbHandCardData, cbHandCardCount, CardTypeResult, vecMinTypeCardResult, tempMinTypeScore, biYing);

	if (FindMaxTypeTakeOneType(cbHandCardData, cbHandCardCount, cbOutCardType, cbTurnCardData, cbTurnCardCount, vecMinTypeCardResult, OutCardResult))
	{
		return;
	}
	if (minTypeCount == 1 && CompareCard(cbTurnCardData, cbHandCardData, cbTurnCardCount, cbHandCardCount))
	{
		OutCardResult.cbCardCount = vecMinTypeCardResult[0].cbCardCount;
		CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[0].cbResultCard, OutCardResult.cbCardCount);
		return;
	}
	if (WuDiCheck(cbHandCardData, cbHandCardCount, cbTurnCardData, cbTurnCardCount, OutCardResult))
	{
		return;
	}
	bool bExistBiYing = OutCardShengYuFenCheck(cbHandCardCount, cbHandCardData, cbTurnCardData, cbTurnCardCount, wOutCardUser, tempMinTypeScore, vecMinTypeCardResult, OutCardResult);
	if (bExistBiYing && OutCardResult.cbCardCount != 0)
	{
		return;
	}
	vector<tagCardIndex> vecSingleData;
	vector<tagCardIndex> vecDoubleData;
	for (int i = 0; i < vecMinTypeCardResult.size(); i++)
	{
		tagCardIndex  cardIndexSt;
		if (vecMinTypeCardResult[i].cbCardType == CT_SINGLE)
		{
			cardIndexSt.cbIndex = i;
			cardIndexSt.cbCardData = vecMinTypeCardResult[i].cbResultCard[0];
			vecSingleData.push_back(cardIndexSt);
		}
		if (vecMinTypeCardResult[i].cbCardType == CT_DOUBLE)
		{
			cardIndexSt.cbIndex = i;
			cardIndexSt.cbCardData = vecMinTypeCardResult[i].cbResultCard[0];
			vecDoubleData.push_back(cardIndexSt);
		}
	}
	if (m_wBankerUser != wOutCardUser)
	{
		//队友出牌
		if (cbOutCardType>=CT_SINGLE_LINE)
		{
			ZeroMemory(&OutCardResult, sizeof(OutCardResult));
		}
		else if (cbOutCardType == CT_SINGLE)
		{
			if ( cbTurnCardLogic<12)
			{
				int count = 0;
				BYTE tempLogic = 12;
				while (count<3)
				{
					count++;
					tempLogic = (cbTurnCardLogic > tempLogic ? cbTurnCardLogic : tempLogic);
					CommonSDTurnCardSearch(vecMinTypeCardResult, tempLogic, OutCardResult);
					if (OutCardResult.cbCardCount == 1)
					{
						break;
					}
					tempLogic -= 3;
				}

			}
			else
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
			}
		}
		else if (cbOutCardType == CT_DOUBLE)
		{
			
		
			if (cbTurnCardLogic>13)
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
			}
			else if (cbTurnCardLogic >= 8 && vecSingleData.size()>1)
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
			}
			else if (cbHandCardCount > 10 && cbTurnCardLogic >= 11)
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
			}
			else if (m_cbUserCardCount[m_wBankerUser]<=2)
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
			}
		}
	}
	else
	{
		//地主出牌
		if (cbOutCardType == CT_SINGLE)
		{
			{
				int count = 0;
				BYTE tempLogic = 12;
				while (count<3)
				{
					count++;
					tempLogic = (cbTurnCardLogic > tempLogic ? cbTurnCardLogic : tempLogic);
					CommonSDTurnCardSearch(vecMinTypeCardResult, tempLogic, OutCardResult);
					if (OutCardResult.cbCardCount == 1)
					{
						break;
					}
					tempLogic -= 3;
				}

			}
		}
	}
	//队友出牌报单不能压牌
	if (m_cbUserCardCount[wOutCardUser] == 1 && m_wBankerUser != wOutCardUser)
	{
		ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	}
	if ((m_cbUserCardCount[m_wBankerUser] == 1 && OutCardResult.cbCardCount == 1))
	{
		OutCardResult.cbCardCount = 1;
		OutCardResult.cbResultCard[0] = cbHandCardData[0];
		return;
	}
	return ;
}



//地主下家（先出牌）
VOID CGameLogicNew::UndersideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID,tagOutCardResultNew & OutCardResult) 
{

	//零下标没用
	tagOutCardTypeResultNew CardTypeResult[CT_TYPE_COUNT];
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult));
	//初始变量
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	vector<tagOutCardResultNew>   vecMinTypeCardResult;
	float tempMinTypeScore = 0;
	bool biYing = false;
	int minTypeCount = FindCardKindMinNum(cbHandCardData, cbHandCardCount, CardTypeResult, vecMinTypeCardResult, tempMinTypeScore, biYing);
	if (FindMaxTypeTakeOneType(cbHandCardData, cbHandCardCount, 0, 0,0, vecMinTypeCardResult, OutCardResult))
	{
		FourTakeOneOrTwoFenChaiCheck(OutCardResult, vecMinTypeCardResult);
		return;
	}
	if (WuDiCheck(cbHandCardData, cbHandCardCount, NULL, 0, OutCardResult))
	{
		FourTakeOneOrTwoFenChaiCheck(OutCardResult, vecMinTypeCardResult);
		return;
	}
	bool bZhiJieChu = false;
	int outIndex = YouXianDaNengShouHuiCard(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult, bZhiJieChu);
	if (bZhiJieChu&&OutCardResult.cbCardCount>0)
	{
		FourTakeOneOrTwoFenChaiCheck(OutCardResult, vecMinTypeCardResult);
		return;
	}
	
	if (outIndex == -1)
	{
		outIndex = SearchMutilType(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult);
	}
	FourTakeOneOrTwoFenChaiCheck(OutCardResult, vecMinTypeCardResult);
	OutCardSpecialCheck(OutCardResult, vecMinTypeCardResult);

	//出牌出倒数第二小的策略
	OutCardDaoShuDiErDaCheck(OutCardResult, vecMinTypeCardResult);

	WORD wUndersideUser = (m_wBankerUser + 1) % GAME_PLAYER;
	WORD wUpsideUser = (wUndersideUser + 1) % GAME_PLAYER;
	//下家报单特殊处理
	if (m_cbUserCardCount[wUpsideUser] == 1)
	{
		    ZeroMemory(&OutCardResult, sizeof(OutCardResult));
			OutCardResult.cbCardCount = 1;
			OutCardResult.cbResultCard[0] = cbHandCardData[cbHandCardCount - 1];
			return;
	}
	
		return ;
}
//地主下家（后出牌）
VOID CGameLogicNew::UndersideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID, WORD wOutCardUser, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResultNew & OutCardResult)
{
	//零下标没用
	tagOutCardTypeResultNew CardTypeResult[CT_TYPE_COUNT];
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult));
	//初始变量
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	vector<tagOutCardResultNew>   vecMinTypeCardResult;
	//出牌类型
	BYTE cbOutCardType = GetCardType(cbTurnCardData, cbTurnCardCount);
	float tempMinTypeScore = 0;
	bool biYing = false;
	int minTypeCount = FindCardKindMinNum(cbHandCardData, cbHandCardCount, CardTypeResult, vecMinTypeCardResult,tempMinTypeScore,biYing);
	if (FindMaxTypeTakeOneType(cbHandCardData, cbHandCardCount, cbOutCardType, cbTurnCardData, cbTurnCardCount, vecMinTypeCardResult, OutCardResult))
	{
		return;
	}
	if (minTypeCount == 1 && CompareCard(cbTurnCardData, cbHandCardData, cbTurnCardCount, cbHandCardCount))
	{
		OutCardResult.cbCardCount = vecMinTypeCardResult[0].cbCardCount;
		CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[0].cbResultCard, OutCardResult.cbCardCount);
		return;
	}
	if (WuDiCheck(cbHandCardData, cbHandCardCount, cbTurnCardData, cbTurnCardCount, OutCardResult))
	{
		return;
	}
	bool bExistBiYing = OutCardShengYuFenCheck(cbHandCardCount, cbHandCardData, cbTurnCardData, cbTurnCardCount, wOutCardUser, tempMinTypeScore, vecMinTypeCardResult, OutCardResult);
	if (bExistBiYing && OutCardResult.cbCardCount != 0)
	{
		return;
	}

	BYTE cbBankerType = GetCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser]);

	BYTE cbTurnCardLogic = GetCardLogicValue(cbTurnCardData[0]);

	if (m_wBankerUser != wOutCardUser)
	{
		if (cbOutCardType >= CT_SINGLE_LINE)
		{
			ZeroMemory(&OutCardResult, sizeof(OutCardResult));
		}
	}
	else
	{
	}
	//队友出牌报单不能压牌
	if (m_cbUserCardCount[wOutCardUser] == 1 && m_wBankerUser != wOutCardUser)
	{
		ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	}

	return ;
}


bool CGameLogicNew::FindMaxTypeTakeOneType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardType, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, vector<tagOutCardResultNew> & vecMinTypeCardResult, tagOutCardResultNew &OutCardResult)
{
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	if (vecMinTypeCardResult.size() != 2)
	{
			int type = 0;
			int index = -1;
			for (int i = 0; i < vecMinTypeCardResult.size(); i++)
			{
				bool bExistMax = false;
				type = vecMinTypeCardResult[i].cbCardType;
				bExistMax = SearchOtherHandCardThan(vecMinTypeCardResult[i].cbResultCard, vecMinTypeCardResult[i].cbCardCount, true);
				if (bExistMax == false)
				{
					if (index != -1)
					{
						if (vecMinTypeCardResult[i].cbCardCount < m_cbUserCardCount[0])
						{
							index = i;
						}
					}
					else{
						index = i;
					}
				}
			}
			if (OutCardResult.cbCardCount == 0 && cbTurnCardCount == 0)
			{
				for (int i = 0; i < vecMinTypeCardResult.size(); i++)
				{
					if (vecMinTypeCardResult[i].cbCardCount >= m_cbUserCardCount[0])
					{
						int tmpIndex = i;
						if (index != -1)
						{
							tmpIndex = index;
						}
						
						if (FourTakeFenChai(vecMinTypeCardResult[tmpIndex].cbResultCard, vecMinTypeCardResult[tmpIndex].cbCardCount, vecMinTypeCardResult, OutCardResult))
						{
						}
						else{
							OutCardResult.cbCardCount = vecMinTypeCardResult[tmpIndex].cbCardCount;
							CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[tmpIndex].cbResultCard, OutCardResult.cbCardCount);
						}
							
						return true;
					}
				}
			}
		
		return false;
	}
	else{
		
		if ((vecMinTypeCardResult[0].cbCardType == CT_MISSILE_CARD || vecMinTypeCardResult[0].cbCardType == CT_BOMB_CARD) && vecMinTypeCardResult[1].cbCardType == CT_FOUR_TAKE_ONE)
		{
			if (vecMinTypeCardResult[1].cbResultCard[4] != vecMinTypeCardResult[1].cbResultCard[5])
			{
				if (m_cbUserCardCount[1]==1)
				{
					OutCardResult.cbCardCount = vecMinTypeCardResult[1].cbCardCount;
					CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[1].cbResultCard, OutCardResult.cbCardCount);
				}
				else{
					OutCardResult.cbCardCount = 1;
					CopyMemory(OutCardResult.cbResultCard, &vecMinTypeCardResult[1].cbResultCard[4], OutCardResult.cbCardCount);
				}
			}
			else{
				OutCardResult.cbCardCount = 4;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[1].cbResultCard, OutCardResult.cbCardCount);
			}
			return true;
		}
		else if ((vecMinTypeCardResult[0].cbCardType == CT_MISSILE_CARD || vecMinTypeCardResult[0].cbCardType == CT_BOMB_CARD) && vecMinTypeCardResult[1].cbCardType == CT_FOUR_TAKE_TWO)
		{
				if (m_cbUserCardCount[1] == 2)
				{
					OutCardResult.cbCardCount = vecMinTypeCardResult[1].cbCardCount;
					CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[1].cbResultCard, OutCardResult.cbCardCount);
				}
				else{
					OutCardResult.cbCardCount = 2;
					CopyMemory(OutCardResult.cbResultCard, &vecMinTypeCardResult[1].cbResultCard[4], OutCardResult.cbCardCount);
				}
				return true;
		}
		for (int i = 0; i < vecMinTypeCardResult.size(); i++)
		{
			if (vecMinTypeCardResult[i].cbCardType == CT_BOMB_CARD || vecMinTypeCardResult[i].cbCardType == CT_MISSILE_CARD || cbTurnCardType == 0)
			{
				BYTE tmpTurnCard[MAX_COUNT] = { 0 };
				CopyMemory(tmpTurnCard, vecMinTypeCardResult[i].cbResultCard, vecMinTypeCardResult[i].cbCardCount);
				bool bExistMax = SearchOtherHandCardThan(tmpTurnCard, vecMinTypeCardResult[i].cbCardCount, false);
				if (bExistMax == false)
				{
					ZeroMemory(&OutCardResult, sizeof(OutCardResult));
					if (FourTakeFenChai(vecMinTypeCardResult[i].cbResultCard, vecMinTypeCardResult[i].cbCardCount, vecMinTypeCardResult, OutCardResult))
					{
					}
					else{
						OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
						CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, OutCardResult.cbCardCount);
					}
					return true;
				}
			}
			else if ((vecMinTypeCardResult[i].cbCardType == cbTurnCardType) &&
				vecMinTypeCardResult[i].cbCardCount == cbTurnCardCount && (GetCardLogicValue(cbTurnCardData[0]) < GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0])))
			{
				BYTE tmpTurnCard[MAX_COUNT] = { 0 };
				CopyMemory(tmpTurnCard, vecMinTypeCardResult[i].cbResultCard, vecMinTypeCardResult[i].cbCardCount);
				bool bExistMax = SearchOtherHandCardThan(tmpTurnCard, vecMinTypeCardResult[i].cbCardCount, false);
				if (bExistMax == false)
				{
					ZeroMemory(&OutCardResult, sizeof(OutCardResult));
					OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
					CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, OutCardResult.cbCardCount);
					return true;
				}
			}
		}
	}

	
	if (OutCardResult.cbCardCount == 0 && cbTurnCardCount==0)
	{
		for (int i = 0; i < vecMinTypeCardResult.size(); i++)
		{
			if (vecMinTypeCardResult[i].cbCardCount >= m_cbUserCardCount[0])
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
				if (FourTakeFenChai(vecMinTypeCardResult[i].cbResultCard, vecMinTypeCardResult[i].cbCardCount, vecMinTypeCardResult, OutCardResult))
				{
				}
				else{

					OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
					CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, OutCardResult.cbCardCount);
				}
				return true;
			}
		}
	}
	return false;
}

bool CGameLogicNew::SearchOtherHandCardThan(const BYTE cbHandCardData[], BYTE cbHandCardCount, bool bNoSearchBomb)
{
	/*if (m_bHavePass)
	{
		bNoSearchBomb = true;
	}*/
	if (m_cbUserCardCount[1]<4)
	{
		bNoSearchBomb = false;
	}
	if ((cbHandCardCount>m_cbUserCardCount[1]) || (cbHandCardCount >= 10))
	{
		return false;
	}
	bool bExistMax = false;
	if (TOU_SHI)
	{
		for (int i = 0; i < GAME_PLAYER;i++)
		{
			if (i != m_wMeChairID)
			{
				tagSearchCardResult SearchCardResult;
				SearchOutCard(m_cbAllCardData[i], m_cbUserCardCount[i], cbHandCardData, cbHandCardCount, &SearchCardResult);
				if (SearchCardResult.cbSearchCount > 0)
					bExistMax = true;
			}
				
		}
	}
	else
	{
		//获取类型
		BYTE cbTmpType = GetCardType(cbHandCardData, cbHandCardCount);
		if (cbTmpType>=CT_FOUR_TAKE_ONE)
		{
			return false;
		}
		{
			bool bEnable = false;
			int indexType = cbTmpType;
			if (m_cbMaxCard[cbTmpType] > 0)
			{
				bEnable = true;
			}
			else if (cbTmpType == CT_DOUBLE&&(m_cbMaxCard[CT_SINGLE]>0))
			{
				bEnable = true;
				indexType = CT_DOUBLE;
			}
			else if ((cbTmpType == CT_THREE || cbTmpType == CT_THREE_TAKE_ONE || cbTmpType == CT_THREE_TAKE_TWO) && ((m_cbMaxCard[CT_SINGLE] > 0) || (m_cbMaxCard[CT_DOUBLE] > 0)))
			{
				bEnable = true;
				for (int i = 0; i < CT_THREE;i++)
				{
					if ((m_cbMaxCard[i] > 0)&&(m_cbMaxCard[indexType] == 0))
					{
						indexType = i;
					}
					else{
						if ((m_cbMaxCard[i] > 0))
						{
							if (GetCardLogicValue(m_cbMaxCard[i]) < GetCardLogicValue(m_cbMaxCard[indexType]))
							{
								indexType = i;
							}
						}
					}
					
				}
				
			}
			if (m_cbMaxCard[indexType]>0)
			{
				if (bEnable&& GetCardLogicValue(cbHandCardData[0]) >= GetCardLogicValue(m_cbMaxCard[indexType]))
				{
				
					return false;
				}
				if (bEnable&&cbTmpType == CT_THREE_TAKE_TWO && (GetCardLogicValue(cbHandCardData[0]) + 1 == GetCardLogicValue(m_cbMaxCard[indexType])))
				{
					return false;
				}

				if (bEnable&&cbTmpType == CT_THREE_TAKE_TWO )
				{
					int count=0;
					for (int i = GetCardLogicValue(cbHandCardData[0])+1; i <=GetCardLogicValue(m_cbMaxCard[indexType]);i++)
					{
						for (int j = 0; j < m_cbOtherDiscardCount;j++)
						{
							if (GetCardLogicValue(m_cbOtherDiscardCard[j]) == i)
							{
								count++;
								break;
							}
						}
					}
					if (count == (GetCardLogicValue(m_cbMaxCard[indexType]) - GetCardLogicValue(cbHandCardData[0])))
					{
						return false;
					}
				}
			}
			
		}
		
		
		BYTE tempCard[FULL_COUNT] = { 0 };
	
		BYTE tempCardCount = 0;
		CopyMemory(tempCard, m_cbCardData, FULL_COUNT);
		tempCardCount = FULL_COUNT;
		RemoveCard(m_cbDiscardCard, m_cbDiscardCardCount, tempCard, tempCardCount);
		tempCardCount = tempCardCount - m_cbDiscardCardCount;
		/*	if (m_cbUserCardCount[2] == 0)
			{
			BYTE tempCardEx[] = { 0x03, 0x04, 0x13, 0x14, 0x23, 0x24, 0x33, 0x34 };
			RemoveCard(tempCardEx, sizeof(tempCardEx), tempCard, tempCardCount);
			tempCardCount = tempCardCount - sizeof(tempCardEx);
			}*/
		if (RemoveCard(m_cbAllCardData[m_wMeChairID], m_cbUserCardCount[m_wMeChairID], tempCard, tempCardCount))
		{
			tempCardCount = tempCardCount - m_cbUserCardCount[m_wMeChairID];
		}
		tagSearchCardResult SearchCardResult = {};
		SearchOutCard(tempCard, tempCardCount, cbHandCardData, cbHandCardCount, &SearchCardResult, bNoSearchBomb);
		if (SearchCardResult.cbSearchCount > 0)
		{
				bExistMax = true;
			
		}
	}
	return bExistMax;
}

bool CGameLogicNew::SearchOtherHandCardSame(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cardType)
{
	if (cardType >= CT_THREE)
	{
		return false;
	}
	bool bExistMax = false;
	if (TOU_SHI)
	{
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			if (i != m_wMeChairID)
			{
				int sameCount = 0;
				bool bHaveYong[MAX_COUNT] = { 0 };
				for (int j = 0; j < m_cbUserCardCount[i];j++)
				{
					int tempLogic = GetCardLogicValue(m_cbAllCardData[i][j]);
					for (int k = 0; k < cbHandCardCount;k++)
					{
						if (bHaveYong[k] == false && GetCardLogicValue(cbHandCardData[k]) == tempLogic)
						{
							sameCount++;
						}
					}
					if (sameCount>=cbHandCardCount)
					{
						bExistMax = true;
						break;
					}
				}
				if (bExistMax)
				{
					break;
				}
			}

		}
	}
	else
	{
		//获取类型
		BYTE cbTmpType = GetCardType(cbHandCardData, cbHandCardCount);
		{
			bool bEnable = false;
			int indexType = cbTmpType;
			if (m_cbMaxCard[cbTmpType] > 0)
			{
				bEnable = true;
			}
			else if (cbTmpType == CT_DOUBLE && (m_cbMaxCard[CT_SINGLE] > 0))
			{
				bEnable = true;
				indexType = CT_DOUBLE;
			}
			else if ((cbTmpType == CT_THREE || cbTmpType == CT_THREE_TAKE_ONE || cbTmpType == CT_THREE_TAKE_TWO) && ((m_cbMaxCard[CT_SINGLE] > 0) || (m_cbMaxCard[CT_DOUBLE] > 0)))
			{
				bEnable = true;
				for (int i = 0; i < CT_THREE; i++)
				{
					if ((m_cbMaxCard[i] > 0) && (m_cbMaxCard[indexType] == 0))
					{
						indexType = i;
					}
					else{
						if ((m_cbMaxCard[i] > 0))
						{
							if (GetCardLogicValue(m_cbMaxCard[i]) < GetCardLogicValue(m_cbMaxCard[indexType]))
							{
								indexType = i;
							}
						}
					}
				}

			}
			if (m_cbMaxCard[indexType] > 0)
			{
				if (bEnable&& GetCardLogicValue(cbHandCardData[0]) > GetCardLogicValue(m_cbMaxCard[indexType]))
				{
					return false;
				}
			}

		}
		BYTE tempCard[FULL_COUNT] = { 0 };

		BYTE tempCardCount = 0;
		CopyMemory(tempCard, m_cbCardData, FULL_COUNT);
		tempCardCount = FULL_COUNT;
		RemoveCard(m_cbDiscardCard, m_cbDiscardCardCount, tempCard, tempCardCount);
		tempCardCount = tempCardCount - m_cbDiscardCardCount;
	/*	if (m_cbUserCardCount[2] == 0)
		{
			BYTE tempCardEx[] = { 0x03, 0x04, 0x13, 0x14, 0x23, 0x24, 0x33, 0x34 };
			RemoveCard(tempCardEx, sizeof(tempCardEx), tempCard, tempCardCount);
			tempCardCount = tempCardCount - sizeof(tempCardEx);
		}*/
		if (RemoveCard(m_cbAllCardData[m_wMeChairID], m_cbUserCardCount[m_wMeChairID], tempCard, tempCardCount))
		{
			tempCardCount = tempCardCount - m_cbUserCardCount[m_wMeChairID];
		}

		int sameCount = 0;
		bool bHaveYong[MAX_COUNT] = { 0 };
		for (int j = 0; j < tempCardCount; j++)
		{
			int tempLogic = GetCardLogicValue(tempCard[j]);
			for (int k = 0; k < cbHandCardCount; k++)
			{
				if (bHaveYong[k]==false&&(cbHandCardData[k]) == tempLogic)
				{
					sameCount++;
					break;
				}
			}
			if (sameCount >= m_cbUserCardCount[0])
			{
				bExistMax = true;
				break;
			}
		}
	}
	return bExistMax;
}


int CGameLogicNew::IsJueDuiDaPai(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cardType, bool bNoSearchBomb )
{
	bool bExistMax = SearchOtherHandCardThan(cbHandCardData, cbHandCardCount, bNoSearchBomb);
	if (bExistMax == false)
	{
		if (SearchOtherHandCardSame(cbHandCardData, cbHandCardCount, cardType) == false)
		{
			return JUE_DUI_DA;
		}
		else
			return XIANG_DUI_DA;
	}
	return -1;
}

float CGameLogicNew::calCardScoreEx(vector<tagOutCardResultNew> &vecMinTypeCardResult, float score, BYTE cbHandCardCount, BYTE * cbHandCardData, tagOutCardTypeResultNew * CardTypeResult)
{
	int singleCount = 0;
	for (int i = 0; i < vecMinTypeCardResult.size(); i++)
	{
		if (vecMinTypeCardResult[i].cbCardType == CT_SINGLE)
		{
			singleCount++;
		}
	}
	if (singleCount <=3)
	{
		score += 5 * (3-singleCount);
	}

/*	if (vecMinTypeCardResult[0].cbCardType == CT_MISSILE_CARD)
	{
		score += 45;
	}
	else*/{
		int erCount = 0;
		int aCount = 0;
		for (int i = 0; i < cbHandCardCount; i++)
		{
			if ((GetCardLogicValue(cbHandCardData[i]) == 15))
			{
				erCount++;
			}
			if ((GetCardLogicValue(cbHandCardData[i]) == 14))
			{
				aCount++;
			}
		}
	
		if (cbHandCardData[0] == 15 )
		{
			score += 25;
		}
		if (cbHandCardData[0] == 14)
		{
			score += 20;
		}
		if (vecMinTypeCardResult.size() <= 5)
		{
			score += 10;
		}
		if (erCount >= 1)
		{
			score += erCount*10;
		}
		if (aCount >=1)
		{
			score += aCount * 3;
		}
		if (cbHandCardData[0]<14 && erCount<2)
		{
			score -= 10;
		}
		if (CardTypeResult[CT_BOMB_CARD].cbCardTypeCount>0)
		{
			score += 10;
		}
		if (score<30&&cbHandCardData[0] == 15 && erCount == 2 && aCount >= 1 && singleCount <= 4)
		{
			score += 15;
		}
		if (cbHandCardData[0]<0x02)
		{
			score -= 45;
		}
	}	return score;
}

bool CGameLogicNew::SearchOutCardErRen(BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardData[], BYTE cbTurnCardCount, BYTE	DiscardCard[], BYTE cbDiscardCardCount, BYTE	cbOtherDiscardCard[], BYTE	cbOtherDiscardCount, BYTE cbCardDataEx[], BYTE MaxCard[], tagOutCardResultNew & OutCardResult)
{
	ReSetData();
	SetDiscardCard(DiscardCard, cbDiscardCardCount, cbOtherDiscardCard, cbOtherDiscardCount);
	SetUserCard(0, cbHandCardData, cbHandCardCount);
	CopyMemory(m_cbMaxCard, MaxCard, sizeof(m_cbMaxCard));
	CopyMemory(m_cbCardDataEx, cbCardDataEx, sizeof(m_cbCardDataEx));
	m_cbCardTypeCount = m_cbCardDataEx[3];
	m_cbRangCount = m_cbCardDataEx[1];
	m_cbBeiRangCount = m_cbCardDataEx[2];
	m_cbUserCardCount[0] = cbHandCardCount - m_cbBeiRangCount;
	if (m_cbCardTypeCount == MAX_COUNT)
	{
		m_cbUserCardCount[1] = (NORMAL_COUNT - cbOtherDiscardCount - m_cbRangCount);
	}
	else{
		m_cbUserCardCount[1] = (MAX_COUNT - cbOtherDiscardCount - m_cbRangCount);
	}
	m_bHavePass = m_cbCardDataEx[0];
	m_cbFirstCard = m_cbCardDataEx[1];

	//玩家判断
	WORD wUndersideOfBanker = (m_wBankerUser + 1) % GAME_PLAYER;	//地主下家
	WORD wUpsideOfBanker = (wUndersideOfBanker + 1) % GAME_PLAYER;	//地主上家

	//初始变量
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	SortCardList(cbHandCardData, cbHandCardCount, ST_ORDER);
	SortCardList(cbTurnCardData, cbTurnCardCount, ST_ORDER);
	//先出牌
	if (cbTurnCardCount == 0)
	{
		m_bTrunMode = false;
		//地主出牌
		ErZhuDongOutCard(cbHandCardData, cbHandCardCount, OutCardResult);

	}
	//压牌
	else
	{
		m_bTrunMode = true;
		//地主出牌
		ErBeiDongOutCard(cbHandCardData, cbHandCardCount, 1, cbTurnCardData, cbTurnCardCount, OutCardResult);

	}
	return true;
}

int CGameLogicNew::LandScoreErRen(BYTE cbHandCardData[], BYTE cbHandCardCount, int &cbCurrentLandScore)
{
	SortCardList(cbHandCardData, cbHandCardCount, ST_ORDER);
	SetUserCard(0, cbHandCardData, cbHandCardCount);
	tagOutCardTypeResultNew   CardTypeResult[CT_TYPE_COUNT];
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult));

	vector<tagOutCardResultNew>   vecMinTypeCardResult;
	float tempMinTypeScore = 0;
	bool biYing = false;
	int minTypeCount = FindCardKindMinNum(cbHandCardData, cbHandCardCount, CardTypeResult, vecMinTypeCardResult, tempMinTypeScore, biYing);
	int score = GetHandScore(vecMinTypeCardResult, minTypeCount,true);
	//score = calCardScoreEx(vecMinTypeCardResult, score, cbHandCardCount, cbHandCardData, CardTypeResult);
	cbCurrentLandScore = score;

	//放弃叫分
	return score;
}

bool CGameLogicNew::CheckBombPercent(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE DiscardCard[], BYTE cbDiscardCardCount)
{
	return true;
	for (int i = 0; i < cbHandCardCount; i++)
	{
		if (cbHandCardData[i] >= 14)
		{
			return true;
		}
	}
	for (int i = 0; i < cbDiscardCardCount; i++)
	{
		if (DiscardCard[i] >= 14)
		{
			return true;
		}
	}
	return false;
}

bool CGameLogicNew::CheckTwoArrayIntersect(BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardData[], BYTE cbTurnCardCount)
{
	for (int i = 0; i < cbHandCardCount; i++)
	{
		for (int j = 0; j < cbTurnCardCount; j++)
		{
			if (cbHandCardData[i] == cbTurnCardData[j])
			{
				return true;
			}
		}
	}
	return false;
}
//地主出牌（先出牌）
VOID CGameLogicNew::ErZhuDongOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagOutCardResultNew & OutCardResult)
{
	//零下标没用
	tagOutCardTypeResultNew CardTypeResult[CT_TYPE_COUNT];
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult));
	//初始变量
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	vector<tagOutCardResultNew>   vecMinTypeCardResult;
	float tempMinTypeScore = 0;
	bool biYing = false;
	int minTypeCount = FindCardKindMinNum(cbHandCardData, cbHandCardCount, CardTypeResult, vecMinTypeCardResult, tempMinTypeScore, biYing);
	if (FindMaxTypeTakeOneType(cbHandCardData, cbHandCardCount, 0,0, 0, vecMinTypeCardResult, OutCardResult))
	{
		int type = GetCardType(OutCardResult.cbResultCard, OutCardResult.cbCardCount);
		//if (type==CT_BOMB_CARD)
		{
			ThreeTakeTwoTakeMinCard(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult, CardTypeResult);
			return;
		}
	}
	
	if (minTypeCount == 1)
	{
		OutCardResult.cbCardCount = vecMinTypeCardResult[0].cbCardCount;
		CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[0].cbResultCard, OutCardResult.cbCardCount);
		return;
	}
	WORD wUndersideUser = (m_wBankerUser + 1) % GAME_PLAYER;
	WORD wUpsideUser = (wUndersideUser + 1) % GAME_PLAYER;
	if (WuDiCheck(cbHandCardData, cbHandCardCount, NULL, 0, OutCardResult, CardTypeResult))
	{
		ThreeTakeTwoTakeMinCard(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult, CardTypeResult);
		BaoDanJiaoYan(OutCardResult, wUndersideUser, cbHandCardData);
		return;
	}
	int tmpType = GetCardType(cbHandCardData, cbHandCardCount);
	
	if (tmpType != CT_ERROR )
	{
		OutCardResult.cbCardCount = cbHandCardCount;
		CopyMemory(OutCardResult.cbResultCard, cbHandCardData, OutCardResult.cbCardCount);
		return;
	}
	bool bZhiJieChu = false;
	int outIndex = YouXianDaNengShouHuiCard(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult, bZhiJieChu);
	if (OutCardResult.cbCardCount>0)
	{
		    int  type =GetCardType(OutCardResult.cbResultCard,OutCardResult.cbCardCount);
			 if (bZhiJieChu == true)
			{
				//ThreeTakeOneChangeTwoTake(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult);
				ThreeTakeTwoTakeMinCard(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult, CardTypeResult);
				if ((type == CT_SINGLE || type == CT_DOUBLE) && OutCardResult.cbCardCount != m_cbUserCardCount[1])
				{
					OutSingleOrDoubleMinCard(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult, type);
				}
				BaoDanJiaoYan(OutCardResult, wUndersideUser, cbHandCardData);
				return;
			}
			 else if ((type == CT_SINGLE || type == CT_DOUBLE) && OutCardResult.cbCardCount!=m_cbUserCardCount[1])
			{
				OutSingleOrDoubleMinCard(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult, type);
			}
			
	}

	if (outIndex == -1)
	{
		outIndex = SearchMutilType(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult);
	}
	OutCardSpecialCheck(OutCardResult, vecMinTypeCardResult);
	

	if ((m_cbUserCardCount[wUpsideUser] == CT_SINGLE || m_cbUserCardCount[wUndersideUser] == CT_SINGLE) && OutCardResult.cbCardCount == 1)
	{
		for (int i = vecMinTypeCardResult.size() - 1; i >0; i--)
		{
			if (vecMinTypeCardResult[i].cbCardType != CT_SINGLE)
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
				OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, OutCardResult.cbCardCount);
				break;
			}
		}
	}
	else if ((m_cbUserCardCount[wUpsideUser] == 2 || m_cbUserCardCount[wUndersideUser] == 2) && OutCardResult.cbCardCount == 2 && vecMinTypeCardResult.size()!=2)
	{
		for (int i = vecMinTypeCardResult.size() - 1; i >0; i--)
		{
			if (vecMinTypeCardResult[i].cbCardType != CT_DOUBLE)
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
				OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, OutCardResult.cbCardCount);
				break;
			}
		}
	}
	if (OutCardResult.cbCardCount == 0)
	{
		for (int k = 0; k < CT_TYPE_COUNT; k++)
		{
			for (BYTE i = 0; i < CardTypeResult[k].cbCardTypeCount; ++i)
			{

				OutCardResult.cbCardCount = CardTypeResult[k].cbEachHandCardCount[i];
				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[k].cbCardData[i], CardTypeResult[k].cbEachHandCardCount[i]);
				break;
			}
		}
	}
	//--出的牌是三带拖时, 要拖最小的牌
	/*if (ThreeTakeOneChangeTwoTake(cbHandCardData,cbHandCardCount, vecMinTypeCardResult, OutCardResult))
	{
	}
	else*/
	{
		ThreeTakeTwoTakeMinCard(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult, CardTypeResult);
		
	}
	BaoDanJiaoYan(OutCardResult, wUndersideUser, cbHandCardData);
	return;
}

//地主出牌（后出牌）
VOID CGameLogicNew::ErBeiDongOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wOutCardUser, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResultNew & OutCardResult)
{
	//初始变量
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	//零下标没用
	tagOutCardTypeResultNew CardTypeResult[CT_TYPE_COUNT];
	ZeroMemory(&CardTypeResult, sizeof(CardTypeResult));

	vector<tagOutCardResultNew>   vecMinTypeCardResult;
	//出牌类型
	BYTE cbOutCardType = GetCardType(cbTurnCardData, cbTurnCardCount);
	float tempMinTypeScore = 0;
	bool biYing = false;
	int minTypeCount = FindCardKindMinNum(cbHandCardData, cbHandCardCount, CardTypeResult, vecMinTypeCardResult, tempMinTypeScore, biYing);

	/*if (FindMaxTypeTakeOneType(cbHandCardData, cbHandCardCount, cbOutCardType, cbTurnCardData, cbTurnCardCount, vecMinTypeCardResult, OutCardResult))
	{
		return;
	}*/
	if (minTypeCount == 1 && CompareCard(cbTurnCardData, cbHandCardData, cbTurnCardCount, cbHandCardCount))
	{
		OutCardResult.cbCardCount = vecMinTypeCardResult[0].cbCardCount;
		CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[0].cbResultCard, OutCardResult.cbCardCount);
		return;
	}
	if (WuDiCheck(cbHandCardData, cbHandCardCount, cbTurnCardData, cbTurnCardCount, OutCardResult))
	{
		return;
	}
	WORD wUndersideUser = (m_wBankerUser + 1) % GAME_PLAYER;
	WORD wUpsideUser = (wUndersideUser + 1) % GAME_PLAYER;
	bool bExistBiYing = OutCardShengYuFenCheck(cbHandCardCount, cbHandCardData, cbTurnCardData, cbTurnCardCount, wOutCardUser, tempMinTypeScore, vecMinTypeCardResult, OutCardResult);

	if (bExistBiYing && OutCardResult.cbCardCount!=0)
	{
		BaoDanJiaoYan(OutCardResult, wUndersideUser, cbHandCardData);
		return;
	}
	//最大牌
	BYTE cbTurnCardLogic = GetCardLogicValue(cbTurnCardData[0]);

	

	if (cbOutCardType == CT_SINGLE && OutCardResult.cbCardCount == 0 && m_cbUserCardCount[wOutCardUser] <= 1)
	{
		if (OutCardResult.cbCardCount == 0)
		{
			for (int i = 0; i < CardTypeResult[CT_THREE].cbCardTypeCount; i++)
			{
				if (GetCardLogicValue(CardTypeResult[CT_THREE].cbCardData[i][0]) >= cbTurnCardLogic)
				{
					OutCardResult.cbCardCount = 1;
					CopyMemory(OutCardResult.cbResultCard, CardTypeResult[CT_THREE].cbCardData[i], OutCardResult.cbCardCount);
					break;
				}
			}
		}

	}
	if (cbOutCardType == CT_DOUBLE && OutCardResult.cbCardCount == 0 && m_cbUserCardCount[wOutCardUser] <= 2)
	{
		if (OutCardResult.cbCardCount == 0)
		{
			for (int i = 0; i < CardTypeResult[CT_THREE].cbCardTypeCount; i++)
			{
				if (GetCardLogicValue(CardTypeResult[CT_THREE].cbCardData[i][0]) > cbTurnCardLogic)
				{
					OutCardResult.cbCardCount = 2;
					CopyMemory(OutCardResult.cbResultCard, CardTypeResult[CT_THREE].cbCardData[i], OutCardResult.cbCardCount);
					break;
				}
			}
		}

	}
	//当两个人中有其中一人手牌小于等于2了,那么地主必须拆牌来压了
	if (OutCardResult.cbCardCount == 0 && (m_cbUserCardCount[wUndersideUser] <= 2))
	{
		for (int i = 0; i < vecMinTypeCardResult.size(); i++)
		{
			BYTE tempLogic = GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0]);
			if (tempLogic > cbTurnCardLogic&& vecMinTypeCardResult[i].cbCardType == cbOutCardType && vecMinTypeCardResult[i].cbCardCount == cbTurnCardCount)
			{
				OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, OutCardResult.cbCardCount);
				break;
			}
		}
		if (OutCardResult.cbCardCount == 0)
		{
			for (int i = CardTypeResult[cbOutCardType].cbCardTypeCount - 1; i >= 0; i--)
			{
				BYTE tempLogic = GetCardLogicValue(CardTypeResult[cbOutCardType].cbCardData[i][0]);
				if (tempLogic > cbTurnCardLogic&& CardTypeResult[cbOutCardType].cbEachHandCardCount[i] == cbTurnCardCount)
				{
					OutCardResult.cbCardCount = CardTypeResult[cbOutCardType].cbEachHandCardCount[i];
					CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbOutCardType].cbCardData[i], OutCardResult.cbCardCount);
					break;
				}
			}
		}
	}
	//当两个人中有其中一人手牌小于等于2了,那么地主必须拆牌来压了
	BaoDanJiaoYan(OutCardResult, wUndersideUser, cbHandCardData);

	return;
}

void CGameLogicNew::BaoDanJiaoYan(tagOutCardResultNew &OutCardResult, WORD wUndersideUser, const BYTE * cbHandCardData)
{
	if (OutCardResult.cbCardCount == 1 && (m_cbUserCardCount[wUndersideUser] == 1) )
	{

		OutCardResult.cbResultCard[0] = cbHandCardData[0];

	}
}

bool CGameLogicNew::ThreeTakeOneChangeTwoTake(const BYTE cbCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew & OutCardResult)
{
	int outCardType = GetCardType(OutCardResult.cbResultCard, OutCardResult.cbCardCount);
	if ((outCardType == CT_THREE&&cbHandCardCount <= 3) || (outCardType == CT_THREE_TAKE_ONE&&cbHandCardCount <= 4))
	{
		return false;
	}
	BYTE cbRemainCardData[MAX_COUNT] = { 0 };
	BYTE cbRemainCardCount = cbHandCardCount;
	CopyMemory(cbRemainCardData, cbCardData, cbRemainCardCount);
	if (outCardType == CT_THREE_TAKE_ONE || outCardType == CT_THREE)
	{
			BYTE cbThreeCard[MAX_COUNT] = { 0 };
		cbThreeCard[0] = OutCardResult.cbResultCard[0];
		cbThreeCard[1] = OutCardResult.cbResultCard[1];
		cbThreeCard[2] = OutCardResult.cbResultCard[2];
		RemoveCard(cbThreeCard, 3, cbRemainCardData, cbRemainCardCount);
		cbRemainCardCount -= 3;
		SortCardList(cbRemainCardData, cbRemainCardCount, ST_ORDER);
		int minTypeCount = 0;
		int resultIndex = -1;
		float MinTypeScore = INT_MIN;
		BYTE ResultThreeTakeCard[MAX_COUNT] = { 0 };
		float tableScore[MAX_RESULT_COUNT] = { 0 };
		//CopyMemory(cbRemainCardData, cbThreeCard, 3);
		int cbNeedCardCount = 2;
		//单牌组合
		BYTE cbComCard[MAX_COLS] = { 0 };
		BYTE cbComResCard[MAX_RESULT_COUNT][MAX_COLS];
		int cbComResLen = 0;
		//-- //利用对牌的下标做组合，再根据下标提取出对牌
		//cbRemainCardCount = ClearReLogicValue(cbRemainCardData, cbRemainCardCount);
		Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCardData, cbNeedCardCount, cbRemainCardCount, cbNeedCardCount);
		vector<vector<tagOutCardResultNew>>  TempMinTypeCardResult(cbComResLen);
		for (int i = 0; i < cbComResLen; ++i)
		{
			//保存对牌
			BYTE tempThreeTakeTwo[MAX_COUNT] = { 0 };
			CopyMemory(tempThreeTakeTwo, cbThreeCard, 3);
			for (int j = 0; j < cbNeedCardCount; j++)
			{
				tempThreeTakeTwo[3 + j] = cbComResCard[i][j];
			}
			tagOutCardTypeResultNew tmpCardTypeResult[CT_TYPE_COUNT];
			BYTE searchCard[MAX_COUNT] = { 0 };
			BYTE searchCardCount = cbHandCardCount;
			CopyMemory(searchCard, cbCardData, searchCardCount);
			RemoveCard(tempThreeTakeTwo, 5, searchCard, searchCardCount);
			searchCardCount -= 5;
			if (searchCardCount > 0)
			{
				int tempMinTypeCount = 0;
				float tempMinTypeScore = 0;
				bool biYing = false;
				tempMinTypeCount = FindCardKindMinNum(searchCard, searchCardCount, tmpCardTypeResult, TempMinTypeCardResult[i], tempMinTypeScore, biYing, true);
				tableScore[i] = tempMinTypeScore;
				if (tempMinTypeScore > MinTypeScore)
				{
					MinTypeScore = tempMinTypeScore;
					minTypeCount = tempMinTypeCount;
					resultIndex = i;
					CopyMemory(ResultThreeTakeCard, tempThreeTakeTwo, 5);
				}

			}
			else{
				CopyMemory(ResultThreeTakeCard, tempThreeTakeTwo, 5);
				break;
			}
		}
		ZeroMemory(&OutCardResult, sizeof(OutCardResult));
		OutCardResult.cbCardCount = 5;
		CopyMemory(OutCardResult.cbResultCard, ResultThreeTakeCard, 5);
		return true;
	}
	return false;
}

bool CGameLogicNew::ThreeTakeTwoTakeMinCard(const BYTE cbCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew & OutCardResult, tagOutCardTypeResultNew * CardTypeResult)
{

	int outCardType = GetCardType(OutCardResult.cbResultCard, OutCardResult.cbCardCount);
	
	if (outCardType == CT_FOUR_TAKE_ONE||outCardType == CT_THREE_TAKE_TWO || outCardType == CT_THREE_TAKE_ONE)
	{
		if (OutCardResult.cbCardCount<=5)
		{
			for (int i = 0; i < vecMinTypeCardResult.size();i++)
			{
				if (vecMinTypeCardResult[i].cbCardType == CT_THREE_TAKE_ONE || vecMinTypeCardResult[i].cbCardType == CT_THREE_TAKE_TWO)
				{
					if (vecMinTypeCardResult[i].cbCardCount <= 5 && GetCardLogicValue(OutCardResult.cbResultCard[0])>GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0]))
					{
						BYTE cbTempCard[MAX_COUNT] = {};
						CopyMemory(cbTempCard, OutCardResult.cbResultCard, 3);
						CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, 3);
						CopyMemory(vecMinTypeCardResult[i].cbResultCard, cbTempCard, 3);
					}
				}
			}
		}
		BYTE cbNeedCardCount = 0;
		if (outCardType == CT_FOUR_TAKE_ONE)
		{
			cbNeedCardCount = 2;
		}
		else if (outCardType == CT_THREE_TAKE_ONE)
		{
			cbNeedCardCount=OutCardResult.cbCardCount / 4 ;
		}
		else{
			cbNeedCardCount = (OutCardResult.cbCardCount / 5)*2;
		}
		BYTE cbRemainCard[MAX_COUNT];
		CopyMemory(cbRemainCard, cbCardData, cbHandCardCount);
		int TuoTouCount = OutCardResult.cbCardCount - cbNeedCardCount;
		BYTE cbRemainCardCount = cbHandCardCount - TuoTouCount;
		RemoveCard(OutCardResult.cbResultCard, TuoTouCount, cbRemainCard, cbHandCardCount);
		for (int j = 0; j < TuoTouCount; j++)
		{
			if (RemoveCard(&OutCardResult.cbResultCard[j], 1, cbRemainCard, cbRemainCardCount))
			{
				cbRemainCardCount--;
			}
		}
		//单牌组合
		BYTE cbComCard[MAX_COLS];
		BYTE cbComResCard[MAX_RESULT_COUNT][MAX_COLS];
		int cbComResLen = 0;
		//单牌组合
		if (outCardType == CT_THREE_TAKE_ONE)
		{
			cbRemainCardCount = ClearReLogicValue(cbRemainCard, cbRemainCardCount);
		}
		Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCard, cbNeedCardCount, cbRemainCardCount, cbNeedCardCount);
		vector<vector<tagOutCardResultNew>>  TempMinTypeCardResult(cbComResLen);
		int minTypeCount = 0;
		int resultIndex = -1;
		float MinTypeScore = INT_MIN;
		BYTE ResultThreeTakeCard[MAX_COUNT] = { 0 };
		float tableScore[MAX_RESULT_COUNT] = { 0 };
		for (int i = 0; i < cbComResLen; ++i)
		{
			if (outCardType == CT_THREE_TAKE_TWO && isAllDoubleType(cbComResCard[i], cbNeedCardCount) == false)
			{
				continue;
			}
			//保存对牌
			BYTE tempThreeTakeTwo[MAX_COUNT] = { 0 };
			
			CopyMemory(tempThreeTakeTwo, OutCardResult.cbResultCard, TuoTouCount);
			for (int j = 0; j < cbNeedCardCount; j++)
			{
				tempThreeTakeTwo[TuoTouCount + j] = cbComResCard[i][j];
			}
			tagOutCardTypeResultNew tmpCardTypeResult[CT_TYPE_COUNT];
			BYTE searchCard[MAX_COUNT] = { 0 };
			BYTE searchCardCount = cbHandCardCount;
			CopyMemory(searchCard, cbCardData, searchCardCount);
			RemoveCard(tempThreeTakeTwo, OutCardResult.cbCardCount, searchCard, searchCardCount);
			searchCardCount -= OutCardResult.cbCardCount;
			if (searchCardCount > 0)
			{
				int tempMinTypeCount = 0;
				float tempMinTypeScore = 0;
				bool biYing = false;
				tempMinTypeCount = FindCardKindMinNum(searchCard, searchCardCount, tmpCardTypeResult, TempMinTypeCardResult[i], tempMinTypeScore, biYing, true);
				if (i == 28)
				{
					int a = 0;
				}
				tableScore[i] = tempMinTypeScore;
				if (tempMinTypeScore > MinTypeScore)
				{
					MinTypeScore = tempMinTypeScore;
					minTypeCount = tempMinTypeCount;
					resultIndex = i;
					CopyMemory(ResultThreeTakeCard, tempThreeTakeTwo, OutCardResult.cbCardCount);
				}

			}
			else{
				CopyMemory(ResultThreeTakeCard, tempThreeTakeTwo, OutCardResult.cbCardCount);
				break;
			}
		}
		CopyMemory(OutCardResult.cbResultCard, ResultThreeTakeCard, OutCardResult.cbCardCount);
		return true;
	}
	else if (OutCardResult.cbCardType==CT_THREE)
	{
		ThreeTakeMinCard(cbCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult, CardTypeResult);
	}
	return false;
}

void CGameLogicNew::CommonSDTurnCardSearch(vector<tagOutCardResultNew> &vecMinTypeCardResult, BYTE cbTurnCardLogic, tagOutCardResultNew &OutCardResult)
{
	vector<tagCardIndex> vecSingleData;
	vector<tagCardIndex> vecDoubleData;
	for (int i = 0; i < vecMinTypeCardResult.size(); i++)
	{
		tagCardIndex  cardIndexSt;
		if (vecMinTypeCardResult[i].cbCardType == CT_SINGLE)
		{
			cardIndexSt.cbIndex = i;
			cardIndexSt.cbCardData = vecMinTypeCardResult[i].cbResultCard[0];
			vecSingleData.push_back(cardIndexSt);
		}
		if (vecMinTypeCardResult[i].cbCardType == CT_DOUBLE)
		{
			cardIndexSt.cbIndex = i;
			cardIndexSt.cbCardData = vecMinTypeCardResult[i].cbResultCard[0];
			vecDoubleData.push_back(cardIndexSt);
		}
	}
	for (int i = vecSingleData.size() - 1; i >= 0; i--)
	{
		if (GetCardLogicValue(vecSingleData[i].cbCardData)>cbTurnCardLogic)
		{
			OutCardResult.cbCardCount = 1;
			CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[vecSingleData[i].cbIndex].cbResultCard, OutCardResult.cbCardCount);
			break;
		}
	}
	if (OutCardResult.cbCardCount == 0)
	{
		for (int i = vecDoubleData.size() - 1; i >= 0; i--)
		{
			if (GetCardLogicValue(vecDoubleData[i].cbCardData) > cbTurnCardLogic)
			{
				OutCardResult.cbCardCount = 1;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[vecDoubleData[i].cbIndex].cbResultCard, OutCardResult.cbCardCount);
				break;
			}
		}
	}
}

void CGameLogicNew::SwitchArray(BYTE bInFirstList[], BYTE bInNextList[], int count)
{
	for (int i = 0; i < count; i++)
	{
		BYTE tempTemp = bInFirstList[i];
		bInFirstList[i] = bInNextList[i];
		bInNextList[i] = tempTemp;
	}
}

float CGameLogicNew::GetCardTurnPercent(const BYTE cbCardData[], BYTE cbCardCount, const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbDisCardData[], BYTE cbDisCardCount)
{
	bool bExistMax = false;

	BYTE allCard[FULL_COUNT] = { 0 };

	BYTE allCardCardCount = sizeof(m_cbCardData);
	CopyMemory(allCard, m_cbCardData, sizeof(m_cbCardData));

	RemoveCard(cbDisCardData, cbDisCardCount, allCard, allCardCardCount);
	allCardCardCount = allCardCardCount - cbDisCardCount;
	if (RemoveCard(cbHandCardData, cbHandCardCount, allCard, allCardCardCount))
	{
		allCardCardCount = allCardCardCount - cbHandCardCount;
	}
	//分析扑克
	tagDistributingNew Distributing = {};
	AnalysebDistributing(allCard, allCardCardCount, Distributing);
	CopyMemory(Distributing.cbDistributing[13], Distributing.cbDistributing[0], sizeof(Distributing.cbDistributing[0]));
	BYTE cbTmpLinkCount = 0;
	BYTE cbValueIndex = 0;
	BYTE cbReferIndex = GetCardLogicValue(cbCardData[0])+1;
	int existEvent = 0;
	for (cbValueIndex = cbReferIndex; cbValueIndex < 14; cbValueIndex++)
	{
		int tempCardCount = Distributing.cbDistributing[cbValueIndex][cbIndexCount];
		if (tempCardCount == cbCardCount)
		{
			existEvent++;
		}
		else if (tempCardCount > cbCardCount)
		{
			existEvent += GetCMNSort(tempCardCount, cbCardCount);
			//炸弹也是其中一种
			if (tempCardCount == 4)
			{
				existEvent++;
			}
		}
	}
	int totalEvent = GetCMNSort(allCardCardCount, cbCardCount);
	if (existEvent==0)
	{
		return 1.0;
	}
	float gaiLv = (float) existEvent / totalEvent;
	return gaiLv;
	
}

int CGameLogicNew::GetCMNSort(int m, int n)
{
	int mResult = 1;
	for (int i = m; i > (m - n); i--){
		mResult = mResult*i;
	}
	int nResult = 1;
	for (int i = n; i >= 1; i--)
	{
		nResult = nResult*i;
	}
	return mResult / nResult;
}

bool CGameLogicNew::isAllDoubleType(BYTE * cbComResCard, BYTE CardCount)
{
	for (int i = 0; i < CardCount;i+=2)
	{
		if (cbComResCard[i] != cbComResCard[i+1] )
		{
			return false;
		}
	}
	return true;
}

BYTE CGameLogicNew::ZhuDongSearchOutCard(tagOutCardTypeResultNew CardTypeResult[CT_TYPE_COUNT], tagSearchCardResult *pSearchCardResult, bool bNoSearchBomb /*= false*/)
{
	ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));
	for (int i = 0; i < CT_TYPE_COUNT; i++)
	{
		for (int j = 0; j < CardTypeResult[i].cbCardTypeCount; j++)
		{
			CopyMemory(pSearchCardResult->cbResultCard[pSearchCardResult->cbSearchCount], CardTypeResult[i].cbCardData[j],
				CardTypeResult[i].cbEachHandCardCount[j]);
			pSearchCardResult->cbCardCount[pSearchCardResult->cbSearchCount] = CardTypeResult[i].cbEachHandCardCount[j];
			pSearchCardResult->cbSearchCount++;
		}
	}
	return true;
}

BYTE CGameLogicNew::SearchThreeTwoLine(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbLineCount, BYTE cbTakeCardCount, tagSearchCardResult *pSearchCardResult)
{
	//设置结果
	if (pSearchCardResult)
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));
	//变量定义
	tagSearchCardResult tmpSearchResult = {};
	//搜索连牌
	BYTE cbTmpResultCount = SearchLineCardType(cbHandCardData, cbHandCardCount, cbReferCard, 3, cbLineCount, &tmpSearchResult);

	if (cbTmpResultCount > 0)
	{
		//提取带牌
		for (BYTE i = 0; i < cbTmpResultCount; i++)
		{
			BYTE cbRemainCard[FULL_COUNT];
			BYTE cbRemainCardCount = cbHandCardCount - tmpSearchResult.cbCardCount[i];
			BYTE cbNeedCardCount = tmpSearchResult.cbCardCount[i] / 3 * cbTakeCardCount;
			if (cbRemainCardCount < cbNeedCardCount)
			{
				continue;
			}
			CopyMemory(cbRemainCard, cbHandCardData, cbHandCardCount*sizeof(BYTE));
			RemoveCard(tmpSearchResult.cbResultCard[i], tmpSearchResult.cbCardCount[i], cbRemainCard, cbHandCardCount);
			for (int j = 0; j < tmpSearchResult.cbCardCount[i]; j++)
			{
				if (RemoveCard(&tmpSearchResult.cbResultCard[i][j], 1, cbRemainCard, cbRemainCardCount))
				{
					cbRemainCardCount--;
				}
			}
			//单牌组合
			BYTE cbComCard[MAX_COLS];
			BYTE cbComResCard[MAX_RESULT_COUNT][MAX_COLS];
			int cbComResLen = 0;
			//单牌组合
			if (cbTakeCardCount == 1)
			{
				cbRemainCardCount = ClearReLogicValue(cbRemainCard, cbRemainCardCount);
			}
			Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCard, cbNeedCardCount, cbRemainCardCount, cbNeedCardCount);
			for (int j = 0; j < cbComResLen; ++j)
			{
				if ((cbTakeCardCount == 2) && isAllDoubleType(cbComResCard[j], cbNeedCardCount) == false)
				{
					continue;
				}
				BYTE cbResultCount = pSearchCardResult->cbSearchCount++;
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchResult.cbResultCard[i],
					tmpSearchResult.cbCardCount[i]);
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount] + tmpSearchResult.cbCardCount[i], cbComResCard[j],
					cbNeedCardCount);
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchResult.cbCardCount[i] + cbNeedCardCount;

			}
		}
	}

	return pSearchCardResult->cbSearchCount;
}

bool CGameLogicNew::FourTakeFenChai(const BYTE cbHandCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew & OutCardResult)
{
	int outtype = GetCardType(cbHandCardData,cbHandCardCount);
	if (outtype != CT_FOUR_TAKE_ONE&&outtype != CT_FOUR_TAKE_TWO)
	{
		return false;
	}
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	vector<BYTE> singleData;
	vector<BYTE> doubleData;
	int bombCount = 0;
	int typeCount[CT_TYPE_COUNT] = { 0 };
	int cbCardCount = 0;
	for (int i = 0; i < vecMinTypeCardResult.size(); i++)
	{
		int type = vecMinTypeCardResult[i].cbCardType;
		typeCount[type]++;
		cbCardCount += vecMinTypeCardResult[i].cbCardCount;
		if (type == CT_SINGLE)
		{
			singleData.push_back(vecMinTypeCardResult[i].cbResultCard[0]);
		}
		else if (type == CT_FOUR_TAKE_ONE)
		{
			if (vecMinTypeCardResult[i].cbResultCard[4] != vecMinTypeCardResult[i].cbResultCard[5])
			{
				singleData.push_back(vecMinTypeCardResult[i].cbResultCard[4]);
				singleData.push_back(vecMinTypeCardResult[i].cbResultCard[5]);
			}
			else{
				doubleData.push_back(vecMinTypeCardResult[i].cbResultCard[4]);
				doubleData.push_back(vecMinTypeCardResult[i].cbResultCard[5]);
			}
		}
		else if (type == CT_FOUR_TAKE_TWO)
		{
			doubleData.push_back(vecMinTypeCardResult[i].cbResultCard[4]);
			doubleData.push_back(vecMinTypeCardResult[i].cbResultCard[5]);
			doubleData.push_back(vecMinTypeCardResult[i].cbResultCard[6]);
			doubleData.push_back(vecMinTypeCardResult[i].cbResultCard[7]);
		}
		else if(type == CT_DOUBLE)
		{
			doubleData.push_back(vecMinTypeCardResult[i].cbResultCard[0]);
			doubleData.push_back(vecMinTypeCardResult[i].cbResultCard[1]);
		}
		else if (type == CT_THREE_TAKE_ONE)
		{
			for (int k = 0; k < vecMinTypeCardResult[i].cbCardCount / 4; k++)
			{
				singleData.push_back(vecMinTypeCardResult[i].cbResultCard[vecMinTypeCardResult[i].cbCardCount - 1 - k]);
			}
		}
		else if (type == CT_THREE_TAKE_TWO)
		{
			if (vecMinTypeCardResult[i].cbCardCount <= 5)
			{
				for (int k = 0; k < vecMinTypeCardResult[i].cbCardCount / 5; k++)
				{
					BYTE data1 = vecMinTypeCardResult[i].cbResultCard[vecMinTypeCardResult[i].cbCardCount - 1 - 2 * k];
					BYTE data2 = vecMinTypeCardResult[i].cbResultCard[vecMinTypeCardResult[i].cbCardCount - 2 - 2 * k];
					if (GetCardLogicValue(data1) == GetCardLogicValue(data2))
					{
						doubleData.push_back(data1);
						doubleData.push_back(data2);
					}
					else{
						singleData.push_back(data1);
						singleData.push_back(data2);
					}
				}
			}

		}
	}
		if (singleData.size() >= 1 && outtype == CT_FOUR_TAKE_ONE)
		{
			sort(singleData.begin(), singleData.end(), [this](BYTE first, BYTE second)
			{
				if (GetCardLogicValue(first) > GetCardLogicValue(second))
				{
					return true;
				}
				return false;
			});
			for (int i = 0; i < singleData.size(); i++)
			{
				BYTE tmpTurnCard[MAX_COUNT] = { 0 };
				tmpTurnCard[0] = { singleData[i] };
				int tmpTurnCardCount = 1;
				int reIndex = IsJueDuiDaPai(tmpTurnCard, tmpTurnCardCount, CT_SINGLE);
				if (reIndex == XIANG_DUI_DA)
				{
					OutCardResult.cbCardCount = 1;
					OutCardResult.cbCardType = CT_SINGLE;
					CopyMemory(OutCardResult.cbResultCard, &singleData[i], 1);
					return true;
				}
				else if(reIndex == JUE_DUI_DA)
				{
					int tempIndex = singleData.size() - 1;
					if (m_cbUserCardCount[1]==1)
					{
						tempIndex = i;
					}
						OutCardResult.cbCardCount = 1;
						OutCardResult.cbCardType = CT_SINGLE;
						CopyMemory(OutCardResult.cbResultCard, &singleData[tempIndex], 1);
						return true;
				}
			}
			if (m_cbUserCardCount[1]>1 )
			{
				if (vecMinTypeCardResult.size() == 1)
				{
					OutCardResult.cbCardCount = 1;
					CopyMemory(OutCardResult.cbResultCard, &singleData[singleData.size() - 1], OutCardResult.cbCardCount);
					return true;
				}
				else if (vecMinTypeCardResult.size() == 2)
				{
					if (singleData.size()==1)
					{
						OutCardResult.cbCardCount = 1;
						CopyMemory(OutCardResult.cbResultCard, &singleData[singleData.size() - 1], OutCardResult.cbCardCount);
						return true;
					}
				}
			}
			else if (m_cbUserCardCount[1]==1)
			{
				if (doubleData.size() >= 2)
				{
					OutCardResult.cbCardCount = 2;
					OutCardResult.cbCardType = CT_DOUBLE;
					CopyMemory(OutCardResult.cbResultCard, &doubleData[0], 2);
					return true;
				}
			}
		}
	

		if (doubleData.size() >= 2 && outtype == CT_FOUR_TAKE_TWO)
	{
		sort(doubleData.begin(), doubleData.end(), [this](BYTE first, BYTE second)
		{
			if (GetCardLogicValue(first) > GetCardLogicValue(second))
			{
				return true;
			}
			return false;
		});
		for (int i = 0; i < doubleData.size(); i += 2)
		{
			BYTE tmpTurnCard[MAX_COUNT] = { 0 };
			tmpTurnCard[0] = { doubleData[i] };
			tmpTurnCard[1] = { doubleData[i + 1] };
			int tmpTurnCardCount = 2;
			int reIndex = IsJueDuiDaPai(tmpTurnCard, tmpTurnCardCount, CT_DOUBLE);
			if (reIndex == XIANG_DUI_DA)
			{
				OutCardResult.cbCardCount = 2;
				OutCardResult.cbCardType = CT_DOUBLE;
				CopyMemory(OutCardResult.cbResultCard, &doubleData[i], 2);
				return true;
			}
			else if (reIndex == JUE_DUI_DA)
			{
				int tempIndex = doubleData.size() - 2;
				if (m_cbUserCardCount[1] == 2)
				{
					tempIndex = i;
				}
				OutCardResult.cbCardCount = 2;
				OutCardResult.cbCardType = CT_DOUBLE;
				CopyMemory(OutCardResult.cbResultCard, &doubleData[i], 2);
				return true;
			}
		}
		if (m_cbUserCardCount[1] != 2 )
		{
			if (vecMinTypeCardResult.size() == 1)
			{
				OutCardResult.cbCardCount = 2;
				OutCardResult.cbCardType = CT_DOUBLE;
				CopyMemory(OutCardResult.cbResultCard, &doubleData[0], 2);
				return true;
			}
			else if (vecMinTypeCardResult.size() == 2)
			{
				if (typeCount[CT_DOUBLE]==1)
				{
					OutCardResult.cbCardCount = 2;
					OutCardResult.cbCardType = CT_DOUBLE;
					CopyMemory(OutCardResult.cbResultCard, &doubleData[0], 2);
					return true;
				}
			}
			
		}
		else{

		}
	}

	
	return false;
}

bool CGameLogicNew::ThreeTakeMinCard(const BYTE cbCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew & OutCardResult, tagOutCardTypeResultNew * CardTypeResult)
{
	int outCardType = GetCardType(OutCardResult.cbResultCard, OutCardResult.cbCardCount);

	if (outCardType == CT_THREE)
	{
		if (OutCardResult.cbCardCount <= 5)
		{
			for (int i = 0; i < vecMinTypeCardResult.size(); i++)
			{
				if (vecMinTypeCardResult[i].cbCardType == CT_THREE_TAKE_ONE || vecMinTypeCardResult[i].cbCardType == CT_THREE_TAKE_TWO || vecMinTypeCardResult[i].cbCardType == CT_THREE)
				{
					if (vecMinTypeCardResult[i].cbCardCount <= 5 && GetCardLogicValue(OutCardResult.cbResultCard[0]) > GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0]))
					{
						BYTE cbTempCard[MAX_COUNT] = {};
						CopyMemory(cbTempCard, OutCardResult.cbResultCard, 3);
						CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, 3);
						CopyMemory(vecMinTypeCardResult[i].cbResultCard, cbTempCard, 3);
					}
				}
			}
		}
		BYTE ResultThreeTakeCard[3][MAX_COUNT] = { 0 };
		float MinTypeScore[3] = { INT_MIN, INT_MIN };
		for (int k = 0; k < 3; k++)
		{
			if (k==0)
			{
				int tempMinTypeCount = 0;
				float tempMinTypeScore = 0;
				bool biYing = false;
				tagOutCardTypeResultNew tmpCardTypeResult[CT_TYPE_COUNT];
				vector<tagOutCardResultNew>  TempMinTypeCardResult ;
				BYTE searchCard[MAX_COUNT] = { 0 };
				BYTE searchCardCount = cbHandCardCount;
				CopyMemory(searchCard, cbCardData, searchCardCount);
				RemoveCard(OutCardResult.cbResultCard, OutCardResult.cbCardCount, searchCard, searchCardCount);
				searchCardCount -= OutCardResult.cbCardCount;
				tempMinTypeCount = FindCardKindMinNum(searchCard, searchCardCount, tmpCardTypeResult, TempMinTypeCardResult, tempMinTypeScore, biYing, true);
				MinTypeScore[k] = tempMinTypeScore;
				CopyMemory(ResultThreeTakeCard[k], OutCardResult.cbResultCard, OutCardResult.cbCardCount);
				continue;
			}
			BYTE cbNeedCardCount = k;
			BYTE cbRemainCard[MAX_COUNT] = {};
			CopyMemory(cbRemainCard, cbCardData, cbHandCardCount);
			int TuoTouCount = OutCardResult.cbCardCount;
			BYTE cbRemainCardCount = cbHandCardCount - TuoTouCount;
			RemoveCard(OutCardResult.cbResultCard, TuoTouCount, cbRemainCard, cbHandCardCount);
			for (int j = 0; j < TuoTouCount; j++)
			{
				if (RemoveCard(&OutCardResult.cbResultCard[j], 1, cbRemainCard, cbRemainCardCount))
				{
					cbRemainCardCount--;
				}
			}
			//单牌组合
			BYTE cbComCard[MAX_COLS];
			BYTE cbComResCard[MAX_RESULT_COUNT][MAX_COLS];
			int cbComResLen = 0;
			//单牌组合
			if (cbNeedCardCount==1)
			{
				cbRemainCardCount = ClearReLogicValue(cbRemainCard, cbRemainCardCount);
			}
			Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCard, cbNeedCardCount, cbRemainCardCount, cbNeedCardCount);
			vector<vector<tagOutCardResultNew>>  TempMinTypeCardResult(cbComResLen);
			int minTypeCount = 0;
			int resultIndex = -1;

			float tableScore[MAX_RESULT_COUNT] = { 0 };
			for (int i = 0; i < cbComResLen; ++i)
			{
				if (cbNeedCardCount == 2 && isAllDoubleType(cbComResCard[i], cbNeedCardCount) == false)
				{
					continue;
				}
				//保存对牌
				BYTE tempThreeTakeTwo[MAX_COUNT] = { 0 };
				BYTE tempThreeCount = OutCardResult.cbCardCount + cbNeedCardCount;
				CopyMemory(tempThreeTakeTwo, OutCardResult.cbResultCard, TuoTouCount);
				for (int j = 0; j < cbNeedCardCount; j++)
				{
					tempThreeTakeTwo[TuoTouCount + j] = cbComResCard[i][j];
				}
				tagOutCardTypeResultNew tmpCardTypeResult[CT_TYPE_COUNT];
				BYTE searchCard[MAX_COUNT] = { 0 };
				BYTE searchCardCount = cbHandCardCount;
				CopyMemory(searchCard, cbCardData, searchCardCount);
				RemoveCard(tempThreeTakeTwo, tempThreeCount, searchCard, searchCardCount);
				searchCardCount -= tempThreeCount;
				if (searchCardCount > 0)
				{
					int tempMinTypeCount = 0;
					float tempMinTypeScore = 0;
					bool biYing = false;
					tempMinTypeCount = FindCardKindMinNum(searchCard, searchCardCount, tmpCardTypeResult, TempMinTypeCardResult[i], tempMinTypeScore, biYing, true);
					if (i == 28)
					{
						int a = 0;
					}
					tableScore[i] = tempMinTypeScore;
					if (tempMinTypeScore > MinTypeScore[k])
					{
						MinTypeScore[k] = tempMinTypeScore;
						minTypeCount = tempMinTypeCount;
						resultIndex = i;
						CopyMemory(ResultThreeTakeCard[k], tempThreeTakeTwo, tempThreeCount);
					}

				}
				else{
					CopyMemory(ResultThreeTakeCard[k], tempThreeTakeTwo, tempThreeCount);
					break;
				}
			}
		}
		int tempIndex = 0;
		for (int i = 1; i < 3;i++)
		{
			if (MinTypeScore[tempIndex] < MinTypeScore[i])
			{
				tempIndex = i;
			}
		}
		CopyMemory(OutCardResult.cbResultCard, ResultThreeTakeCard[tempIndex], OutCardResult.cbCardCount+tempIndex);
		return true;
	}
	return false;
}

int CGameLogicNew::getMostCountIndex( vector<vector<int>> MaxIndexSet, int type, vector<tagOutCardResultNew>&  vecMinTypeCardResult)
{
	int resultIndex = -1;
	int tempSize = MaxIndexSet[type].size();
	if ((type == CT_THREE_TAKE_ONE || type == CT_THREE_TAKE_TWO) && tempSize>=2)
	{
		resultIndex = resultIndex = MaxIndexSet[type][tempSize - 1];
		return resultIndex;
	}
	if (tempSize>0)
	{
		resultIndex = MaxIndexSet[type][tempSize - 1];
		int tempCount = vecMinTypeCardResult[resultIndex].cbCardCount;
		for (int i = 0; i < tempSize;i++)
		{
			if (vecMinTypeCardResult[MaxIndexSet[type][i]].cbCardCount>tempCount)
			{
				resultIndex = MaxIndexSet[type][i];
			}
		}
	}
	return resultIndex;
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
