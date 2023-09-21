#include "StdAfx.h"
#include "GameLogicNew.h"
#include<thread>
#include<numeric>
#include<functional>
using namespace std;
//排列类型
enum enSortCardType
{
	enDescend,																//降序类型 
	enAscend,																//升序类型
	enColor																	//花色类型
};
enum COMRESULT
{
	enCRLess = -1,
	enCREqual,
	enCRGreater,
	enCRError
};
//////////////////////////////////////////////////////////////////////////
//静态变量
//#define ASSERT(f)    ((void)0)
//#define VERIFY(f)          ((void)(f))
////索引变量
const BYTE cbIndexCount=5;
static int tableRe2[100] = { 0 };

//扑克数据
const BYTE	CGameLogicNew::m_cbCardData[FULL_COUNT]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
	0x4E,0x4F
};


//////////////////////////////////////////////////////////////////////////

void CGameLogicNew::SetDiscardCard(BYTE cbCardData[], BYTE cbCardCount)
{
	ZeroMemory(m_cbDiscardCard, sizeof(m_cbDiscardCard));
	CopyMemory(m_cbDiscardCard , cbCardData, cbCardCount);
	m_cbDiscardCardCount = cbCardCount;
}

void CGameLogicNew::ReSetDiscardCard()
{
	ZeroMemory(m_cbDiscardCard, sizeof(m_cbDiscardCard));
	m_cbDiscardCardCount = 0;
}

//构造函数
CGameLogicNew::CGameLogicNew()
{
	ZeroMemory(m_cbDiscardCard, sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbAllCardData, sizeof(m_cbAllCardData));
	ZeroMemory(m_cbLandScoreCardData, sizeof(m_cbLandScoreCardData));
	ZeroMemory(m_cbUserCardCount, sizeof(m_cbUserCardCount));
	
}

//析构函数
CGameLogicNew::~CGameLogicNew()
{
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

bool CGameLogicNew::RemoveCard(const BYTE bRemoveCard[], BYTE bRemoveCount, BYTE bCardData[], BYTE cbTemp[], BYTE bCardCount)
{
	//检验数据
	ASSERT(bRemoveCount <= bCardCount);

	//定义变量
	BYTE bDeleteCount = 0, bTempCardData[13];
	if (bCardCount > CountArray(bTempCardData)) return false;
	CopyMemory(bTempCardData, bCardData, bCardCount*sizeof(bCardData[0]));

	//置零扑克
	for (BYTE i = 0; i < bRemoveCount; i++)
	{
		for (BYTE j = 0; j < bCardCount; j++)
		{
			if (bRemoveCard[i] == bTempCardData[j])
			{
				bDeleteCount++;
				bTempCardData[j] = 0;
				break;
			}
		}
	}
	if (bDeleteCount != bRemoveCount) return false;

	//清理扑克
	BYTE bCardPos = 0;
	for (BYTE i = 0; i < bCardCount; i++)
	{
		if (bTempCardData[i] != 0) cbTemp[bCardPos++] = bTempCardData[i];
	}

	return true;
}

int CGameLogicNew::RemoveCardWang(BYTE bCardData[], BYTE bCardCount, BYTE bWangResultCardData[])
{
	int count = 0;
	for (int i = 0; i < bCardCount; i++)
	{
		if (bCardData[i] == 0x4E)
		{
			bWangResultCardData[count++] = 0x4E;
		}
		else if (bCardData[i] == 0x4F)
		{
			bWangResultCardData[count++] = 0x4F;
		}
	}
	if (count>0)
	{
		RemoveCard(bWangResultCardData, count, bCardData, bCardCount);
	}
	return count;
}

bool CGameLogicNew::IsStraightDragon(const BYTE cbCardData[], BYTE bCardCount)
{
	//校验数据
	ASSERT(bCardCount > 0 && bCardCount <= 13);
	if (bCardCount <= 0 || bCardCount > 13)	return false;

	bool b1 = IsLinkCard(cbCardData, bCardCount);
	bool b2 = IsSameColorCard(cbCardData, bCardCount);
	if (b1 == false || b2 == false)
		return false;

	return true;
}

bool CGameLogicNew::IsSameColorCard(const BYTE cbCardData[], BYTE cbCardCount)
{
	ASSERT(cbCardCount > 0);
	if (cbCardCount <= 0) return false;

	bool bRet = true;

	BYTE cbFirstCard = GetCardColor(cbCardData[0]);
	for (WORD i = 1; i < cbCardCount; i++)
	{
		BYTE cbNextCard = GetCardColor(cbCardData[i]);
		if (cbNextCard != cbFirstCard) bRet = false;
	}

	return bRet;
}

bool CGameLogicNew::IsChouYiSe(BYTE cbCardData[], BYTE bCardCount)
{
	//校验数据
	ASSERT(bCardCount > 0 && bCardCount <= 13);
	if (bCardCount <= 0 || bCardCount > 13)	return false;
	for (int i = 0; i < bCardCount; i++)
	{
		if (((GetCardColor(cbCardData[i]) + 1) % 2) != ((GetCardColor(cbCardData[0]) + 1) % 2))
		{
			return false;
		}

	}

	return true;
}

bool CGameLogicNew::IsLiuDuiBan(BYTE cbCardData[], BYTE bCardCount, tagAnalyseData& AnalyseData)
{
	BYTE tempCardData[NORMAL_COUNT] = { 0 };

	//六对半
	if (AnalyseData.bThreeCount == 0 && AnalyseData.bOneCount == 1)
		return true;
	//六对半
	if (AnalyseData.bThreeCount == 1 && AnalyseData.bOneCount == 0)
		return true;

	return 0;
}

bool CGameLogicNew::IsSanShunZi(BYTE * bCardData, BYTE bCardCount, BYTE cbReusltData[])
{
	BYTE tempCardData[NORMAL_COUNT] = { 0 };

	CopyMemory(tempCardData, bCardData, sizeof(tempCardData));
	tagSearchCardResult LineCardResult;
	BYTE cbLineCardResult5 = SearchAllLineCardType(bCardData, bCardCount, 5, &LineCardResult);
	if (cbLineCardResult5 >= 2)
	{
		for (int i = 0; i < LineCardResult.cbSearchCount; i++)
		{
			for (int j = i + 1; j < LineCardResult.cbSearchCount; j++)
			{
				CopyMemory(tempCardData, bCardData, sizeof(tempCardData));
				if (RemoveCard(LineCardResult.cbResultCard[i], 5, tempCardData, NORMAL_COUNT)
					&& RemoveCard(LineCardResult.cbResultCard[j], 5, tempCardData, NORMAL_COUNT - 5))
				{
					tagSearchCardResult sanLineCardResult;
					// SortCardList()
					BYTE cbLineCardResult3 = SearchLineCardType(tempCardData, 3,0,1, 3, &sanLineCardResult);
					if (cbLineCardResult3 == 1)
					{
						CopyMemory(cbReusltData, sanLineCardResult.cbResultCard[0], 3);
						CopyMemory(cbReusltData + 3, LineCardResult.cbResultCard[i], 5);
						CopyMemory(cbReusltData + 8, LineCardResult.cbResultCard[j], 5);

						return true;
					}
				}

			}

		}
	}
	return 0;
}

bool CGameLogicNew::IsSanTongHua(BYTE * cbCardData, BYTE bCardCount, BYTE cbReusltData[])
{
	BYTE tempCardData[NORMAL_COUNT] = { 0 };

	CopyMemory(tempCardData, cbCardData, sizeof(tempCardData));
	tagSearchCardResult SameColorResult;
	ZeroMemory(&SameColorResult, sizeof(SameColorResult));
	BYTE cbSameColorResult5 = SearchSameColorType(cbCardData, bCardCount, 5, &SameColorResult);
	if (cbSameColorResult5 >= 2)
	{
		for (int i = 0; i < SameColorResult.cbSearchCount; i++)
		{
			for (int j = i + 1; j < SameColorResult.cbSearchCount; j++)
			{
				CopyMemory(tempCardData, cbCardData, sizeof(tempCardData));
				BYTE CardColor1 = GetCardColor(SameColorResult.cbResultCard[i][0]);
				BYTE CardColor2 = GetCardColor(SameColorResult.cbResultCard[j][0]);
				if (RemoveCard(SameColorResult.cbResultCard[i], 5, tempCardData, NORMAL_COUNT)
					&& RemoveCard(SameColorResult.cbResultCard[j], 5, tempCardData, NORMAL_COUNT - 5))
				{

					// SortCardList()
					BYTE CardColor3 = GetCardColor(tempCardData[0]);
					//if ((CardColor1 != CardColor2) && (CardColor2 != CardColor3) && (CardColor3 != CardColor1))
					{
						tagSearchCardResult sanHuaCardResult;
						BYTE cbLineCardResult3 = SearchSameColorType(tempCardData, 3, 3, &sanHuaCardResult);
						if (cbLineCardResult3 == 1)
						{
							CopyMemory(cbReusltData, sanHuaCardResult.cbResultCard[0], 3);
							CopyMemory(cbReusltData + 3, SameColorResult.cbResultCard[i], 5);
							CopyMemory(cbReusltData + 8, SameColorResult.cbResultCard[j], 5);

							return true;
						}
					}

				}

			}

		}
	}
	return 0;
}

BYTE CGameLogicNew::SearchSameColorType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbSameCount, tagSearchCardResult *pSearchCardResult)
{
	//设置结果
	if (pSearchCardResult)
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//复制扑克
	BYTE cbCardData[13] = { 0 };
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
	SortCardList(cbCardData, cbHandCardCount, enDescend);

	//同花变量
	BYTE cbSameCardCount[5] = { 0 };
	BYTE cbSameCardData[5][13] = { 0 };

	//统计同花
	for (BYTE i = 0; i < cbHandCardCount; i++)
	{
		//获取牌色
		BYTE cbCardColor = (GetCardColor(cbCardData[i]) >> 4);

		//原牌数目
		BYTE cbCount = cbSameCardCount[cbCardColor];

		//追加扑克
		cbSameCardData[cbCardColor][cbCount] = cbCardData[i];
		cbSameCardCount[cbCardColor]++;
	}

	//判断是否满cbSameCount
	for (int i = 0; i < 4; i++)
	{
		if (cbSameCardCount[i] >= cbSameCount)
		{
			for (int j = 0; j <= (cbSameCardCount[i] % cbSameCount); j++)
			{
				pSearchCardResult->cbCardCount[cbResultCount] = cbSameCount;
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], &cbSameCardData[i][j], sizeof(BYTE)*cbSameCount);

				cbResultCount++;
			}
		}
	}

	if (pSearchCardResult)
		pSearchCardResult->cbSearchCount = cbResultCount;

	return pSearchCardResult->cbSearchCount;
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
	if ((cbCardData==0x4E)||(cbCardData==0x4F)) return true;
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
	if (cbCardColor==0x40) return cbCardValue+2;
	return (cbCardValue==1)?(cbCardValue+13):cbCardValue;
}


//出牌搜索
BYTE CGameLogicNew::SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagSearchCardResult *pSearchCardResult, bool bNoSearchBomb)
{
	return 0;
}



float CGameLogicNew::GetHandScore(vector<tagOutCardResultNew> &CardTypeResult, int minTypeCount)
{
	float score = 0;
	int cbHandCardCount = 0;
	int CT_THREE_COUNT = 0;
	int maxType = 0;
	for (int i = 0; i < CardTypeResult.size(); i++)
	{
		float tempScore = GetCardTypeScore(CardTypeResult[i]);
		CardTypeResult[i].cbCardScore = tempScore;
		score += tempScore;
		if (CardTypeResult[i].cbCardType>maxType)
		{
			maxType = CardTypeResult[i].cbCardType;
		}
		if (CardTypeResult[i].cbCardType==CT_THREE)
		{
			CT_THREE_COUNT++;
		}
	}
	if (maxType>CT_THREE&&CT_THREE_COUNT>=2)
	{
		score += 100.0;
	}
	//如果最小牌型数小于10的话,应该适当加分
	//score = score - CardTypeResult.size() * 7;
	
	return score;
}

float  CGameLogicNew::GetCardTypeScore(tagOutCardResultNew& CardTypeResult)
{
	//-- 2. maxCard：牌型大小的牌值，单张和对子就是自身，顺子和连对是最大那张牌，三带是有三张的那个牌，这个很容易理解，相同的牌型比较大小，就是比较maxCard
	BYTE cbCardData[MAX_COUNT] = { 0 };
	int  cbCardCount = CardTypeResult.cbCardCount;
	CopyMemory(cbCardData, CardTypeResult.cbResultCard, cbCardCount);
	BYTE cbMaxCard = 0;
	int type = GetCardType(cbCardData, cbCardCount, cbMaxCard);
	SortOutCardList(cbCardData, cbCardCount);
	//float typeScore[CT_TYPE_COUNT + 1] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
	int spaceScore = 0;
	for (int i = 0; i < CT_TYPE_COUNT;i++)
	{
		if (i == CT_ONE_DOUBLE)
		{
			spaceScore += 14;
		}
		else if (i==CT_TWO_DOUBLE)
		{
			spaceScore += 28;
		}
		else if (i == CT_FIVE_THREE_DEOUBLE)
		{
			spaceScore += 80;
		}
		else if (i == CT_FIVE_FOUR_ONE)
		{
			spaceScore += 200;
		}
		else if (i == CT_FIVE_STRAIGHT_FLUSH)
		{
			spaceScore += 50;
		}
		else if (i == CT_FIVE_TONG)
		{
			spaceScore += 100;
		}
		if (i==type)
		{
			spaceScore += (i - 1) * 14;
			if (i>=CT_FIVE_MIXED_FLUSH_BACK_A)
			{
				spaceScore -= 29;
			}
			else if (i>=CT_FIVE_MIXED_FLUSH_FIRST_A)
			{
				spaceScore -= 17;
			}
			break;
		}
	}

	if (type == CT_INVALID)
	{
		return 0;
	}
	else if (type == CT_SINGLE)
	{
		return  (float)GetCardLogicValue(cbCardData[0])/10 ;
	}
	else if (type == CT_ONE_DOUBLE)
	{
		float tempscore = (float)(GetCardLogicValue(cbCardData[0]) + spaceScore);
		return tempscore ;
	}
	else if (type == CT_TWO_DOUBLE)
	{
		float score = (float)GetCardLogicValue(cbCardData[2]);//10+1.0f  ;
		float tempscore = (float)GetCardLogicValue(cbCardData[0]) + spaceScore;
		return  tempscore + score ;
	}
	else if (type == CT_THREE)
	{
		float tempscore = (float)GetCardLogicValue(cbCardData[0])  + spaceScore;
		return  tempscore;
	}
	else if (type == CT_FIVE_MIXED_FLUSH_NO_A)
	{
		float tempscore = (float)GetCardLogicValue(cbCardData[0])  + spaceScore;
		return  tempscore;
	}
	else if (type == CT_FIVE_MIXED_FLUSH_FIRST_A)
	{
		float tempscore = (float)GetCardLogicValue(cbCardData[0]) + spaceScore;
		return  tempscore;
	}
	else if (type == CT_FIVE_MIXED_FLUSH_BACK_A)
	{
		float tempscore = (float)GetCardLogicValue(cbCardData[0]) + spaceScore;
		return  tempscore;
	}
	
	else if (type == CT_FIVE_FLUSH)
	{
		float tempscore = (float)GetCardLogicValue(cbCardData[0]) + spaceScore;
		return  tempscore;
	}
	else if (type == CT_FIVE_THREE_DEOUBLE)
	{
		float tempscore = (float)GetCardLogicValue(cbCardData[0]) + spaceScore;
		return  tempscore;
	}
	else if (type == CT_FIVE_FOUR_ONE)
	{
		float tempscore = (float)GetCardLogicValue(cbCardData[0]) + spaceScore;
		return  tempscore;

	}
	else if (type == CT_FIVE_STRAIGHT_FLUSH)
	{
		float tempscore = (float)GetCardLogicValue(cbCardData[0]) + spaceScore;
		return  tempscore;
	}
	else if (type == CT_FIVE_STRAIGHT_FLUSH_FIRST_A)
	{
		float tempscore = (float)GetCardLogicValue(cbCardData[0]) + spaceScore;
		return  tempscore;
	}
	else if (type == CT_FIVE_STRAIGHT_FLUSH_BACK_A)
	{
		float tempscore = (float)GetCardLogicValue(cbCardData[0]) + spaceScore;
		return  tempscore;
	}
	else if (type == CT_FIVE_TONG)
	{
		float tempscore = (float)GetCardLogicValue(cbCardData[0]) + spaceScore;
		return  tempscore;
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
	if ((cbTmpCardData[0] == 0x4F) && (cbTmpCardData[1] == 0x4E))
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
	/*if (cbHandCardCount < 6)
	{
		return;
	}*/
	//恢复扑克，防止分析时改变扑克
	//BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	//CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

	//BYTE cbFirstCard = 0;
	////去除王牌
	//for (BYTE i = 0; i < cbHandCardCount; ++i)	if (GetCardColor(cbTmpCardData[i]) != 0x40)	{ cbFirstCard = i; break; }

	//BYTE cbHandAllFourCardData[MAX_COUNT];
	//BYTE cbHandAllFourCardCount = 0;
	////抽取四张
	//GetAllBomCard(cbTmpCardData + cbFirstCard, cbHandCardCount - cbFirstCard, cbHandAllFourCardData, cbHandAllFourCardCount);

	////移除四条
	//BYTE cbRemainCard[MAX_COUNT];
	//BYTE cbRemainCardCount = cbHandCardCount - cbHandAllFourCardCount;
	//CopyMemory(cbRemainCard, cbTmpCardData, cbHandCardCount*sizeof(BYTE));
	//RemoveCard(cbHandAllFourCardData, cbHandAllFourCardCount, cbRemainCard, cbHandCardCount);
	//cbRemainCardCount = ClearReLogicValue(cbRemainCard, cbRemainCardCount);
	//for (BYTE Start = 0; Start < cbHandAllFourCardCount; Start += 4)
	//{
	//	BYTE Index;
	//	//单牌组合
	//	BYTE cbComCard[8];
	//	BYTE cbComResCard[MAX_RESULT_COUNT][8];
	//	int cbComResLen = 0;
	//	//单牌组合
	//	Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCard, 2, cbRemainCardCount, 2);
	//	for (int i = 0; i < cbComResLen; ++i)
	//	{
	//		//不能带对
	//		if (GetCardValue(cbComResCard[i][0]) == GetCardValue(cbComResCard[i][1])) continue;

	//		Index = CardTypeResult[CT_FIVE_FOUR_ONE].cbCardTypeCount;
	//		CardTypeResult[CT_FIVE_FOUR_ONE].cbCardType = CT_FIVE_FOUR_ONE;
	//		CopyMemory(CardTypeResult[CT_FIVE_FOUR_ONE].cbCardData[Index], cbHandAllFourCardData + Start, 4);
	//		CardTypeResult[CT_FIVE_FOUR_ONE].cbCardData[Index][4] = cbComResCard[i][0];
	//		CardTypeResult[CT_FIVE_FOUR_ONE].cbCardData[Index][4 + 1] = cbComResCard[i][1];
	//		CardTypeResult[CT_FIVE_FOUR_ONE].cbEachHandCardCount[Index] = 6;
	//		CardTypeResult[CT_FIVE_FOUR_ONE].cbCardTypeCount++;

	//		ASSERT(CardTypeResult[CT_FIVE_FOUR_ONE].cbCardTypeCount < MAX_TYPE_COUNT);
	//	}
	//}
}

void CGameLogicNew::GetFourTakeTwoResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	if (cbHandCardCount < 6)
	{
		return;
	}
	////恢复扑克，防止分析时改变扑克
	//BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	//CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

	//BYTE cbFirstCard = 0;
	////去除王牌
	//for (BYTE i = 0; i < cbHandCardCount; ++i)	if (GetCardColor(cbTmpCardData[i]) != 0x40)	{ cbFirstCard = i; break; }

	//BYTE cbHandAllFourCardData[MAX_COUNT];
	//BYTE cbHandAllFourCardCount = 0;

	////抽取四张
	//GetAllBomCard(cbTmpCardData + cbFirstCard, cbHandCardCount - cbFirstCard, cbHandAllFourCardData, cbHandAllFourCardCount);

	////移除四条
	//BYTE cbRemainCard[MAX_COUNT];
	//BYTE cbRemainCardCount = cbHandCardCount - cbHandAllFourCardCount;
	//CopyMemory(cbRemainCard, cbTmpCardData, cbHandCardCount*sizeof(BYTE));
	//RemoveCard(cbHandAllFourCardData, cbHandAllFourCardCount, cbRemainCard, cbHandCardCount);

	//for (BYTE Start = 0; Start < cbHandAllFourCardCount; Start += 4)
	//{
	//	//抽取对牌
	//	BYTE cbAllDoubleCardData[MAX_COUNT];
	//	BYTE cbAllDoubleCardCount = 0;
	//	GetAllDoubleCard(cbRemainCard, cbRemainCardCount, cbAllDoubleCardData, cbAllDoubleCardCount);

	//	BYTE cbDoubleCardIndex[10]; //对牌下标
	//	for (BYTE i = 0, j = 0; i < cbAllDoubleCardCount; i += 2, ++j)
	//		cbDoubleCardIndex[j] = i;

	//	//对牌组合
	//	BYTE cbComCard[8];
	//	BYTE cbComResCard[MAX_RESULT_COUNT][8];
	//	int cbComResLen = 0;

	//	//利用对牌的下标做组合，再根据下标提取出对牌
	//	Combination(cbComCard, 0, cbComResCard, cbComResLen, cbDoubleCardIndex, 2, cbAllDoubleCardCount / 2, 2);
	//	for (BYTE i = 0; i < cbComResLen; ++i)
	//	{
	//		BYTE Index = CardTypeResult[CT_FOUR_TAKE_TWO].cbCardTypeCount;
	//		CardTypeResult[CT_FOUR_TAKE_TWO].cbCardType = CT_FOUR_TAKE_TWO;
	//		CopyMemory(CardTypeResult[CT_FOUR_TAKE_TWO].cbCardData[Index], cbHandAllFourCardData + Start, 4);

	//		//保存对牌
	//		for (BYTE j = 0, k = 0; j < 4; ++j, k += 2)
	//		{
	//			CardTypeResult[CT_FOUR_TAKE_TWO].cbCardData[Index][4 + k] = cbAllDoubleCardData[cbComResCard[i][j]];
	//			CardTypeResult[CT_FOUR_TAKE_TWO].cbCardData[Index][4 + k + 1] = cbAllDoubleCardData[cbComResCard[i][j] + 1];
	//		}

	//		CardTypeResult[CT_FOUR_TAKE_TWO].cbEachHandCardCount[Index] = 8;
	//		CardTypeResult[CT_FOUR_TAKE_TWO].cbCardTypeCount++;

	//		ASSERT(CardTypeResult[CT_FOUR_TAKE_TWO].cbCardTypeCount < MAX_TYPE_COUNT);
	//	}
	//}
}

void CGameLogicNew::GetThreeResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	if (cbHandCardCount < 3)
	{
		return;
	}
	//恢复扑克，防止分析时改变扑克
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	int cbTmpCardCount = cbHandCardCount;
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);
	BYTE cbRemainCarData[MAX_COUNT] = { 0 };
	BYTE cbRemainCardCount = 0;

	BYTE cbWangCardData[MAX_COUNT];
	BYTE cbWangCount = 0;
	cbWangCount = RemoveCardWang(cbTmpCardData, cbHandCardCount, cbWangCardData);
	cbTmpCardCount -= cbWangCount;
	int maxSearch = 3;
	for (int m = 1; m <= maxSearch; m++)
	{
		//搜索同张
		if (m + cbWangCount >= maxSearch)
		{
			tagSearchCardResult SameCardResult = {};
			BYTE cbSameCardResultCount = SearchSameCard(cbTmpCardData, cbTmpCardCount, 0, m, &SameCardResult);
			for (BYTE i = 0; i < SameCardResult.cbSearchCount; i++)
			{
				CopyMemory(cbRemainCarData, cbTmpCardData, cbTmpCardCount);
				RemoveCard(SameCardResult.cbResultCard[i], m, cbRemainCarData, cbTmpCardCount);
				cbRemainCardCount = cbTmpCardCount - m;
				//单牌组合
				for (int j = 0; j < cbRemainCardCount; j++)
				{
					BYTE Index = CardTypeResult[CT_THREE].cbCardTypeCount;
					CardTypeResult[CT_THREE].cbCardType = CT_THREE;
					CopyMemory(CardTypeResult[CT_THREE].cbCardData[Index], SameCardResult.cbResultCard[i], m);
					for (int n = 0; n < (maxSearch - m); n++)
					{
						CardTypeResult[CT_THREE].cbCardData[Index][m + n] = cbWangCardData[n];
					}
					CardTypeResult[CT_THREE].cbEachHandCardCount[Index] = 3;
					CardTypeResult[CT_THREE].cbCardTypeCount++;
				}
				if (m == 3)
				{
					////单牌组合
					ZeroMemory(cbRemainCarData, sizeof(cbRemainCarData));
					CopyMemory(cbRemainCarData, SameCardResult.cbResultCard[i], 3);
					BYTE cbComCard[8];
					BYTE cbComResCard[MAX_RESULT_COUNT][8];
					int cbComResLen = 0;
					BYTE cbSingleCardCount = 2;
					cbRemainCardCount = 3;
					Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCarData, cbSingleCardCount, cbRemainCardCount, cbSingleCardCount);
					
					for (BYTE m = 0; m < cbComResLen; m++)
					{
						
						{
							BYTE Index = CardTypeResult[CT_ONE_DOUBLE].cbCardTypeCount;
							CardTypeResult[CT_ONE_DOUBLE].cbCardType = CT_ONE_DOUBLE;
							CardTypeResult[CT_ONE_DOUBLE].cbCardData[Index][0] = cbComResCard[m][0];
							CardTypeResult[CT_ONE_DOUBLE].cbCardData[Index][1] = cbComResCard[m][1];
							CardTypeResult[CT_ONE_DOUBLE].cbEachHandCardCount[Index] = 2;
							CardTypeResult[CT_ONE_DOUBLE].cbCardTypeCount++;
						}
						
					}
				}
			}
			
		}
	}
	
}

void CGameLogicNew::GetDoubleResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	if (cbHandCardCount < 2)
	{
		return;
	}
	//恢复扑克，防止分析时改变扑克
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	int cbTmpCardCount = cbHandCardCount;
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);
	BYTE cbRemainCarData[MAX_COUNT] = { 0 };
	BYTE cbRemainCardCount = 0;

	BYTE cbWangCardData[MAX_COUNT];
	BYTE cbWangCount = 0;
	cbWangCount = RemoveCardWang(cbTmpCardData, cbHandCardCount, cbWangCardData);
	cbTmpCardCount -= cbWangCount;
	int maxSearch = 2;
	for (int m = 1; m <= maxSearch; m++)
	{
		//搜索同张
		if (m + cbWangCount >= maxSearch)
		{
			tagSearchCardResult SameCardResult = {};
			BYTE cbSameCardResultCount = SearchSameCard(cbTmpCardData, cbTmpCardCount, 0, m, &SameCardResult);
			for (BYTE i = 0; i < SameCardResult.cbSearchCount; i++)
			{
				if (m == 2 && IsReResult(CardTypeResult, CT_ONE_DOUBLE, SameCardResult.cbResultCard[i], m) == true)
				{
					continue;
				}
				{
					BYTE Index = CardTypeResult[CT_ONE_DOUBLE].cbCardTypeCount;
					CardTypeResult[CT_ONE_DOUBLE].cbCardType = CT_ONE_DOUBLE;
					CopyMemory(CardTypeResult[CT_ONE_DOUBLE].cbCardData[Index], SameCardResult.cbResultCard[i], m);
					for (int n = 0; n < (maxSearch - m); n++)
					{
						CardTypeResult[CT_ONE_DOUBLE].cbCardData[Index][m + n] = cbWangCardData[n];
					}
					CardTypeResult[CT_ONE_DOUBLE].cbEachHandCardCount[Index] = 2;
					CardTypeResult[CT_ONE_DOUBLE].cbCardTypeCount++;
				}
			
			}
		}
	}
	
}

void CGameLogicNew::GetSingleResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);
	for (BYTE i = 0; i < cbHandCardCount; ++i)
	{
		//if (GetACardCount(cbHandCardData,cbHandCardCount,cbHandCardData[i])==1)
		
		if (GetCardColor(cbHandCardData[i])!=0x40)
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
	//恢复扑克，防止分析时改变扑克
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	int cbTmpCardCount = cbHandCardCount;
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);
	BYTE cbRemainCarData[MAX_COUNT] = { 0 };
	BYTE cbRemainCardCount = 0;

	BYTE cbWangCardData[MAX_COUNT];
	BYTE cbWangCount = 0;
	cbWangCount = RemoveCardWang(cbTmpCardData, cbHandCardCount, cbWangCardData);
	cbTmpCardCount -= cbWangCount;
	int maxSearch = 4;
	for (int m=1;m<= maxSearch;m++)
	{
		//搜索同张
		if (m +cbWangCount>= maxSearch)
		{
			tagSearchCardResult SameCardResult = {};
			BYTE cbSameCardResultCount = SearchSameCard(cbTmpCardData, cbTmpCardCount, 0, m, &SameCardResult);
			for (BYTE i = 0; i < SameCardResult.cbSearchCount; i++)
			{
				CopyMemory(cbRemainCarData, cbTmpCardData, cbTmpCardCount);
				RemoveCard(SameCardResult.cbResultCard[i], m, cbRemainCarData, cbTmpCardCount);
				cbRemainCardCount = cbTmpCardCount - m;
				//单牌组合
				for (int j = 0; j < cbRemainCardCount; j++)
				{
					BYTE Index = CardTypeResult[CT_FIVE_FOUR_ONE].cbCardTypeCount;
					CardTypeResult[CT_FIVE_FOUR_ONE].cbCardType = CT_FIVE_FOUR_ONE;
					CopyMemory(CardTypeResult[CT_FIVE_FOUR_ONE].cbCardData[Index], SameCardResult.cbResultCard[i], m);
					for (int n=0;n<(maxSearch -m);n++)
					{
						CardTypeResult[CT_FIVE_FOUR_ONE].cbCardData[Index][m+n]= cbWangCardData[n];
					}
					CardTypeResult[CT_FIVE_FOUR_ONE].cbCardData[Index][4] = cbRemainCarData[j];
					CardTypeResult[CT_FIVE_FOUR_ONE].cbEachHandCardCount[Index] = 5;
					CardTypeResult[CT_FIVE_FOUR_ONE].cbCardTypeCount++;
				}
			}
		}
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
	int   cbTmpCardCount = cbHandCardCount;
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

	BYTE cbRemainCarData[MAX_COUNT] = { 0 };
	BYTE cbRemainCardCount = 0;
	
	BYTE cbWangCardData[MAX_COUNT];
	BYTE cbWangCount = 0;
	cbWangCount = RemoveCardWang(cbTmpCardData, cbHandCardCount, cbWangCardData);
	cbTmpCardCount -= cbWangCount;
	int maxSearch = 3;
	for (int m = 1; m <= maxSearch; m++)
	{
		//搜索同张
		if (m + cbWangCount >= maxSearch)
		{
			tagSearchCardResult SameCardResult = {};
			BYTE cbSameCardResultCount = SearchSameCard(cbTmpCardData, cbTmpCardCount, 0, m, &SameCardResult);
			for (BYTE i = 0; i < SameCardResult.cbSearchCount; i++)
			{
				
				for (BYTE j = 0; j < CardTypeResult[CT_ONE_DOUBLE].cbCardTypeCount; j++)
				{
					if (GetCardLogicValue(CardTypeResult[CT_ONE_DOUBLE].cbCardData[j][0]) !=
						GetCardLogicValue(SameCardResult.cbResultCard[i][0]))
					{
						BYTE Index = CardTypeResult[CT_FIVE_THREE_DEOUBLE].cbCardTypeCount;
						CardTypeResult[CT_FIVE_THREE_DEOUBLE].cbCardType = CT_FIVE_THREE_DEOUBLE;
						CopyMemory(CardTypeResult[CT_FIVE_THREE_DEOUBLE].cbCardData[Index], SameCardResult.cbResultCard[i], m);
						for (int n = 0; n < (maxSearch - m); n++)
						{
							CardTypeResult[CT_FIVE_THREE_DEOUBLE].cbCardData[Index][m + n] = cbWangCardData[n];
						}
						CardTypeResult[CT_FIVE_THREE_DEOUBLE].cbCardData[Index][3] = CardTypeResult[CT_ONE_DOUBLE].cbCardData[j][0];
						CardTypeResult[CT_FIVE_THREE_DEOUBLE].cbCardData[Index][4] = CardTypeResult[CT_ONE_DOUBLE].cbCardData[j][1];
						CardTypeResult[CT_FIVE_THREE_DEOUBLE].cbEachHandCardCount[Index] = 5;
						CardTypeResult[CT_FIVE_THREE_DEOUBLE].cbCardTypeCount++;
					}

				}
				
			}

		}
	}
	
	
}

void CGameLogicNew::GetThreeTakeOneCardResult(BYTE * cbHandCardData,  BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	if (cbHandCardCount < 4)
	{
		return;
	}
	////恢复扑克，防止分析时改变扑克
	//BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	//CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

	//BYTE cbHandThreeCard[MAX_COUNT] = { 0 };
	//BYTE cbHandThreeCount = 0;

	////移除炸弹
	//BYTE cbAllBomCardData[MAX_COUNT];
	//BYTE cbAllBomCardCount = 0;

	///*优化*/
	//	BYTE cbTestHandThreeCard[MAX_COUNT] = { 0 };
	//BYTE cbTestHandThreeCount = 0;
	//GetAllThreeCard(cbTmpCardData, cbHandCardCount - cbAllBomCardCount, cbTestHandThreeCard, cbTestHandThreeCount);
	////if (cbHandCardCount > NORMAL_COUNT )
	////{
	////	GetAllBomCard(cbTmpCardData, cbHandCardCount, cbAllBomCardData, cbAllBomCardCount);
	////	RemoveCard(cbAllBomCardData, cbAllBomCardCount, cbTmpCardData, cbHandCardCount);
	////}


	//GetAllThreeCard(cbTmpCardData, cbHandCardCount - cbAllBomCardCount, cbHandThreeCard, cbHandThreeCount);

	//{
	//	
	//	//三条带一张
	//	for (BYTE i = 0; i < cbHandThreeCount; i += 3)
	//	{
	//		//恢复扑克，防止分析时改变扑克
	//		BYTE  cbTmpCardData[FULL_COUNT] = { 0 };
	//		CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

	//		//去掉三条
	//		BYTE cbRemainCardData[FULL_COUNT];
	//		CopyMemory(cbRemainCardData, cbTmpCardData, cbHandCardCount);
	//		BYTE cbRemainCardCount = cbHandCardCount - 3;
	//		RemoveCard(&cbHandThreeCard[i], 3, cbRemainCardData, cbHandCardCount);
	//		//cbRemainCardCount = ClearReLogicValue(cbRemainCardData, cbRemainCardCount);
	//		//三条带一张
	//		for (BYTE j = 0; j < cbRemainCardCount; ++j)
	//		{
	//			{
	//				BYTE Index = CardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount;
	//				CardTypeResult[CT_THREE_TAKE_ONE].cbCardType = CT_THREE_TAKE_ONE;
	//				CardTypeResult[CT_THREE_TAKE_ONE].cbCardData[Index][0] = cbHandThreeCard[i];
	//				CardTypeResult[CT_THREE_TAKE_ONE].cbCardData[Index][1] = cbHandThreeCard[i + 1];
	//				CardTypeResult[CT_THREE_TAKE_ONE].cbCardData[Index][2] = cbHandThreeCard[i + 2];
	//				CardTypeResult[CT_THREE_TAKE_ONE].cbCardData[Index][3] = cbRemainCardData[j];
	//				CardTypeResult[CT_THREE_TAKE_ONE].cbEachHandCardCount[Index] = 4;
	//				CardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount++;
	//			}
	//		}
	//	}
	//}
	////三连带单
	//BYTE cbLeftThreeCardCount = cbHandThreeCount;
	//bool bFindThreeLine = true;
	//BYTE cbLastIndex = 0;
	//if (GetCardLogicValue(cbHandThreeCard[0]) == 15) cbLastIndex = 3;
	//while (cbLeftThreeCardCount >= 6 && bFindThreeLine)
	//{
	//	BYTE cbLastLogicCard = GetCardLogicValue(cbHandThreeCard[cbLastIndex]);
	//	BYTE cbThreeLineCard[MAX_COUNT];
	//	BYTE cbThreeLineCardCount = 3;
	//	cbThreeLineCard[0] = cbHandThreeCard[cbLastIndex];
	//	cbThreeLineCard[1] = cbHandThreeCard[cbLastIndex + 1];
	//	cbThreeLineCard[2] = cbHandThreeCard[cbLastIndex + 2];

	//	bFindThreeLine = false;
	//	for (BYTE j = 3 + cbLastIndex; j < cbLeftThreeCardCount; j += 3)
	//	{
	//		//连续判断
	//		if (1 != (cbLastLogicCard - (GetCardLogicValue(cbHandThreeCard[j]))))
	//		{
	//			cbLastIndex = j;
	//			if (cbLeftThreeCardCount - j >= 6) bFindThreeLine = true;

	//			break;
	//		}

	//		cbLastLogicCard = GetCardLogicValue(cbHandThreeCard[j]);
	//		cbThreeLineCard[cbThreeLineCardCount] = cbHandThreeCard[j];
	//		cbThreeLineCard[cbThreeLineCardCount + 1] = cbHandThreeCard[j + 1];
	//		cbThreeLineCard[cbThreeLineCardCount + 2] = cbHandThreeCard[j + 2];
	//		cbThreeLineCardCount += 3;
	//	}
	//	if (cbThreeLineCardCount > 3)
	//	{
	//		BYTE Index;

	//		BYTE cbRemainCard[MAX_COUNT];
	//		BYTE cbRemainCardCount = cbHandCardCount - cbAllBomCardCount - cbHandThreeCount;


	//		//移除三条（还应该移除炸弹王等）
	//		CopyMemory(cbRemainCard, cbTmpCardData, (cbHandCardCount - cbAllBomCardCount)*sizeof(BYTE));
	//		RemoveCard(cbHandThreeCard, cbHandThreeCount, cbRemainCard, cbHandCardCount - cbAllBomCardCount);
	//		//计算单牌
	//		BYTE cbSingleCard[MAX_COUNT] = { 0 };
	//		BYTE cbSingleCardCount = 0;
	//		GetAllSingleCard(cbRemainCard, cbRemainCardCount, cbSingleCard, cbSingleCardCount);
	//		//策略优化
	//		if (cbSingleCardCount >= 2 && cbHandCardCount >= 17)
	//		{
	//			ZeroMemory(cbRemainCard, MAX_COUNT);
	//			CopyMemory(cbRemainCard, cbSingleCard, cbSingleCardCount);
	//			cbRemainCardCount = cbSingleCardCount;
	//		}
	//		SortCardList(cbRemainCard, cbRemainCardCount, ST_ASCENDING);
	//		for (BYTE start = 0; start < cbThreeLineCardCount - 3; start += 3)
	//		{
	//			//本顺数目
	//			BYTE cbThisTreeLineCardCount = cbThreeLineCardCount - start;
	//			//单牌个数
	//			BYTE cbSingleCardCount = (cbThisTreeLineCardCount) / 3;

	//			//单牌不够
	//			if (cbRemainCardCount < cbSingleCardCount) continue;

	//			//单牌组合
	//			BYTE cbComCard[8];
	//			BYTE cbComResCard[MAX_RESULT_COUNT][8];
	//			int cbComResLen = 0;

	//			Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCard, cbSingleCardCount, cbRemainCardCount, cbSingleCardCount);
	//		
	//			for (BYTE i = 0; i < cbComResLen; ++i)
	//			{
	//				vector<BYTE> vecCard;
	//				for (int k = 0; k < cbThisTreeLineCardCount; k++)
	//				{
	//					vecCard.push_back(cbThreeLineCard[start+k]);
	//				}
	//				for (int k = 0; k < cbSingleCardCount; k++)
	//				{
	//					vecCard.push_back(cbComResCard[i][k]);
	//				}
	//				{
	//					Index = CardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount;
	//					CardTypeResult[CT_THREE_TAKE_ONE].cbCardType = CT_THREE_TAKE_ONE;
	//					//保存三条
	//					CopyMemory(CardTypeResult[CT_THREE_TAKE_ONE].cbCardData[Index], cbThreeLineCard + start, sizeof(BYTE)*cbThisTreeLineCardCount);
	//					//保存单牌
	//					CopyMemory(CardTypeResult[CT_THREE_TAKE_ONE].cbCardData[Index] + cbThisTreeLineCardCount, cbComResCard[i], cbSingleCardCount);


	//					CardTypeResult[CT_THREE_TAKE_ONE].cbEachHandCardCount[Index] = cbThisTreeLineCardCount + cbSingleCardCount;
	//					CardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount++;

	//					ASSERT(CardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount < MAX_TYPE_COUNT);
	//				}
	//			}

	//		}

	//		//移除三连
	//		bFindThreeLine = true;
	//		RemoveCard(cbThreeLineCard, cbThreeLineCardCount, cbHandThreeCard, cbLeftThreeCardCount);
	//		cbLeftThreeCardCount -= cbThreeLineCardCount;
	//	}
	//}
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
	//BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	//CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

	////变量定义
	//int cbBlockCount = 3;
	////搜索边牌
	//tagSearchCardResult tmpSearchCardResult = {};
	////搜索边牌
	//for (int i = 2; i < 7; i++)
	//{
	//	SearchLineCardType(cbTmpCardData, cbHandCardCount, 0, cbBlockCount, i, &tmpSearchCardResult);
	//	if (tmpSearchCardResult.cbSearchCount > 0)
	//	{
	//		//保存数据
	//		for (int j = 0; j < tmpSearchCardResult.cbSearchCount; j++)
	//		{
	//			int Index = CardTypeResult[CT_THREE_LINE].cbCardTypeCount;
	//			CardTypeResult[CT_THREE_LINE].cbCardType = CT_THREE_LINE;
	//			CopyMemory(CardTypeResult[CT_THREE_LINE].cbCardData[Index], tmpSearchCardResult.cbResultCard[j], tmpSearchCardResult.cbCardCount[j]);
	//			CardTypeResult[CT_THREE_LINE].cbEachHandCardCount[Index] = tmpSearchCardResult.cbCardCount[j];
	//			CardTypeResult[CT_THREE_LINE].cbCardTypeCount++;
	//		}
	//	}
	//	else
	//	{
	//		break;
	//	}
	//}
}

void CGameLogicNew::GetDoubleLineCardResult(BYTE *cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	if (cbHandCardCount < 4)
	{
		return;
	}
	//对连类型
		//恢复扑克，防止分析时改变扑克
		BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
		CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);
		BYTE cbRemainCarData[MAX_COUNT] = { 0 };
		BYTE cbRemainCardCount = 0;
		 //变量定义
		int cbBlockCount = 2;
		//搜索边牌
		tagSearchCardResult tmpSearchCardResult = {};
			//搜索边牌
		BYTE cbSameCardResultCount = SearchSameCard(cbTmpCardData, cbHandCardCount, 0, 2, &tmpSearchCardResult);
		if (tmpSearchCardResult.cbSearchCount > 1)
		{
			//保存数据
			for (int i = 0; i < tmpSearchCardResult.cbSearchCount; i++)
			{
				for (int j = 1; j < tmpSearchCardResult.cbSearchCount; j++)
				{
					CopyMemory(cbRemainCarData, cbTmpCardData, cbHandCardCount);
					RemoveCard(tmpSearchCardResult.cbResultCard[i], 2, cbRemainCarData, cbHandCardCount);
					RemoveCard(tmpSearchCardResult.cbResultCard[j], 2, cbRemainCarData, cbHandCardCount-2);
					cbRemainCardCount = cbHandCardCount - 4;
					//单牌组合
					for (int k = 0; k < cbRemainCardCount; k++)
					{
						int Index = CardTypeResult[CT_TWO_DOUBLE].cbCardTypeCount;
						CardTypeResult[CT_TWO_DOUBLE].cbCardType = CT_TWO_DOUBLE;
						CopyMemory(CardTypeResult[CT_TWO_DOUBLE].cbCardData[Index], tmpSearchCardResult.cbResultCard[j], tmpSearchCardResult.cbCardCount[j]);
						CopyMemory(CardTypeResult[CT_TWO_DOUBLE].cbCardData[Index] + 2, tmpSearchCardResult.cbResultCard[i], tmpSearchCardResult.cbCardCount[i]);
						CardTypeResult[CT_TWO_DOUBLE].cbCardData[Index][4] = cbRemainCarData[k];
						CardTypeResult[CT_TWO_DOUBLE].cbEachHandCardCount[Index] = tmpSearchCardResult.cbCardCount[j] * 2+1;
						if (GetCardLogicValue(CardTypeResult[CT_TWO_DOUBLE].cbCardData[Index][2])>GetCardLogicValue(CardTypeResult[CT_TWO_DOUBLE].cbCardData[Index][0]))
						{
							SwitchArray(CardTypeResult[CT_TWO_DOUBLE].cbCardData[Index], CardTypeResult[CT_TWO_DOUBLE].cbCardData[Index] + 2, 2);
						}
						CardTypeResult[CT_TWO_DOUBLE].cbCardTypeCount++;
					}
					
				}
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
	int   tmpCardCount = cbHandCardCount;
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

		 //搜索边牌
		tagSearchCardResult tmpSearchCardResult = {};
		BYTE cbLineCardResult5 = SearchAllLineCardType(cbTmpCardData, tmpCardCount, 5, &tmpSearchCardResult);
	if (tmpSearchCardResult.cbSearchCount > 0)
	{
		//策略优化, 如果有顺子有重叠数据, 先让其他不重叠
		for (int j = 0; j < tmpSearchCardResult.cbSearchCount; j++)
		{
			
				BYTE cbMaxCard = 0;
				BYTE bNextType = GetCardType(tmpSearchCardResult.cbResultCard[j], tmpSearchCardResult.cbCardCount[j], cbMaxCard);
				if (bNextType == CT_INVALID)
				{
					continue;;
				}
			if (IsReResult(CardTypeResult, bNextType, tmpSearchCardResult.cbResultCard[j], tmpSearchCardResult.cbCardCount[j]) == false)
			{
				int Index = CardTypeResult[bNextType].cbCardTypeCount;
				CardTypeResult[bNextType].cbCardType = bNextType;
				CopyMemory(CardTypeResult[bNextType].cbCardData[Index], tmpSearchCardResult.cbResultCard[j], tmpSearchCardResult.cbCardCount[j]);
				CardTypeResult[bNextType].cbEachHandCardCount[Index] = tmpSearchCardResult.cbCardCount[j];
				CardTypeResult[bNextType].cbCardTypeCount++;
			}
			
		}
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

void com(vector<int> &candidates, int start, int sum, int target, int path[], int &pathCount, vector<tagOutCardResultNew>& resultVec, vector<vector<tagOutCardResultNew> > &resultAllVec)
{
	if (sum > target)//超出目标值 退出
		return;
	if (pathCount >= 2)
	{
		bool tableRe[100] = { 0 };
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
		 int tableRe[100] = { 0 };
		for (int j = 0; j < pathCount; j++)
		{
			for (int i = 0; i < resultVec[path[j]].cbCardCount; i++)
			{
				BYTE tempIndex = resultVec[path[j]].cbResultCard[i];
				tableRe[tempIndex] ++;
				if (tableRe[tempIndex]>tableRe2[tempIndex])
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
		int   vecTempIndex[20] = { 0 };
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
int CGameLogicNew::FindCardKindMinNum(BYTE const cbHandCardData[], BYTE const cbHandCardCount, tagOutCardTypeResultNew CardTypeResult[CT_TYPE_COUNT], vector<tagOutCardResultNew>   &CardResult, vector<vector<tagOutCardResultNew>>   &CardResultShao, float &MinTypeScore, vector<float> &MinTypeScoreShao, bool & bBiYing, bool bSearchBiYing)
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
	ZeroMemory(tableRe2, sizeof(tableRe2));
	for (int i = 0; i < cbHandCardCount; i++)
	{
		tableRe2[cbHandCardData[i]]++;
	}
	BYTE cbTmpHReminCardData[MAX_COUNT] = { 0 };
	//保留扑克，防止分析时改变扑克
	BYTE cbReserveCardData[MAX_COUNT];
	BYTE cbReserveCardCount=cbHandCardCount;
	CopyMemory(cbReserveCardData, cbHandCardData, cbHandCardCount);
	
	/*BYTE MISSILE_CARD[] = { 0x4E, 0x4F };
	if (RemoveCard(MISSILE_CARD, sizeof(MISSILE_CARD), cbReserveCardData, cbReserveCardCount))
	{
		cbReserveCardCount -= sizeof(MISSILE_CARD);
	}*/
	int iLeftCardCount = cbReserveCardCount;
	SortCardList(cbReserveCardData, cbReserveCardCount, ST_ORDER);
	CopyMemory(cbTmpHReminCardData, cbReserveCardData, cbReserveCardCount);
	//炸弹类型
	GetBombCardResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);

	//iLeftCardCount = RemoveTypeCard(CardTypeResult[CT_FIVE_FOUR_ONE], cbTmpHReminCardData, iLeftCardCount);
	//	//--跑得快里面炸弹类型价值比较高, 所以需要将炸弹先移出来
	//CopyMemory(cbReserveCardData, cbTmpHReminCardData, iLeftCardCount);
	//cbReserveCardCount = iLeftCardCount;

	//获取同花
	GetTongHuaResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
	//三条类型
	GetThreeResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
	//对连类型
	GetDoubleLineCardResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
	//单连类型
	GetSingleLineCardResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
	
	//对牌类型
	GetDoubleResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);

	//三带一对
	GetThreeTakeTwoCardResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
	//单牌类型
	GetSingleResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
	
	//单牌类型
	CopyMemory(cbTmpHReminCardData, cbReserveCardData, cbReserveCardCount);
	iLeftCardCount = cbReserveCardCount;
	

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
	for (int i = 0; i < 2;i++)
	{
		vector<tagOutCardResultNew> temp;
		CardResultShao.push_back(temp);
		MinTypeScoreShao.push_back(0);
	}
	for (int j = 0; j < 4;j++)
	{
		for (int i = 0; i < resultAllVec[j].size(); i++)
		{
			float tempScore = GetHandScore(resultAllVec[j].at(i), MinTypeCount);
			if (/*resultAllVec[j].at(i).size() < MinTypeCount || ((resultAllVec[j].at(i).size() == MinTypeCount &&*/ tempScore > MinTypeScore)
			{
				MinTypeScore = tempScore;
				index_x = j;
				index_y = i;
				MinTypeCount = resultAllVec[j].at(i).size();
			}
			
			if (resultAllVec[j].at(i).size() == 3 && tempScore >MinTypeScoreShao[0])
			{
				CardResultShao[0]=resultAllVec[j][i];
				MinTypeScoreShao[0]=tempScore;
			}
			else if (resultAllVec[j].at(i).size() == 4 && tempScore >MinTypeScoreShao[1])
			{
				CardResultShao[1] = resultAllVec[j][i];
				MinTypeScoreShao[1]=tempScore;
			}
			else if (m_bSanDaYi&&resultAllVec[j].at(i).size() <=7 /*&& tempScore > MinTypeScoreShao[1]*/)
			{
				CardResultShao[1] = resultAllVec[j][i];
				MinTypeScoreShao[1] = tempScore;
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





VOID CGameLogicNew::SortOutCardList(BYTE cbCardData[], BYTE cbCardCount)
{
	//获取牌型
	BYTE cbMaxCard = 0;
	BYTE cbCardType = GetCardType(cbCardData, cbCardCount, cbMaxCard);

	if (cbCardType == CT_FIVE_THREE_DEOUBLE )
	{
		//分析牌
		SortCardList(cbCardData, cbCardCount, ST_ORDER);
		tagAnalyseResultNew AnalyseResult = {};
		AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

		BYTE tmpCardCount = AnalyseResult.cbBlockCount[2] * 3;
		CopyMemory(cbCardData, AnalyseResult.cbCardData[2], sizeof(BYTE)*tmpCardCount);
		if (GetTakeWangCount(cbCardData, cbCardCount)>0)
		{
			for (int i =0; i < CountArray(AnalyseResult.cbBlockCount); i++)
			{
				if (i == 2) continue;

				if (AnalyseResult.cbBlockCount[i] > 0)
				{
					CopyMemory(&cbCardData[tmpCardCount], AnalyseResult.cbCardData[i],
						sizeof(BYTE)*(i + 1)*AnalyseResult.cbBlockCount[i]);
					tmpCardCount += (i + 1)*AnalyseResult.cbBlockCount[i];
				}
			}
		}
		else{
			for (int i = CountArray(AnalyseResult.cbBlockCount) - 1; i >= 0; i--)
			{
				if (i == 2) continue;

				if (AnalyseResult.cbBlockCount[i] > 0)
				{
					CopyMemory(&cbCardData[tmpCardCount], AnalyseResult.cbCardData[i],
						sizeof(BYTE)*(i + 1)*AnalyseResult.cbBlockCount[i]);
					tmpCardCount += (i + 1)*AnalyseResult.cbBlockCount[i];
				}
			}
		}
	}
	if (cbCardType == CT_TWO_DOUBLE)
	{
		//分析牌
		tagAnalyseResultNew AnalyseResult = {};
		AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

		BYTE tmpCardCount = AnalyseResult.cbBlockCount[1] * 2;
		CopyMemory(cbCardData, AnalyseResult.cbCardData[1], sizeof(BYTE)*tmpCardCount);
		for (int i = CountArray(AnalyseResult.cbBlockCount) - 1; i >= 0; i--)
		{
			if (i == 1) continue;

			if (AnalyseResult.cbBlockCount[i] > 0)
			{
				CopyMemory(&cbCardData[tmpCardCount], AnalyseResult.cbCardData[i],
					sizeof(BYTE)*(i + 1)*AnalyseResult.cbBlockCount[i]);
				tmpCardCount += (i + 1)*AnalyseResult.cbBlockCount[i];
			}
		}
	}
	else if (cbCardType == CT_FIVE_FOUR_ONE )
	{
		//分析牌
		tagAnalyseResultNew AnalyseResult = {};
		AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

		BYTE tmpCardCount = AnalyseResult.cbBlockCount[3] * 4;
		CopyMemory(cbCardData, AnalyseResult.cbCardData[3], sizeof(BYTE)*tmpCardCount);
		for (int i = CountArray(AnalyseResult.cbBlockCount) - 1; i >= 0; i--)
		{
			if (i == 3) continue;

			if (AnalyseResult.cbBlockCount[i] > 0)
			{
				CopyMemory(&cbCardData[tmpCardCount], AnalyseResult.cbCardData[i],
					sizeof(BYTE)*(i + 1)*AnalyseResult.cbBlockCount[i]);
				tmpCardCount += (i + 1)*AnalyseResult.cbBlockCount[i];
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
	if (cbSameCardCount==0)
	{
		return 0;
	}
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
	if (cbSameCardCount==1)
	{
		for (int i=0;i<cbHandCardCount;i++)
		{
			pSearchCardResult->cbResultCard[cbResultCount][0] = cbHandCardData[i];
			pSearchCardResult->cbCardCount[cbResultCount] = cbSameCardCount;
			cbResultCount++;
		}
	}
	else 
	{
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
						cbSameCardCount * sizeof(BYTE));
					pSearchCardResult->cbCardCount[cbResultCount] = cbSameCardCount;

					cbResultCount++;
				}
			}

			cbBlockIndex++;
		} while (cbBlockIndex < CountArray(AnalyseResult.cbBlockCount));
	}
	

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
	ASSERT(cbSameCount == 3 || cbSameCount == 4);
	ASSERT(cbTakeCardCount == 1 || cbTakeCardCount == 2);
	if (cbSameCount != 3 && cbSameCount != 4)
		return cbResultCount;
	if (cbTakeCardCount != 1 && cbTakeCardCount != 2)
		return cbResultCount;

	//长度判断
	if (cbSameCount == 4 && cbHandCardCount < cbSameCount + cbTakeCardCount * 2 ||
		cbHandCardCount < cbSameCount + cbTakeCardCount)
		return cbResultCount;

	//构造扑克
	BYTE cbCardData[FULL_COUNT];
	BYTE cbCardCount = cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//排列扑克
	SortCardList(cbCardData, cbCardCount, ST_ORDER);

	//搜索同张
	tagSearchCardResult SameCardResult = {};
	BYTE cbSameCardResultCount = SearchSameCard(cbCardData, cbCardCount, cbReferCard, cbSameCount, &SameCardResult);

	if (cbSameCardResultCount > 0)
	{
		//分析扑克
		tagAnalyseResultNew AnalyseResult;
		AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

		//需要牌数
		BYTE cbNeedCount = cbSameCount + cbTakeCardCount;
		if (cbSameCount == 4) cbNeedCount += cbTakeCardCount;
		int startValue = cbTakeCardCount - 1;
		if (cbSameCount==3&&cbTakeCardCount == 2 && G_THREE_TAKE_TWO_DAN)
		{
			startValue = 0;
		}
				
		//提取带牌
		for (BYTE i = 0; i < cbSameCardResultCount; i++)
		{
			bool bMerge = false;

			for (BYTE j = startValue; j < CountArray(AnalyseResult.cbBlockCount); j++)
			{
				for (BYTE k = 0; k < AnalyseResult.cbBlockCount[j]; k++)
				{
					//从小到大
					BYTE cbIndex = (AnalyseResult.cbBlockCount[j] - k - 1)*(j + 1);

					//过滤相同牌
					if (GetCardValue(SameCardResult.cbResultCard[i][0]) ==
						GetCardValue(AnalyseResult.cbCardData[j][cbIndex]))
						continue;
					if (cbHandCardCount<=MAX_COUNT)
					{
						if ((cbTakeCardCount == 1 && (cbSameCount == 3 || cbSameCount == 4)) || (G_THREE_TAKE_TWO_DAN&&cbTakeCardCount == 2 && cbSameCount==3))
						{
							//恢复扑克，防止分析时改变扑克
							BYTE  cbTmpCardData[FULL_COUNT] = { 0 };
							CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

							BYTE cbHandThreeCard[FULL_COUNT] = { 0 };
							BYTE cbHandThreeCount = 0;

							//去掉三条
							BYTE cbRemainCardData[FULL_COUNT];
							CopyMemory(cbRemainCardData, cbTmpCardData, cbHandCardCount);
							BYTE cbRemainCardCount = cbHandCardCount - SameCardResult.cbCardCount[i];
							RemoveCard(SameCardResult.cbResultCard[i], SameCardResult.cbCardCount[i], cbRemainCardData, cbHandCardCount);
							if (cbSameCount == 4 || (G_THREE_TAKE_TWO_DAN&&cbTakeCardCount == 2 && cbSameCount == 3))
							{
								//单牌组合
								BYTE cbComCard[8];
								BYTE cbComResCard[MAX_RESULT_COUNT][8];
								int cbComResLen = 0;
								BYTE cbSingleCardCount = 2;
								//cbRemainCardCount = ClearReLogicValue(cbRemainCardData, cbRemainCardCount);
								Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCardData, cbSingleCardCount, cbRemainCardCount, cbSingleCardCount);
								for (BYTE m = 0; m < cbComResLen; m++)
								{
									//复制带牌
									CopyMemory(&SameCardResult.cbResultCard[i][cbSameCount], cbComResCard[m],
										sizeof(BYTE)*(cbNeedCount - cbSameCount));
									SameCardResult.cbCardCount[i] = cbNeedCount;
									//复制结果
									CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], SameCardResult.cbResultCard[i],
										sizeof(BYTE)*SameCardResult.cbCardCount[i]);
									pSearchCardResult->cbCardCount[cbResultCount] = SameCardResult.cbCardCount[i];
									cbResultCount++;
								}
							}
							else
							{
								for (int m = 0; m < cbRemainCardCount; m += cbTakeCardCount)
								{
									//复制带牌
									CopyMemory(&SameCardResult.cbResultCard[i][cbSameCount], &cbRemainCardData[m],
										sizeof(BYTE)*(cbNeedCount - cbSameCount));
									SameCardResult.cbCardCount[i] = cbNeedCount;
									//复制结果
									CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], SameCardResult.cbResultCard[i],
										sizeof(BYTE)*SameCardResult.cbCardCount[i]);
									pSearchCardResult->cbCardCount[cbResultCount] = SameCardResult.cbCardCount[i];
									cbResultCount++;
								}
							}

							bMerge = true;

							//下一组合
							break;
						}
						else if (cbTakeCardCount == 2 && cbSameCount == 3)
						{
							//恢复扑克，防止分析时改变扑克
							BYTE  cbTmpCardData[FULL_COUNT] = { 0 };
							CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

							BYTE cbHandThreeCard[FULL_COUNT] = { 0 };
							BYTE cbHandThreeCount = 0;

							//去掉三条
							BYTE cbRemainCardData[FULL_COUNT];
							CopyMemory(cbRemainCardData, cbTmpCardData, cbHandCardCount);
							BYTE cbRemainCardCount = cbHandCardCount - SameCardResult.cbCardCount[i];
							RemoveCard(SameCardResult.cbResultCard[i], SameCardResult.cbCardCount[i], cbRemainCardData, cbHandCardCount);
							tagSearchCardResult tmpSearchCardResult = {};
						
							int	cbTmpCount = SearchSameCard(cbRemainCardData, cbRemainCardCount, 0, 2, &tmpSearchCardResult);

							for (int m = 0; m < tmpSearchCardResult.cbSearchCount; m++)
							{
								//复制带牌
								CopyMemory(&SameCardResult.cbResultCard[i][3], tmpSearchCardResult.cbResultCard[m],
									sizeof(BYTE)*cbTakeCardCount);
								SameCardResult.cbCardCount[i] = cbSameCount + 2;
								//复制结果
								CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], SameCardResult.cbResultCard[i],
									sizeof(BYTE)*SameCardResult.cbCardCount[i]);
								pSearchCardResult->cbCardCount[cbResultCount] = SameCardResult.cbCardCount[i];
								cbResultCount++;
							}
							bMerge = true;

							//下一组合
							break;
						}
					}
					else
					{
						//复制带牌
						BYTE cbCount = SameCardResult.cbCardCount[i];
						CopyMemory(&SameCardResult.cbResultCard[i][cbCount], &AnalyseResult.cbCardData[j][cbIndex],
							sizeof(BYTE)*cbTakeCardCount);
						SameCardResult.cbCardCount[i] += cbTakeCardCount;

						if (SameCardResult.cbCardCount[i] < cbNeedCount) continue;

						if (pSearchCardResult == NULL) return 1;

						//复制结果
						CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], SameCardResult.cbResultCard[i],
							sizeof(BYTE)*SameCardResult.cbCardCount[i]);
						pSearchCardResult->cbCardCount[cbResultCount] = SameCardResult.cbCardCount[i];
						cbResultCount++;

						bMerge = true;

						//下一组合
						break;
					}
				
				}

				if (bMerge) break;
			}
		}
	}

	if (pSearchCardResult)
		pSearchCardResult->cbSearchCount = cbResultCount;
	return cbResultCount;
}

BYTE CGameLogicNew::SearchThreeTwoLine(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagSearchCardResult *pSearchCardResult)
{
	//设置结果
	if (pSearchCardResult)
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));

	//变量定义
	tagSearchCardResult tmpSearchResult = {};
	tagSearchCardResult tmpSingleWing = {};
	tagSearchCardResult tmpDoubleWing = {};
	BYTE cbTmpResultCount = 0;

	//搜索连牌
	cbTmpResultCount = SearchLineCardType(cbHandCardData, cbHandCardCount, 0, 3, 0, &tmpSearchResult);

	if (cbTmpResultCount > 0)
	{
		//提取带牌
		for (BYTE i = 0; i < cbTmpResultCount; i++)
		{
			//变量定义
			BYTE cbTmpCardData[FULL_COUNT];
			BYTE cbTmpCardCount = cbHandCardCount;

			//不够牌
			if (cbHandCardCount - tmpSearchResult.cbCardCount[i] < tmpSearchResult.cbCardCount[i] / 3)
			{
				BYTE cbNeedDelCount = 3;
				while (cbHandCardCount + cbNeedDelCount - tmpSearchResult.cbCardCount[i] < (tmpSearchResult.cbCardCount[i] - cbNeedDelCount) / 3)
					cbNeedDelCount += 3;
				//不够连牌
				if ((tmpSearchResult.cbCardCount[i] - cbNeedDelCount) / 3 < 2)
				{
					//废除连牌
					continue;
				}

				//拆分连牌
				RemoveCard(tmpSearchResult.cbResultCard[i], cbNeedDelCount, tmpSearchResult.cbResultCard[i],
					tmpSearchResult.cbCardCount[i]);
				tmpSearchResult.cbCardCount[i] -= cbNeedDelCount;
			}

			if (pSearchCardResult == NULL) return 1;

			//删除连牌
			CopyMemory(cbTmpCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
			VERIFY(RemoveCard(tmpSearchResult.cbResultCard[i], tmpSearchResult.cbCardCount[i],
				cbTmpCardData, cbTmpCardCount));
			cbTmpCardCount -= tmpSearchResult.cbCardCount[i];

			//组合飞机
			BYTE cbNeedCount = tmpSearchResult.cbCardCount[i] / 3;
			ASSERT(cbNeedCount <= cbTmpCardCount);

			BYTE cbResultCount = tmpSingleWing.cbSearchCount++;
			CopyMemory(tmpSingleWing.cbResultCard[cbResultCount], tmpSearchResult.cbResultCard[i],
				sizeof(BYTE)*tmpSearchResult.cbCardCount[i]);
			CopyMemory(&tmpSingleWing.cbResultCard[cbResultCount][tmpSearchResult.cbCardCount[i]],
				&cbTmpCardData[cbTmpCardCount - cbNeedCount], sizeof(BYTE)*cbNeedCount);
			tmpSingleWing.cbCardCount[i] = tmpSearchResult.cbCardCount[i] + cbNeedCount;

			//不够带翅膀
			if (cbTmpCardCount < tmpSearchResult.cbCardCount[i] / 3 * 2)
			{
				BYTE cbNeedDelCount = 3;
				while (cbTmpCardCount + cbNeedDelCount - tmpSearchResult.cbCardCount[i] < (tmpSearchResult.cbCardCount[i] - cbNeedDelCount) / 3 * 2)
					cbNeedDelCount += 3;
				//不够连牌
				if ((tmpSearchResult.cbCardCount[i] - cbNeedDelCount) / 3 < 2)
				{
					//废除连牌
					continue;
				}

				//拆分连牌
				RemoveCard(tmpSearchResult.cbResultCard[i], cbNeedDelCount, tmpSearchResult.cbResultCard[i],
					tmpSearchResult.cbCardCount[i]);
				tmpSearchResult.cbCardCount[i] -= cbNeedDelCount;

				//重新删除连牌
				CopyMemory(cbTmpCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
				VERIFY(RemoveCard(tmpSearchResult.cbResultCard[i], tmpSearchResult.cbCardCount[i],
					cbTmpCardData, cbTmpCardCount));
				cbTmpCardCount = cbHandCardCount - tmpSearchResult.cbCardCount[i];
			}

			//分析牌
			tagAnalyseResultNew  TmpResult = {};
			AnalysebCardData(cbTmpCardData, cbTmpCardCount, TmpResult);

			//提取翅膀
			BYTE cbDistillCard[FULL_COUNT] = {};
			BYTE cbDistillCount = 0;
			BYTE cbLineCount = tmpSearchResult.cbCardCount[i] / 3;
			for (BYTE j = 1; j < CountArray(TmpResult.cbBlockCount); j++)
			{
				if (TmpResult.cbBlockCount[j] > 0)
				{
					if (j + 1 == 2 && TmpResult.cbBlockCount[j] >= cbLineCount)
					{
						BYTE cbTmpBlockCount = TmpResult.cbBlockCount[j];
						CopyMemory(cbDistillCard, &TmpResult.cbCardData[j][(cbTmpBlockCount - cbLineCount)*(j + 1)],
							sizeof(BYTE)*(j + 1)*cbLineCount);
						cbDistillCount = (j + 1)*cbLineCount;
						break;
					}
					else
					{
						for (BYTE k = 0; k < TmpResult.cbBlockCount[j]; k++)
						{
							BYTE cbTmpBlockCount = TmpResult.cbBlockCount[j];
							CopyMemory(&cbDistillCard[cbDistillCount], &TmpResult.cbCardData[j][(cbTmpBlockCount - k - 1)*(j + 1)],
								sizeof(BYTE) * 2);
							cbDistillCount += 2;

							//提取完成
							if (cbDistillCount == 2 * cbLineCount) break;
						}
					}
				}

				//提取完成
				if (cbDistillCount == 2 * cbLineCount) break;
			}

			//提取完成
			if (cbDistillCount == 2 * cbLineCount)
			{
				//复制翅膀
				cbResultCount = tmpDoubleWing.cbSearchCount++;
				CopyMemory(tmpDoubleWing.cbResultCard[cbResultCount], tmpSearchResult.cbResultCard[i],
					sizeof(BYTE)*tmpSearchResult.cbCardCount[i]);
				CopyMemory(&tmpDoubleWing.cbResultCard[cbResultCount][tmpSearchResult.cbCardCount[i]],
					cbDistillCard, sizeof(BYTE)*cbDistillCount);
				tmpDoubleWing.cbCardCount[i] = tmpSearchResult.cbCardCount[i] + cbDistillCount;
			}
		}

		//复制结果
		for (BYTE i = 0; i < tmpDoubleWing.cbSearchCount; i++)
		{
			BYTE cbResultCount = pSearchCardResult->cbSearchCount++;

			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpDoubleWing.cbResultCard[i],
				sizeof(BYTE)*tmpDoubleWing.cbCardCount[i]);
			pSearchCardResult->cbCardCount[cbResultCount] = tmpDoubleWing.cbCardCount[i];
		}
		for (BYTE i = 0; i < tmpSingleWing.cbSearchCount; i++)
		{
			BYTE cbResultCount = pSearchCardResult->cbSearchCount++;

			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSingleWing.cbResultCard[i],
				sizeof(BYTE)*tmpSingleWing.cbCardCount[i]);
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSingleWing.cbCardCount[i];
		}
	}

	return pSearchCardResult == NULL ? 0 : pSearchCardResult->cbSearchCount;
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
	for (int i = 0; i < vecMinTypeCardResult.size(); i++)
	{
		int type = vecMinTypeCardResult[i].cbCardType;
		typeCount[type]++;
		bool bExistMax = false;
		BYTE tmpTurnCard[MAX_COUNT] = { 0 };
		int tmpTurnCardCount = vecMinTypeCardResult[i].cbCardCount;
		CopyMemory(tmpTurnCard, vecMinTypeCardResult[i].cbResultCard, vecMinTypeCardResult[i].cbCardCount);
		bool bCheck = CheckBombPercent(cbHandCardData, cbHandCardCount, m_cbDiscardCard, m_cbDiscardCardCount);
		if (cbHandCardCount >= NORMAL_COUNT)
		{
			bExistMax = SearchOtherHandCardThan(tmpTurnCard, tmpTurnCardCount, true);
		}
		else{
			bCheck = false;
			if ((type == CT_ONE_DOUBLE || type == CT_SINGLE))
			{
				bCheck = true;
			}
			bExistMax = SearchOtherHandCardThan(tmpTurnCard, tmpTurnCardCount, bCheck);
		}
		if (m_cbUserCardCount[1]<=4&&m_cbUserCardCount[2]==0)
		{
			if (tmpTurnCardCount > m_cbUserCardCount[1])
			{
				bExistMax = false;
			}
		}
		if (type == CT_ONE_DOUBLE && bExistMax && juDuiMaxIndex[type] <= 0 && vecMinTypeCardResult.size()>2 && vecMinTypeCardResult[vecMinTypeCardResult.size()-1].cbCardType==CT_SINGLE)
		{
			//--优化, 从连对中找出最大对子
			BYTE maxTrunCardLogic = GetCardLogicValue(tmpTurnCard[0]);
			int doubleMaxIndex =- 1;
			for (int j = vecMinTypeCardResult.size()-1; j >=0; j--)
			{
				if (vecMinTypeCardResult[j].cbCardType == CT_TWO_DOUBLE)
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
				else if ( vecMinTypeCardResult[j].cbCardType == CT_FIVE_THREE_DEOUBLE )
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
				if (cbHandCardCount >= NORMAL_COUNT)
				{
					bExistDoubleMax = SearchOtherHandCardThan(tmpTurnCard, tmpTurnCardCount, true);
				}
				else{
					bExistDoubleMax = SearchOtherHandCardThan(tmpTurnCard, tmpTurnCardCount, bCheck);
				}
				if ((bExistDoubleMax != true) || GetCardLogicValue(tmpTurnCard[0])==13)
				{
					DoubleReMaxCount++;
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

	/*	if (type >= CT_FIVE_FOUR_ONE)
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
		if ((tempAllCount + NoXiangDuiDaPaiMaxCount) >= cbHandCardCount - m_cbOthreRangCardCount)
		{
			bZhiJieChu = true;
		}
	}
	if (bZhiJieChu)
	{
		if (allMaxCardIndex.size() > 0)
		{
			int maxTypeCount = 0;
			resultIndex = allMaxCardIndex[allMaxCardIndex.size() - 1];
			for (int i = 0; i < allMaxCardIndex.size(); i++)
			{
				if ((vecMinTypeCardResult[allMaxCardIndex[i]].cbCardType != CT_MISSILE_CARD) && vecMinTypeCardResult[allMaxCardIndex[i]].cbCardCount>maxTypeCount)
				{
					maxTypeCount = vecMinTypeCardResult[allMaxCardIndex[i]].cbCardCount;
					resultIndex = allMaxCardIndex[i];
				}
			}
		}
	}
	if (resultIndex == -1)
	{
		int tempTypeCount = 0;
		for (int i = 0; i < CT_TYPE_COUNT; i++)
		{
			int tempSize = MaxIndexSet[i].size();
			if (juDuiMaxIndex[i] > 0 && typeCount[i] > 1 && tempSize > 0)
			{
				resultIndex = MaxIndexSet[i][tempSize - 1];
				tempTypeCount = typeCount[i];
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
					resultIndex = MaxIndexSet[i][tempSize - 1];
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
						resultIndex = MaxIndexSet[i][tempSize - 1];
						break;
					}

				}
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
		if ((type>CT_SINGLE && type <= CT_FIVE_THREE_DEOUBLE) || bSpecal)
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
				resultIndexType == CT_FIVE_MIXED_FLUSH_NO_A && vecMinTypeCardResult[i].cbCardType == CT_FIVE_MIXED_FLUSH_NO_A &&
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
				if (typeCount[type] >= 1 && ( tmpType <= CT_FIVE_THREE_DEOUBLE &&  type <= CT_FIVE_THREE_DEOUBLE))
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
			if (bExist == false && (tmpType == CT_FIVE_THREE_DEOUBLE  || tmpType == CT_ONE_DOUBLE))
			{
				if ((cbHandCardCount - m_cbOthreRangCardCount - OutCardResult.cbCardCount) <= 1)
				{
					bExist = true;
				}
			}
			if (bExist == false && (tmpType == CT_FIVE_THREE_DEOUBLE  || tmpType == CT_ONE_DOUBLE ) && GetCardLogicValue(vecMinTypeCardResult[resultIndex].cbResultCard[0]) >= 11 && typeCount[tmpType] == 1)
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
			if (bExist == false && (tmpType == CT_TWO_DOUBLE) && (typeCount[CT_FIVE_FOUR_ONE]>0 || GetCardLogicValue(vecMinTypeCardResult[resultIndex].cbResultCard[0]) ==14 ) && GetCardLogicValue(vecMinTypeCardResult[resultIndex].cbResultCard[0]) >= 11 && typeCount[tmpType] == 1)
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
			if (bExist == false && (tmpType == CT_FIVE_MIXED_FLUSH_NO_A) && OutCardResult.cbCardCount>5&&(cbHandCardCount<NORMAL_COUNT))
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
	}
	return resultIndex;
}

bool CGameLogicNew::isBiYing(vector<tagOutCardResultNew > & CardTypeResult)
{
	

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
	ZeroMemory(&AnalyseResult, sizeof(AnalyseResult));

	//扑克分析
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		//变量定义
		BYTE cbSameCount = 1;
		BYTE cbLogicValue = GetCardLogicValue(cbCardData[i]);
		//大小王计数
		if (GetCardColor(cbCardData[i]) == GetCardColor(0x4E))
		{
			AnalyseResult.cbKingCount++;
		}
		//搜索同牌
		for (BYTE j = i + 1; j < cbCardCount; j++)
		{
			//获取扑克
			if (GetCardLogicValue(cbCardData[j]) != cbLogicValue) break;

			//设置变量
			cbSameCount++;
		}

		//设置结果
		BYTE cbIndex = AnalyseResult.cbBlockCount[cbSameCount - 1]++;
		for (BYTE j = 0; j < cbSameCount; j++)
			AnalyseResult.cbCardData[cbSameCount - 1][cbIndex*cbSameCount + j] = cbCardData[i + j];

		//设置索引
		i += (cbSameCount - 1);
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

	
	return true ;
}

//分析炸弹
VOID CGameLogicNew::GetAllBomCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbBomCardData[], BYTE &cbBomCardCount, tagOutCardTypeResultNew   *CardTypeResult )
{
	BYTE cbTmpCardData[MAX_COUNT];
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

	//大小排序
	SortCardList(cbTmpCardData, cbHandCardCount, ST_ORDER);

	cbBomCardCount = 0;

	if (cbHandCardCount < 4) return;

	//双王炸弹
	//if(0x4F==cbTmpCardData[0] && 0x4E==cbTmpCardData[1])
	//{
	//	cbBomCardData[cbBomCardCount++] = cbTmpCardData[0] ;
	//	cbBomCardData[cbBomCardCount++] = cbTmpCardData[1] ;
	//}

	//扑克分析
	for (BYTE i = 0; i < cbHandCardCount; i++)
	{
		//变量定义
		BYTE cbSameCount = 1;
		BYTE cbLogicValue = GetCardLogicValue(cbTmpCardData[i]);

		//搜索同牌
		for (BYTE j = i + 1; j < cbHandCardCount; j++)
		{
			//获取扑克
			if (GetCardLogicValue(cbTmpCardData[j]) != cbLogicValue) break;

			//设置变量
			cbSameCount++;
		}

		if (4 == cbSameCount)
		{
			cbBomCardData[cbBomCardCount++] = cbTmpCardData[i];
			cbBomCardData[cbBomCardCount++] = cbTmpCardData[i + 1];
			cbBomCardData[cbBomCardCount++] = cbTmpCardData[i + 2];
			cbBomCardData[cbBomCardCount++] = cbTmpCardData[i + 3];
		
		}

		//设置索引
		i += cbSameCount - 1;
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
				int Index = CardTypeResult[CT_FIVE_MIXED_FLUSH_NO_A].cbCardTypeCount;
				CardTypeResult[CT_FIVE_MIXED_FLUSH_NO_A].cbCardType = CT_FIVE_MIXED_FLUSH_NO_A;
				CopyMemory(CardTypeResult[CT_FIVE_MIXED_FLUSH_NO_A].cbCardData[Index], cbSingleLineCard, sizeof(BYTE)*cbSingleLineCount);
				CardTypeResult[CT_FIVE_MIXED_FLUSH_NO_A].cbEachHandCardCount[Index] = cbSingleLineCount;
				CardTypeResult[CT_FIVE_MIXED_FLUSH_NO_A].cbCardTypeCount++;
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
VOID CGameLogicNew::Combination(BYTE cbCombineCardData[], int cbResComLen,  BYTE cbResultCardData[500][8], int &cbResCardLen,BYTE cbSrcCardData[] , BYTE cbCombineLen1, BYTE cbSrcLen, const BYTE cbCombineLen2)
{
	if (cbResCardLen>=MAX_RESULT_COUNT)
	{
		return;
	}
	if( cbResComLen == cbCombineLen2 )
	{
		CopyMemory(&cbResultCardData[cbResCardLen], cbCombineCardData, cbResComLen) ;
		++cbResCardLen ;

		ASSERT(cbResCardLen<255) ;

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
//叫牌扑克
VOID CGameLogicNew::SetLandScoreCardData(BYTE cbCardData[], BYTE cbCardCount) 
{
	ASSERT(cbCardCount==MAX_COUNT) ;
	if(cbCardCount!=MAX_COUNT) return ;

	CopyMemory(m_cbLandScoreCardData, cbCardData, cbCardCount*sizeof(BYTE)) ;
	//排列扑克
	SortCardList(m_cbLandScoreCardData, cbCardCount, ST_ORDER) ;
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
			cbLessLineCount = 2;
		else cbLessLineCount = 2;
	}
	else cbLessLineCount = cbLineCount;

	BYTE cbReferIndex = 0;
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
	BYTE cbCardData[FULL_COUNT];
	BYTE cbCardCount = cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//排列扑克
	SortCardList(cbCardData, cbCardCount, ST_ORDER);

	//分析扑克
	tagDistributingNew Distributing = {};
	AnalysebDistributing(cbCardData, cbCardCount, Distributing);

	//搜索顺子
	BYTE cbTmpLinkCount = 0;
	BYTE cbValueIndex = 0;
	for (cbValueIndex = cbReferIndex; cbValueIndex < 13; cbValueIndex++)
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
						pSearchCardResult->cbResultCard[cbResultCount][cbCount++] = MakeCardData(cbIndex, 3 - cbColorIndex);

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

	//特殊顺子
	if (cbTmpLinkCount >= cbLessLineCount - 1 && cbValueIndex == 13)
	{
		if (Distributing.cbDistributing[0][cbIndexCount] >= cbBlockCount ||
			cbTmpLinkCount >= cbLessLineCount)
		{
			if (pSearchCardResult == NULL) return 1;

			ASSERT(cbResultCount < CountArray(pSearchCardResult->cbCardCount));

			//复制扑克
			BYTE cbCount = 0;
			BYTE cbTmpCount = 0;
			for (BYTE cbIndex = cbValueIndex - cbTmpLinkCount; cbIndex < 13; cbIndex++)
			{
				cbTmpCount = 0;
				for (BYTE cbColorIndex = 0; cbColorIndex < 4; cbColorIndex++)
				{
					for (BYTE cbColorCount = 0; cbColorCount < Distributing.cbDistributing[cbIndex][3 - cbColorIndex]; cbColorCount++)
					{
						pSearchCardResult->cbResultCard[cbResultCount][cbCount++] = MakeCardData(cbIndex, 3 - cbColorIndex);

						if (++cbTmpCount == cbBlockCount) break;
					}
					if (cbTmpCount == cbBlockCount) break;
				}
			}
			//复制A
			if (Distributing.cbDistributing[0][cbIndexCount] >= cbBlockCount)
			{
				cbTmpCount = 0;
				for (BYTE cbColorIndex = 0; cbColorIndex < 4; cbColorIndex++)
				{
					for (BYTE cbColorCount = 0; cbColorCount < Distributing.cbDistributing[0][3 - cbColorIndex]; cbColorCount++)
					{
						pSearchCardResult->cbResultCard[cbResultCount][cbCount++] = MakeCardData(0, 3 - cbColorIndex);

						if (++cbTmpCount == cbBlockCount) break;
					}
					if (cbTmpCount == cbBlockCount) break;
				}
			}

			//设置变量
			pSearchCardResult->cbCardCount[cbResultCount] = cbCount;
			cbResultCount++;
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
BYTE CGameLogicNew::SearchAllLineCardType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbLineCount, tagSearchCardResult *pSearchCardResult)
{
	//设置结果
	if (pSearchCardResult)
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));

	//定义变量
	BYTE cbResultCount = 0;
	BYTE cbBlockCount = 1;

	//超过A
	if (cbLineCount > 14) return cbResultCount;

	//长度判断
	if (cbHandCardCount < cbLineCount) return cbResultCount;

	//构造扑克
	BYTE cbCardData[NORMAL_COUNT] = { 0 };
	BYTE cbCardCount = cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	BYTE cbWangCardData[MAX_COUNT] = { 0 };
	BYTE cbWangCount = 0;
	cbWangCount = RemoveCardWang(cbCardData, cbHandCardCount, cbWangCardData);
	cbCardCount -= cbWangCount;
	//排列扑克
	SortCardList(cbCardData, cbCardCount, ST_ORDER);

	//分析扑克
	tagDistributingNew Distributing;
	ZeroMemory(&Distributing, sizeof(Distributing));
	AnalysebDistributing(cbCardData, cbCardCount, Distributing);
	CopyMemory(Distributing.cbDistributing[13], Distributing.cbDistributing[0], sizeof(Distributing.cbDistributing[0]));
	//搜索顺子
	BYTE cbTmpLinkCount = 0;
	int needLaiZiCount = 0;
	for (BYTE cbValueIndex = 0; cbValueIndex < 14; cbValueIndex++)
	{
		//继续判断
		if (Distributing.cbDistributing[cbValueIndex][cbIndexCount] < cbBlockCount)
		{
			if (needLaiZiCount < cbWangCount)
				needLaiZiCount++;
			else 
			{
				if (cbTmpLinkCount < cbLineCount)
				{
					cbTmpLinkCount = 0;
					needLaiZiCount = 0;

					if (cbWangCount>0)
					{
						BYTE tmpCount = 0;
						BYTE tmpNeedWangCount = 0;
						for (int j = cbValueIndex - 4+1; j < cbValueIndex + 1;j++)
						{
							if (j>0)
							{
								if (Distributing.cbDistributing[j][cbIndexCount] > 0)
								{
									tmpCount++;
								}
								else{
									tmpNeedWangCount++;
								}
							}
						}
						if ((tmpNeedWangCount<=cbWangCount)&&((tmpCount + tmpNeedWangCount)==4))
						{
							cbTmpLinkCount = tmpCount;
							needLaiZiCount = tmpNeedWangCount;
						}
						
					}

				
					continue;
				}
				else cbValueIndex--;
			}
			
		}
		else
		{
			if (cbTmpLinkCount == 0)
				needLaiZiCount = 0;
			cbTmpLinkCount++;

			//寻找最长连
			if (cbLineCount == 0 && cbValueIndex!=13)
			{
				continue;
			}
			
		}
		if (cbTmpLinkCount >= cbLineCount || (cbTmpLinkCount + needLaiZiCount >= cbLineCount))
		{
			if (pSearchCardResult == NULL) return 1;

			vector<BYTE>  lineArray;
			GetNextLineCard(cbValueIndex + 1 - cbTmpLinkCount- needLaiZiCount, cbValueIndex, cbLineCount, needLaiZiCount,cbWangCount, cbWangCardData, lineArray, Distributing, pSearchCardResult);

			if (Distributing.cbDistributing[cbValueIndex - cbLineCount + 1][cbIndexCount] > 0)
			{
				cbTmpLinkCount--;
			}
			else{
				if (needLaiZiCount > 0)
				{
					needLaiZiCount--;
				}
			}
		}
		
	}
	//寻找9,10

	return pSearchCardResult->cbSearchCount;
}


bool CGameLogicNew::OutCardShengYuFenCheck(BYTE cbHandCardCount, const BYTE * cbHandCardData, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, WORD wOutCardUser, float MinTypeScoreVec, vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew & OutCardResult)
{
	return false;
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
			else if (GetCardLogicValue(vecMinTypeCardResultBak[index].cbResultCard[0]) > GetCardLogicValue(SearchCardResult.cbResultCard[resultIndex][0]))
			{
				OutCardResult.cbCardCount = 1;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResultBak[index].cbResultCard, OutCardResult.cbCardCount);
				return true;
			}
		}
		else if (count == 0 || singleData.size() == 1)
		{
			if ((singleData.size() == 1) && (GetCardLogicValue(singleData[0]) > GetCardLogicValue(SearchCardResult.cbResultCard[resultIndex][0])))
			{
				OutCardResult.cbCardCount = 1;
				CopyMemory(OutCardResult.cbResultCard, &singleData[0], OutCardResult.cbCardCount);
				return true;
			}
		}
	}
	return false;
}

bool CGameLogicNew::ShuangDingPaiCeLue(int tempCardType, vector<tagOutCardResultNew> &vecMinTypeCardResultBak, tagSearchCardResult &SearchCardResult, int resultIndex, vector<BYTE> &singleData, tagOutCardResultNew &OutCardResult, bool bExistBiYing)
{
	if (tempCardType == CT_ONE_DOUBLE)
	{
		int count = 0;
		int index = 0;
		for (int i = 0; i < vecMinTypeCardResultBak.size(); i++)
		{
			if (vecMinTypeCardResultBak[i].cbCardType == CT_ONE_DOUBLE)
			{
				count++;
				index = i;
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
		if (TempMinTypeCardResult[i].cbCardType == CT_ONE_DOUBLE)
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
			if (vecMinTypeCardResult[j].cbCardType == CT_FIVE_THREE_DEOUBLE)
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
		if (cbOutCardType == CT_ONE_DOUBLE)
		{
			if (vecMinTypeCardResult[j].cbCardType == CT_FIVE_THREE_DEOUBLE)
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

bool CGameLogicNew::SwitchOneOrTwoFromThreeTake(vector<tagOutCardResultNew> &vecMinTypeCardResult, BYTE cbOutCardType, tagOutCardResultNew &OutCardResult)
{
	vector<BYTE> vecResultData;
	for (int j = 0; j < vecMinTypeCardResult.size(); j++)
	{
		if (cbOutCardType == CT_FIVE_THREE_DEOUBLE)
		{
			if (vecMinTypeCardResult[j].cbCardType == cbOutCardType)
			{
				for (int k = 0; k < vecMinTypeCardResult[j].cbCardCount / 5; k++)
				{
					vecResultData.push_back(vecMinTypeCardResult[j].cbResultCard[vecMinTypeCardResult[j].cbCardCount - 1 -2*k]);
					vecResultData.push_back(vecMinTypeCardResult[j].cbResultCard[vecMinTypeCardResult[j].cbCardCount - 2 -2*k]);
				}
			}
		}

	}

	if (vecResultData.size() > 0)
	{
		sort(vecResultData.begin(), vecResultData.end(), [this](BYTE first, BYTE second)
		{
			if (GetCardLogicValue(first) < GetCardLogicValue(second))
			{
				return true;
			}
			return false;
		});
		int count = OutCardResult.cbCardCount / 4;
		int takeCount = 1;
		if (cbOutCardType == CT_FIVE_THREE_DEOUBLE)
		{
			count = OutCardResult.cbCardCount / 5;
			takeCount = 2;
		}
		CopyMemory(OutCardResult.cbResultCard + 3 * count, &vecResultData[0], takeCount*count);
	}
	return true;
}




void CGameLogicNew::FourTakeOneOrTwoFenChaiCheck(tagOutCardResultNew &OutCardResult, vector<tagOutCardResultNew> &vecMinTypeCardResult)
{
	
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
	if (type == CT_ONE_DOUBLE)
	{
		cardCount = 2;
	}
	vecResultData = SearchOneOrTwoFromThreeTake(vecMinTypeCardResult, type, OutCardResult);
	for (int i = 0; i < vecMinTypeCardResult.size(); i++)
	{
		if (vecMinTypeCardResult[i].cbCardType == type)
			for (int j = 0; j < cardCount; j++)
			{
				vecResultData.push_back(vecMinTypeCardResult[i].cbResultCard[j]);
			}
	}
	// --防止单牌从三带一中出去
	if (type == CT_SINGLE)
	{
		vector<BYTE>::iterator itor;
		for (itor = vecResultData.begin(); itor != vecResultData.end(); itor++)
		{
			if (GetACardCount(cbHandCardData, cbHandCardCount, *itor) >= 3)
			{
				itor = vecResultData.erase(itor);
			}
		}
	}

	if (vecResultData.size() > 0)
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

void CGameLogicNew::OutCardSpecialCheck(tagOutCardResultNew &OutCardResult, vector<tagOutCardResultNew>& vecMinTypeCardResult)
{
	

}

bool CGameLogicNew::WuDiCheck(const BYTE * cbHandCardData, BYTE cbHandCardCount, const BYTE * cbTurnCardData, BYTE cbTurnCardCount, tagOutCardResultNew &OutCardResult)
{
	tagSearchCardResult SearchCardResult;
	SearchOutCard(cbHandCardData, cbHandCardCount, cbTurnCardData, cbTurnCardCount, &SearchCardResult);
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
		else if (cbHandCardCount - SearchCardResult.cbCardCount[i]-m_cbOthreRangCardCount<=0){
			ZeroMemory(&OutCardResult, sizeof(OutCardResult));
			OutCardResult.cbCardCount = SearchCardResult.cbCardCount[i];
			CopyMemory(OutCardResult.cbResultCard, SearchCardResult.cbResultCard[i], OutCardResult.cbCardCount);
			return true;
		}
	}
	/*if (m_cbUserCardCount[1]<=4&&m_cbUserCardCount[2]==0)
	{
	for (int i = 0; i < SearchCardResult.cbSearchCount; i++)
	{

	if (CheckOutOneTypeWillWin(cbHandCardData, cbHandCardCount, SearchCardResult.cbResultCard[i], SearchCardResult.cbCardCount[i], OutCardResult))
	{
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	OutCardResult.cbCardCount = SearchCardResult.cbCardCount[i];
	CopyMemory(OutCardResult.cbResultCard, SearchCardResult.cbResultCard[i], OutCardResult.cbCardCount);
	return true;
	}
	}
	else if (cbHandCardCount - SearchCardResult.cbCardCount[i] - m_cbOthreRangCardCount <= 0){
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	OutCardResult.cbCardCount = SearchCardResult.cbCardCount[i];
	CopyMemory(OutCardResult.cbResultCard, SearchCardResult.cbResultCard[i], OutCardResult.cbCardCount);
	return true;
	}
	}
	}*/

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
			if (SearchCardResult.cbCardCount[i] >= cbReserveCardCount - m_cbOthreRangCardCount)
			{
				OutCardResult.cbCardCount = SearchCardResult.cbCardCount[i];
				CopyMemory(OutCardResult.cbResultCard, SearchCardResult.cbResultCard[i], OutCardResult.cbCardCount);
				return true;
			}
		}
	}
	
	return false;
}




bool CGameLogicNew::FindMaxTypeTakeOneType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardType, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, vector<tagOutCardResultNew> & vecMinTypeCardResult, tagOutCardResultNew &OutCardResult)
{
	if (vecMinTypeCardResult.size() != 2)
	{
		if (OutCardResult.cbCardCount == 0 && cbTurnCardCount == 0)
		{
			for (int i = 0; i < vecMinTypeCardResult.size(); i++)
			{
				if (vecMinTypeCardResult[i].cbCardCount + m_cbOthreRangCardCount >= cbHandCardCount)
				{
					ZeroMemory(&OutCardResult, sizeof(OutCardResult));
					OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
					CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, OutCardResult.cbCardCount);
					return true;
				}
			}
		}
		return false;
	}

	for (int i = 0; i < vecMinTypeCardResult.size();i++)
	{
		if (vecMinTypeCardResult[i].cbCardType == CT_FIVE_FOUR_ONE || vecMinTypeCardResult[i].cbCardType == CT_MISSILE_CARD || cbTurnCardType == 0)
		{
			BYTE tmpTurnCard[MAX_COUNT] = { 0 };
			CopyMemory(tmpTurnCard, vecMinTypeCardResult[i].cbResultCard, vecMinTypeCardResult[i].cbCardCount);
			bool bExistMax = SearchOtherHandCardThan(tmpTurnCard, vecMinTypeCardResult[i].cbCardCount, false);
			if (bExistMax==false)
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
				OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, OutCardResult.cbCardCount);
				return true;
			}
		}
		else if ((vecMinTypeCardResult[i].cbCardType == cbTurnCardType) &&
			vecMinTypeCardResult[i].cbCardCount == cbTurnCardCount && (GetCardLogicValue(cbTurnCardData[0])>GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0])))
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
	if (OutCardResult.cbCardCount == 0 && cbTurnCardCount==0)
	{
		for (int i = 0; i < vecMinTypeCardResult.size(); i++)
		{
			if (vecMinTypeCardResult[i].cbCardCount + m_cbOthreRangCardCount >= cbHandCardCount)
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
				OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, OutCardResult.cbCardCount);
				return true;
			}
		}
	}
	return false;
}

bool CGameLogicNew::SearchOtherHandCardThan(const BYTE cbHandCardData[], BYTE cbHandCardCount, bool bNoSearchBomb)
{
	if (m_bHavePass)
	{
		bNoSearchBomb = true;
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
		tagSearchCardResult SearchCardResult;
		SearchOutCard(tempCard, tempCardCount, cbHandCardData, cbHandCardCount, &SearchCardResult, bNoSearchBomb);
		if (SearchCardResult.cbSearchCount > 0)
			bExistMax = true;
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
				for (int j = 0; j < m_cbUserCardCount[i];j++)
				{
					int tempLogic = GetCardLogicValue(m_cbAllCardData[i][j]);
					for (int k = 0; k < cbHandCardCount;k++)
					{
						if (GetCardLogicValue(cbHandCardData[k])==tempLogic)
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
		for (int j = 0; j < tempCardCount; j++)
		{
			int tempLogic = GetCardLogicValue(tempCard[j]);
			for (int k = 0; k < cbHandCardCount; k++)
			{
				if (GetCardLogicValue(cbHandCardData[k]) == tempLogic)
				{
					sameCount++;
				}
			}
			if (sameCount >= cbHandCardCount)
			{
				bExistMax = true;
				break;
			}
		}
	}
	return bExistMax;
}

//叫分判断
int CGameLogicNew::LandScore(BYTE cbHandCardData[], BYTE cbHandCardCount, int &cbCurrentLandScore)
{
	//大牌数目
	BYTE cbLargeCardCount = 0;
	BYTE Index = 0;

	tagOutCardTypeResultNew   CardTypeResult[CT_TYPE_COUNT];
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult));

	vector<tagOutCardResultNew>   vecMinTypeCardResult;
	vector<vector<tagOutCardResultNew>>   vecMinTypeCardResultShao;
	float tempMinTypeScore = 0;
	vector<float> tempMinTypeScoreShao;
	bool biYing = false;
	int minTypeCount = FindCardKindMinNum(cbHandCardData, cbHandCardCount, CardTypeResult, vecMinTypeCardResult, vecMinTypeCardResultShao, tempMinTypeScore, tempMinTypeScoreShao, biYing);
	float score = GetHandScore(vecMinTypeCardResult, minTypeCount);
	score = calCardScoreEx(vecMinTypeCardResult, score, cbHandCardCount, cbHandCardData, CardTypeResult);

	cbCurrentLandScore = score;

	//放弃叫分
	return score;
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
	
		if (cbHandCardData[0] == 0x4F )
		{
			score += 25;
		}
		if (cbHandCardData[0] == 0x4E)
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
		if (cbHandCardData[0]<0x4E && erCount<2)
		{
			score -= 10;
		}
		if (CardTypeResult[CT_FIVE_FOUR_ONE].cbCardTypeCount>0)
		{
			score += 10;
		}
		if (score<30&&cbHandCardData[0] == 0x4F && erCount == 2 && aCount >= 1 && singleCount <= 4)
		{
			score += 15;
		}
		if (cbHandCardData[0]<0x02)
		{
			score -= 45;
		}
	}	return score;
}

bool CGameLogicNew::SearchOutCardErRen(BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardData[], BYTE cbTurnCardCount, BYTE	DiscardCard[], BYTE cbDiscardCardCount, BYTE cbRangCardCount, BYTE cbOthreRangCardCount, tagOutCardResultNew & OutCardResult)
{
	SetDiscardCard(DiscardCard, cbDiscardCardCount);

	SetUserCard(0, cbHandCardData, cbHandCardCount);
	
	m_cbUserCardCount[1] =  (FULL_COUNT - cbHandCardCount - cbDiscardCardCount-16);
	if (cbRangCardCount>0)
	{
		//m_cbUserCardCount[1] -= cbRangCardCount;
		m_bHavePass = true;
	}
	else{
		m_bHavePass = false;
	}
	m_cbOthreRangCardCount = cbOthreRangCardCount;
	//玩家判断
	WORD wUndersideOfBanker = (m_wBankerUser + 1) % GAME_PLAYER;	//地主下家
	WORD wUpsideOfBanker = (wUndersideOfBanker + 1) % GAME_PLAYER;	//地主上家

	//初始变量
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	SortCardList(cbHandCardData, cbHandCardCount, ST_ORDER);
	SortCardList(cbTurnCardData, cbTurnCardCount, ST_ORDER);
	vector<BYTE> tmpWang;
	for (int i=0;i<cbHandCardCount;i++)
	{
		if (cbHandCardData[i]==0x4F)
		{
			tmpWang.push_back(cbHandCardData[i]);
			cbHandCardData[i] = 0x4E;
		}
	}
	//先出牌
	if (cbTurnCardCount == 0)
	{
		m_bSanDaYi = false;
		SearchOutCardShiSanZhang(cbHandCardData, cbHandCardCount, OutCardResult);
	}
	else if (cbTurnCardCount == 13)
	{
		m_bSanDaYi = true;
		tagOutCardResultNew  TurnOutCardResult;
		SearchOutCardShiSanZhang(cbTurnCardData, cbTurnCardCount, TurnOutCardResult);
		SearchOutCardShiSanZhangTurn(cbHandCardData, cbHandCardCount, OutCardResult, TurnOutCardResult);
		
	}
	int count = 0;
	if (tmpWang.size()>0)
	{
		for (int i = 0; i < OutCardResult.cbCardCount; i++)
		{
			if (OutCardResult.cbResultCard[i] == 0x4E)
			{
				count++;
				OutCardResult.cbResultCard[i] = 0x4F;
				if (count == tmpWang.size())
				{
					break;
				}
			}
		}
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
	vector<vector<tagOutCardResultNew>>   vecMinTypeCardResultShao;
	float tempMinTypeScore = 0;
	vector<float> tempMinTypeScoreShao ;
	bool biYing = false;
	int minTypeCount = FindCardKindMinNum(cbHandCardData, cbHandCardCount, CardTypeResult, vecMinTypeCardResult, vecMinTypeCardResultShao, tempMinTypeScore, tempMinTypeScoreShao, biYing);
	float score = GetHandScore(vecMinTypeCardResult, minTypeCount);
	score = calCardScoreEx(vecMinTypeCardResult, score, cbHandCardCount, cbHandCardData, CardTypeResult);
	cbCurrentLandScore = score;

	//放弃叫分
	return score;
}

bool CGameLogicNew::CheckBombPercent(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE DiscardCard[], BYTE cbDiscardCardCount)
{
	for (int i = 0; i < cbHandCardCount; i++)
	{
		if (cbHandCardData[i] >= 0x4E)
		{
			return true;
		}
	}
	for (int i = 0; i < cbDiscardCardCount; i++)
	{
		if (DiscardCard[i] >= 0x4E)
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

VOID CGameLogicNew::SearchOutCardShiSanZhang(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagOutCardResultNew & OutCardResult)
{
	//零下标没用
	tagOutCardTypeResultNew CardTypeResult[CT_TYPE_COUNT];
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult));
	//初始变量
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	vector<tagOutCardResultNew>   vecMinTypeCardResult;
	vector<vector<tagOutCardResultNew>>   vecMinTypeCardResultShao;
	OutCardResult.cbCardCount = cbHandCardCount;
	float tempMinTypeScore = 0;
	vector<float> tempMinTypeScoreShao;
	bool biYing = false;
	int duoZhongBaiFa = 1;
	int minTypeCount = FindCardKindMinNum(cbHandCardData, cbHandCardCount, CardTypeResult, vecMinTypeCardResult, vecMinTypeCardResultShao, tempMinTypeScore, tempMinTypeScoreShao, biYing);
	for (int i = 0; i < vecMinTypeCardResultShao.size(); i++)
	{
		if (vecMinTypeCardResultShao[i].size() > 0 && ((tempMinTypeScore - tempMinTypeScoreShao[i])<200))
		{
			duoZhongBaiFa++;
			int tmpSize = vecMinTypeCardResult.size();
			int tmpSize2 = vecMinTypeCardResultShao[i].size();
			if (tmpSize2 == 3 && (vecMinTypeCardResultShao[i][1].cbCardType == CT_TWO_DOUBLE && vecMinTypeCardResultShao[i][2].cbCardType == CT_TWO_DOUBLE))
			{
			}
			else if ((tmpSize - tmpSize2) >= 2)
			{
				swap(vecMinTypeCardResult, vecMinTypeCardResultShao[i]);
			}
			else
			{
				BYTE Array[DOU_NUM][DOU_HAND_COUNT] = { 0 };
				shengChengSanDou(vecMinTypeCardResult, Array);
				ShiSanZhangOutCardCeLue(cbHandCardData, cbHandCardCount, Array, CardTypeResult);

				BYTE Array2[DOU_NUM][DOU_HAND_COUNT] = { 0 };
				int  ArrayCount2[DOU_NUM] = { 0 };
				shengChengSanDou(vecMinTypeCardResultShao[i], Array2);
				int resultCompare = 0;
				for (int j = 0; j < DOU_NUM; j++)
				{
					resultCompare += CompareCard(Array[j], Array2[j], douNum[j], douNum[j], true, true);
				}

				if (resultCompare >= 1)
				{
					swap(vecMinTypeCardResult, vecMinTypeCardResultShao[i]);
				}
				else if (vecMinTypeCardResultShao[i].size() == 4){

					BYTE cbMaxCard = 0;
					int type1_2 = GetCardType(Array[2], 3, cbMaxCard);
					int type2_2 = GetCardType(Array2[2], 3, cbMaxCard);

					int type1_1 = GetCardType(Array[1], DOU_HAND_COUNT, cbMaxCard);
					int type2_1 = GetCardType(Array2[1], DOU_HAND_COUNT, cbMaxCard);
					if (vecMinTypeCardResult[0].cbCardType == CT_FIVE_THREE_DEOUBLE&&
						vecMinTypeCardResult[1].cbCardType == CT_FIVE_THREE_DEOUBLE/*&&
																				   GetCardLogicValue(vecMinTypeCardResult[0].cbResultCard[0])>=10&&
																				   GetCardLogicValue(vecMinTypeCardResult[1].cbResultCard[0]) >= 10&&
																				   GetCardLogicValue(vecMinTypeCardResult[1].cbResultCard[3]) >= 10*/)
					{
					}
					else if (type2_2 >= CT_ONE_DOUBLE&&type1_2 == CT_SINGLE)
					{
						swap(vecMinTypeCardResult, vecMinTypeCardResultShao[i]);
					}
					else if (resultCompare == 0 && type2_1>type1_1)
					{
						swap(vecMinTypeCardResult, vecMinTypeCardResultShao[i]);
					}

				}
			}
		}
	}


	BYTE Array[DOU_NUM][DOU_HAND_COUNT] = { 0 };
	shengChengSanDou(vecMinTypeCardResult, Array);
	ShiSanZhangOutCardCeLue(cbHandCardData, cbHandCardCount, Array, CardTypeResult);
	//校验乌龙
	int resultCompare1 = CompareCard(Array[2], Array[1], 3, DOU_HAND_COUNT, true, true);
	if (resultCompare1 < 1)
	{
		BYTE cbMaxCard = 0;
		int type2 = GetCardType(Array[2], 3, cbMaxCard);
		if (type2==CT_THREE)
		{
			SwitchArray(&Array[2][0], &Array[1][0], 3);
		}
		else{
			SwitchArray(&Array[2][2], &Array[1][4], 1);
		}
		

	}
	resultCompare1 = CompareCard(Array[1], Array[0], DOU_HAND_COUNT, DOU_HAND_COUNT, true, true);
	if (resultCompare1 < 1)
	{
		SwitchArray(&Array[0][0], &Array[1][0], DOU_HAND_COUNT);
	}
	else 
	{
	 //有的地方A开头顺子最小，调整一下顺序
		/*BYTE  cbMaxCard = 0;
		int type1 = GetCardType(Array[1], DOU_HAND_COUNT, cbMaxCard);
		int type0 = GetCardType(Array[0], DOU_HAND_COUNT, cbMaxCard);
		if (type1== CT_FIVE_MIXED_FLUSH_NO_A&&type0== CT_FIVE_MIXED_FLUSH_FIRST_A)
		{
			SwitchArray(&Array[0][0], &Array[1][0], DOU_HAND_COUNT);
		}*/

	}

	resultCompare1 = CompareCard(Array[2], Array[1], 3, DOU_HAND_COUNT, true, true);
	int resultCompare2 = CompareCard(Array[1], Array[0], 5, DOU_HAND_COUNT, true, true);
	if (resultCompare1 < 1 || resultCompare2<1)
	{
		int a = 4;
	}
	int num = 0;

	for (int i = 0; i < DOU_NUM; i++)
	{
		CopyMemory(OutCardResult.cbResultCard + num, Array[2 - i], douNum[2 - i]);
		num += douNum[2 - i];
	}
	OutCardResult.cbCardType = duoZhongBaiFa;

	return;
}



VOID CGameLogicNew::SearchOutCardShiSanZhangTurn(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagOutCardResultNew & OutCardResult, tagOutCardResultNew & TurnOutCardResult2)
{
	//零下标没用
	tagOutCardTypeResultNew CardTypeResult[CT_TYPE_COUNT];
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult));
	//初始变量
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	vector<tagOutCardResultNew>   vecMinTypeCardResult;
	vector<vector<tagOutCardResultNew>>   vecMinTypeCardResultShao;
	OutCardResult.cbCardCount = cbHandCardCount;
	float tempMinTypeScore = 0;
	vector<float> tempMinTypeScoreShao;
	bool biYing = false;
	int duoZhongBaiFa = 1;
	int minTypeCount = FindCardKindMinNum(cbHandCardData, cbHandCardCount, CardTypeResult, vecMinTypeCardResult, vecMinTypeCardResultShao, tempMinTypeScore, tempMinTypeScoreShao, biYing);
	for (int i = 0; i < vecMinTypeCardResultShao.size(); i++)
	{
		if (vecMinTypeCardResultShao[i].size() > 0)
		{
			duoZhongBaiFa++;
			int tmpSize = vecMinTypeCardResult.size();
			int tmpSize2 = vecMinTypeCardResultShao[i].size();
			{
				BYTE Array[DOU_NUM][DOU_HAND_COUNT] = { 0 };
				CopyMemory(Array, TurnOutCardResult2.cbResultCard, douNum[2]);
				CopyMemory(Array, TurnOutCardResult2.cbResultCard + 3, douNum[1]);
				CopyMemory(Array, TurnOutCardResult2.cbResultCard + 8, douNum[0]);

				BYTE Array2[DOU_NUM][DOU_HAND_COUNT] = { 0 };
				int  ArrayCount2[DOU_NUM] = { 0 };
				shengChengSanDou(vecMinTypeCardResultShao[i], Array2);
				int resultCompare = 0;
				for (int j = 0; j < DOU_NUM; j++)
				{
					resultCompare += CompareCard(Array[j], Array2[j], douNum[j], douNum[j], true, true);
				}

				if (resultCompare >= 3)
				{
					//校验乌龙
					JiaoYanWuLong(Array2);
					int num = 0;
					for (int i = 0; i < DOU_NUM; i++)
					{
						CopyMemory(OutCardResult.cbResultCard + num, Array2[2 - i], douNum[2 - i]);
						num += douNum[2 - i];
					}
					OutCardResult.cbCardType = 1;
					return;
				}

			}
		}
	}

	BYTE Array[DOU_NUM][DOU_HAND_COUNT] = { 0 };
	shengChengSanDou(vecMinTypeCardResult, Array);
	ShiSanZhangOutCardCeLue(cbHandCardData, cbHandCardCount, Array, CardTypeResult);
	//校验乌龙
	JiaoYanWuLong(Array);

	int num = 0;

	for (int i = 0; i < DOU_NUM; i++)
	{
		CopyMemory(OutCardResult.cbResultCard + num, Array[2 - i], douNum[2 - i]);
		num += douNum[2 - i];
	}
	OutCardResult.cbCardType = duoZhongBaiFa;

	return;
}

void CGameLogicNew::JiaoYanWuLong(BYTE Array[DOU_NUM][DOU_HAND_COUNT])
{
	int resultCompare1 = CompareCard(Array[2], Array[1], 3, DOU_HAND_COUNT, true, true);
	if (resultCompare1 < 1)
	{
		BYTE cbMaxCard = 0;
		int type2 = GetCardType(Array[2], 3, cbMaxCard);
		if (type2 == CT_THREE)
		{
			SwitchArray(&Array[2][0], &Array[1][0], 3);
		}
		else{
			SwitchArray(&Array[2][2], &Array[1][4], 1);
		}


	}
	resultCompare1 = CompareCard(Array[1], Array[0], DOU_HAND_COUNT, DOU_HAND_COUNT, true, true);
	if (resultCompare1 < 1)
	{
		SwitchArray(&Array[0][0], &Array[1][0], DOU_HAND_COUNT);
	}
	else
	{
		//有的地方A开头顺子最小，调整一下顺序
		/*BYTE  cbMaxCard = 0;
		int type1 = GetCardType(Array[1], DOU_HAND_COUNT, cbMaxCard);
		int type0 = GetCardType(Array[0], DOU_HAND_COUNT, cbMaxCard);
		if (type1== CT_FIVE_MIXED_FLUSH_NO_A&&type0== CT_FIVE_MIXED_FLUSH_FIRST_A)
		{
		SwitchArray(&Array[0][0], &Array[1][0], DOU_HAND_COUNT);
		}*/

	}

	resultCompare1 = CompareCard(Array[2], Array[1], 3, DOU_HAND_COUNT, true, true);
	int resultCompare2 = CompareCard(Array[1], Array[0], 5, DOU_HAND_COUNT, true, true);
	if (resultCompare1 < 1 || resultCompare2 < 1)
	{
		int a = 4;
	}
}

VOID CGameLogicNew::shengChengSanDou(vector<tagOutCardResultNew> &vecMinTypeCardResult, BYTE  Array[DOU_NUM][DOU_HAND_COUNT])
{
	sort(vecMinTypeCardResult.begin(), vecMinTypeCardResult.end(), [this](tagOutCardResultNew & first, tagOutCardResultNew & second){
		return first.cbCardCount > second.cbCardCount; });
		int  ArrayCount[DOU_NUM] = { 0 };
		vector<bool> bOpate(vecMinTypeCardResult.size());
		int opIndex = 0;
		int chaiPaiCount = 0;
		for (int i = 0; i < DOU_NUM; i++)
		{
			int count = 5;
			if (i == 2)
			{
				count = 3;
			}
			while (true)
			{
				int index = FindCardType(count, vecMinTypeCardResult, bOpate);
				if (index != -1)
				{
					CopyMemory(Array[i] + ArrayCount[i], vecMinTypeCardResult[index].cbResultCard, vecMinTypeCardResult[index].cbCardCount);
					ArrayCount[i] += vecMinTypeCardResult[index].cbCardCount;
					bOpate[index] = true;
					count = douNum[i] - ArrayCount[i];
				}
				else{
					if (count == 1)
					{
						//单牌没有就从双牌里面拆
						int doubleIndex = -1;
						for (int j = vecMinTypeCardResult.size() - 1; j >= 0; j--)
						{
							if (vecMinTypeCardResult[j].cbCardCount >= 2 && bOpate[j] == false)
							{
								doubleIndex = j;
								break;
							}
						}
						if (doubleIndex != -1)
						{
							CopyMemory(Array[i] + ArrayCount[i], &vecMinTypeCardResult[doubleIndex].cbResultCard[chaiPaiCount++], 1);
							ArrayCount[i] += 1;
							count = douNum[i] - ArrayCount[i];
						}

					}
					else{
						count -= 1;
					}

				}
				if (ArrayCount[i] == DOU_HAND_COUNT || (ArrayCount[i] == 3 && i == 2))
				{
					break;
				}
			}

		}
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
	return false;
}

bool CGameLogicNew::ThreeTakeTwoTakeMinCard(const BYTE cbCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew & OutCardResult)
{
	BYTE cbMaxCard = 0;
	int outCardType = GetCardType(OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbMaxCard);
	BYTE cbRemainCardData[MAX_COUNT] = { 0 };
	BYTE cbRemainCardCount = cbHandCardCount;
	CopyMemory(cbRemainCardData, cbCardData, cbRemainCardCount);
	if (outCardType == CT_FIVE_THREE_DEOUBLE)
	{
		BYTE cbThreeCard[MAX_COUNT] = { 0 };
		int tempCount = OutCardResult.cbCardCount / 5;
		int cbNeedCardCount = 2 * tempCount;
		for (int i = 0; i < 3 * tempCount; i++)
		{
			cbThreeCard[i] = OutCardResult.cbResultCard[i];
		}
		RemoveCard(cbThreeCard, 3 * tempCount, cbRemainCardData, cbRemainCardCount);
		cbRemainCardCount -= 3 * tempCount;
		SortCardList(cbRemainCardData, cbRemainCardCount, ST_ORDER);
		int minTypeCount = 0;
		int resultIndex = -1;
		float MinTypeScore = INT_MIN;
		BYTE ResultThreeTakeCard[MAX_COUNT] = { 0 };
		float tableScore[MAX_RESULT_COUNT] = { 0 };
		//CopyMemory(cbRemainCardData, cbThreeCard, 3);
		//单牌组合
		BYTE cbComCard[8] = { 0 };
		BYTE cbComResCard[MAX_RESULT_COUNT][8];
		int cbComResLen = 0;
		//-- //利用对牌的下标做组合，再根据下标提取出对牌
		//cbRemainCardCount = ClearReLogicValue(cbRemainCardData, cbRemainCardCount);
		Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCardData, cbNeedCardCount, cbRemainCardCount, cbNeedCardCount);

		for (int i = 0; i < cbComResLen; ++i)
		{
			//保存对牌
			BYTE tempThreeTakeTwo[MAX_COUNT] = { 0 };
			CopyMemory(tempThreeTakeTwo, cbThreeCard, 3 * tempCount);
			for (int j = 0; j < cbNeedCardCount; j++)
			{
				tempThreeTakeTwo[3 * tempCount + j] = cbComResCard[i][j];
			}
			vector<tagOutCardResultNew>  TempMinTypeCardResult;
			vector<vector<tagOutCardResultNew>>   vecMinTypeCardResultShao;
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
				vector<float> tempMinTypeScoreShao ;
				bool biYing = false;
				tempMinTypeCount = FindCardKindMinNum(searchCard, searchCardCount, tmpCardTypeResult, TempMinTypeCardResult, vecMinTypeCardResultShao, tempMinTypeScore, tempMinTypeScoreShao, biYing, true);
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
		if (vecMinTypeCardResult[i].cbCardType == CT_ONE_DOUBLE)
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


void CGameLogicNew::GetNextLineCard(BYTE cbIndexSatart, BYTE cbIndexEnd, BYTE cbLineCount,int needLaiZiCount, BYTE cbWangCount, BYTE cbWangCardData[MAX_COUNT], vector<BYTE> & lineArray, tagDistributingNew& Distributing, tagSearchCardResult *pSearchCardResult)
{
	//复制扑克
	BYTE cbCount = 0;

	for (BYTE cbIndex = cbIndexSatart; cbIndex <= cbIndexEnd; cbIndex++)
	{
		BYTE cbTmpCount = 0;
		for (BYTE cbColorIndex = 0; cbColorIndex < 4; cbColorIndex++)
		{
			for (BYTE cbColorCount = 0; cbColorCount < Distributing.cbDistributing[cbIndex][3 - cbColorIndex]; cbColorCount++)
			{
				if (cbIndex == 13)
				{
					lineArray.push_back(MakeCardData(0, 3 - cbColorIndex));
				}
				else
				{
					lineArray.push_back(MakeCardData(cbIndex, 3 - cbColorIndex));
				}

				if (lineArray.size() == (cbLineCount- needLaiZiCount))
				{
					for (int i = 0; i < lineArray.size(); i++)
					{
						pSearchCardResult->cbResultCard[pSearchCardResult->cbSearchCount][i] = lineArray[i];
					}
					//设置变量
					pSearchCardResult->cbCardCount[pSearchCardResult->cbSearchCount] = cbLineCount;

					//--添加癞子
					for (int i = 0; i < needLaiZiCount; i++)
					{
						int tmpIndex = lineArray.size()+i;
						pSearchCardResult->cbResultCard[pSearchCardResult->cbSearchCount][tmpIndex] = cbWangCardData[i];
					}
					pSearchCardResult->cbSearchCount++;

					
					if (cbWangCount>0)
					{
						//构造扑克
						BYTE cbCardData[MAX_COUNT] = { 0 };
						BYTE cbCardCount = cbLineCount;
						CopyMemory(cbCardData, pSearchCardResult->cbResultCard[pSearchCardResult->cbSearchCount-1], sizeof(BYTE)*cbLineCount);
						BYTE tmpWangCardData[MAX_COUNT] = { 0 };
						BYTE tmpWangCount = 0;
						tmpWangCount = RemoveCardWang(cbCardData, cbCardCount, tmpWangCardData);
						cbCardCount -= tmpWangCount;
						if (cbWangCount>tmpWangCount)
						{
							int color = 0;
							int maxColorCount = GetCardMaxHuaSe(cbCardData, cbCardCount, color);
							if (((maxColorCount + cbWangCount) >= cbLineCount)&& ((maxColorCount+ tmpWangCount)!=cbLineCount))
							{
								int tmpCardCount = 0;
								for (int i = 0; i < cbCardCount; i++)
								{
									if ((GetCardColor(cbCardData[i]) >> 4)==color)
									{
										pSearchCardResult->cbResultCard[pSearchCardResult->cbSearchCount][tmpCardCount++] = cbCardData[i];
									}
								}
								for (int i=0;i<cbWangCount;i++)
								{
									pSearchCardResult->cbResultCard[pSearchCardResult->cbSearchCount][tmpCardCount++] = cbWangCardData[i];
									if (tmpCardCount == cbLineCount)
									{
										break;
									}
									
								}
								pSearchCardResult->cbCardCount[pSearchCardResult->cbSearchCount] = cbLineCount;
								pSearchCardResult->cbSearchCount++;
							}
						}
						
					}
				}
				else
				{
					GetNextLineCard(cbIndex + 1, cbIndexEnd, cbLineCount, needLaiZiCount, cbWangCount, cbWangCardData, lineArray, Distributing, pSearchCardResult);
				}

				lineArray.pop_back();
			}

		}

	}
}

int CGameLogicNew::GetCardMaxHuaSe(BYTE lineArray[],int lineArrayCount, int &color)
{
	vector < vector<BYTE >> tempVec;
	tempVec.resize(5);
	for (int i = 0; i < lineArrayCount; i++)
	{
		int color = (GetCardColor(lineArray[i]) >> 4);
		tempVec[color].push_back(lineArray[i]);
	}
	int maxColorCount = 0;
	for (int j = 0; j < 4; j++)
	{
		if (tempVec[j].size() > maxColorCount)
		{
			maxColorCount = tempVec[j].size();
			color = j;
		}
	}
	return maxColorCount;
}

BYTE CGameLogicNew::GetCardType(BYTE bCardData[], BYTE bCardCount, BYTE & bMaxCardData)
{
	//数据校验

	bMaxCardData = 0;
	//构造扑克
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount = bCardCount;
	CopyMemory(cbCardData, bCardData, sizeof(BYTE)*bCardCount);
	bCardData = cbCardData;
	//分析扑克
	tagAnalyseData AnalyseData;
	ZeroMemory(&AnalyseData, sizeof(AnalyseData));
	AnalyseCard(bCardData, bCardCount, AnalyseData);

	SortCardList(bCardData, bCardCount, enDescend);

	//分析扑克
	tagAnalyseResultNew AnalyseResult;
	ZeroMemory(&AnalyseResult, sizeof(AnalyseResult));
	AnalysebCardData(bCardData, bCardCount, AnalyseResult);

	//开始分析
	switch (bCardCount)
	{
	case 1:	//三条类型
		  return CT_SINGLE;
	case 2:	//三条类型
	case 3:	//三条类型
	{
				if (AnalyseData.cbKingCount == 0)
				{
					//单牌类型
					if (3 == AnalyseData.bOneCount )
					{
						bMaxCardData = GetCardLogicValue(AnalyseResult.cbCardData[0][0]);
						return CT_SINGLE;
					}

					//对带一张
					if (1 == AnalyseData.bTwoCount )
					{
						bMaxCardData = GetCardLogicValue(AnalyseResult.cbCardData[1][0]);
						return CT_ONE_DOUBLE;
					}

					//三张牌型
					if (1 == AnalyseData.bThreeCount)
					{
						bMaxCardData = GetCardLogicValue(AnalyseResult.cbCardData[2][0]);
						return CT_THREE;
					}
				}
				else
				{
					//单牌类型
					if (2 == AnalyseData.bOneCount&&AnalyseData.cbKingCount == 1)
					{
						bMaxCardData = GetCardLogicValue(AnalyseResult.cbCardData[0][0]);
						bMaxCardData = GetMaxCardLogic(AnalyseResult.cbCardData[0], 3, false);
						return CT_ONE_DOUBLE;
					}

					//对带一张
					if ((1 == AnalyseData.bTwoCount && 1 == AnalyseData.cbKingCount) || (1 == AnalyseData.bOneCount && 2 == AnalyseData.cbKingCount))
					{
						bMaxCardData = GetCardLogicValue(AnalyseResult.cbCardData[1][0]);
						return CT_THREE;
					}

				}

				//错误类型
				return CT_INVALID;
	}
	case 5:	//五张牌型
	{
				BYTE cardWangType = GetTakeWangCardType(bCardData, bCardCount, bMaxCardData, AnalyseData);
				if (cardWangType != CT_INVALID)
				{
					return cardWangType;

				}
				if (AnalyseData.cbKingCount == 0)
				{
					bool bFlushNoA = false;
					bool bFlushFirstA = false;
					bool bFlushBackA = false;
					int count = 0;
					for (BYTE i = 1; i < 5; ++i)
					{
						if (GetCardLogicValue(bCardData[0]) == GetCardLogicValue(bCardData[i]))
						{
							count++;
						}
					}
					if (count == 4)
					{
						return CT_FIVE_TONG;
					}
					//A连在后
					if (14 == GetCardLogicValue(bCardData[0]) && 10 == GetCardLogicValue(bCardData[4]))
						bFlushBackA = true;
					else
						bFlushNoA = true;
					for (BYTE i = 0; i < 4; ++i)
					{
						if (1 != GetCardLogicValue(bCardData[i]) - GetCardLogicValue(bCardData[i + 1]))
						{
							bFlushBackA = false;
							bFlushNoA = false;
						}
					}

					//A连在前
					if (false == bFlushBackA && false == bFlushNoA && 14 == GetCardLogicValue(bCardData[0]))
					{
						bFlushFirstA = true;
						for (BYTE i = 1; i < 4; ++i)
						if (1 != GetCardLogicValue(bCardData[i]) - GetCardLogicValue(bCardData[i + 1]))
							bFlushFirstA = false;
						if (2 != GetCardLogicValue(bCardData[4]))
							bFlushFirstA = false;
					}

					//同花五牌
					if (false == bFlushBackA && false == bFlushNoA && false == bFlushFirstA)
					{
						if (true == AnalyseData.bSameColor)
						{
							bMaxCardData = GetCardLogicValue(AnalyseResult.cbCardData[0][0]);
							return CT_FIVE_FLUSH;
						}
					}
					else if (true == bFlushNoA)
					{
						bMaxCardData = GetCardLogicValue(AnalyseResult.cbCardData[0][0]);
						//杂顺类型
						if (false == AnalyseData.bSameColor) return CT_FIVE_MIXED_FLUSH_NO_A;
						//同花顺牌
						else return CT_FIVE_STRAIGHT_FLUSH;
					}
					else if (true == bFlushFirstA)
					{
						bMaxCardData = GetCardLogicValue(AnalyseResult.cbCardData[0][0]);
						//杂顺类型
						if (false == AnalyseData.bSameColor) return CT_FIVE_MIXED_FLUSH_FIRST_A;
						//同花顺牌
						else							  return CT_FIVE_STRAIGHT_FLUSH_FIRST_A;
					}
					else if (true == bFlushBackA)
					{
						bMaxCardData = GetCardLogicValue(AnalyseResult.cbCardData[0][0]);
						//杂顺类型
						if (false == AnalyseData.bSameColor) return CT_FIVE_MIXED_FLUSH_BACK_A;
						//同花顺牌
						else							  return CT_FIVE_STRAIGHT_FLUSH_BACK_A;
					}

					//四带单张
					if (1 == AnalyseData.bFourCount && 1 == AnalyseData.bOneCount)
					{
						bMaxCardData = GetCardLogicValue(AnalyseResult.cbCardData[3][0]);
						return	 CT_FIVE_FOUR_ONE;
					}

					//三条一对
					if (1 == AnalyseData.bThreeCount && 1 == AnalyseData.bTwoCount)
					{
						bMaxCardData = GetCardLogicValue(AnalyseResult.cbCardData[2][0]);
						return	 CT_FIVE_THREE_DEOUBLE;
					}

					//三条带单
					if (1 == AnalyseData.bThreeCount && 2 == AnalyseData.bOneCount)
					{
						bMaxCardData = GetCardLogicValue(AnalyseResult.cbCardData[2][0]);
						return	 CT_THREE;
					}

					//两对牌型
					if (2 == AnalyseData.bTwoCount && 1 == AnalyseData.bOneCount)
					{
						bMaxCardData = GetCardLogicValue(AnalyseResult.cbCardData[1][0]);
						return	 CT_TWO_DOUBLE;
					}

					//只有一对
					if (1 == AnalyseData.bTwoCount && 3 == AnalyseData.bOneCount)
					{
						bMaxCardData = GetCardLogicValue(AnalyseResult.cbCardData[1][0]);
						return	 CT_ONE_DOUBLE;
					}

					//单牌类型
					if (5 == AnalyseData.bOneCount && false == AnalyseData.bSameColor)
					{
						bMaxCardData = GetCardLogicValue(AnalyseResult.cbCardData[0][0]);
						return  CT_SINGLE;
					}
				}
				else{
					bool bFlush = true;
					bool bContainA = IsContainA(bCardData, bCardCount);
					if ((AnalyseData.bFourCount == 1 && AnalyseData.cbKingCount == 1) || (AnalyseData.bThreeCount == 1 && AnalyseData.cbKingCount == 2)
						|| (AnalyseData.bTwoCount == 1 && AnalyseData.cbKingCount == 3) || (AnalyseData.cbKingCount == 4))
					{
						return CT_FIVE_TONG;
					}
					int bKingCount = AnalyseData.cbKingCount;
					int bNeedKingcount = 0;
					for (BYTE i = bKingCount; i < 4; ++i)
					{
						int first = GetCardLogicValue(bCardData[i]);
						int chaZhi = 0;
						if (GetCardLogicValue(bCardData[i]) == GetCardLogicValue(bCardData[i + 1]))
						{
							bFlush = false;
							break;
						}
						if (10 > GetCardLogicValue(bCardData[4]) && first == 14)
						{
							chaZhi = GetCardLogicValue(bCardData[4]) - 1;
						}
						else{
							chaZhi = GetCardLogicValue(bCardData[i]) - GetCardLogicValue(bCardData[i + 1]);
						}

						if (chaZhi == 2)
						{
							if (bNeedKingcount == bKingCount)
							{
								bFlush = false;
								break;
							}
							bNeedKingcount++;

						}
						else if (chaZhi == 3 && bKingCount == 2)
						{
							if (bNeedKingcount == bKingCount)
							{
								bFlush = false;
								break;
							}
							bNeedKingcount += 2;
						}
						else if (chaZhi == 0 || chaZhi > 3)
						{
							bFlush = false;
							break;
						}
						else if (chaZhi == 1)
						{
						}
						else{
							bFlush = false;
							break;
						}
					}

					//同花五牌
					if (false == bFlush)
					{
						if (true == AnalyseData.bSameColor)
						{
							bMaxCardData = GetCardLogicValue(AnalyseResult.cbCardData[0][0]);
							return CT_FIVE_FLUSH;
						}
					}
					else
					{
						bMaxCardData = GetMaxCardLogic(AnalyseResult.cbCardData[0], 5, true);
						//杂顺类型
						if (false == AnalyseData.bSameColor)
						{
							if (bContainA)
							{
								if (10 <= GetCardLogicValue(bCardData[4]))
								{
									return CT_FIVE_MIXED_FLUSH_BACK_A;
								}
								else{
									return CT_FIVE_MIXED_FLUSH_FIRST_A;
								}
							}
							else
							{

								return CT_FIVE_MIXED_FLUSH_NO_A;
							}

						}
						//同花顺牌
						else {
							if (bContainA)
							{
								if (10 <= GetCardLogicValue(bCardData[4]))
								{
									return CT_FIVE_STRAIGHT_FLUSH_BACK_A;
								}
								else{
									return CT_FIVE_STRAIGHT_FLUSH_FIRST_A;
								}
							}
							else
							{
								return CT_FIVE_STRAIGHT_FLUSH;
							}
						}
					}

					//四带单张
					if (1 == AnalyseData.bThreeCount && 1 == AnalyseData.cbKingCount || (1 == AnalyseData.bTwoCount && 2 == AnalyseData.cbKingCount) || (2 == AnalyseData.bOneCount && 3 == AnalyseData.cbKingCount))
					{
						bMaxCardData = GetCardLogicValue(AnalyseResult.cbCardData[3][0]);
						return	 CT_FIVE_FOUR_ONE;
					}

					//三条一对
					if (1 == AnalyseData.cbKingCount && 2 == AnalyseData.bTwoCount)
					{
						bMaxCardData = GetCardLogicValue(AnalyseResult.cbCardData[1][0]);
						return	 CT_FIVE_THREE_DEOUBLE;
					}

					//三条带单
					if ((1 == AnalyseData.bTwoCount && 1 == AnalyseData.cbKingCount) || (3 == AnalyseData.bOneCount && 2 == AnalyseData.cbKingCount))
					{
						bMaxCardData = GetCardLogicValue(AnalyseResult.cbCardData[1][0]);
						if ((3 == AnalyseData.bOneCount && 2 == AnalyseData.cbKingCount))
						{
							bMaxCardData = GetMaxCardLogic(AnalyseResult.cbCardData[0], 3, false);
						}
						return	 CT_THREE;
					}

					//单牌类型
					if (4 == AnalyseData.bOneCount && false == AnalyseData.bSameColor)
					{
						bMaxCardData = GetCardLogicValue(AnalyseResult.cbCardData[1][0]);
						return  CT_ONE_DOUBLE;
					}
				}


				//错误类型
				return CT_INVALID;
	}
	}

	return CT_INVALID;
}

void CGameLogicNew::AnalyseCard(const BYTE bCardDataList[], const BYTE bCardCount, tagAnalyseData& AnalyseData)
{
	ASSERT(3 == bCardCount || 5 == bCardCount);

	//排列扑克
	BYTE bCardData[MAX_COUNT];
	CopyMemory(bCardData, bCardDataList, bCardCount);
	SortCardList(bCardData, bCardCount, enDescend);

	//变量定义
	BYTE bSameCount = 1,
		bCardValueTemp = 0,
		bSameColorCount = 1,
		bFirstCardIndex = 0;	//记录下标
	//设置结果
	ZeroMemory(&AnalyseData, sizeof(AnalyseData));
	BYTE bLogicValue = GetCardLogicValue(bCardData[0]);
	BYTE bCardColor = GetCardColor(bCardData[0]);
	if (bCardColor == GetCardColor(0x4E))
	{
		AnalyseData.cbKingCount++;
	}

	//扑克分析
	for (BYTE i = 1; i < bCardCount; i++)
	{
		//大小王计数
		if (GetCardColor(bCardData[i]) == GetCardColor(0x4E))
		{
			AnalyseData.cbKingCount++;
			//王牌自动转化同花
			if (bCardData[i] == 0x4E || bCardData[i] == 0x4F)
			{
				bSameColorCount += 1;
			}
			continue;
		}
		if (bCardColor == GetCardColor(0x4E))
		{
			bSameCount = 1;
			bLogicValue = bCardValueTemp;
			bFirstCardIndex = i;
			bCardColor = GetCardColor(bCardData[i]);
		}
		//获取扑克
		bCardValueTemp = GetCardLogicValue(bCardData[i]);
		if (bCardValueTemp == bLogicValue) bSameCount++;

		//保存结果
		if ((bCardValueTemp != bLogicValue) || (i == (bCardCount - 1)) || bCardData[i] == 0)
		{
			switch (bSameCount)
			{
			case 1:		//一张
				break;
			case 2:		//两张
			{
							AnalyseData.bTwoFirst[AnalyseData.bTwoCount] = bFirstCardIndex;
							AnalyseData.bTwoCount++;
							break;
			}
			case 3:		//三张
			{
							AnalyseData.bThreeFirst[AnalyseData.bThreeCount] = bFirstCardIndex;
							AnalyseData.bThreeCount++;
							break;
			}
			case 4:		//四张
			{
							AnalyseData.bFourFirst[AnalyseData.bFourCount] = bFirstCardIndex;
							AnalyseData.bFourCount++;
							break;
			}
			default:
				//MyMsgBox(_T("AnalyseCard：错误扑克！: %d") , bSameCount) ;
				break;
			}
		}



		//设置变量
		if (bCardValueTemp != bLogicValue)
		{
			if (bSameCount == 1)
			{
				if (AnalyseData.cbKingCount <= 0)
				{
					if (i != bCardCount - 1)
					{
						AnalyseData.bOneFirst[AnalyseData.bOneCount] = bFirstCardIndex;
						AnalyseData.bOneCount++;
					}
					else
					{
						AnalyseData.bOneFirst[AnalyseData.bOneCount] = bFirstCardIndex;
						AnalyseData.bOneCount++;
						AnalyseData.bOneFirst[AnalyseData.bOneCount] = i;
						AnalyseData.bOneCount++;
					}
				}
				else{
					AnalyseData.bOneFirst[AnalyseData.bOneCount] = i;
					AnalyseData.bOneCount++;
				}

			}
			else
			{
				if (i == bCardCount - 1)
				{
					AnalyseData.bOneFirst[AnalyseData.bOneCount] = i;
					AnalyseData.bOneCount++;
				}
			}
			bSameCount = 1;
			bLogicValue = bCardValueTemp;
			bFirstCardIndex = i;

		}
		if (GetCardColor(bCardData[i]) != bCardColor) bSameColorCount = 1;
		else									   ++bSameColorCount;
	}

	//是否同花
	AnalyseData.bSameColor = (5 == bSameColorCount) ? true : false;

	return;
}

int CGameLogicNew::CompareCard(BYTE bInFirstList[], BYTE bInNextList[], BYTE bFirstCount, BYTE bNextCount,  bool bCompareLogic, bool bCompareHuaSe)
{
	//定义变量
	BYTE bFirstList[MAX_COUNT] = { 0 }, bNextList[MAX_COUNT] = { 0 };
	tagAnalyseData FirstAnalyseData, NextAnalyseData;
	ZeroMemory(&FirstAnalyseData, sizeof(FirstAnalyseData));
	ZeroMemory(&NextAnalyseData, sizeof(NextAnalyseData));

	bool bComperWithOther = (bFirstCount == bNextCount);
	////检查转化
	//tagTransData ttdFst;
	//tagTransData ttdNxt;
	//AnalyseMaxTransform(bInFirstList, bFirstCount, bFirstList, ttdFst);
	//AnalyseMaxTransform(bInNextList, bNextCount, bNextList, ttdNxt);

	CopyMemory(bFirstList, bInFirstList, sizeof(BYTE)*bFirstCount);
	CopyMemory(bNextList, bInNextList, sizeof(BYTE)*bNextCount);
	//排序牌组
	SortCardList(bFirstList, bFirstCount, enDescend);
	SortCardList(bNextList, bNextCount, enDescend);

	//分析牌组
	AnalyseCard(bFirstList, bFirstCount, FirstAnalyseData);
	AnalyseCard(bNextList, bNextCount, NextAnalyseData);

	

	if (!((bFirstCount == bNextCount) || (bFirstCount != bNextCount && (3 == bFirstCount && 5 == bNextCount || 5 == bFirstCount && 3 == bNextCount)))) return enCRError;


	//获取类型
	BYTE cbNextMaxCard = 0, cbFirstMaxCard = 0;
	BYTE bNextType = GetCardType(bNextList, bNextCount, cbNextMaxCard);
	BYTE bFirstType = GetCardType(bFirstList, bFirstCount, cbFirstMaxCard);
	//浙江玩法区别对待,他们是A2345最小,玉林的是第二大的

	if (bCompareHuaSe == false)
	{

		if ((bNextType == CT_FIVE_MIXED_FLUSH_NO_A) && (bFirstType == CT_FIVE_MIXED_FLUSH_FIRST_A))
		{
			return enCRGreater;
		}
		else if ((bNextType == CT_FIVE_MIXED_FLUSH_FIRST_A) && (bFirstType == CT_FIVE_MIXED_FLUSH_NO_A))
		{
			return enCRLess;
		}
		else if ((bNextType == CT_FIVE_STRAIGHT_FLUSH) && (bFirstType == CT_FIVE_STRAIGHT_FLUSH_FIRST_A))
		{
			return enCRGreater;
		}
		else if ((bNextType == CT_FIVE_STRAIGHT_FLUSH_FIRST_A) && (bFirstType == CT_FIVE_STRAIGHT_FLUSH))
		{
			return enCRLess;
		}
	}
	if (CT_INVALID == bFirstType || CT_INVALID == bNextType) return enCRError;

	//同段比较
	if (true == bComperWithOther)
	{
		//开始对比
		if (bNextType == bFirstType)
		{
			switch (bFirstType)
			{
			case CT_SINGLE:				//单牌类型
			{
											//数据验证
											ASSERT(bNextList[0] != bFirstList[0]);
											if (bNextList[0] == bFirstList[0]) return enCRError;

											for (BYTE i = 0; i<bFirstCount; ++i)
											{
												if (GetCardLogicValue(bNextList[i]) > GetCardLogicValue(bFirstList[i]))
													return enCRGreater;
												else if (GetCardLogicValue(bNextList[i]) < GetCardLogicValue(bFirstList[i]))
													return enCRLess;
											}
											return enCREqual;
			}
			case CT_ONE_DOUBLE:			//对带一张
			{

											if (cbNextMaxCard > cbFirstMaxCard)
											{
												return enCRGreater;
											}
											else if (cbNextMaxCard < cbFirstMaxCard)
											{
												return enCRLess;
											}
											else
											{
												return enCREqual;
											}
											if (GetCardLogicValue(bNextList[NextAnalyseData.bTwoFirst[0]]) == GetCardLogicValue(bFirstList[FirstAnalyseData.bTwoFirst[0]]))
											{
												for (int i = 0; i<3; i++)
												{
													if (GetCardLogicValue(bNextList[NextAnalyseData.bOneFirst[i]]) > GetCardLogicValue(bFirstList[FirstAnalyseData.bOneFirst[i]]))
														return enCRGreater;
													else if (GetCardLogicValue(bNextList[NextAnalyseData.bOneFirst[i]]) < GetCardLogicValue(bFirstList[FirstAnalyseData.bOneFirst[i]]))
														return enCRLess;
												}
												return enCREqual;
											}
											else
											{
												if (GetCardLogicValue(bNextList[NextAnalyseData.bTwoFirst[0]]) > GetCardLogicValue(bFirstList[FirstAnalyseData.bTwoFirst[0]]))
													return enCRGreater;
												else
													return enCRLess;
											}
			}
			case CT_TWO_DOUBLE:	//两对牌型
			{
									//数据验证
									ASSERT(bNextList[NextAnalyseData.bTwoFirst[0]] != bFirstList[FirstAnalyseData.bTwoFirst[0]]);
									if (bNextList[NextAnalyseData.bTwoFirst[0]] == bFirstList[FirstAnalyseData.bTwoFirst[0]]) return enCRError;

									if (GetCardLogicValue(bNextList[NextAnalyseData.bTwoFirst[0]]) == GetCardLogicValue(bFirstList[FirstAnalyseData.bTwoFirst[0]]))
									{
										if (GetCardLogicValue(bNextList[NextAnalyseData.bTwoFirst[1]]) == GetCardLogicValue(bFirstList[FirstAnalyseData.bTwoFirst[1]]))
										{
											if (GetCardLogicValue(bNextList[NextAnalyseData.bOneFirst[0]]) > GetCardLogicValue(bFirstList[FirstAnalyseData.bOneFirst[0]]))
												return enCRGreater;
											else if (GetCardLogicValue(bNextList[NextAnalyseData.bOneFirst[0]]) < GetCardLogicValue(bFirstList[FirstAnalyseData.bOneFirst[0]]))
												return enCRLess;
											else
												return enCREqual;
										}
										else
										{
											if (GetCardLogicValue(bNextList[NextAnalyseData.bTwoFirst[1]]) > GetCardLogicValue(bFirstList[FirstAnalyseData.bTwoFirst[1]]))
												return enCRGreater;
											else
												return enCRLess;
										}
									}
									else
									{
										if (GetCardLogicValue(bNextList[NextAnalyseData.bTwoFirst[0]]) > GetCardLogicValue(bFirstList[FirstAnalyseData.bTwoFirst[0]]))
											return enCRGreater;
										else
											return enCRLess;
									}
			}
			case CT_THREE:						//三张牌型
			{
													if (cbNextMaxCard > cbFirstMaxCard)
													{
														return enCRGreater;
													}
													else if (cbNextMaxCard < cbFirstMaxCard)
													{
														return enCRLess;
													}
													else
													{
														return enCREqual;
													}
													if (GetCardLogicValue(bNextList[NextAnalyseData.bThreeFirst[0]]) > GetCardLogicValue(bFirstList[FirstAnalyseData.bThreeFirst[0]]))
														return enCRGreater;
													else if (GetCardLogicValue(bNextList[NextAnalyseData.bThreeFirst[0]]) < GetCardLogicValue(bFirstList[FirstAnalyseData.bThreeFirst[0]]))
														return enCRLess;
													else
														return enCREqual;
			}
			case CT_FIVE_THREE_DEOUBLE:			//三条一对
			{
													if (cbNextMaxCard > cbFirstMaxCard)
													{
														return enCRGreater;
													}
													else if (cbNextMaxCard < cbFirstMaxCard)
													{
														return enCRLess;
													}
													else
													{
														return enCREqual;
													}
													if (GetCardLogicValue(bNextList[NextAnalyseData.bThreeFirst[0]]) == GetCardLogicValue(bFirstList[FirstAnalyseData.bThreeFirst[0]]))
													{
														if (GetCardLogicValue(bNextList[NextAnalyseData.bTwoFirst[0]]) > GetCardLogicValue(bFirstList[FirstAnalyseData.bTwoFirst[0]]))
															return enCRGreater;
														else if (GetCardLogicValue(bNextList[NextAnalyseData.bTwoFirst[0]]) < GetCardLogicValue(bFirstList[FirstAnalyseData.bTwoFirst[0]]))
															return enCRLess;
														else
															return enCREqual;
													}
													else
													{
														if (GetCardLogicValue(bNextList[NextAnalyseData.bThreeFirst[0]]) > GetCardLogicValue(bFirstList[FirstAnalyseData.bThreeFirst[0]]))
															return enCRGreater;
														else
															return enCRLess;
													}
			}
			case CT_FIVE_MIXED_FLUSH_FIRST_A:	//A在前顺子
			case CT_FIVE_MIXED_FLUSH_BACK_A:	//A在后顺子
			{
													//比较最大的一张牌
													//比较数值
													//比较数值
													if (cbNextMaxCard > cbFirstMaxCard)
													{
														return enCRGreater;
													}
													else if (cbNextMaxCard < cbFirstMaxCard)
													{
														return enCRLess;
													}
													else
													{
														return enCREqual;
													}
													for (BYTE i = 0; i<5; ++i)
													{
														if (GetCardLogicValue(bNextList[i]) > GetCardLogicValue(bFirstList[i]))
															return enCRGreater;
														else if (GetCardLogicValue(bNextList[i]) < GetCardLogicValue(bFirstList[i]))
															return enCRLess;
													}
													return enCREqual;
			}
			case CT_FIVE_MIXED_FLUSH_NO_A:		//没A杂顺
			{
													//比较数值
													if (cbNextMaxCard > cbFirstMaxCard)
													{
														return enCRGreater;
													}
													else if (cbNextMaxCard < cbFirstMaxCard)
													{
														return enCRLess;
													}
													else
													{
														return enCREqual;
													}
			}
			case CT_FIVE_FLUSH:					//同花五牌
			{
													if (bCompareLogic == false)
													{
														if (bCompareHuaSe)
														{
															//比较花色
															for (BYTE i = 0; i < 5; i++)
															{
																if (GetCardColor(bNextList[i]) > GetCardColor(bFirstList[i]))
																	return enCRGreater;
																else if (GetCardColor(bNextList[i]) < GetCardColor(bFirstList[i]))
																	return enCRLess;
															}
														}
														if (NextAnalyseData.bTwoCount>FirstAnalyseData.bTwoCount)
														{
															return enCRGreater;
														}
														else if (NextAnalyseData.bTwoCount<FirstAnalyseData.bTwoCount)
														{
															return enCRLess;
														}
														else if (NextAnalyseData.bTwoCount == FirstAnalyseData.bTwoCount&&NextAnalyseData.bTwoCount>0)
														{
															if (GetCardLogicValue(bNextList[NextAnalyseData.bTwoFirst[0]]) > GetCardLogicValue(bFirstList[FirstAnalyseData.bTwoFirst[0]]))
															{
																return enCRGreater;
															}
															else
															{
																return enCRLess;
															}

														}

														//比较数值
														for (BYTE i = 0; i<5; ++i)
														{
															if (GetCardLogicValue(bNextList[i]) > GetCardLogicValue(bFirstList[i]))
																return enCRGreater;
															else if (GetCardLogicValue(bNextList[i]) < GetCardLogicValue(bFirstList[i]))
																return enCRLess;
														}
														return enCREqual;
													}
													else
													{
														if (NextAnalyseData.bTwoCount > FirstAnalyseData.bTwoCount)
														{
															return enCRGreater;
														}
														else if (NextAnalyseData.bTwoCount < FirstAnalyseData.bTwoCount)
														{
															return enCRLess;
														}
														else if (NextAnalyseData.bTwoCount == FirstAnalyseData.bTwoCount&&NextAnalyseData.bTwoCount > 0)
														{
															if (GetCardLogicValue(bNextList[NextAnalyseData.bTwoFirst[0]]) > GetCardLogicValue(bFirstList[FirstAnalyseData.bTwoFirst[0]]))
															{
																return enCRGreater;
															}
															else
															{
																return enCRLess;
															}

														}
														//比较数值
														for (BYTE i = 0; i<5; ++i)
														{
															if (GetCardLogicValue(bNextList[i]) > GetCardLogicValue(bFirstList[i]))
																return enCRGreater;
															else if (GetCardLogicValue(bNextList[i]) < GetCardLogicValue(bFirstList[i]))
																return enCRLess;
														}
														if (bCompareHuaSe)
														{
															//比较花色
															for (BYTE i = 0; i < 5; i++)
															{
																if (GetCardColor(bNextList[i]) > GetCardColor(bFirstList[i]))
																	return enCRGreater;
																else if (GetCardColor(bNextList[i]) < GetCardColor(bFirstList[i]))
																	return enCRLess;
															}
														}
														return enCREqual;

													}



			}
			case CT_FIVE_FOUR_ONE:				//四带一张
			{
													if (cbNextMaxCard > cbFirstMaxCard)
														return enCRGreater;
													else
														return enCRLess;
			}
			case CT_FIVE_STRAIGHT_FLUSH_FIRST_A://A在前同花顺
			{
													if (cbNextMaxCard > cbFirstMaxCard)
													{
														return enCRGreater;
													}
													else if (cbNextMaxCard < cbFirstMaxCard)
													{
														return enCRLess;
													}
													else
													{
														return enCREqual;
													}
													//比较数值
													for (BYTE i = 0; i<5; ++i)
													{
														if (GetCardLogicValue(bNextList[i]) > GetCardLogicValue(bFirstList[i]))
															return enCRGreater;
														else if (GetCardLogicValue(bNextList[i]) < GetCardLogicValue(bFirstList[i]))
															return enCRLess;
													}
													if (bCompareHuaSe)
													{
														//比较花色
														for (BYTE i = 0; i < 5; i++)
														{
															if (GetCardColor(bNextList[i]) > GetCardColor(bFirstList[i]))
																return enCRGreater;
															else if (GetCardColor(bNextList[i]) < GetCardColor(bFirstList[i]))
																return enCRLess;
														}
													}

													return enCREqual;

			}
			case CT_FIVE_STRAIGHT_FLUSH:		//同花顺牌
			{
													//比较数值
													if (cbNextMaxCard > cbFirstMaxCard)
													{
														return enCRGreater;
													}
													else if (cbNextMaxCard < cbFirstMaxCard)
													{
														return enCRLess;
													}
													else
													{
														return enCREqual;
													}
													for (BYTE i = 0; i < 5; ++i)
													{
														if (GetCardLogicValue(bNextList[i]) > GetCardLogicValue(bFirstList[i]))
															return enCRGreater;
														else if (GetCardLogicValue(bNextList[i]) < GetCardLogicValue(bFirstList[i]))
															return enCRLess;
													}

													if (bCompareHuaSe)
													{
														//比较花色
														for (BYTE i = 0; i < 5; i++)
														{
															if (GetCardColor(bNextList[i]) > GetCardColor(bFirstList[i]))
																return enCRGreater;
															else if (GetCardColor(bNextList[i]) < GetCardColor(bFirstList[i]))
																return enCRLess;
														}
													}
													return enCREqual;
			}
			case CT_FIVE_TONG:
			case CT_SIX_TONG://
			{
								 if (GetCardLogicValue(bNextList[4]) > GetCardLogicValue(bFirstList[4]))
									 return enCRGreater;
								 else
									 return enCRLess;
			}
			default:
				return enCRError;
			}
		}
		else
		{
			if (bNextType > bFirstType)
				return enCRGreater;
			else
				return enCRLess;
		}
	}
	else
	{
		ASSERT(bFirstType == CT_SINGLE || bFirstType == CT_ONE_DOUBLE || bFirstType == CT_THREE);
		if (bFirstType != CT_SINGLE && bFirstType != CT_ONE_DOUBLE && bFirstType != CT_THREE)
			return enCRError;
		//开始对比
		if (bNextType == bFirstType)
		{
			switch (bFirstType)
			{
			case CT_SINGLE:				//单牌类型
			{
											//数据验证
											ASSERT(bNextList[0] != bFirstList[0]);
											if (bNextList[0] == bFirstList[0]) return enCRError;

											for (BYTE i = 0; i<bFirstCount; ++i)
											{
												if (GetCardLogicValue(bNextList[i]) > GetCardLogicValue(bFirstList[i]))
													return enCRGreater;
												else if (GetCardLogicValue(bNextList[i]) < GetCardLogicValue(bFirstList[i]))
													return enCRLess;
											}

											if (bNextCount > bFirstCount)
												return enCRGreater;
											else
												return enCRLess;
			}
			case CT_ONE_DOUBLE:			//对带一张
			{
											if (cbNextMaxCard > cbFirstMaxCard)
											{
												return enCRGreater;
											}
											else if (cbNextMaxCard < cbFirstMaxCard)
											{
												return enCRLess;
											}
											else
											{
												return enCREqual;
											}
											if (GetCardLogicValue(bNextList[NextAnalyseData.bTwoFirst[0]]) == GetCardLogicValue(bFirstList[FirstAnalyseData.bTwoFirst[0]]))
											{
												if (GetCardLogicValue(bNextList[NextAnalyseData.bOneFirst[0]]) > GetCardLogicValue(bFirstList[FirstAnalyseData.bOneFirst[0]]))
													return enCRGreater;
												else if (GetCardLogicValue(bNextList[NextAnalyseData.bOneFirst[0]]) < GetCardLogicValue(bFirstList[FirstAnalyseData.bOneFirst[0]]))
													return enCRLess;
												else
												{
													if (bNextCount > bFirstCount)
														return enCRGreater;
													else
														return enCRLess;
												}
											}
											else
											{
												if (GetCardLogicValue(bNextList[NextAnalyseData.bTwoFirst[0]]) > GetCardLogicValue(bFirstList[FirstAnalyseData.bTwoFirst[0]]))
													return enCRGreater;
												else
													return enCRLess;
											}
			}
			case CT_THREE:				//三张牌型
			{
				if (cbNextMaxCard > cbFirstMaxCard)
				{
					return enCRGreater;
				}
				else if (cbNextMaxCard < cbFirstMaxCard)
				{
					return enCRLess;
				}
				else
				{
					return enCREqual;
				}
											if (GetCardLogicValue(bNextList[NextAnalyseData.bThreeFirst[0]]) > GetCardLogicValue(bFirstList[FirstAnalyseData.bThreeFirst[0]]))
												return enCRGreater;
											else if (GetCardLogicValue(bNextList[NextAnalyseData.bThreeFirst[0]]) < GetCardLogicValue(bFirstList[FirstAnalyseData.bThreeFirst[0]]))
												return enCRLess;
											else
											{
												if (bNextCount > bFirstCount)
													return enCRGreater;
												else
													return enCRLess;
											}
			}
			default:
				return enCRError;
			}
		}
		else
		{
			if (bNextType > bFirstType)
				return enCRGreater;
			else
				return enCRLess;
		}
	}

	return enCRError;
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

bool CGameLogicNew::IsContainA(BYTE cbCardData[], BYTE cbCardCount)
{
	for (int i = 0; i < cbCardCount; i++)
	{
		if (GetCardValue(cbCardData[i]) == 1)
		{
			return true;
		}
	}
	return false;
}

BYTE CGameLogicNew::GetTakeWangCardType(BYTE bCardData[], BYTE bCardCount, BYTE & bMaxCardData, tagAnalyseData &AnalyseData)
{
	if (AnalyseData.cbKingCount == 0)
	{
		return CT_INVALID;
	}

	BYTE limit = AnalyseData.cbKingCount + 2;
	//for (int i = 2; i < limit; i++)
	{
		if (AnalyseData.bThreeCount >= 1 && AnalyseData.cbKingCount == 1)
		{
			bMaxCardData = GetCardLogicValue(bCardData[AnalyseData.bThreeFirst[0]]);
			return CT_FIVE_FOUR_ONE;
		}
		else if ((AnalyseData.bFourCount >= 1 && AnalyseData.cbKingCount == 1))
		{
			bMaxCardData = GetCardLogicValue(bCardData[AnalyseData.bFourFirst[0]]);
			return CT_FIVE_TONG;
		}
		else if ((AnalyseData.bThreeCount >= 1 && AnalyseData.cbKingCount == 2))
		{
			bMaxCardData = GetCardLogicValue(bCardData[AnalyseData.bThreeFirst[0]]);
			return CT_FIVE_TONG;
		}


	}
	//错误类型
	return CT_INVALID;
}

BYTE CGameLogicNew::GetMaxCardLogic(BYTE bCardData[], BYTE bCardCount, bool bEnable)
{
	BYTE tempCard = 0;

	for (int i = 0; i < bCardCount; i++)
	{
		if (GetTakeWangCount(&bCardData[i], 1) == 0)
		{
			if (GetCardLogicValue(tempCard) < GetCardLogicValue(bCardData[i]))
			{
				tempCard = bCardData[i];
				if (GetCardLogicValue(tempCard) == 14)
				{
					return GetCardLogicValue(tempCard);
				}
			}
		}
	}
	if (bEnable && (GetTakeWangCount(bCardData, bCardCount)>0))
	{
		int firstLogic = 0;
		for (int i = 0; i < bCardCount; i++)
		{
			if (GetTakeWangCount(&bCardData[i], 1) == 0)
			{
				firstLogic = GetCardLogicValue(bCardData[i]);
				tempCard = bCardData[i];
				break;
			}
		}
		if (firstLogic == 14)
		{
			return firstLogic;
		}
		int spaceZhi = firstLogic - GetCardLogicValue(bCardData[4]);
		if (spaceZhi == 4)
		{
			return firstLogic;
		}
		else{
			if (firstLogic == 13)
			{
				return 14;

			}
			else{
				return firstLogic + 1;
			}
		}

	}
	return GetCardLogicValue(tempCard);
}

int CGameLogicNew::GetTakeWangCount(BYTE bCardData[], BYTE bCardCount)
{
	int count = 0;
	for (int i = 0; i < bCardCount; i++)
	{
		if (bCardData[i] == 0x4E || bCardData[i] == 0x4F)
		{
			count++;
		}
	}
	return count;
}

void CGameLogicNew::GetTongHuaResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	//恢复扑克，防止分析时改变扑克
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);
	vector < vector<BYTE >> tempVec;
	tempVec.resize(5);
	for (int i = 0; i < cbHandCardCount; i++)
	{
		int color = (GetCardColor(cbHandCardData[i]) >> 4);
		tempVec[color].push_back(cbHandCardData[i]);
	}

	for (int j = 0; j < 4; j++)
	{
		if (tempVec[j].size()+ tempVec[4].size() >= 5)
		{
			BYTE cbRemainCarData[MAX_COUNT] = { 0 };
			BYTE cbRemainCardCount = 0;
				CopyMemory(cbRemainCarData, tempVec[j].data(), tempVec[j].size());
				cbRemainCardCount = tempVec[j].size();
				if (5 - cbRemainCardCount>0)
				{
					CopyMemory(cbRemainCarData + cbRemainCardCount, tempVec[4].data(), 5-cbRemainCardCount);
					cbRemainCardCount =5;
				}
				BYTE cbComCard[8];
				BYTE cbComResCard[MAX_RESULT_COUNT][8];
				int cbComResLen = 0;
				BYTE cbSingleCardCount = 5;
				Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCarData, cbSingleCardCount, cbRemainCardCount, cbSingleCardCount);
				for (int m = 0; m < cbComResLen; m++)
				{
					BYTE maxCard = 0;
					if (GetCardType(cbComResCard[m], 5, maxCard) >= CT_FIVE_STRAIGHT_FLUSH)
					{
						continue;
					}
					BYTE Index = CardTypeResult[CT_FIVE_FLUSH].cbCardTypeCount;
					CardTypeResult[CT_FIVE_FLUSH].cbCardType = CT_FIVE_FLUSH;
					CopyMemory(CardTypeResult[CT_FIVE_FLUSH].cbCardData[Index],cbComResCard[m], 5);
					CardTypeResult[CT_FIVE_FLUSH].cbEachHandCardCount[Index] = 5;
					CardTypeResult[CT_FIVE_FLUSH].cbCardTypeCount++;
					if (CardTypeResult[CT_FIVE_FLUSH].cbCardTypeCount>=150)
					{
						break;
					}
				}
				if (CardTypeResult[CT_FIVE_FLUSH].cbCardTypeCount >= 150)
				{
					break;
				}
		}

	}
}

int CGameLogicNew::FindCardType(int Count, vector<tagOutCardResultNew>& vecMinTypeCardResult, vector<bool> &bOpate)
{
	if (Count==1)
	{
		for (int i = vecMinTypeCardResult.size()-1; i >=0; i--)
		{
			if (vecMinTypeCardResult[i].cbCardCount == Count&&bOpate[i] == false)
			{
				return i;
			}
		}
	}
	else{
		for (int i = 0; i < vecMinTypeCardResult.size(); i++)
		{
			if (vecMinTypeCardResult[i].cbCardCount == Count&&bOpate[i] == false)
			{
				return i;
			}
		}
	}
	return -1;
}

void CGameLogicNew::ShiSanZhangOutCardCeLue(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE  Array[DOU_NUM][DOU_HAND_COUNT], tagOutCardTypeResultNew * CardTypeResult)
{
	BYTE cbMaxCard = 0;
	int  ArrayCount[DOU_NUM] = { douNum[0], douNum[1], douNum[2] };

	//策略优化
	//中间是两对的大牌情况优化
	tagOutCardTypeResultNew NoReDoubleCardTypeResult;
	ZeroMemory(&NoReDoubleCardTypeResult, sizeof(NoReDoubleCardTypeResult));
	GetNoReResult(CardTypeResult, CT_ONE_DOUBLE, NoReDoubleCardTypeResult);
	if (checkSpecial(cbHandCardData, cbHandCardCount, NoReDoubleCardTypeResult, Array))
	{
		return;
	}
	const int fenJieZhi = 8;
	//末对是三拖2,中间是一对的情况的优化
	int type2 = GetCardType(Array[2], ArrayCount[2], cbMaxCard);
	int type1 = GetCardType(Array[1], ArrayCount[1], cbMaxCard);
	int type0 = GetCardType(Array[0], ArrayCount[0], cbMaxCard);
	if ((type0 <= CT_FIVE_MIXED_FLUSH_BACK_A && type1 == CT_TWO_DOUBLE) && type2 == CT_SINGLE && (GetCardLogicValue(Array[1][2]) >= 8))
	{
		if (type0 == CT_TWO_DOUBLE)
		{
			SwitchArray(&Array[1][0], &Array[0][0], DOU_HAND_COUNT);
		}
		SwitchArray(&Array[1][2], &Array[2][0], 2);
	}
	else if (type0 == CT_FIVE_THREE_DEOUBLE && (type1 >= CT_TWO_DOUBLE&&type1 != CT_FIVE_THREE_DEOUBLE) && type2 == CT_SINGLE)
	{
		bool bExist = false;
		for (int i = 0; i < 3; i++)
		{
			if (GetCardLogicValue(Array[2][i]) == GetCardLogicValue(Array[0][3]))
			{
				int count = 0;
				for (int j = 0; j < 3; j++)
				{
					if (GetCardLogicValue(Array[2][i]) != GetCardLogicValue(Array[0][3]))
					{
						SwitchArray(&Array[0][3 + count], &Array[2][j], 1);
						count++;
					}
				}
				if (type1 != CT_TWO_DOUBLE)
				{
					SwitchArray(Array[0], Array[1], DOU_HAND_COUNT);
				}
				bExist = true;
				break;
			}
		}
		if (bExist == false && type1 == CT_THREE)
		{
			for (int i = 0; i < 2; i++)
			{
				if (GetCardLogicValue(Array[1][3 + i]) == GetCardLogicValue(Array[0][3]))
				{
					SwitchArray(&Array[1][3 + i], &Array[2][2], 1);
					SwitchArray(&Array[0][3], Array[2], 2);
					bExist = true;
					break;
				}
			}
		}
		if (GetCardLogicValue(Array[0][3]) > fenJieZhi && (GetCardLogicValue(Array[2][2]) < 14) && bExist == false)
		{
			if (type1 != CT_FIVE_THREE_DEOUBLE)
			{
				SwitchArray(&Array[0][3], &Array[2][0], 2);
			}
			else{
				SwitchArray(&Array[1][3], &Array[2][0], 2);
			}
			if (type1 > CT_TWO_DOUBLE)
			{
				SwitchArray(&Array[0][0], &Array[1][0], 5);
			}
		}
	}
	
	else  if ((type0 == CT_FIVE_MIXED_FLUSH_FIRST_A || type1 == CT_FIVE_MIXED_FLUSH_FIRST_A) && type2 == CT_SINGLE)
	{
		int index = 0;
		bool bExist = false;
		if (type0 == CT_FIVE_MIXED_FLUSH_FIRST_A)
		{
			for (int i = 0; i < 3; i++)
			{
				if (GetCardLogicValue(Array[2][i]) == 6)
				{
					SwitchArray(&Array[2][i], &Array[index][0], 1);
					SortCardList(Array[2], 3, ST_ASCENDING);
					SortCardList(Array[index], DOU_HAND_COUNT, ST_ASCENDING);
					bExist = true;
					break;
				}
			}
		}
		if (type1 == CT_FIVE_MIXED_FLUSH_FIRST_A&&bExist == false)
		{
			index = 1;
			for (int i = 0; i < 3; i++)
			{
				if (GetCardLogicValue(Array[2][i]) == 6)
				{
					SwitchArray(&Array[2][i], &Array[index][0], 1);
					SortCardList(Array[2], 3, ST_ASCENDING);
					SortCardList(Array[index], DOU_HAND_COUNT, ST_ASCENDING);
					bExist = true;
					break;
				}
			}
		}
		index = 0;
		if (type0 == CT_FIVE_MIXED_FLUSH_BACK_A&&bExist == false)
		{
			for (int i = 0; i < 3; i++)
			{
				if (GetCardLogicValue(Array[2][i]) == 9)
				{
					SwitchArray(&Array[2][i], &Array[index][4], 1);
					SortCardList(Array[2], 3, ST_ASCENDING);
					SortCardList(Array[index], DOU_HAND_COUNT, ST_ASCENDING);
					bExist = true;
					break;
				}
			}
		}
		if (type1 == CT_FIVE_MIXED_FLUSH_BACK_A&&bExist == false)
		{
			index = 1;
			for (int i = 0; i < 3; i++)
			{
				if (GetCardLogicValue(Array[2][i]) == 9)
				{
					SwitchArray(&Array[2][i], &Array[index][4], 1);
					SortCardList(Array[2], 3, ST_ASCENDING);
					SortCardList(Array[index], DOU_HAND_COUNT, ST_ASCENDING);
					break;
				}
			}
		}

	}
	else  if ((type0 == CT_FIVE_MIXED_FLUSH_BACK_A || type1 == CT_FIVE_MIXED_FLUSH_BACK_A) && type2 == CT_SINGLE)
	{
		int index = 0;
		bool bExist = false;
		if (type0 == CT_FIVE_MIXED_FLUSH_BACK_A)
		{
			for (int i = 0; i < 3; i++)
			{
				if (GetCardLogicValue(Array[2][i]) == 9)
				{
					SwitchArray(&Array[2][i], &Array[index][4], 1);
					SortCardList(Array[2], 3, ST_ASCENDING);
					SortCardList(Array[index], DOU_HAND_COUNT, ST_ASCENDING);
					bExist = true;
					break;
				}
			}
		}
		if (type1 == CT_FIVE_MIXED_FLUSH_BACK_A&&bExist == false)
		{
			index = 1;
			for (int i = 0; i < 3; i++)
			{
				if (GetCardLogicValue(Array[2][i]) == 9)
				{
					SwitchArray(&Array[2][i], &Array[index][4], 1);
					SortCardList(Array[2], 3, ST_ASCENDING);
					SortCardList(Array[index], DOU_HAND_COUNT, ST_ASCENDING);
					break;
				}
			}
		}

	}
	else  if ((type0 == CT_FIVE_FLUSH || type1 == CT_FIVE_FLUSH) && type2 == CT_SINGLE)
	{
		int index = 0;
		if (type0 == CT_FIVE_FLUSH)
		{
			index = 0;
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < DOU_HAND_COUNT; j++)
				{
					if (GetCardColor(Array[2][i]) == GetCardColor(Array[index][j]) && (GetCardLogicValue(Array[index][j]) > GetCardLogicValue(Array[2][i])))
					{
						SwitchArray(&Array[2][i], &Array[index][j], 1);
						SortCardList(Array[2], 3, ST_ASCENDING);
						SortCardList(Array[index], DOU_HAND_COUNT, ST_ASCENDING);
						break;
					}
				}
			}
		}
		if (type1 == CT_FIVE_FLUSH)
		{
			index = 1;
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < DOU_HAND_COUNT; j++)
				{
					if (GetCardColor(Array[2][i]) == GetCardColor(Array[index][j]) && (GetCardLogicValue(Array[index][j]) > GetCardLogicValue(Array[2][i])))
					{
						SwitchArray(&Array[2][i], &Array[index][j], 1);
						SortCardList(Array[2], 3, ST_ASCENDING);
						SortCardList(Array[index], DOU_HAND_COUNT, ST_ASCENDING);
						break;
					}
				}
			}
		}

	}


	type2 = GetCardType(Array[2], ArrayCount[2], cbMaxCard);
	type1 = GetCardType(Array[1], ArrayCount[1], cbMaxCard);
	type0 = GetCardType(Array[0], ArrayCount[0], cbMaxCard);
	if (type0 == CT_FIVE_THREE_DEOUBLE&& type1 == CT_ONE_DOUBLE)
	{
		if (GetCardLogicValue(Array[0][0]) > fenJieZhi && (GetCardLogicValue(Array[0][0]) > GetCardLogicValue(Array[0][3])))
		{
		}
		else if (GetCardLogicValue(Array[0][3]) > fenJieZhi && (GetCardLogicValue(Array[2][2]) < 14))
		{
			SwitchArray(&Array[0][3], &Array[2][0], 2);
		}
	}
	if (type0 == CT_FIVE_THREE_DEOUBLE&& type1 == CT_THREE&& type2 == CT_SINGLE)
	{
		if (GetCardLogicValue(Array[1][0]) > GetCardLogicValue(Array[0][0]))
		{
			SwitchArray(&Array[0][0], &Array[1][0], 3);
		}
	}
	else if ((type0 >= CT_FIVE_MIXED_FLUSH_NO_A  && type1 == CT_FIVE_MIXED_FLUSH_NO_A) && type2 == CT_SINGLE)
	{
		int index = 1;
		int tmpCount = 0;
		while (tmpCount < 3)
		{
			for (int i = 0; i < 3; i++)
			{
				if (GetCardLogicValue(Array[2][i]) == (GetCardLogicValue(Array[index][0]) - 1))
				{
					SwitchArray(&Array[2][i], &Array[index][4], 1);
					SortCardList(Array[2], 3, ST_ASCENDING);
					SortCardList(Array[index], DOU_HAND_COUNT, ST_ASCENDING);
					break;
				}
			}

			tmpCount++;
		}
		if (type0 == CT_FIVE_MIXED_FLUSH_NO_A || type0 == CT_FIVE_MIXED_FLUSH_BACK_A)
		{
			index = 0;
			int tmpCount = 0;
			while (tmpCount < 3)
			{
				for (int i = 0; i < 3; i++)
				{
					if (GetCardLogicValue(Array[2][i]) == (GetCardLogicValue(Array[index][0]) - 1))
					{
						SwitchArray(&Array[2][i], &Array[index][4], 1);
						SortCardList(Array[2], 3, ST_ASCENDING);
						SortCardList(Array[index], DOU_HAND_COUNT, ST_ASCENDING);
						break;
					}
				}

				tmpCount++;
			}
		}
	}
	else if ((type0 >= CT_FIVE_STRAIGHT_FLUSH) && type2 == CT_SINGLE)
	{
		int index = 0;
		if (type0 == CT_FIVE_STRAIGHT_FLUSH_FIRST_A)
		{
			for (int i = 0; i < 3; i++)
			{
				if (GetCardColor(Array[2][i]) == GetCardColor(Array[index][0]) && GetCardLogicValue(Array[2][i]) == 6)
				{
					SwitchArray(&Array[2][i], &Array[index][0], 1);
					SortCardList(Array[2], 3, ST_ASCENDING);
					SortCardList(Array[index], DOU_HAND_COUNT, ST_ASCENDING);
					break;
				}
			}
		}
		else if (type0 == CT_FIVE_STRAIGHT_FLUSH || type0 == CT_FIVE_STRAIGHT_FLUSH_BACK_A)
		{
			int tmpCount = 0;
			while (tmpCount < 3)
			{
				for (int i = 0; i < 3; i++)
				{
					if (GetCardColor(Array[2][i]) == GetCardColor(Array[index][0]) && GetCardLogicValue(Array[2][i]) == (GetCardLogicValue(Array[index][0]) - 1))
					{
						SwitchArray(&Array[2][i], &Array[index][4], 1);
						SortCardList(Array[2], 3, ST_ASCENDING);
						SortCardList(Array[index], DOU_HAND_COUNT, ST_ASCENDING);
						break;
					}
				}

				tmpCount++;
			}
		}
	}


	//两道三拖2,
	type0 = GetCardType(Array[0], ArrayCount[0], cbMaxCard);
	type1 = GetCardType(Array[1], ArrayCount[1], cbMaxCard);
	type2 = GetCardType(Array[2], ArrayCount[2], cbMaxCard);
	if (type0 == CT_FIVE_THREE_DEOUBLE&& type1 == CT_FIVE_THREE_DEOUBLE&& type2 == CT_SINGLE)
	{
		bool bExist = false;

		if ((type0 == CT_FIVE_THREE_DEOUBLE && type1 == CT_FIVE_THREE_DEOUBLE) && type2 == CT_SINGLE)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int i = 0; i < 3; i++)
				{
					if (GetCardLogicValue(Array[2][i]) == GetCardLogicValue(Array[j][3]))
					{
						if (i == 0)
						{
							SwitchArray(&Array[2][1], &Array[j][3], 2);
						}
						else if (i == 1)
						{
							SwitchArray(&Array[2][0], &Array[j][3], 1);
							SwitchArray(&Array[2][2], &Array[j][4], 1);
						}
						else if (i == 2)
						{
							SwitchArray(&Array[2][0], &Array[j][3], 2);
						}

						if (j == 0)
						{
							SwitchArray(&Array[0][3], &Array[1][3], 2);
						}
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
		if (((GetCardLogicValue(Array[2][2]) < 14) || GetCardLogicValue(Array[1][3]) >= 8) && bExist == false)
		{
			if (GetCardLogicValue(Array[0][3]) > GetCardLogicValue(Array[1][3]))
			{
				SwitchArray(&Array[0][3], &Array[2][0], 2);

				SwitchArray(&Array[1][3], &Array[0][3], 2);
			}
			else{
				SwitchArray(&Array[1][3], &Array[2][0], 2);
			}
		}
	}
	else if (type0 > CT_FIVE_THREE_DEOUBLE&& type1 == CT_FIVE_THREE_DEOUBLE&& type2 == CT_SINGLE)
	{
		bool bExist = false;

		for (int i = 0; i < 3; i++)
		{
			if (GetCardLogicValue(Array[2][i]) == GetCardLogicValue(Array[1][3]))
			{
				if (i == 0)
				{
					SwitchArray(&Array[2][1], &Array[1][3], 2);
				}
				else if (i == 1)
				{
					SwitchArray(&Array[2][0], &Array[1][3], 1);
					SwitchArray(&Array[2][2], &Array[1][4], 1);
				}
				else if (i == 2)
				{
					SwitchArray(&Array[2][0], &Array[1][3], 2);
				}

				bExist = true;
				break;
			}
		}
		if ((GetCardLogicValue(Array[2][2]) < 14) && GetCardLogicValue(Array[1][3]) >= 5 && bExist == false)
		{
			SwitchArray(&Array[1][3], &Array[2][0], 2);
		}
	}
	else if (((type0 <= CT_FIVE_MIXED_FLUSH_BACK_A&& type1 == CT_ONE_DOUBLE && (type2 <= CT_ONE_DOUBLE&&GetCardLogicValue(Array[2][0]) < fenJieZhi)) || (type0 == CT_TWO_DOUBLE&& type1 == CT_TWO_DOUBLE && type2 <= CT_ONE_DOUBLE))
		&& NoReDoubleCardTypeResult.cbCardTypeCount >= 3 && NoReDoubleCardTypeResult.cbCardTypeCount <= 4)
	{
		if (NoReDoubleCardTypeResult.cbCardTypeCount == 3)
		{
			BYTE cbReserveCardData[MAX_COUNT] = { 0 };
			int cbReserveCardCount = cbHandCardCount;
			CopyMemory(cbReserveCardData, cbHandCardData, cbHandCardCount);
			RemoveTypeCard(NoReDoubleCardTypeResult, cbReserveCardData, cbReserveCardCount);
			SortCardList(cbReserveCardData, cbReserveCardCount, ST_ASCENDING);
			for (int i = 0; i < DOU_NUM; i++)
			{
				if (i == 0)
				{
					CopyMemory(Array[i], NoReDoubleCardTypeResult.cbCardData[2], 2);
					CopyMemory(Array[i] + 2, &cbReserveCardData[0], 3);
				}
				else if (i == 1)
				{
					CopyMemory(Array[i], NoReDoubleCardTypeResult.cbCardData[1], 2);
					CopyMemory(Array[i] + 2, &cbReserveCardData[3], 3);
				}
				else if (i == 2)
				{
					CopyMemory(Array[i], NoReDoubleCardTypeResult.cbCardData[0], 2);
					CopyMemory(Array[i] + 2, &cbReserveCardData[6], 1);
				}
			}
		}
		else if (NoReDoubleCardTypeResult.cbCardTypeCount == 4)
		{
			BYTE cbReserveCardData[MAX_COUNT] = { 0 };
			int cbReserveCardCount = cbHandCardCount;
			CopyMemory(cbReserveCardData, cbHandCardData, cbHandCardCount);
			RemoveTypeCard(NoReDoubleCardTypeResult, cbReserveCardData, cbReserveCardCount);
			SortCardList(cbReserveCardData, cbReserveCardCount, ST_ASCENDING);
			for (int i = 0; i < DOU_NUM; i++)
			{
				if (i == 0)
				{
					CopyMemory(Array[i], NoReDoubleCardTypeResult.cbCardData[1], 2);
					CopyMemory(Array[i] + 2, NoReDoubleCardTypeResult.cbCardData[0], 2);
					CopyMemory(Array[i] + 4, &cbReserveCardData[0], 1);
				}
				else if (i == 1)
				{
					CopyMemory(Array[i], NoReDoubleCardTypeResult.cbCardData[3], 2);
					CopyMemory(Array[i] + 2, &cbReserveCardData[1], 3);
				}
				else if (i == 2)
				{
					CopyMemory(Array[i], NoReDoubleCardTypeResult.cbCardData[2], 2);
					CopyMemory(Array[i] + 2, &cbReserveCardData[4], 1);
				}
			}
		}
	}
	//两对加顺子
	else if (type0 == CT_FIVE_MIXED_FLUSH_NO_A&& type1 == CT_TWO_DOUBLE&& type2 == CT_SINGLE&&NoReDoubleCardTypeResult.cbCardTypeCount == 2)
	{

		BYTE cbReserveCardData[MAX_COUNT] = { 0 };
		int cbReserveCardCount = cbHandCardCount;
		CopyMemory(cbReserveCardData, cbHandCardData, cbHandCardCount);
		RemoveTypeCard(CardTypeResult[CT_ONE_DOUBLE], cbReserveCardData, cbReserveCardCount);
		SortCardList(cbReserveCardData, cbReserveCardCount, ST_ASCENDING);

		//搜索边牌
		tagSearchCardResult tmpSearchCardResult = {};
		BYTE cbLineCardResult5 = SearchAllLineCardType(cbReserveCardData, cbReserveCardCount, 5, &tmpSearchCardResult);
		if (tmpSearchCardResult.cbSearchCount > 0)
		{
			RemoveCard(tmpSearchCardResult.cbResultCard[tmpSearchCardResult.cbSearchCount - 1], 5, cbReserveCardData, cbReserveCardCount);
			cbReserveCardCount -= 5;
			SortCardList(cbReserveCardData, cbReserveCardCount, ST_ASCENDING);
			for (int i = 0; i < DOU_NUM; i++)
			{
				if (i == 0)
				{
					CopyMemory(Array[i], tmpSearchCardResult.cbResultCard[tmpSearchCardResult.cbSearchCount - 1], 5);
				}
				else if (i == 1)
				{
					CopyMemory(Array[i], NoReDoubleCardTypeResult.cbCardData[1], 2);
					CopyMemory(Array[i] + 2, NoReDoubleCardTypeResult.cbCardData[0], 2);
					CopyMemory(Array[i] + 4, &cbReserveCardData[0], 1);
				}
				else if (i == 2)
				{
					CopyMemory(Array[i], &cbReserveCardData[1], 3);
				}
			}
		}

	}
	//}//一手三拖二，带一堆散牌
	else if (type0 == CT_FIVE_THREE_DEOUBLE&& type1 == CT_SINGLE&& type2 == CT_SINGLE)
	{
		if (CardTypeResult[CT_FIVE_FLUSH].cbCardTypeCount > 0)
		{
			for (int i = 0; i < CardTypeResult[CT_FIVE_FLUSH].cbCardTypeCount; i++)
			{
				BYTE cbReserveCardData[MAX_COUNT] = { 0 };
				CopyMemory(cbReserveCardData, cbHandCardData, cbHandCardCount);
				int cbReserveCardCount = cbHandCardCount;
				RemoveCard(CardTypeResult[CT_FIVE_FLUSH].cbCardData[i], 5, cbReserveCardData, cbReserveCardCount);
				cbReserveCardCount -= 5;
				tagSearchCardResult tmpSearchCardResult = {};
				BYTE cbSameCardResultCount = SearchSameCard(cbReserveCardData, cbReserveCardCount, 0, 3, &tmpSearchCardResult);

				tagSearchCardResult tmpDoubleSearchCardResult = {};
				BYTE cbDoubleSameCardResultCount = SearchSameCard(cbReserveCardData, cbReserveCardCount, 0, 2, &tmpDoubleSearchCardResult);

				if (tmpSearchCardResult.cbSearchCount > 0)
				{
					RemoveCard(tmpSearchCardResult.cbResultCard[tmpSearchCardResult.cbSearchCount - 1], tmpSearchCardResult.cbCardCount[tmpSearchCardResult.cbSearchCount - 1], cbReserveCardData, cbReserveCardCount);
					cbReserveCardCount -= tmpSearchCardResult.cbCardCount[tmpSearchCardResult.cbSearchCount - 1];
					SortCardList(cbReserveCardData, cbReserveCardCount, ST_ASCENDING);
					for (int i = 0; i < DOU_NUM; i++)
					{
						if (i == 0)
						{
							CopyMemory(Array[i], CardTypeResult[CT_FIVE_FLUSH].cbCardData[i], 5);
						}
						else if (i == 1)
						{
							CopyMemory(Array[i], tmpSearchCardResult.cbResultCard[tmpSearchCardResult.cbSearchCount - 1], 3);
							CopyMemory(Array[i] + 3, &cbReserveCardData[0], 2);
						}
						else if (i == 2)
						{

							CopyMemory(Array[i], &cbReserveCardData[2], 3);
						}
					}
					break;
				}
				else if (tmpDoubleSearchCardResult.cbSearchCount == 2)
				{
					for (int j = 0; j < tmpDoubleSearchCardResult.cbSearchCount; j++)
					{
						RemoveCard(tmpDoubleSearchCardResult.cbResultCard[j], tmpDoubleSearchCardResult.cbCardCount[j], cbReserveCardData, cbReserveCardCount);
						cbReserveCardCount -= tmpDoubleSearchCardResult.cbCardCount[j];
					}

					SortCardList(cbReserveCardData, cbReserveCardCount, ST_ASCENDING);
					for (int i = 0; i < DOU_NUM; i++)
					{
						if (i == 0)
						{
							CopyMemory(Array[i], CardTypeResult[CT_FIVE_FLUSH].cbCardData[i], 5);
						}
						else if (i == 1)
						{
							CopyMemory(Array[i], tmpDoubleSearchCardResult.cbResultCard[tmpDoubleSearchCardResult.cbSearchCount - 1], 2);
							CopyMemory(Array[i] + 2, tmpDoubleSearchCardResult.cbResultCard[tmpDoubleSearchCardResult.cbSearchCount - 2], 2);
							CopyMemory(Array[i] + 4, &cbReserveCardData[0], 1);
						}
						else if (i == 2)
						{

							CopyMemory(Array[i], &cbReserveCardData[1], 3);
						}
					}
				}
			}
		}

	}
	//五驾车摆法
	if (NoReDoubleCardTypeResult.cbCardTypeCount == 5)
	{
		type0 = GetCardType(Array[0], ArrayCount[0], cbMaxCard);
		type1 = GetCardType(Array[1], ArrayCount[1], cbMaxCard);
		type2 = GetCardType(Array[2], ArrayCount[2], cbMaxCard);
		if ((type0 == CT_TWO_DOUBLE&&type1 == CT_TWO_DOUBLE) ||
			(type0 == CT_FIVE_FLUSH&&type1 == CT_TWO_DOUBLE && GetCardLogicValue(Array[1][0]) == 14) ||
			(type0 == CT_FIVE_FLUSH&&type1 == CT_FIVE_FLUSH && type2 == CT_SINGLE) ||
			(type0 <= CT_FIVE_MIXED_FLUSH_BACK_A&&type1 <= CT_ONE_DOUBLE && type2 <= CT_ONE_DOUBLE&&GetCardLogicValue(Array[2][0]) < fenJieZhi))
		{
			BYTE cbReserveCardData[MAX_COUNT] = { 0 };
			int cbReserveCardCount = cbHandCardCount;
			CopyMemory(cbReserveCardData, cbHandCardData, cbHandCardCount);
			RemoveTypeCard(NoReDoubleCardTypeResult, cbReserveCardData, cbReserveCardCount);
			SortCardList(cbReserveCardData, cbReserveCardCount, ST_ASCENDING);
			//if (type0 >= CT_FIVE_FLUSH&& type1 < CT_FIVE_FLUSH&& type2 == CT_SINGLE)
			{
				for (int i = 0; i < DOU_NUM; i++)
				{
					if (i == 0)
					{
						CopyMemory(Array[i], NoReDoubleCardTypeResult.cbCardData[3], 2);
						CopyMemory(Array[i] + 2, NoReDoubleCardTypeResult.cbCardData[0], 2);
						CopyMemory(Array[i] + 4, &cbReserveCardData[i], 1);
					}
					else if (i == 1)
					{
						CopyMemory(Array[i], NoReDoubleCardTypeResult.cbCardData[2], 2);
						CopyMemory(Array[i] + 2, NoReDoubleCardTypeResult.cbCardData[1], 2);
						CopyMemory(Array[i] + 4, &cbReserveCardData[i], 1);
					}
					else if (i == 2)
					{
						CopyMemory(Array[i], NoReDoubleCardTypeResult.cbCardData[4], 2);
						CopyMemory(Array[i] + 2, &cbReserveCardData[i], 1);
					}
				}
			}
		}

	}
	//两同花,头小摆法
	else if (CardTypeResult[CT_THREE].cbCardTypeCount == 2)
	{
		type0 = GetCardType(Array[0], ArrayCount[0], cbMaxCard);
		type1 = GetCardType(Array[1], ArrayCount[1], cbMaxCard);
		type2 = GetCardType(Array[2], ArrayCount[2], cbMaxCard);

		BYTE cbReserveCardData[MAX_COUNT] = { 0 };
		int cbReserveCardCount = cbHandCardCount;
		CopyMemory(cbReserveCardData, cbHandCardData, cbHandCardCount);
		RemoveTypeCard(CardTypeResult[CT_THREE], cbReserveCardData, cbReserveCardCount);
		SortCardList(cbReserveCardData, cbReserveCardCount, ST_ASCENDING);

		if ((type0 == CT_FIVE_FLUSH) && (GetCardLogicValue(Array[2][2]) < 14) && (type2 == CT_SINGLE))
		{

			if ((NoReDoubleCardTypeResult.cbCardTypeCount >= 3))
			{
				tagSearchCardResult SameCardResult = {};
				BYTE cbSameCardResultCount = SearchSameCard(cbReserveCardData, cbReserveCardCount, 0, 2, &SameCardResult);
				RemoveTypeCard(NoReDoubleCardTypeResult, cbReserveCardData, cbReserveCardCount);
				if (GetCardLogicValue(cbReserveCardData[cbReserveCardCount - 1]) == 14)
				{
					for (int i = 0; i < DOU_NUM; i++)
					{
						if (i == 0)
						{
							CopyMemory(Array[i], CardTypeResult[CT_THREE].cbCardData[0], 3);
							CopyMemory(Array[i] + 3, SameCardResult.cbResultCard[0], 2);
						}
						else if (i == 1)
						{
							CopyMemory(Array[i], CardTypeResult[CT_THREE].cbCardData[1], 3);
							CopyMemory(Array[i] + 3, &cbReserveCardData[0], 2);
						}
						else if (i == 2)
						{
							CopyMemory(Array[i], &cbReserveCardData[2], 3);
						}
					}
				}
				else{
					for (int i = 0; i < DOU_NUM; i++)
					{
						if (i == 0)
						{
							CopyMemory(Array[i], CardTypeResult[CT_THREE].cbCardData[0], 3);
							CopyMemory(Array[i] + 3, &cbReserveCardData[0], 2);
						}
						else if (i == 1)
						{
							CopyMemory(Array[i], CardTypeResult[CT_THREE].cbCardData[1], 3);
							CopyMemory(Array[i] + 3, &cbReserveCardData[2], 2);
						}
						else if (i == 2)
						{

							CopyMemory(Array[i], SameCardResult.cbResultCard[0], 2);
							CopyMemory(Array[i] + 2, &cbReserveCardData[4], 1);
						}
					}
				}

			}
		}
		else if (type0 >= CT_FIVE_MIXED_FLUSH_NO_A&& type0 <= CT_FIVE_MIXED_FLUSH_BACK_A){
			//搜索边牌
			//搜索同张
			BYTE cbReserveCardData[MAX_COUNT] = { 0 };
			int cbReserveCardCount = cbHandCardCount;
			CopyMemory(cbReserveCardData, cbHandCardData, cbHandCardCount);
			RemoveCard(Array[0], 5, cbReserveCardData, cbReserveCardCount);
			cbReserveCardCount -= 5;

			tagSearchCardResult SameCardResult3 = {};
			BYTE cbSameCardResultCount = SearchSameCard(cbReserveCardData, cbReserveCardCount, 0, 3, &SameCardResult3);


			if (SameCardResult3.cbSearchCount >= 2)
			{
				RemoveCard(SameCardResult3.cbResultCard[0], 3, cbReserveCardData, cbReserveCardCount);
				cbReserveCardCount -= 3;
				SortCardList(cbReserveCardData, cbReserveCardCount, ST_ASCENDING);
				for (int i = 0; i < DOU_NUM; i++)
				{
					if (i == 2)
					{
						CopyMemory(Array[i], SameCardResult3.cbResultCard[0], 3);
					}
					else if (i == 1)
					{
						CopyMemory(Array[i], &cbReserveCardData[0], 5);
					}
				}
			}
		}

	}
	type2 = GetCardType(Array[2], ArrayCount[2], cbMaxCard);
	type1 = GetCardType(Array[1], ArrayCount[1], cbMaxCard);
	type0 = GetCardType(Array[0], ArrayCount[0], cbMaxCard);
	if (type0 == CT_TWO_DOUBLE&& type1 == CT_TWO_DOUBLE)
	{
		if (GetCardLogicValue(Array[0][3]) > GetCardLogicValue(Array[1][3]))
		{
			SwitchArray(&Array[0][2], &Array[1][2], 2);
		}
	}
	else if (type0 == CT_FIVE_THREE_DEOUBLE&&type1 == CT_FIVE_FLUSH&& type2 == CT_SINGLE&&GetCardLogicValue(Array[0][0]) >= 12)
	{

		int index = -1;
		int index2 = -1;
		for (int i = 0; i < DOU_HAND_COUNT; i++)
		{
			if (GetCardColor(Array[0][i]) == GetCardColor(Array[1][0]))
			{
				index = i;
			}
		}
		for (int i = 0; i < DOU_HAND_COUNT; i++)
		{
			if (GetCardLogicValue(Array[1][i]) == GetCardLogicValue(Array[0][4]))
			{
				index2 = i;
			}
		}
		if (index != -1 && index2 != -1)
		{
			SwitchArray(&Array[0][index], &Array[1][index2], 1);
			SwitchArray(Array[0], Array[1], 5);
			SortOutCardList(Array[1], DOU_HAND_COUNT);
			SwitchArray(Array[1] + 3, Array[2], 2);
		}
	}
	else if (type0 == CT_FIVE_THREE_DEOUBLE&&type1 == CT_TWO_DOUBLE&& type2 == CT_SINGLE&&GetCardLogicValue(Array[0][0]) >= 12)
	{

		if (CardTypeResult[CT_FIVE_FLUSH].cbCardTypeCount == 1)
		{
			bool Exist = false;
			for (int i = 0; i < 4; i++)
			{
				if (GetCardColor(Array[1][i]) == GetCardColor(CardTypeResult[CT_FIVE_FLUSH].cbCardData[0][0]))
				{
					Exist = true;
					break;
				}
			}
			if (Exist == false)
			{
				BYTE cbReserveCardData[MAX_COUNT] = { 0 };
				int cbReserveCardCount = cbHandCardCount;
				CopyMemory(cbReserveCardData, cbHandCardData, cbHandCardCount);
				RemoveTypeCard(CardTypeResult[CT_FIVE_FLUSH], cbReserveCardData, cbReserveCardCount);
				RemoveCard(Array[1], 4, cbReserveCardData, cbReserveCardCount);
				cbReserveCardCount -= 4;
				tagSearchCardResult SameCardResult = {};
				BYTE cbSameCardResultCount = SearchSameCard(cbReserveCardData, cbReserveCardCount, 0, 2, &SameCardResult);

				if (SameCardResult.cbSearchCount >= 1)
				{
					RemoveCard(SameCardResult.cbResultCard[0], 2, cbReserveCardData, cbReserveCardCount);
					cbReserveCardCount -= 2;
				}
				SortCardList(cbReserveCardData, cbReserveCardCount, ST_ASCENDING);


				for (int i = 0; i < DOU_NUM; i++)
				{
					if (i == 0)
					{
						CopyMemory(Array[i], CardTypeResult[CT_FIVE_FLUSH].cbCardData[0], 5);
					}
					else if (i == 1)
					{
						CopyMemory(Array[i] + 4, &cbReserveCardData[0], 1);
					}
					else if (i == 2)
					{

						CopyMemory(Array[i], SameCardResult.cbResultCard[0], 2);
						CopyMemory(Array[i] + 2, &cbReserveCardData[1], 1);
					}
				}

				int a = 4;
			}

		}

	}

	//两个三条的情况分开摆的情况的优化
	type0 = GetCardType(Array[0], ArrayCount[0], cbMaxCard);
	type1 = GetCardType(Array[1], ArrayCount[1], cbMaxCard);
	type2 = GetCardType(Array[2], ArrayCount[2], cbMaxCard);
	if (type0 == CT_FIVE_THREE_DEOUBLE&& type1 == CT_SINGLE&&CardTypeResult[CT_THREE].cbCardTypeCount == 2)
	{
		int changeType = CT_THREE;
		int num = 0;
		for (int i = 0; i < CardTypeResult[changeType].cbEachHandCardCount[0]; i++)
		{
			bool bBreake = false;
			for (int j = 0; j < DOU_NUM; j++)
			{
				for (int k = 0; k < DOU_HAND_COUNT; k++)
				{
					if (Array[j][k] == CardTypeResult[changeType].cbCardData[0][i])
					{
						SwitchArray(&Array[1][num], &Array[j][k], 1);
						num++;
						bBreake = true;
						break;
					}

				}
				if (bBreake)
				{
					break;
				}
			}
		}
	}

	type0 = GetCardType(Array[0], ArrayCount[0], cbMaxCard);
	type1 = GetCardType(Array[1], ArrayCount[1], cbMaxCard);
	type2 = GetCardType(Array[2], ArrayCount[2], cbMaxCard);

	if (type0 == CT_THREE && type1 == CT_TWO_DOUBLE)
	{
		if (type2 == CT_ONE_DOUBLE)
		{
			SwitchArray(&Array[0][3], &Array[2][0], 2);
		}
		else
		{
			SwitchArray(&Array[0][3], &Array[1][2], 2);
		}
	}

	type0 = GetCardType(Array[0], ArrayCount[0], cbMaxCard);
	type1 = GetCardType(Array[1], ArrayCount[1], cbMaxCard);
	type2 = GetCardType(Array[2], ArrayCount[2], cbMaxCard);
	if (type2 == CT_SINGLE && type1 == CT_TWO_DOUBLE)
	{
		if (GetCardLogicValue(Array[2][2]) >= 14)
		{

		}
		else{
			SwitchArray(&Array[1][2], &Array[2][0], 2);
		}
	}
	else if (type2 == CT_ONE_DOUBLE && type1 == CT_TWO_DOUBLE)
	{
		if (GetCardLogicValue(Array[1][0]) > GetCardLogicValue(Array[2][0]))
		{
			SwitchArray(&Array[1][0], &Array[2][0], 2);
			SwitchArray(&Array[1][0], &Array[1][2], 2);
		}

	}
	else if (type2 == CT_ONE_DOUBLE && type1 == CT_ONE_DOUBLE)
	{
		if (GetCardLogicValue(Array[1][0]) < GetCardLogicValue(Array[2][0]))
		{
			SwitchArray(&Array[1][0], &Array[2][0], 2);
		}

	}
	else if (type2 == CT_THREE &&type1 <= CT_TWO_DOUBLE)
	{
		if (type1 == CT_TWO_DOUBLE)
		{
			SwitchArray(&Array[2][0], &Array[1][0], 3);
			SwitchArray(&Array[2][2], &Array[1][4], 1);
		}
		else{
			SwitchArray(&Array[2][0], &Array[1][0], 3);
		}
	}

	type0 = GetCardType(Array[0], ArrayCount[0], cbMaxCard);
	type1 = GetCardType(Array[1], ArrayCount[1], cbMaxCard);
	type2 = GetCardType(Array[2], ArrayCount[2], cbMaxCard);
	if (type0 == CT_TWO_DOUBLE &&type2 == CT_SINGLE && type1 == CT_ONE_DOUBLE)
	{
		SwitchArray(&Array[1][0], &Array[0][0], 5);
		SwitchArray(&Array[2][0], &Array[1][2], 2);
	}
	else if (type0 == CT_TWO_DOUBLE &&type2 == CT_SINGLE && type1 == CT_SINGLE)
	{
		SwitchArray(&Array[0][2], &Array[1][0], 2);
	}

	else if (((type0 <= CT_FIVE_MIXED_FLUSH_BACK_A && type0 >= CT_FIVE_MIXED_FLUSH_NO_A) || (type1 <= CT_FIVE_MIXED_FLUSH_BACK_A&&type1 >= CT_FIVE_MIXED_FLUSH_NO_A)) && type2 == CT_ONE_DOUBLE)
	{
		if (GetCardLogicValue(Array[2][0]) < GetCardLogicValue(Array[2][2]))
		{
			int index = 0;
			bool bExist = false;
			if ((type0 <= CT_FIVE_MIXED_FLUSH_BACK_A && type0 >= CT_FIVE_MIXED_FLUSH_NO_A))
			{
				index = 0;
				if (((GetCardLogicValue(Array[index][0]) - 1) == GetCardLogicValue(Array[2][0])) && GetCardLogicValue(Array[2][2]) == GetCardLogicValue(Array[index][4]))
				{
					SwitchArray(&Array[2][0], &Array[index][4], 1);
					SortCardList(Array[2], 3, ST_ASCENDING);
					SortCardList(Array[index], DOU_HAND_COUNT, ST_ASCENDING);
					bExist = true;
				}
			}
			if ((type1 <= CT_FIVE_MIXED_FLUSH_BACK_A && type1 >= CT_FIVE_MIXED_FLUSH_NO_A) && bExist == false)
			{
				index = 1;
				if (((GetCardLogicValue(Array[index][0]) - 1) == GetCardLogicValue(Array[2][0])) && GetCardLogicValue(Array[2][2]) == GetCardLogicValue(Array[index][4]))
				{
					SwitchArray(&Array[2][0], &Array[index][4], 1);
					SortCardList(Array[2], 3, ST_ASCENDING);
					SortCardList(Array[index], DOU_HAND_COUNT, ST_ASCENDING);
				}
			}
			if ((type0 == CT_FIVE_MIXED_FLUSH_NO_A || type1 == CT_FIVE_MIXED_FLUSH_NO_A) && bExist == false)
			{
				index = 1;
				if (type0 == CT_FIVE_MIXED_FLUSH_NO_A)
				{
					index = 0;
				}
				if (((GetCardLogicValue(Array[index][4]) + 1) == GetCardLogicValue(Array[2][2])))
				{
					SwitchArray(&Array[2][2], &Array[index][0], 1);
					SortCardList(Array[index], DOU_HAND_COUNT, ST_ASCENDING);
				}
			}
		}

	}

	if (type0 == CT_FIVE_FLUSH&&type1 == CT_TWO_DOUBLE)
	{
		bool bExist = false;
		for (int i = 0; i < DOU_HAND_COUNT; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (GetCardColor(Array[0][i]) == GetCardColor(Array[1][j]))
				{
					int index = 0;
					if (j < 2)
					{
						index = 2;
					}
					if ((GetCardLogicValue(Array[0][i]) == GetCardLogicValue(Array[1][index])))
					{
						SwitchArray(&Array[0][i], &Array[1][j], 1);
						SortCardList(Array[0], DOU_HAND_COUNT, ST_ASCENDING);
						SortCardList(Array[1], DOU_HAND_COUNT, ST_ASCENDING);
						bExist = true;
						break;
					}

				}
			}

			if (bExist == true)
			{
				break;
			}
		}
	}

}

bool CGameLogicNew::checkSpecial(const BYTE * cbHandCardData, BYTE cbHandCardCount, tagOutCardTypeResultNew &NoReDoubleCardTypeResult, BYTE  Array[DOU_NUM][DOU_HAND_COUNT])
{
	//获取牌型
	BYTE tempResultCardData[NORMAL_COUNT] = { 0 };
	int speciapType = GetSpecialType(cbHandCardData, cbHandCardCount, tempResultCardData);
	if (speciapType > CT_EX_INVALID)
	{
		if (speciapType == CT_EX_LIUDUIBAN)
		{   
			BYTE cbMaxCard = 0;
			int type0 = GetCardType(Array[0],DOU_HAND_COUNT, cbMaxCard);
			if (type0 == CT_FIVE_FOUR_ONE)
			{
				return false;
			}
			//保留扑克，防止分析时改变扑克
			BYTE cbReserveCardData[MAX_COUNT] = { 0 };
			int cbReserveCardCount = cbHandCardCount;
			CopyMemory(cbReserveCardData, cbHandCardData, cbHandCardCount);
			RemoveTypeCard(NoReDoubleCardTypeResult, cbReserveCardData, cbReserveCardCount);
			for (int i = 0; i < DOU_NUM; i++)
			{
				if (i == 0)
				{
					CopyMemory(Array[i], NoReDoubleCardTypeResult.cbCardData[0], 2);
					CopyMemory(Array[i] + 2, NoReDoubleCardTypeResult.cbCardData[1], 2);
					CopyMemory(Array[i] + 4, &cbReserveCardData[0], 1);
				}
				else if (i == 1)
				{
					CopyMemory(Array[i], NoReDoubleCardTypeResult.cbCardData[2], 2);
					CopyMemory(Array[i] + 2, NoReDoubleCardTypeResult.cbCardData[3], 2);
					CopyMemory(Array[i] + 4, NoReDoubleCardTypeResult.cbCardData[5], 1);
				}
				else if (i == 2)
				{
					CopyMemory(Array[i], NoReDoubleCardTypeResult.cbCardData[4], 2);
					CopyMemory(Array[i] + 2, &NoReDoubleCardTypeResult.cbCardData[5][1], 1);
				}
			}
			return true;

		}
		else if (speciapType == CT_EX_WUDUISANTIAO)
		{
			BYTE cbMaxCard = 0;
			int type0 = GetCardType(Array[0], DOU_HAND_COUNT, cbMaxCard);
			if (type0 == CT_FIVE_FOUR_ONE)
			{
				return true;
			}
			//保留扑克，防止分析时改变扑克
			BYTE cbReserveCardData[MAX_COUNT] = { 0 };
			int cbReserveCardCount = cbHandCardCount;
			CopyMemory(cbReserveCardData, cbHandCardData, cbHandCardCount);
			RemoveTypeCard(NoReDoubleCardTypeResult, cbReserveCardData, cbReserveCardCount);
			int index = 0;
			for (int i = 0; i < NoReDoubleCardTypeResult.cbCardTypeCount;i++)
			{
				if (GetCardLogicValue(NoReDoubleCardTypeResult.cbCardData[i][0]) == GetCardLogicValue(cbReserveCardData[0]))
				{
					index = i;
				}
			}
			int duiIndex = 0;
			for (int i = 0; i < DOU_NUM; i++)
			{
				if (duiIndex==index)
				{
					duiIndex++;
				}
				if (i == 0)
				{
					CopyMemory(Array[i], NoReDoubleCardTypeResult.cbCardData[duiIndex++], 2);
					if (duiIndex == index)
					{
						duiIndex++;
					}
					CopyMemory(Array[i] + 2, NoReDoubleCardTypeResult.cbCardData[duiIndex++], 2);
					if (duiIndex == index)
					{
						duiIndex++;
					}
					CopyMemory(Array[i] + 4, &cbReserveCardData[0], 1);
				}
				else if (i == 1)
				{
					CopyMemory(Array[i], NoReDoubleCardTypeResult.cbCardData[duiIndex++], 2);
					if (duiIndex == index)
					{
						duiIndex++;
					}
					CopyMemory(Array[i] + 2, NoReDoubleCardTypeResult.cbCardData[duiIndex++], 2);
					if (duiIndex == index)
					{
						duiIndex++;
					}
					CopyMemory(Array[i] + 4, NoReDoubleCardTypeResult.cbCardData[index], 1);
				}
				else if (i == 2)
				{
					CopyMemory(Array[i], NoReDoubleCardTypeResult.cbCardData[duiIndex], 2);
					CopyMemory(Array[i] + 2, &NoReDoubleCardTypeResult.cbCardData[index][1], 1);
				}
			}
			return true;

		}
		else if (speciapType == CT_EX_SANTONGHUA || speciapType == CT_EX_SANSHUNZI)
		{
			for (int i = 0; i < DOU_NUM; i++)
			{
				if (i == 0)
				{
					CopyMemory(Array[i], &tempResultCardData[0]+8, 5);
				}
				else if (i == 1)
				{
					CopyMemory(Array[i], &tempResultCardData[0]+3, 5);
				}
				else if (i == 2)
				{
					CopyMemory(Array[i], &tempResultCardData[0], 3);
				}
			}
			BYTE cbMaxCard = 0;
			int type0 = GetCardType(Array[0], DOU_HAND_COUNT, cbMaxCard);
			int type1 = GetCardType(Array[1], DOU_HAND_COUNT, cbMaxCard);
			if (type0 == CT_FIVE_STRAIGHT_FLUSH_BACK_A || type1 == CT_FIVE_STRAIGHT_FLUSH_BACK_A)
			{
				return false;
			}
			return true;

		}

	}
	return false;
}

float CGameLogicNew::GetAllTypePercent(BYTE cbCardData[], BYTE cbCardCount, BYTE cbDouData[], BYTE cbDouCount, int cbCardType)
{
	BYTE tempCardList[DISPATCH_COUNT] = { 0 };
	BYTE tempCardCount = DISPATCH_COUNT;
	CopyMemory(tempCardList, m_cbCardData, DISPATCH_COUNT);
	if (RemoveCard(cbCardData, cbCardCount, tempCardList, tempCardCount))
	{
		tempCardCount -= cbCardCount;
	}

	float shengLv = 0;
	int WinCount = 0;
	int AllCount = 0;
	BYTE tempCard[DOU_HAND_COUNT] = { 0 };
	for (int i = 0; i < tempCardCount; i++)
	{
		tempCard[0] = tempCard[i];
		for (int j = i+1; j < tempCardCount; j++)
		{
			tempCard[1] = tempCard[j];
			for (int k = j+1; k < tempCardCount; k++)
			{
				tempCard[2] = tempCard[k];
				for (int l = k+1; l < tempCardCount; l++)
				{
					tempCard[3] = tempCard[l];
					for (int m = l+1; m < tempCardCount; m++)
					{
						tempCard[4] = tempCard[m];

					}
				}
			}
		}
	}
	return 0;
}

bool CGameLogicNew::IsReResult(tagOutCardTypeResultNew * CardTypeResult, int type, BYTE cbCardData[], BYTE cbCardCount)
{

	
		for (BYTE i = 0; i < CardTypeResult[type].cbCardTypeCount; i++)
		{
			int sameCount = 0;
			bool bSelect[MAX_COUNT] = { 0 };
			for (int j = 0; j < CardTypeResult[type].cbEachHandCardCount[i]; j++)
			{
				for (int k = 0; k < cbCardCount; k++)
				{

					if (bSelect[k] == false && cbCardData[k] == CardTypeResult[type].cbCardData[i][j])
					{
						bSelect[k] = true;
						sameCount++;
						break;
					}
				}

			}
			if (sameCount== cbCardCount)
			{
				return true;
			}
		}
	
	return false;
}

bool CGameLogicNew::GetNoReResult(tagOutCardTypeResultNew * CardTypeResult, int type, tagOutCardTypeResultNew &noReCardTypeResult)
{
	int iLogic = -1;
	for (BYTE i = 0; i < CardTypeResult[type].cbCardTypeCount; i++)
	{
		if (iLogic != GetCardLogicValue(CardTypeResult[type].cbCardData[i][0]))
		{
			iLogic = GetCardLogicValue(CardTypeResult[type].cbCardData[i][0]);

			BYTE Index = noReCardTypeResult.cbCardTypeCount;
			noReCardTypeResult.cbCardType = type;
			CopyMemory(noReCardTypeResult.cbCardData[Index], CardTypeResult[type].cbCardData[i], CardTypeResult[type].cbEachHandCardCount[i]);
			noReCardTypeResult.cbEachHandCardCount[Index] = CardTypeResult[type].cbEachHandCardCount[i];
			noReCardTypeResult.cbCardTypeCount++;
		}
	}

	return true;
}

BYTE CGameLogicNew::GetSpecialType(const BYTE bHandCardData[], BYTE bCardCount, BYTE cbReusltData[])
{
	ASSERT(bCardCount == NORMAL_COUNT);
	if (bCardCount != NORMAL_COUNT) return CT_EX_INVALID;

	//排列扑克
	BYTE bCardData[NORMAL_COUNT];
	CopyMemory(bCardData, bHandCardData, sizeof(BYTE) * NORMAL_COUNT);
	//排序扑克
	SortCardList(bCardData, bCardCount, enDescend);
	if (GetTakeWangCount(bCardData,NORMAL_COUNT)>0)
	{
		 return CT_EX_INVALID;
	}
	//设置结果
	tagAnalyseData AnalyseData;
	ZeroMemory(&AnalyseData, sizeof(AnalyseData));
	AnalyseCard(bCardData, bCardCount, AnalyseData);
	//////////////////////////////////////////////////////////////////////////
	//至尊青龙
	if (IsStraightDragon(bCardData, bCardCount) == true)
		return CT_EX_ZHIZUNQINGLONG;

	//一条龙
	if ((IsLinkCard(bCardData, bCardCount) == true))
		return CT_EX_YITIAOLONG;

	////凑一色
	//if ((IsChouYiSe(bCardData, bCardCount) == true))
	//	return CT_EX_COUYISE;
	////四套三条
	//if (AnalyseData.bThreeCount == 4)
	//	return CT_EX_SITAOSANTIAO;
	//五对三条
	if ((AnalyseData.bTwoCount == 5 && AnalyseData.bThreeCount == 1) || (AnalyseData.bThreeCount == 1 && AnalyseData.bOneCount == 0))
	   return CT_EX_WUDUISANTIAO;
	//六对半
	if ((IsLiuDuiBan(bCardData, bCardCount, AnalyseData) == true))
		return CT_EX_LIUDUIBAN;

	//三顺子
	if ((IsSanShunZi(bCardData, bCardCount, cbReusltData) == true))
		return CT_EX_SANSHUNZI;



	//三同花
	if ((IsSanTongHua(bCardData, bCardCount, cbReusltData) == true))
		return CT_EX_SANTONGHUA;




	//非特殊牌
	return CT_EX_INVALID;
}

bool CGameLogicNew::SetLinkCard(BYTE cbCardData[], BYTE cbCardCount, BYTE cbLineCardData[3][5])
{
	if (cbCardCount % 5 != 0 && cbCardCount % 5 != 3)
		return false;

	tagSearchCardResult LineCardResult;

	BYTE cbLineCardResult5 = SearchLineCardType(cbCardData, cbCardCount,0,1, 5, &LineCardResult);
	if (cbLineCardResult5 < 1) return false;
	for (int i = 0; i < cbLineCardResult5; i++)
	{
		BYTE cbTempCardData[13] = { 0 };
		BYTE cbTempCount = cbCardCount;

		bool value = RemoveCard(LineCardResult.cbResultCard[i], LineCardResult.cbCardCount[i], cbCardData, cbTempCardData, cbTempCount);
		cbTempCount -= LineCardResult.cbCardCount[i];
		if (cbTempCount == 8)
		{
			while (true)
			{
				BYTE LineData[3][5] = { 0 };
				bool setResult = SetLinkCard(cbTempCardData, cbTempCount, LineData);
				if (setResult == false) break;
				for (int cbindex = 0; cbindex < 5; cbindex++)
				{
					cbLineCardData[0][cbindex] = LineData[0][cbindex];
					cbLineCardData[1][cbindex] = LineData[1][cbindex];
					cbLineCardData[2][cbindex] = LineCardResult.cbResultCard[i][cbindex];
				}
				return true;
			}
		}

		if (IsLinkCard(cbTempCardData, cbTempCount) == true)
		{
			//八张牌，放前中墩
			if (cbTempCount == 3)
			{
				for (int cbindex = 0; cbindex < 3; cbindex++)
				{
					cbLineCardData[0][cbindex] = cbTempCardData[cbindex];
				}
				for (int cbindex = 0; cbindex < LineCardResult.cbCardCount[i]; cbindex++)
				{
					cbLineCardData[1][cbindex] = LineCardResult.cbResultCard[i][cbindex];
				}
			}
			//十张，放中后墩
			else
			{
				for (int cbindex = 0; cbindex < 3; cbindex++)
				{
					cbLineCardData[1][cbindex] = cbTempCardData[cbindex];
				}
				for (int cbindex = 0; cbindex < LineCardResult.cbCardCount[i]; cbindex++)
				{
					cbLineCardData[2][cbindex] = LineCardResult.cbResultCard[i][cbindex];
				}
			}
			return true;
		}
	}
	return false;
}

bool CGameLogicNew::IsLinkCard(const BYTE cbCardData[], BYTE cbCardCount)
{
	ASSERT(cbCardCount > 0);
	if (cbCardCount <= 0) return false;

	bool bRet = true;
	BYTE cbCardBuffer[13] = { 0 };
	CopyMemory(cbCardBuffer, cbCardData, sizeof(BYTE)*cbCardCount);

	//降序排列
	SortCardList(cbCardBuffer, cbCardCount, enDescend);

	BYTE cbFirstCard = GetCardLogicValue(cbCardBuffer[0]);
	for (WORD i = 1; i < cbCardCount; i++)
	{
		BYTE cbNextCard = GetCardLogicValue(cbCardBuffer[i]);
		if (cbFirstCard != cbNextCard + i) bRet = false;
	}

	//A前顺子单独处理
	if (bRet == false && cbFirstCard == 14)
	{
		if (GetCardLogicValue(cbCardBuffer[cbCardCount - 1]) != 2)
		{
			return bRet;
		}
		else
		{
			cbFirstCard = GetCardLogicValue(cbCardBuffer[1]);
			for (int i = 2; i < cbCardCount; i++)
			{
				BYTE cbNextCard = GetCardLogicValue(cbCardBuffer[i]);
				if (cbFirstCard != (cbNextCard + i - 1)) return false;
			}
			return true;
		}
	}

	return bRet;
}
float CGameLogicNew::GetCardTurnPercent(const BYTE cbCardData[], BYTE cbCardCount, const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbDisCardData[], BYTE cbDisCardCount)
{
	bool bExistMax = false;

	BYTE allCard[FULL_COUNT] = { 0 };

	BYTE allCardCardCount = sizeof(m_cbCardData);
	CopyMemory(allCard, m_cbCardData, sizeof(m_cbCardData));
	
	if (RemoveCard(cbDisCardData, cbDisCardCount, allCard, allCardCardCount))
	{
		allCardCardCount = allCardCardCount - cbDisCardCount;
	}
	
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
	BYTE cbReferIndex = GetCardLogicValue(cbCardData[0]) + 1;
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
	if (existEvent == 0)
	{
		return 1.0;
	}
	float gaiLv = (float)existEvent / totalEvent;
	return gaiLv;
}

int CGameLogicNew::GetCMNSort(int m, int n)
{
	if (m<n)
	{
		return 0;
	}
	int mResult = 1;
	for (int i = m; i > (m - n); i--) {
		mResult = mResult*i;
	}
	int nResult = 1;
	for (int i = n; i >= 1; i--)
	{
		nResult = nResult*i;
	}
	return mResult / nResult;
}

int CGameLogicNew::CalSanDouFen(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE Array[DOU_NUM][DOU_HAND_COUNT])
{
	int  quanZhong[DOU_NUM] = { 1,2,3 };
	int  score[DOU_NUM] = { 0 };
	BYTE cbMaxCard = 0;
	int totalScore = 0;
	for (int i = 0; i < DOU_NUM; i++)
	{
		int turnCount = CalCardTurnCount(Array[i], douNum[i], cbHandCardData, cbHandCardCount, NULL, 0,i);
		score[i] = quanZhong[i] * turnCount;
		totalScore += score[i];
	}
	return totalScore;
}

int CGameLogicNew::CalCardTurnCount( BYTE cbCardData[], BYTE cbCardCount, const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbDisCardData[], BYTE cbDisCardCount,int dou)
{

	BYTE allCard[FULL_COUNT] = { 0 };

	BYTE allCardCardCount = sizeof(m_cbCardData);
	CopyMemory(allCard, m_cbCardData, sizeof(m_cbCardData));

	if (RemoveCard(cbDisCardData, cbDisCardCount, allCard, allCardCardCount))
	{
		allCardCardCount = allCardCardCount - cbDisCardCount;
	}

	BYTE cbMaxCard = 0;
	int existEvent =0;

	int type = GetCardType(cbCardData, cbCardCount, cbMaxCard);
	
	 if (type == CT_SINGLE)
	{
		int tempCount = 1;
		existEvent = 1;
		for (int i=0;i<cbCardCount;i++)
		{
			existEvent *= SearchSameCardTypeTurnCount(allCard, allCardCardCount, &cbCardData[i], tempCount);
		}
		if (dou != 2)
		{
			BYTE tmpCard[2] = { 0 };
			existEvent += SearchTongHuaTypeTurnCount(allCard, allCardCardCount, tmpCard, 1);
			existEvent += SearchAllLineCardTypeTurnCount(allCard, allCardCardCount, 5, 0, type);
		}
	}
	else if (type == CT_ONE_DOUBLE)
	{
		int tempCount = 2;
		existEvent = SearchSameCardTypeTurnCount(allCard, allCardCardCount, cbCardData, tempCount);
		if (dou != 2)
		{
			BYTE tmpCard[2] = { 0 };
			existEvent += SearchTongHuaTypeTurnCount(allCard, allCardCardCount, tmpCard, 1);
			existEvent += SearchAllLineCardTypeTurnCount(allCard, allCardCardCount, 5, 0, type);
		}
	}
	
	else if (type == CT_TWO_DOUBLE)
	{
		BYTE tmpCard[2] = { 0 };
		int tempCount = 2;
		existEvent = 1;
		existEvent = SearchSameCardTypeTurnCount(allCard, allCardCardCount, cbCardData, tempCount);
		existEvent*= SearchSameCardTypeTurnCount(allCard, allCardCardCount, &cbCardData[2], tempCount);
		existEvent += SearchAllLineCardTypeTurnCount(allCard, allCardCardCount, cbCardCount, cbCardData[0], type);
		existEvent += SearchTongHuaTypeTurnCount(allCard, allCardCardCount, tmpCard, 1);
	}
	else if (type == CT_THREE)
	{
		if (dou == 2)
		{
			return 0;
		}
		int tempCount = 3;
		BYTE tmpCard[2] = { 0 };
		existEvent = SearchSameCardTypeTurnCount(allCard, allCardCardCount, cbCardData, tempCount);
		existEvent += SearchTongHuaTypeTurnCount(allCard, allCardCardCount, tmpCard, 1);
		existEvent += SearchAllLineCardTypeTurnCount(allCard, allCardCardCount, 5, 0,type);
	}
	else if (type >= CT_FIVE_MIXED_FLUSH_NO_A&&type <= CT_FIVE_MIXED_FLUSH_BACK_A)
	{
		existEvent = SearchAllLineCardTypeTurnCount(allCard, allCardCardCount, cbCardCount, cbCardData[0], type);
		BYTE tmpCard[2] = { 0 };
		existEvent += SearchTongHuaTypeTurnCount(allCard, allCardCardCount, tmpCard, 1);
	}
	else if (type == CT_FIVE_FLUSH)
	{
		int tempCount = 5;
		SortCardList(cbCardData, cbCardCount, ST_ORDER);
		existEvent = SearchTongHuaTypeTurnCount(allCard, allCardCardCount, cbCardData, tempCount);
	}
	else if (
		type == CT_FIVE_THREE_DEOUBLE)
	{
		int tempCount = 3;
		existEvent = SearchSameCardTypeTurnCount(allCard, allCardCardCount, cbCardData, tempCount);
	}

	
	//int totalEvent = GetCMNSort(allCardCardCount, cbCardCount);
	return existEvent;
}

int CGameLogicNew::SearchSameCardTypeTurnCount(BYTE * allCard, BYTE allCardCardCount, BYTE * cbCardData, BYTE cbCardCount)
{
	//分析扑克
	tagDistributingNew Distributing = {};
	AnalysebDistributing(allCard, allCardCardCount, Distributing);
	CopyMemory(Distributing.cbDistributing[13], Distributing.cbDistributing[0], sizeof(Distributing.cbDistributing[0]));
	BYTE cbTmpLinkCount = 0;
	BYTE cbValueIndex = 0;
	BYTE cbReferIndex = GetCardLogicValue(cbCardData[0]);
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
	return existEvent;
}

int CGameLogicNew::SearchTongHuaTypeTurnCount(BYTE * allCard, BYTE allCardCardCount, BYTE * cbCardData, BYTE cbCardCount)
{
	int huaSe[4] = { 0 };
	for (int i = 0; i < allCardCardCount; i++)
	{
		huaSe[GetCardColor(allCard[i]) >> 4]++;
	}
	int resultCount = 0;
	int tempCount = 1;

	if (cbCardData[0]==0)
	{
		for (int i = 0; i < 4; i++)
		{
			resultCount += (GetCMNSort(huaSe[i] - 1, 5));
		}
	}
	else 
	{
		BYTE tempAllCard[DISPATCH_COUNT] = { 0 };
		CopyMemory(tempAllCard, allCard, sizeof(tempAllCard));
		int tempAllCount = allCardCardCount;
		BYTE includeCard[DISPATCH_COUNT] = { 0 };
		int  includeCardCount = 0;
		for (int i = 0; i < allCardCardCount; i++)
		{
			if (GetCardLogicValue(tempAllCard[i]) > GetCardLogicValue(cbCardData[0]))
			{
				includeCard[includeCardCount++] = tempAllCard[i];
			}
		}
		if (RemoveCard(includeCard, includeCardCount, tempAllCard, tempAllCount))
		{
			tempAllCount -= includeCardCount;
		}
		ZeroMemory(huaSe, sizeof(huaSe));
		for (int i = 0; i < tempAllCount; i++)
		{
			huaSe[GetCardColor(tempAllCard[i]) >> 4]++;
		}
		for (int i = 0; i < includeCardCount; i++)
		{
			int tempSe = (GetCardColor(includeCard[i]) >> 4);
			resultCount += (GetCMNSort(huaSe[tempSe], 4));
		}
	}
	
	return resultCount;
}

int CGameLogicNew::SearchAllLineCardTypeTurnCount(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbLineCount, BYTE cbReferCard, int type)
{

	//定义变量
	BYTE cbResultCount = 0;
	BYTE cbBlockCount = 1;

	//超过A
	if (cbLineCount > 14) return cbResultCount;

	//长度判断
	if (cbHandCardCount < cbLineCount) return cbResultCount;

	//构造扑克
	BYTE cbCardData[FULL_COUNT];
	BYTE cbCardCount = cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//排列扑克
	SortCardList(cbCardData, cbCardCount, ST_ORDER);

	//分析扑克
	tagDistributingNew Distributing;
	ZeroMemory(&Distributing, sizeof(Distributing));
	AnalysebDistributing(cbCardData, cbCardCount, Distributing);
	CopyMemory(Distributing.cbDistributing[13], Distributing.cbDistributing[0], sizeof(Distributing.cbDistributing[0]));
	//搜索顺子
	BYTE cbTmpLinkCount = 0;
	int resultCount = 0;
	BYTE cbReferIndex = GetCardLogicValue(cbReferCard);
	if (type == CT_FIVE_MIXED_FLUSH_FIRST_A)
	{
		cbReferIndex = 9;
	}
	for (BYTE cbValueIndex = cbReferIndex; cbValueIndex < 14; cbValueIndex++)
	{
		//继续判断
		if (Distributing.cbDistributing[cbValueIndex][cbIndexCount] < cbBlockCount)
		{
			if (cbTmpLinkCount < cbLineCount)
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

		if (cbTmpLinkCount >= cbLineCount)
		{

			//复制扑克
			BYTE cbCount = 0;
			int cbTmpCount = 1;
			for (BYTE cbIndex = cbValueIndex + 1 - cbTmpLinkCount; cbIndex <= cbValueIndex; cbIndex++)
			{
				cbTmpCount *= Distributing.cbDistributing[cbIndex][cbIndexCount];
				
			}
			resultCount += cbTmpCount;
			cbTmpLinkCount--;
		}
	}
	if (type == CT_FIVE_MIXED_FLUSH_NO_A)
	{
		cbTmpLinkCount = 0;
		for (BYTE cbValueIndex = 0; cbValueIndex < 5; cbValueIndex++)
		{
			//继续判断
			if (Distributing.cbDistributing[cbValueIndex][cbIndexCount] < cbBlockCount)
			{
				if (cbTmpLinkCount < cbLineCount)
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

			if (cbTmpLinkCount >= cbLineCount)
			{

				//复制扑克
				BYTE cbCount = 0;
				int cbTmpCount = 1;
				for (BYTE cbIndex = cbValueIndex + 1 - cbTmpLinkCount; cbIndex <= cbValueIndex; cbIndex++)
				{
					cbTmpCount *= Distributing.cbDistributing[cbIndex][cbIndexCount];

				}
				resultCount += cbTmpCount;
				cbTmpLinkCount--;
			}
		}
	}
	//寻找9,10

	return resultCount;
}


//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
