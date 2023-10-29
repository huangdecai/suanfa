#include "StdAfx.h"
#include ".\androidai.h"


/////////////////////////////////////////////////////////////////////////////////////
//CAndroidAIBase

CAndroidAIBase::CAndroidAIBase()
{
	memset(m_byCardData,0xff,sizeof(m_byCardData));
}

CAndroidAIBase::~CAndroidAIBase()
{
}

//设置牌
void CAndroidAIBase::SetCardData( const BYTE cbCardData[],BYTE byCardCount, tagWeaveItem wi[MAX_WEAVE], BYTE cbWeaveCount )
{
	//复制牌
	CopyMemory(m_byCardData,cbCardData,sizeof(BYTE)*byCardCount);
	m_byCardCount = byCardCount;

	//初始化
	ZeroMemory(m_bSelect,sizeof(m_bSelect));

	//
	ZeroMemory(m_byThreeCard,sizeof(m_byThreeCard));
	m_byThreeCount = 0;
	ZeroMemory(m_byGoodThreeCard,sizeof(m_byGoodThreeCard));
	m_byGoodThreeCount = 0;

	//
	ZeroMemory(m_byTwoCard,sizeof(m_byTwoCard));
	m_byTwoCount = 0;
	ZeroMemory(m_byGoodTwoCard,sizeof(m_byGoodTwoCard));
	m_byGoodTwoCount = 0;

	//
	ZeroMemory(m_byRemainThree,sizeof(m_byRemainThree));
	m_byRemainThreeCount = 0;
	ZeroMemory(m_byRemainTwo,sizeof(m_byRemainTwo));
	m_byRemainTwoCount = 0;

	//
	m_nMaxScoreThree = m_nMaxScoreTwo = 0;
	m_nActionScore = 0;
	m_nScoreThree = m_nScoreTwo = 0;

	//
	m_bHaveJiang = false;

	//
	m_byBadlyCard = 0xff;

	//胡牌限制条件
	m_cbColorCount = 0;
	ZeroMemory( m_cbColor,sizeof(m_cbColor) );
	m_cbMinColorCount = MAX_COUNT;

	//搜索花色
	for( BYTE i = 0; i < byCardCount; i++ )
	{
		m_cbColor[(cbCardData[i]/9)]++;	
	}
	for( BYTE i = 0; i < cbWeaveCount; i++ )
	{
		m_cbColor[(wi[i].cbCenterCard&MASK_COLOR)>>4]++;
	}
	
	for( BYTE i = 0; i < 3; i++ )
	{
		if( m_cbColor[i] > 0 )
			m_cbColorCount++;

		if( m_cbColor[i] < m_cbMinColorCount )
			m_cbMinColorCount = m_cbColor[i];
	}
}

//获取最差牌
BYTE CAndroidAIBase::GetBadlyCard()
{
	return m_byBadlyCard;
}

//获取最大分
int CAndroidAIBase::GetMaxScore()
{
	return m_nActionScore+m_nMaxScoreThree+m_nMaxScoreTwo;
}

//两只牌是否是边的
bool CAndroidAIBase::IsEdge( BYTE byCard1,BYTE byCard2 )
{
	if( 0 == byCard1%9 || 8 == byCard2%9 )
		return true;
	return false;
}

//搜索相同牌
bool CAndroidAIBase::SearchSameCard( BYTE byCardData,BYTE &byIndex1,BYTE &byIndex2 )
{
	//
	byIndex1 = FindIndex(byCardData);
	if( byIndex1 == 0xff ) return false;
	byIndex2 = FindIndex(byCardData,byIndex1+1);
	if( byIndex2 == 0xff ) return false;
	return true;
}

//搜索连牌
bool CAndroidAIBase::SearchLinkCard( BYTE byCardData,BYTE &byIndex1,BYTE &byIndex2 )
{
	//效验
	if( byCardData >= 27 ) return false;
	//第二,三只
	BYTE byCard1 = byCardData+1;
	BYTE byCard2 = byCardData+2;
	if( byCard1 >= 27 || byCard2 >= 27 || byCardData/9 != byCard1/9 || byCardData/9 != byCard2/9 )
		return false;
	//寻找
	byIndex1 = FindIndex(byCard1);
	if( byIndex1 == 0xff ) return false;
	byIndex2 = FindIndex(byCard2);
	if( byIndex2 == 0xff ) return false;

	return true;
}

//搜索两只同牌
bool CAndroidAIBase::SearchSameCardRemain( BYTE byCardData,BYTE &byIndex,BYTE byStart )
{
	byIndex = FindIndexRemain(byCardData,byStart);
	return 0xff==byIndex?false:true;
}

//搜索有卡连牌
bool CAndroidAIBase::SearchLinkCardRemain( BYTE byCardData,BYTE byLinkType,BYTE &byIndex,BYTE byStart )
{
	//验证
	if( byCardData >= 27 ) return false;
	//判断类型
	BYTE byCard1 = 0xff;
	if( 0 == byLinkType )			//紧连
		byCard1 = byCardData+1;
	else if( 1 == byLinkType )		//有卡
		byCard1 = byCardData+2;
	//过滤
	if( byCard1 >= 27 || byCardData/9 != byCard1/9 ) return false;
	byIndex = FindIndexRemain(byCard1,byStart);
	return 0xff==byIndex?false:true;
}

//搜索牌
BYTE CAndroidAIBase::FindIndex( BYTE byCardData,BYTE byStart )
{
	for( BYTE i = byStart; i < m_byCardCount; i++ )
	{
		if( byCardData == m_byCardData[i] && !m_bSelect[i] )
			return i;
	}
	return 0xff;
}

//在移除最佳三只后搜索牌
BYTE CAndroidAIBase::FindIndexRemain( BYTE byCardData,BYTE byStart )
{
	for( BYTE i = byStart; i < m_byRemainThreeCount; i++ )
	{
		if( byCardData == m_byRemainThree[i] && !m_bSelect[i] )
			return i;
	}
	return 0xff;
}

//移除牌
bool CAndroidAIBase::RemoveCard( BYTE byCardIndex )
{
	//效验
	ASSERT( m_byCardCount > 0 );
	if( m_byCardCount == 0 ) return false;
	ASSERT( byCardIndex >= 0 && byCardIndex < MAX_INDEX );
	if( byCardIndex < 0 || byCardIndex >= MAX_INDEX ) return false;

	//删除
	BYTE byCount = m_byCardCount;
	m_byCardCount = 0;
	bool bFound = false;
	for( BYTE i = 0; i < byCount; i++ )
	{
		if( i == byCardIndex ) 
		{
			bFound = true;
			continue;
		}
		m_byCardData[m_byCardCount++] = m_byCardData[i];
	}

	if( bFound )
		m_byCardData[byCount-1] = 0xff;

	return bFound;
}

//移除牌值
bool CAndroidAIBase::RemoveCardData( BYTE byCardData )
{
	//效验
	ASSERT( m_byCardCount > 0 );
	if( m_byCardCount == 0 ) return false;

	//删除
	BYTE byCount = m_byCardCount;
	m_byCardCount = 0;
	bool bFound = false;
	for( BYTE i = 0; i < byCount; i++ )
	{
		if( !bFound && m_byCardData[i] == byCardData )
		{
			bFound = true;
			continue;
		}
		m_byCardData[m_byCardCount++] = m_byCardData[i];
	}

	if( bFound)
		m_byCardData[byCount-1] = 0xff;
	
	return bFound;
}

/////////////////////////////////////////////////////////////////////////////////////
//CAndroidAI

CAndroidAI::CAndroidAI(void)
{
	ZeroMemory(m_byEnjoinOutCard,sizeof(m_byEnjoinOutCard));
	m_byEnjoinOutCount = 0;
}

CAndroidAI::~CAndroidAI(void)
{
}

//思考
void CAndroidAI::Think()
{
	//重置
	m_nMaxScoreThree = 0;
	m_nMaxScoreTwo = 0;

	//分析三只
	AnalyseThree();
	//如果没三只
	BYTE i;
	if( m_nMaxScoreThree == 0 || m_byRemainThreeCount == 0 )
	{
		m_byRemainThreeCount = m_byCardCount;
		for( i = 0; i < m_byRemainThreeCount; i++ )
			m_byRemainThree[i] = m_byCardData[i];
	}
	//分析两只
	AnalyseTwo();
	if( m_nMaxScoreTwo == 0 )
	{
		m_byRemainTwoCount = m_byRemainThreeCount;
		for( i = 0; i < m_byRemainTwoCount; i++ )
			m_byRemainTwo[i] = m_byRemainThree[i];
	}
	//如果全部是两只
	if( m_byRemainTwoCount == 0 )
	{
		SearchTwo();
		return;
	}
	//分析一只
	AnalyseOne();
}

//从最佳两只牌组合中搜索最差牌
BYTE CAndroidAI::GetBadlyIn2Card()
{
	BYTE byBadly = 0xff;
	int nMin = 33;
	int nScore;
	BYTE byCard;
	for( BYTE i = 0; i < m_byGoodTwoCount*2; i++ )
	{
		byCard = m_byGoodTwoCard[i];

		if( IsEnjoinOutCard(byCard) ) continue;

		if( byCard >= 27 )						//如果是字
		{
			nScore = 2;
		}
		else if( byCard%9 == 0 || byCard%9 == 8 )	//如果是一或者九
		{
			nScore = 6;
		}
		else 
		{
			nScore = 10;
		}
		nScore += AddScore(byCard);

		//去除一种花色
		if( m_cbColorCount == 3 && m_cbColor[(byCard/9)] == m_cbMinColorCount )
			nScore -= 10;

		if( nScore < nMin )
		{
			nMin = nScore;
			byBadly = byCard;
		}
	}
	return byBadly;
}

//从最佳三只牌组合中搜索最差牌
BYTE CAndroidAI::GetBadlyIn3Card()
{
	BYTE byBadly = 0xff;
	int nMin = 33;
	int nScore;
	BYTE byCard;
	for( BYTE i = 0; i < m_byGoodThreeCount*3; i++ )
	{
		byCard = m_byGoodThreeCard[i];

		if( IsEnjoinOutCard(byCard) ) continue;

		if( byCard >= 27 )						//如果是字
		{
			nScore = 2;
		}
		else if( byCard%9 == 0 || byCard%9 == 8 )	//如果是一或者九
		{
			nScore = 6;
		}
		else 
		{
			nScore = 10;
		}
		nScore += AddScore(byCard);

		//去除一种花色
		if( m_cbColorCount == 3 && m_cbColor[(byCard/9)] == m_cbMinColorCount )
			nScore -= 10;

		if( nScore < nMin )
		{
			nMin = nScore;
			byBadly = byCard;
		}
	}
	return byBadly;
}

//设置禁止出的牌
void CAndroidAI::SetEnjoinOutCard( const BYTE cbEnjoinOutCard[],BYTE cbEnjoinOutCount )
{
	m_byEnjoinOutCount = cbEnjoinOutCount;
	if( m_byEnjoinOutCount > 0 )
	{
		CopyMemory(m_byEnjoinOutCard,cbEnjoinOutCard,sizeof(BYTE)*cbEnjoinOutCount);
	}
}

//模拟操作
bool CAndroidAI::SetAction(BYTE byActionMask, BYTE byActionCard, int GangKind )
{
	//验证
	ASSERT( byActionCard >=0 && byActionCard < 34 );
	if( byActionCard >= 34 ) return false;

	//枚举
	switch( byActionMask )
	{
	
	case WIK_PENG:
		{
			m_nActionScore = 300;;
			VERIFY( RemoveCardData(byActionCard) );
			VERIFY( RemoveCardData(byActionCard) );
			//禁止出的牌
			m_byEnjoinOutCount = 0;
			m_byEnjoinOutCard[m_byEnjoinOutCount++] = byActionCard;
			break;
		}
	case WIK_GANG:
		{
			VERIFY( RemoveCardData(byActionCard) );
			BYTE byIndex = FindIndex(byActionCard);
			if( byIndex != 0xff )
			{
				m_nActionScore = 350;
				if (GangKind==1)//明杠
				{
					m_nActionScore += 35;
				}
				else if (GangKind == 2)//暗杠
				{
					m_nActionScore += 65;
				}
				VERIFY( RemoveCardData(byActionCard) );
				VERIFY( RemoveCardData(byActionCard) );
				byIndex = FindIndex(byActionCard);
				if( byIndex != 0xff )
					VERIFY( RemoveCardData(byActionCard) );
			}
			break;
		}
	case WIK_LEFT:
	{
			m_nActionScore = 149;
			if (RemoveCardData(byActionCard + 1)==false)
			{
				return false;
			}
			if (RemoveCardData(byActionCard + 2) == false)
			{
				return false;
			}
		break;
	}
	case WIK_CENTER:
	{
			m_nActionScore = 149;
			if (RemoveCardData(byActionCard - 1) == false)
			{
				return false;
			}
			if (RemoveCardData(byActionCard + 1) == false)
			{
				return false;
			}
			
		break;
	}
	case WIK_RIGHT:
	{
			m_nActionScore = 149;
			if (RemoveCardData(byActionCard - 2) == false)
			{
				return false;
			}
			if (RemoveCardData(byActionCard - 1) == false)
			{
				return false;
			}
		break;
	}
	default:
		ASSERT( FALSE );
	}
	return true;
}

//重置得分
void CAndroidAI::ResetScore()
{
	m_nActionScore = m_nMaxScoreThree = m_nMaxScoreTwo = 0;
}

//加权
int CAndroidAI::AddScore( BYTE byCardData )
{
	int nScore = 0;
	if( byCardData >= 27 )
	{
		return 0;
	}
	if( byCardData%9 != 0 && FindIndex(byCardData-1) != 0xff )	//如果剩余的牌中有比其少一的牌
	{
		if( byCardData%9 != 1 )						//如果当前牌不是二类即加3
		{
			nScore += 3;
		}
		else
		{
			nScore += 1;
		}
	}
	if( byCardData%9 != 8 && FindIndex(byCardData+1) != 0xff )	//如果剩余的牌中有比起多一个的牌
	{
		if( byCardData%9 != 7 )
		{
			nScore += 3;
		}
		else
		{
			nScore += 1;
		}
	}
	if( byCardData%9 > 1 && FindIndex(byCardData-2) != 0xff )		//如果剩余的牌中有比其少二的牌（如3—5,1_3等）
	{
		nScore += 2;
	}
	
	if( byCardData%9 < 7 && FindIndex(byCardData+2) != 0xff )		//如果剩余的牌中有比其多二的牌
	{
		nScore += 2;
	}

	return nScore;
}

//分析三只
void CAndroidAI::AnalyseThree()
{
	BYTE byIndex1, byIndex2;
	for (BYTE i = 0; i < m_byCardCount; i++)
	{
		if (!m_bSelect[i])
		{
			m_bSelect[i] = true;
			//搜索三只
			if (SearchSameCard(m_byCardData[i], byIndex1, byIndex2))
			{
				//临时记录
				m_byThreeCard[m_byThreeCount * 3] = m_byCardData[i];
				m_byThreeCard[m_byThreeCount * 3 + 1] = m_byCardData[i];
				m_byThreeCard[m_byThreeCount * 3 + 2] = m_byCardData[i];

				//递归
				m_byThreeCount++;
				m_nScoreThree += 170;
				m_bSelect[byIndex1] = true;
				m_bSelect[byIndex2] = true;
				AnalyseThree();
				m_bSelect[byIndex1] = false;
				m_bSelect[byIndex2] = false;
				m_nScoreThree -= 170;
				m_byThreeCount--;
			}
			//搜索连牌
			if (SearchLinkCard(m_byCardData[i], byIndex1, byIndex2))
			{
				//临时记录
				m_byThreeCard[m_byThreeCount * 3] = m_byCardData[i];
				m_byThreeCard[m_byThreeCount * 3 + 1] = m_byCardData[i] + 1;
				m_byThreeCard[m_byThreeCount * 3 + 2] = m_byCardData[i] + 2;

				//递归
				m_byThreeCount++;
				m_nScoreThree += 150;
				m_bSelect[byIndex1] = true;
				m_bSelect[byIndex2] = true;
				AnalyseThree();
				m_bSelect[byIndex1] = false;
				m_bSelect[byIndex2] = false;
				m_nScoreThree -= 150;
				m_byThreeCount--;
			}
			m_bSelect[i] = false;
		}
	}
	//如果搜索到分数更高的
	if (m_nScoreThree > m_nMaxScoreThree)
	{
		//记录剩下的
		m_byRemainThreeCount = 0;
		m_nMaxScoreThree = m_nScoreThree;
		for (int i = 0; i < m_byCardCount; i++)
		{
			if (!m_bSelect[i])
				m_byRemainThree[m_byRemainThreeCount++] = m_byCardData[i];
		}
		//记录最佳三只组合
		m_byGoodThreeCount = m_byThreeCount;
		CopyMemory(m_byGoodThreeCard, m_byThreeCard, sizeof(m_byThreeCard));
	}
}

//分析两只
void CAndroidAI::AnalyseTwo()
{
	BYTE byIndex;
	for( BYTE i = 0; i < m_byRemainThreeCount; i++ )
	{
		if( !m_bSelect[i] )
		{
			m_bSelect[i] = true;
			//搜索两只相同
			if( SearchSameCardRemain(m_byRemainThree[i],byIndex,i+1) )
			{
				//临时记录
				m_byTwoCard[m_byTwoCount*2] = m_byRemainThree[i];
				m_byTwoCard[m_byTwoCount*2+1] = m_byRemainThree[byIndex];

				//判断将
				m_byTwoCount++;
				int nGoodSame = 100;
				if( !m_bHaveJiang )
				{
					m_bHaveJiang = true;
					nGoodSame = 120;
				}
				//递归
				m_nScoreTwo += nGoodSame;
				m_bSelect[byIndex] = true;
				AnalyseTwo();
				m_bSelect[byIndex] = false;
				if( 120 == nGoodSame )
					m_bHaveJiang = false;
				m_nScoreTwo -= nGoodSame;
				m_byTwoCount--;
			}
			//搜索紧连牌
			if( SearchLinkCardRemain(m_byRemainThree[i],0,byIndex,i+1) )
			{
				//临时记录
				m_byTwoCard[m_byTwoCount*2] = m_byRemainThree[i];
				m_byTwoCard[m_byTwoCount*2+1] = m_byRemainThree[byIndex];

				//判断边
				m_byTwoCount++;
				int nGoodLink;
				if( IsEdge(m_byRemainThree[i],m_byRemainThree[byIndex]) )
					nGoodLink = 80;
				else nGoodLink = 90;
				//递归
				m_nScoreTwo += nGoodLink;
				m_bSelect[byIndex] = true;
				AnalyseTwo();
				m_bSelect[byIndex] = false;
				m_nScoreTwo -= nGoodLink;
				m_byTwoCount--;
			}
			//搜索有卡的连牌
			if( SearchLinkCardRemain(m_byRemainThree[i],1,byIndex,i+1) )
			{				
				//临时记录
				m_byTwoCard[m_byTwoCount*2] = m_byRemainThree[i];
				m_byTwoCard[m_byTwoCount*2+1] = m_byRemainThree[byIndex];

				//判断边
				m_byTwoCount++;
				int nGoodLink;
				if( IsEdge(m_byRemainThree[i],m_byRemainThree[byIndex]) )
					nGoodLink = 60;
				else nGoodLink = 70;
				//递归
				m_nScoreTwo += nGoodLink;
				m_bSelect[byIndex] = true;
				AnalyseTwo();
				m_bSelect[byIndex] = false;
				m_nScoreTwo -= nGoodLink;
				m_byTwoCount--;
			}
			m_bSelect[i] = false;
		}
	}
	//如果有分数更高的
	if( m_nScoreTwo > m_nMaxScoreTwo )
	{
		//记录剩下的
		m_nMaxScoreTwo = m_nScoreTwo;
		m_byRemainTwoCount = 0;
		for(int i = 0; i < m_byRemainThreeCount; i++ )
		{
			if( !m_bSelect[i] )
				m_byRemainTwo[m_byRemainTwoCount++] = m_byRemainThree[i];
		}
		//记录最佳两只组合
		m_byGoodTwoCount = m_byTwoCount;
		CopyMemory(m_byGoodTwoCard,m_byTwoCard,sizeof(m_byTwoCard));
	}
}

//分析单只
void CAndroidAI::AnalyseOne()
{
	BYTE byCard;
	int nScore;
	int nMin = 33;
	for(int i = 0;i < m_byRemainTwoCount;i++ )	//找出最差的一张牌
	{
		byCard = m_byRemainTwo[i];

		if( IsEnjoinOutCard(byCard) ) continue;

		if( byCard >= 27 )						//如果是字
		{
			nScore = 2;
		}
		else if( byCard%9 == 0 || byCard%9 == 8 )	//如果是一或者九
		{
			nScore = 6;
		}
		else 
		{
			nScore = 10;
		}

		nScore += AddScore(byCard);

		//去除一种花色
		/*	if( m_cbColorCount == 3 && m_cbColor[(byCard/9)] == m_cbMinColorCount )
				nScore -= 10;*/

		if( nScore < nMin )
		{
			nMin = nScore;
			m_byBadlyCard = byCard;
		}
	}
}

//从两只组合中分析
void CAndroidAI::SearchTwo()
{
	//定义变量
	BYTE byCardTwo[MAX_COUNT];
	BYTE byCardTwoCount = 0;
	bool bTeamHave = false;
	//设置变量
	for( int i = 0;i < m_byRemainThreeCount;i++ )
		byCardTwo[byCardTwoCount++] = m_byRemainThree[i];
	//仅有一对
	if( byCardTwoCount == 2 )												//如果只有两张牌
	{
		if( byCardTwo[0] == byCardTwo[1] )								//胡牌
		{
			if( !IsEnjoinOutCard(byCardTwo[0]) )
				m_byBadlyCard = byCardTwo[0];								
			return;
		}
		m_byRemainTwoCount = 2;
		m_byRemainTwo[0] = byCardTwo[0];
		m_byRemainTwo[1] = byCardTwo[1];
		AnalyseOne();
	}
	else
	{
		bool bSameHave = false;
		int nMinScore = 33;
		int nScore[8];
		memset(nScore,33,sizeof(nScore));
		for( BYTE j = 0;j < byCardTwoCount/2;j++ )					//循环纪录每张牌的分数
		{
			if( byCardTwo[j*2] == byCardTwo[j*2 + 1] )				//对子
			{
				if (bSameHave)
				{
					nScore[j] = 6;
				}
				else
				{
					nScore[j] = 8;
					bSameHave = true;
				}
			}
			else if (byCardTwo[j * 2] == byCardTwo[j * 2 + 1] - 1)		//紧连门子
			{
				if (byCardTwo[j * 2] % 9 == 0 || byCardTwo[j * 2 + 1] % 9 == 8)
				{
					nScore[j] = 4;
				}
				else
				{
					nScore[j] = 7;
				}
			}
			else													//漏一个门子
			{
				if (byCardTwo[j * 2] % 9 == 0 || byCardTwo[j * 2 + 1] % 9 == 8)
					nScore[j] = 3;
				else
					nScore[j] = 5;
			}

		}
		for (BYTE k = 0; k < byCardTwoCount / 2; k++)						//找出分数最小的一张牌就是最差的一张
		{
			if (nScore[k] < nMinScore)
			{
				if (byCardTwo[k * 2] % 9 == 0 && !IsEnjoinOutCard(byCardTwo[k * 2]))
				{
					m_byBadlyCard = byCardTwo[k * 2];
				}
				else if (byCardTwo[k * 2 + 1] % 9 == 8 && !IsEnjoinOutCard(byCardTwo[k * 2 + 1]))
				{
					m_byBadlyCard = byCardTwo[k * 2 + 1];
				}
				else
				{
					int nIndex = rand() % 2;
					if (IsEnjoinOutCard(byCardTwo[k * 2 + nIndex]))
					{
						nIndex = (nIndex + 1) % 2;
						if (IsEnjoinOutCard(byCardTwo[k * 2 + nIndex])) continue;
						else m_byBadlyCard = byCardTwo[k * 2 + nIndex];
					}
					else m_byBadlyCard = byCardTwo[k * 2 + nIndex];
				}
				nMinScore = nScore[k];
			}
		}
	}
}

//判断牌是否禁止出
bool CAndroidAI::IsEnjoinOutCard(BYTE byCard)
{
	for (BYTE i = 0; i < m_byEnjoinOutCount; i++)
	{
		if (byCard == m_byEnjoinOutCard[i])
			return true;
	}
	return false;
}


bool CAndroidAI::SearchOutCard(tagOutCardResult &OutCardResult, WORD wMeChairId, WORD  wCurrentUser, BYTE cbCardDataEx[], BYTE  cbActionMask, BYTE	cbActionCard, BYTE cbCardIndex[], tagWeaveItem WeaveItemArray[GAME_PLAYER][MAX_WEAVE], BYTE cbWeaveCount[], BYTE cbDiscardCard[], BYTE cbDiscardCount)
{
	//初始化
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	BYTE cbEnjoinOutCardIndex[MAX_INDEX] = { 0 };
	//禁止出牌
	BYTE cbEnjoinOutCardCount = 0;

	BYTE cbCallCard = cbCardDataEx[0];
	if (cbCardDataEx[1]>0)
	{
		BYTE cbLaiZi1 = m_GameLogic.SwitchToCardIndex(cbCardDataEx[1]);
		BYTE cbLaiZi2 = MAX_INDEX;
		cbEnjoinOutCardIndex[cbEnjoinOutCardCount++] = cbLaiZi1;
		if (cbCardDataEx[2]>0)
		{
			cbLaiZi2 = m_GameLogic.SwitchToCardIndex(cbCardDataEx[2]);
			cbEnjoinOutCardIndex[cbEnjoinOutCardCount++] = cbLaiZi2;
		}
		BYTE MagicIndex[2] = { cbLaiZi1, cbLaiZi2 };
		m_GameLogic.SetMagicIndex(MagicIndex);
	}

	//转换索引
	BYTE byCard[MAX_COUNT], byCardCount = 0;
	for (BYTE i = 0; i < MAX_INDEX; i++)
	{
		for (BYTE j = 0; j < cbCardIndex[i]; j++)
		{

			byCard[byCardCount++] = i;
		}
	}


	BYTE cbShengYuIndex[MAX_INDEX] = { 0 };
	GetRemainingCount(WeaveItemArray, cbWeaveCount, cbCardIndex, cbShengYuIndex, cbDiscardCard, cbDiscardCount);
	 //判断听
	if ((cbActionMask & WIK_CHI_HU) != 0 )
	{
		
		if (wCurrentUser != wMeChairId)
		{
			CMD_S_TING_DATA tingData;
			
			GetTingData(cbCardIndex, cbShengYuIndex, WeaveItemArray[wMeChairId], cbWeaveCount[wMeChairId], tingData);
			int huPaiIndex = 0;
			int maxRemainingCount = 0;
			for (int i = 0; i < MAX_COUNT; i++)
			{
				int cbHuCardRemainingCount = 0;
				for (int j = 0; j < tingData.cbHuCardCount[i]; j++)
				{
					cbHuCardRemainingCount =
						tingData.cbHuCardRemainingCount[i][j] + cbHuCardRemainingCount;
				}
				maxRemainingCount += cbHuCardRemainingCount;
			}
			int Percent[] = { 100, 100, 100, 100, 50, 45, 40, 35,30, 20, 10, 5 };
			bool bHu = false;
			 if (maxRemainingCount >= CountArray(Percent))
			{
			}
			else
			{
				if (rand() % 100 <= Percent[maxRemainingCount])
				{
					bHu = true;
				}
			}
			 if (cbDiscardCount>=10)
			 {
				 bHu = true;
			 }
			if (bHu)
			{
				OutCardResult.cbOperateCode = WIK_CHI_HU;
				OutCardResult.cbOperateCard = cbActionCard;
			}
			else
			{
				OutCardResult.cbOperateCode = WIK_NULL;
				OutCardResult.cbOperateCard = 0;
			}
		}
		else
		{
			OutCardResult.cbOperateCode = WIK_CHI_HU;
			OutCardResult.cbOperateCard = cbActionCard;
		}
		return true;
	}
	else if (wCurrentUser == wMeChairId)
	{
	
		BYTE cbRes = m_GameLogic.AnalyseTingCard(cbCardIndex, WeaveItemArray[wMeChairId], cbWeaveCount[wMeChairId]);
		if (cbRes == WIK_LISTEN)
		{
			CMD_S_TING_DATA tingData;
			GetTingData(cbCardIndex, cbShengYuIndex, WeaveItemArray[wMeChairId], cbWeaveCount[wMeChairId], tingData);
			int huPaiIndex = 0;
			int maxRemainingCount = 0;
			int maxFanCount = 0;
			int cardScore[MAX_COUNT] = { 0 };
			int nOrgScore = 0;
			for (int i = 0; i < MAX_COUNT; i++)
			{
				if (i==tingData.cbOutCardCount)
				{
					break;;
				}
				int cbHuCardRemainingCount = 0;
				int FanCount = 0;
				for (int j = 0; j < tingData.cbHuCardCount[i]; j++)
				{
					cbHuCardRemainingCount =
						tingData.cbHuCardRemainingCount[i][j] + cbHuCardRemainingCount;
					if (tingData.cbHuFan[i][j]>FanCount)
					{
						FanCount = tingData.cbHuFan[i][j];
					}
					
				}
				//计算原始分
				CAndroidAI AndroidAi;
				AndroidAi.SetEnjoinOutCard(cbEnjoinOutCardIndex, cbEnjoinOutCardCount);
				BYTE tmpindex = m_GameLogic.SwitchToCardIndex(tingData.cbOutCardData[i]);
				AndroidAi.SetCardData(byCard, byCardCount, WeaveItemArray[wMeChairId], cbWeaveCount[wMeChairId]);
				AndroidAi.RemoveCardData(tmpindex);
				AndroidAi.Think();
				cardScore[i] = AndroidAi.GetMaxScore();

				if (cbHuCardRemainingCount > maxRemainingCount)
				{
					maxRemainingCount = cbHuCardRemainingCount;
					maxFanCount = FanCount;
					huPaiIndex = i;
					nOrgScore = cardScore[i];
				}
				if (FanCount>maxFanCount&&cbHuCardRemainingCount>=1)
				{
					maxFanCount = FanCount;
					huPaiIndex = i;
					nOrgScore = -1;
				}
			}
			
			OutCardResult.cbOperateCode = WIK_LISTEN;
			OutCardResult.cbOperateCard = tingData.cbOutCardData[huPaiIndex];
		}
		else 
		{
			if (cbActionMask==0)
			{
				CMD_S_ER_XING_TING_DATA tingData;
				int resultCount = GetErXiangTingData(cbCardIndex, cbShengYuIndex, WeaveItemArray[wMeChairId], cbWeaveCount[wMeChairId], tingData);
				if (resultCount > 0)
				{
					int huPaiIndex = 0;
					int maxRemainingCount = 0;
					int maxFanCount = 0;
					int cardScore[MAX_COUNT] = { 0 };
					int nOrgScore = 0;
					for (int i = 0; i < MAX_COUNT; i++)
					{
						if (i == tingData.cbOutCardCount)
						{
							break;;
						}
						int cbHuCardRemainingCount = 0;
						for (int j = 0; j < tingData.cbHuCardCount[i]; j++)
						{
							cbHuCardRemainingCount =
								tingData.cbHuCardRemainingCount[i][j] + cbHuCardRemainingCount;
						}

						if (cbHuCardRemainingCount > maxRemainingCount)
						{
							maxRemainingCount = cbHuCardRemainingCount;
							huPaiIndex = i;
						}
						else if (cbHuCardRemainingCount == maxRemainingCount)
						{
							if (tingData.cbOutCardData[i] > 0x30)
							{
								maxRemainingCount = cbHuCardRemainingCount;
								huPaiIndex = i;
							}
						}
					}

					OutCardResult.cbOperateCode = 0;
					OutCardResult.cbOperateCard = tingData.cbOutCardData[huPaiIndex];
					return true;
				}
			}
			
		}
	}
			
	/*BYTE cbActionCard = cbActionCard;*/

	//计算各种操作得分
	BYTE cbOperateCode[] = { WIK_GANG, WIK_PENG,WIK_RIGHT,WIK_CENTER,WIK_LEFT, WIK_NULL };
	int nOperateScore[] = { 0, 0, 0 ,0, 0, 0 };
	//计算原始分
	CAndroidAI AndroidAi;
	AndroidAi.SetEnjoinOutCard(cbEnjoinOutCardIndex, cbEnjoinOutCardCount);
	AndroidAi.SetCardData(byCard, byCardCount, WeaveItemArray[wMeChairId], cbWeaveCount[wMeChairId]);
	AndroidAi.Think();
	int nOrgScore = AndroidAi.GetMaxScore();
	//判断杠
	if (cbActionMask & WIK_GANG)
	{
		if (wCurrentUser == wMeChairId)
		{
			tagGangCardResult GangCardResult;
			m_GameLogic.AnalyseGangCard(cbCardIndex, WeaveItemArray[wMeChairId], cbWeaveCount[wMeChairId], GangCardResult, cbActionCard,cbCallCard);

			//寻找最高分杠牌
			BYTE cbGangCard = 0;
			int nMaxGangScore = -1;
			for (BYTE i = 0; i < GangCardResult.cbCardCount; i++)
			{
				if (cbCardIndex[m_GameLogic.SwitchToCardIndex(GangCardResult.cbCardData[i])] == 0) 

				//计算杠后得分
				AndroidAi.SetCardData(byCard, byCardCount, WeaveItemArray[wMeChairId], cbWeaveCount[wMeChairId]);
				AndroidAi.SetAction(WIK_GANG, m_GameLogic.SwitchToCardIndex(GangCardResult.cbCardData[i]), GangCardResult.GangKind);
				AndroidAi.Think();

				int nScore = AndroidAi.GetMaxScore();
				if (nScore > nMaxGangScore)
				{
					nMaxGangScore = nScore;
					cbGangCard = GangCardResult.cbCardData[i];
				}
			}
			if (cbGangCard!=0)
			{
				cbActionCard = cbGangCard;
			}
			if (cbCardIndex[m_GameLogic.SwitchToCardIndex(cbActionCard)]==1)
			{
				nMaxGangScore = 500;
			}
			nOperateScore[0] = nMaxGangScore - nOrgScore;

		}
		else
		{
			if (cbCardIndex[m_GameLogic.SwitchToCardIndex(cbActionCard)] == 0) throw 0;

			//计算杠后得分
			AndroidAi.SetCardData(byCard, byCardCount, WeaveItemArray[wMeChairId], cbWeaveCount[wMeChairId]);
			AndroidAi.SetAction(WIK_GANG, m_GameLogic.SwitchToCardIndex(cbActionCard),true);
			AndroidAi.Think();
			nOperateScore[0] = AndroidAi.GetMaxScore() - nOrgScore;
			if (cbActionCard>0x30)
			{
				nOperateScore[0] += 500;
			}
			int score = ActionAfterScore(wMeChairId, cbCardIndex, cbShengYuIndex, WeaveItemArray[wMeChairId], cbWeaveCount[wMeChairId], cbDiscardCard, cbDiscardCount, cbActionCard, WIK_GANG,true);
			nOperateScore[0] += score;
		}
	}
	//判断碰
	if (cbActionMask & WIK_PENG&&wMeChairId != wCurrentUser)
	{
		AndroidAi.SetCardData(byCard, byCardCount, WeaveItemArray[wMeChairId], cbWeaveCount[wMeChairId]);
		AndroidAi.SetAction(WIK_PENG, m_GameLogic.SwitchToCardIndex(cbActionCard));
		AndroidAi.Think();
		BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
		if (byBadlyIndex == 0xff)
		{
			byBadlyIndex = AndroidAi.GetBadlyIn2Card();
			if (byBadlyIndex == 0xff)
				byBadlyIndex = AndroidAi.GetBadlyIn3Card();
		}
		if (byBadlyIndex != 0xff)
		{
			AndroidAi.RemoveCardData(byBadlyIndex);
			AndroidAi.Think();
			nOperateScore[1] = AndroidAi.GetMaxScore() - nOrgScore;
		}
		int score = ActionAfterScore(wMeChairId, cbCardIndex, cbShengYuIndex, WeaveItemArray[wMeChairId], cbWeaveCount[wMeChairId], cbDiscardCard, cbDiscardCount, cbActionCard, WIK_PENG,false);
		nOperateScore[1] += score;
	}
	//判断吃
	if ((cbActionMask & (WIK_LEFT|WIK_CENTER|WIK_RIGHT))&&wMeChairId != wCurrentUser)
	{
		int tmpOperate[] = { WIK_RIGHT,WIK_CENTER, WIK_LEFT };
		for (int i = 0; i < CountArray(tmpOperate);i++)
		{
			AndroidAi.SetCardData(byCard, byCardCount, WeaveItemArray[wMeChairId], cbWeaveCount[wMeChairId]);
			if (AndroidAi.SetAction(tmpOperate[i], m_GameLogic.SwitchToCardIndex(cbActionCard))==false)
			{
				continue;
			}
			AndroidAi.Think();
			BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
			if (byBadlyIndex == 0xff)
			{
				byBadlyIndex = AndroidAi.GetBadlyIn2Card();
				if (byBadlyIndex == 0xff)
					byBadlyIndex = AndroidAi.GetBadlyIn3Card();
			}
			if (byBadlyIndex != 0xff)
			{
				AndroidAi.RemoveCardData(byBadlyIndex);
				AndroidAi.Think();
				nOperateScore[2+i] = AndroidAi.GetMaxScore() - nOrgScore;
			}
			int score = 0;
			bool noCheckErXiangTing = false;
			if (nOperateScore[2 + i]<=-1)
			{
				noCheckErXiangTing = true;
			}
			ActionAfterScore(wMeChairId, cbCardIndex, cbShengYuIndex, WeaveItemArray[wMeChairId], cbWeaveCount[wMeChairId], cbDiscardCard, cbDiscardCount, cbActionCard, tmpOperate[i], false, noCheckErXiangTing);
			nOperateScore[2 + i] += score;
		}
		
	}
	//搜索废牌
	BYTE cbOutCardData = cbActionCard;
	if (wCurrentUser == wMeChairId)
	{

		AndroidAi.SetCardData(byCard, byCardCount, WeaveItemArray[wMeChairId], cbWeaveCount[wMeChairId]);
		AndroidAi.Think();
		BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
		if (byBadlyIndex == 0xff)
		{
			byBadlyIndex = AndroidAi.GetBadlyIn2Card();
			if (byBadlyIndex == 0xff)
			{
				byBadlyIndex = AndroidAi.GetBadlyIn3Card();
				ASSERT(byBadlyIndex != 0xff);
				if (byBadlyIndex == 0xff) throw 0;
			}
		}
		AndroidAi.RemoveCardData(byBadlyIndex);
		AndroidAi.Think();
		nOperateScore[2] = AndroidAi.GetMaxScore() - nOrgScore;
		cbOutCardData = m_GameLogic.SwitchToCardData(byBadlyIndex);

		BYTE tempCard = m_GameLogic.GetCallCard(cbCardIndex, cbCallCard);
		if (tempCard != 0)
		{
			OutCardResult.cbOperateCode = WIK_NULL;
			OutCardResult.cbOperateCard = 0;
			cbOutCardData = tempCard;
		}

	}

	//获取最高分操作
	BYTE cbIndex = 0;
	for (BYTE i = 1; i < CountArray(nOperateScore); i++)
	{
		if (nOperateScore[cbIndex] < nOperateScore[i])
			cbIndex = i;
	}
	if (((cbOperateCode[cbIndex] & cbActionMask)||(cbOperateCode[cbIndex]==WIK_LEFT|| cbOperateCode[cbIndex] == WIK_CENTER|| cbOperateCode[cbIndex] == WIK_RIGHT)) &&
		(nOperateScore[cbIndex] > 0))
	{
		OutCardResult.cbOperateCode = cbOperateCode[cbIndex];
		OutCardResult.cbOperateCard = cbActionCard;
		return true;
	}
	else
	{
		
		if (wCurrentUser == wMeChairId)
		{
			if (OutCardResult.cbOperateCode == WIK_LISTEN&&OutCardResult.cbOperateCard>0)
			{
				OutCardResult.cbOperateCode = WIK_NULL;
				return true;
			}
			OutCardResult.cbOperateCode = WIK_NULL;
			OutCardResult.cbOperateCard = cbOutCardData;
			return true;
		}
		else return false;
	}
	ASSERT(FALSE);
	return false;
}

int  CAndroidAI::ActionAfterScore(WORD wMeChairId, BYTE * cbCardIndex, BYTE * cbShengYuIndex, tagWeaveItem * WeaveItemArray, BYTE  cbWeaveCount, BYTE * cbDiscardCard, BYTE cbDiscardCount, BYTE cbActionCard, int  actionType,  bool bJianAction, bool noCheckErXinagTing)
{
	if (bJianAction)
	{
		cbCardIndex[m_GameLogic.SwitchToCardIndex(cbActionCard)]--;
	}
	int ErXiangCount = 0;
	int totalNum = SearchTingTotalCount(wMeChairId, cbCardIndex, cbShengYuIndex, WeaveItemArray, cbWeaveCount, cbDiscardCard, cbDiscardCount,false, ErXiangCount);
	if (bJianAction)
	{
		cbCardIndex[m_GameLogic.SwitchToCardIndex(cbActionCard)]++;
	}
	int score = 0;
	BYTE cbTmpCardIndex[MAX_INDEX] = { 0 };
	CopyMemory(cbTmpCardIndex, cbCardIndex, sizeof(cbTmpCardIndex));
	bool bChi = false;
	if (actionType&WIK_GANG)
	{
		cbTmpCardIndex[m_GameLogic.SwitchToCardIndex(cbActionCard)] = 0;
	}
	else if (actionType&WIK_PENG)
	{
		cbTmpCardIndex[m_GameLogic.SwitchToCardIndex(cbActionCard)]--;
		cbTmpCardIndex[m_GameLogic.SwitchToCardIndex(cbActionCard)]--;
	}
   else if (actionType&WIK_LEFT)
	{
		cbTmpCardIndex[m_GameLogic.SwitchToCardIndex(cbActionCard+1)]--;
		cbTmpCardIndex[m_GameLogic.SwitchToCardIndex(cbActionCard+2)]--;
		bChi = true;
	}
	else if (actionType&WIK_CENTER)
	{
		cbTmpCardIndex[m_GameLogic.SwitchToCardIndex(cbActionCard-1)]--;
		cbTmpCardIndex[m_GameLogic.SwitchToCardIndex(cbActionCard+1)]--;
		bChi = true;
	}
	else if (actionType&WIK_RIGHT)
	{
		cbTmpCardIndex[m_GameLogic.SwitchToCardIndex(cbActionCard-2)]--;
		cbTmpCardIndex[m_GameLogic.SwitchToCardIndex(cbActionCard-1)]--;
		bChi = true;
	}
	
	int tmpWeaveCount = cbWeaveCount;
	tagWeaveItem tmpWeaveItemArray[MAX_WEAVE] = {};
	CopyMemory(tmpWeaveItemArray, WeaveItemArray, sizeof(tmpWeaveItemArray));

	tmpWeaveItemArray[tmpWeaveCount].cbCenterCard = cbActionCard;
	tmpWeaveItemArray[tmpWeaveCount].cbPublicCard = cbActionCard;
	tmpWeaveItemArray[tmpWeaveCount].cbWeaveKind = actionType;
	tmpWeaveItemArray[tmpWeaveCount].wProvideUser = 1;
	tmpWeaveCount++;
	ErXiangCount = 0;
	bool bCheckErXiang = false;
	if (noCheckErXinagTing==false&&totalNum == 0)
	{
		bCheckErXiang = true;
	}
	int totalNum2 = SearchTingTotalCount(wMeChairId, cbTmpCardIndex, cbShengYuIndex, tmpWeaveItemArray, tmpWeaveCount, cbDiscardCard, cbDiscardCount, bCheckErXiang, ErXiangCount);
	if (ErXiangCount)
	{
		score = totalNum2*5;
	}
	else if (totalNum < totalNum2)
	{
		score = 500;
	}
	else if ((actionType == WIK_GANG ) && totalNum == totalNum2)
	{
		score = 500;
	}
	else{
		score = -500;
	}
	if (totalNum==0&&totalNum2==0)
	{
		score = 0;
	}
	return score;
}

void CAndroidAI::GetTingData(const BYTE cbCardIndex[MAX_INDEX], const BYTE cbShengYuCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, CMD_S_TING_DATA &TingData)
{
	ZeroMemory(&TingData, sizeof(TingData));

	m_GameLogic.GetTingDataEx(cbCardIndex, WeaveItem, cbWeaveCount, TingData.cbOutCardCount, TingData.cbOutCardData, TingData.cbHuCardCount, TingData.cbHuCardData, TingData.cbHuFan);
	for (int i = 0; i < MAX_COUNT; i++)
	{
		if (TingData.cbHuCardCount[i] > 0)
		{
			for (int j = 0; j < TingData.cbHuCardCount[i]; j++)
			{
				int index = m_GameLogic.SwitchToCardIndex(TingData.cbHuCardData[i][j]);
				TingData.cbHuCardRemainingCount[i][j] = cbShengYuCardIndex[index];
			}
		}
		else
			break;
	}
}

int CAndroidAI::GetErXiangTingData(const BYTE cbCardIndex[MAX_INDEX], const BYTE cbShengYuCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, CMD_S_ER_XING_TING_DATA &TingData)
{
	ZeroMemory(&TingData, sizeof(TingData));

	m_GameLogic.GetErXiangTingDataEx(cbCardIndex, WeaveItem, cbWeaveCount, TingData.cbOutCardCount, TingData.cbOutCardData, TingData.cbHuCardCount, TingData.cbHuCardData, TingData.cbHuFan);
	for (int i = 0; i < MAX_COUNT; i++)
	{
		if (TingData.cbHuCardCount[i] > 0)
		{
			for (int j = 0; j < TingData.cbHuCardCount[i]; j++)
			{
				int index = m_GameLogic.SwitchToCardIndex(TingData.cbHuCardData[i][j]);
				TingData.cbHuCardRemainingCount[i][j] = cbShengYuCardIndex[index];
			}
		}
		else
			break;
	}
	return TingData.cbOutCardCount;
}

void CAndroidAI::GetRemainingCount(tagWeaveItem WeaveItemArray[GAME_PLAYER][MAX_WEAVE], BYTE cbWeaveCount[], const BYTE cbCardIndex[MAX_INDEX], BYTE cbShengYuIndex[MAX_INDEX], BYTE cbDiscardCard[], BYTE cbDiscardCount)
{
	BYTE cbCardDataTemp[MAX_REPERTORY] = { 0 };
	BYTE cbCardCount = 0;
	cbCardCount = m_GameLogic.SwitchToCardData(cbCardIndex, cbCardDataTemp);
	CopyMemory(cbCardDataTemp + cbCardCount, cbDiscardCard, cbDiscardCount);
	cbCardCount += cbDiscardCount;
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		for (int j = 0; j < cbWeaveCount[i]; j++)
		{
			if (WeaveItemArray[i][j].cbWeaveKind == WIK_LEFT)
			{
				cbCardDataTemp[cbCardCount++] = WeaveItemArray[i][j].cbCenterCard;
				cbCardDataTemp[cbCardCount++] = WeaveItemArray[i][j].cbCenterCard + 1;
				cbCardDataTemp[cbCardCount++] = WeaveItemArray[i][j].cbCenterCard + 2;
			}
			else if (WeaveItemArray[i][j].cbWeaveKind == WIK_CENTER)
			{
				cbCardDataTemp[cbCardCount++] = WeaveItemArray[i][j].cbCenterCard;
				cbCardDataTemp[cbCardCount++] = WeaveItemArray[i][j].cbCenterCard - 1;
				cbCardDataTemp[cbCardCount++] = WeaveItemArray[i][j].cbCenterCard + 1;
			}
			else if (WeaveItemArray[i][j].cbWeaveKind == WIK_RIGHT)
			{
				cbCardDataTemp[cbCardCount++] = WeaveItemArray[i][j].cbCenterCard;
				cbCardDataTemp[cbCardCount++] = WeaveItemArray[i][j].cbCenterCard - 1;
				cbCardDataTemp[cbCardCount++] = WeaveItemArray[i][j].cbCenterCard - 2;
			}
			else if (WeaveItemArray[i][j].cbWeaveKind == WIK_PENG)
			{
				cbCardDataTemp[cbCardCount++] = WeaveItemArray[i][j].cbCenterCard;
				cbCardDataTemp[cbCardCount++] = WeaveItemArray[i][j].cbCenterCard;
				cbCardDataTemp[cbCardCount++] = WeaveItemArray[i][j].cbCenterCard;
			}
			else if (WeaveItemArray[i][j].cbWeaveKind == WIK_GANG)
			{
				cbCardDataTemp[cbCardCount++] = WeaveItemArray[i][j].cbCenterCard;
				cbCardDataTemp[cbCardCount++] = WeaveItemArray[i][j].cbCenterCard;
				cbCardDataTemp[cbCardCount++] = WeaveItemArray[i][j].cbCenterCard;
				cbCardDataTemp[cbCardCount++] = WeaveItemArray[i][j].cbCenterCard;
			}
		}
	}
	m_GameLogic.GetACardShengYuCount(cbCardDataTemp, cbCardCount, cbShengYuIndex);
}

int CAndroidAI::SearchTingTotalCount(WORD wMeChairId, BYTE cbCardIndex[], BYTE cbShengYuIndex[MAX_INDEX], tagWeaveItem WeaveItemArray[MAX_WEAVE], BYTE cbWeaveCount, BYTE cbDiscardCard[], BYTE cbDiscardCount, bool isErXingCheck, int &ErXiangCount)
{
	int totalNum = 0;
	BYTE cbRes = m_GameLogic.AnalyseTingCard(cbCardIndex, WeaveItemArray, cbWeaveCount);
	if (cbRes == WIK_LISTEN)
	{
		CMD_S_TING_DATA tingData;
		GetTingData(cbCardIndex, cbShengYuIndex, WeaveItemArray, cbWeaveCount, tingData);
		int huPaiIndex = 0;
		int maxRemainingCount = 0;
		int maxFanCount = 0;
		int cardScore[MAX_COUNT] = { 0 };
		int nOrgScore = 0;
		
		for (int i = 0; i < MAX_COUNT; i++)
		{
			if (tingData.cbHuCardCount[i] > 0)
			{
				int maxRemainCount = 0;
				for (int j = 0; j < tingData.cbHuCardCount[i]; j++)
				{
					maxRemainCount += tingData.cbHuCardRemainingCount[i][j];
				}
				if (maxRemainCount>totalNum)
				{
					totalNum = maxRemainCount;
				}
			}
			else
				break;
		}
	}
	else if(isErXingCheck)
	{
		CMD_S_ER_XING_TING_DATA tingData;
		int resultCount = GetErXiangTingData(cbCardIndex, cbShengYuIndex, WeaveItemArray, cbWeaveCount, tingData);
		if (resultCount > 0)
		{
			int huPaiIndex = 0;
			int maxRemainingCount = 0;
			int maxFanCount = 0;
			int cardScore[MAX_COUNT] = { 0 };
			int nOrgScore = 0;
			for (int i = 0; i < MAX_COUNT; i++)
			{
				if (i == tingData.cbOutCardCount)
				{
					break;;
				}
				int cbHuCardRemainingCount = 0;
				for (int j = 0; j < tingData.cbHuCardCount[i]; j++)
				{
					cbHuCardRemainingCount =
						tingData.cbHuCardRemainingCount[i][j] + cbHuCardRemainingCount;
				}

				if (cbHuCardRemainingCount > maxRemainingCount)
				{
					maxRemainingCount = cbHuCardRemainingCount;
					huPaiIndex = i;
				}
				else if (cbHuCardRemainingCount == maxRemainingCount)
				{
					if (tingData.cbOutCardData[i] > 0x30)
					{
						maxRemainingCount = cbHuCardRemainingCount;
						huPaiIndex = i;
					}
				}
			}
			totalNum = maxRemainingCount;
			ErXiangCount = maxRemainingCount;
			//OutCardResult.cbOperateCode = 0;
			//OutCardResult.cbOperateCard = tingData.cbOutCardData[huPaiIndex];
		}
	}
	return totalNum;
}

/////////////////////////////////////////////////////////////////////////////////////
