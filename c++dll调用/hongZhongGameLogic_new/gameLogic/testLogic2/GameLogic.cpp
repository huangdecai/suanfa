#include "StdAfx.h"
#include "GameLogic.h"
#include "math.h"

//////////////////////////////////////////////////////////////////////////

map<uint64, string> FanString =
{
	{CHR_YI_BAN_GAO ,"一般高 1番"},{CHR_LIAN_LIU ,"连六 1番"},{CHR_LAO_SHAO_FU ,"老少副 1番"},
	{CHR_YAO_JIU_KE ,"幺九刻 1番"},{CHR_SHI_SAN_YAO ,"十三幺 1番"},{CHR_BIAN_ZHANG ,"边张 1番"},{CHR_KAN_ZHANG ,"坎张 1番"},{CHR_DAN_DIAO_JIANG ,"单钓将 1番"},
	{CHR_ZI_MO ,"自摸 1番"},{CHR_ER_WU_BA_JIANG ,"二五八将 1番"},{CHR_YAO_JIU_TOU ,"幺九头 1番"},
	{CHR_JIAN_KE ,"箭刻 2番"},{CHR_MEN_QIANG_QING ,"门前清 2番"},{CHR_PING_HU ,"平胡 2番"},{CHR_SI_GUI_YI ,"四归一 2番"},{CHR_SHUANG_AN_KE ,"双暗刻 2番"},
	{CHR_AN_GANG,"暗杠 2番"},{CHR_DUAN_YAO ,"断幺 2番"},{CHR_TING_PAI ,"听牌 2番"},
	{CHR_QUANG_DAI_YAO ,"全带幺 4番"},{CHR_BU_QIU_REN ,"不求人 4番"},{CHR_SHUANG_MING_GANG ,"双明杠 4番"},{CHR_HU_JUE_ZHANG ,"胡绝张 4番"},
	{CHR_PENG_PENG ,"碰碰胡 6番"},{CHR_HUN_YI_SE ,"混一色 6番"},{CHR_QUAN_QIU_REN ,"全求人 6番"},{CHR_SHUANG_AN_GANG ,"双暗杠 6番"},{CHR_SHUANG_JIAN_KE ,"双箭刻 6番"},
	{CHR_MIAO_SHOU_HUI_CHUN ,"妙手回春 8番"},{CHR_HAI_DI_LAO_YUE ,"海底捞月 8番"},{CHR_GANG_SHANG_HUA ,"杠上开花 8番"},{CHR_QIANG_GANG_HU ,"抢杠胡 8番"},
	{CHR_QI_DA_DUI ,"七大对 12番"},{CHR_SHUANG_QI_DA_DUI ,"双七大对 12番"},{CHR_SAN_FENG_KE ,"三风刻 12番"},
	{CHR_TIAN_TING,"天听 16番"},{CHR_QING_LONG ,"清龙 16番"},{CHR_YI_SE_SAN_BU_GAO ,"一色三步高 16番"},{CHR_SAN_AN_KE ,"三暗刻 16番"},
	{CHR_QI_DUI ,"七对 24番"},{CHR_QING_YI_SE ,"清一色 24番"},{CHR_YI_SE_SAN_TONG_SHUN ,"一色三同顺 24番"},{CHR_YI_SE_SAN_JIE_GAO ,"一色三节高 24番"},{CHR_SAN_QI_DA_DUI ,"三七大对 24番"},
	{CHR_YI_SE_SI_BU_GAO ,"一色四步高 32番"},{CHR_SAN_GANG ,"三杠 32番"},{CHR_HUN_YAO_JIU ,"混幺九 32番"},{CHR_DI_HU ,"地胡 32番"},
	{CHR_TIAN_HU,"天胡 48番"},{CHR_YI_SE_SI_TONG_SHUN ,"一色四同顺 48番"},{CHR_YI_SE_SI_JIE_GAO ,"一色四节高 48番"},
	{CHR_XIAO_SI_XI ,"小四喜 64番"},{CHR_XIAO_SAN_YUAN ,"小三元 64番"},{CHR_ZI_YI_SE ,"字一色 64番"},{CHR_YI_SS_LONG_HUI ,"一色双龙会 64番"},{CHR_SI_AN_KE ,"四暗刻 64番"},
	{CHR_DA_SI_XI ,"大四喜 88番"},{CHR_DA_SAN_YUAN ,"大三元 88番"},{CHR_JIU_LIAN_BAO_DENG ,"九连宝灯 88番"},{CHR_SI_GANG ,"四杠 88番"},{CHR_QI_LIAN_DUI ,"连七对 88番"}
};
//静态变量
bool		CChiHuRight::m_bInit = false;
uint64		CChiHuRight::m_dwRightMask[MAX_RIGHT_COUNT];

//构造函数
CChiHuRight::CChiHuRight()
{
	ZeroMemory(m_dwRight,sizeof(m_dwRight));

	if(!m_bInit)
	{
		m_bInit = true;
		for(BYTE i = 0; i < CountArray(m_dwRightMask); i++)
		{
			if(0 == i)
				m_dwRightMask[i] = 0;
			else
				m_dwRightMask[i] = (uint64(pow(2,i-1)))<<28;
		}
	}
}

//赋值符重载
CChiHuRight & CChiHuRight::operator = (uint64 dwRight)
{
	uint64 dwOtherRight = 0;
	//验证权位
	if(!IsValidRight(dwRight))
	{
		//验证取反权位
		ASSERT(IsValidRight(~dwRight));
		if(!IsValidRight(~dwRight)) return *this;
		dwRight = ~dwRight;
		dwOtherRight = MASK_CHI_HU_RIGHT;
	}

	for(BYTE i = 0; i < CountArray(m_dwRightMask); i++)
	{
		if((dwRight&m_dwRightMask[i]) || (i==0&&dwRight<= 0x8000000000000000))
			m_dwRight[i] = dwRight&MASK_CHI_HU_RIGHT;
		else m_dwRight[i] = dwOtherRight;
	}

	return *this;
}

//与等于
CChiHuRight & CChiHuRight::operator &= (uint64 dwRight)
{
	bool bNavigate = false;
	//验证权位
	if(!IsValidRight(dwRight))
	{
		//验证取反权位
		ASSERT(IsValidRight(~dwRight));
		if(!IsValidRight(~dwRight)) return *this;
		//调整权位                        0x1000000000000000
		uint64 dwHeadRight = (~dwRight)&0xF000000000000000;
		uint64 dwTailRight = dwRight&MASK_CHI_HU_RIGHT;
		dwRight = dwHeadRight|dwTailRight;
		bNavigate = true;
	}

	for(BYTE i = 0; i < CountArray(m_dwRightMask); i++)
	{              //                                    0x1000000000000000
		if((dwRight&m_dwRightMask[i]) || (i==0&&dwRight<= 0x8000000000000000))
		{
			m_dwRight[i] &= (dwRight&MASK_CHI_HU_RIGHT);
		}
		else if(!bNavigate)
			m_dwRight[i] = 0;
	}

	return *this;
}

//或等于
CChiHuRight & CChiHuRight::operator |= (uint64 dwRight)
{
	//验证权位
	if(!IsValidRight(dwRight)) return *this;

	for(BYTE i = 0; i < CountArray(m_dwRightMask); i++)
	{
		if((dwRight&m_dwRightMask[i]) || (i==0&&dwRight<= 0x8000000000000000))
		{
			m_dwRight[i] |= (dwRight&MASK_CHI_HU_RIGHT);
			break;
		}
	}

	return *this;
}

//与
CChiHuRight CChiHuRight::operator & (uint64 dwRight)
{
	CChiHuRight chr = *this;
	return (chr &= dwRight);
}

//与
CChiHuRight CChiHuRight::operator & (uint64 dwRight) const
{
	CChiHuRight chr = *this;
	return (chr &= dwRight);
}

//或
CChiHuRight CChiHuRight::operator | (uint64 dwRight)
{
	CChiHuRight chr = *this;
	return chr |= dwRight;
}

//或
CChiHuRight CChiHuRight::operator | (uint64 dwRight) const
{
	CChiHuRight chr = *this;
	return chr |= dwRight;
}

//相等
bool CChiHuRight::operator == (uint64 dwRight) const
{
	CChiHuRight chr;
	chr = dwRight;
	return (*this)==chr;
}

//相等
bool CChiHuRight::operator == (const CChiHuRight chr) const
{
	for(WORD i = 0; i < CountArray(m_dwRight); i++)
	{
		if(m_dwRight[i] != chr.m_dwRight[i]) return false;
	}
	return true;
}

//不相等
bool CChiHuRight::operator != (uint64 dwRight) const
{
	CChiHuRight chr;
	chr = dwRight;
	return (*this)!=chr;
}

//不相等
bool CChiHuRight::operator != (const CChiHuRight chr) const
{
	return !((*this)==chr);
}

//是否权位为空
bool CChiHuRight::IsEmpty()
{
	for(BYTE i = 0; i < CountArray(m_dwRight); i++)
		if(m_dwRight[i]) return false;
	return true;
}

//设置权位为空
void CChiHuRight::SetEmpty()
{
	ZeroMemory(m_dwRight,sizeof(m_dwRight));
	return;
}

//获取权位数值
int CChiHuRight::GetRightData(uint64 dwRight[], BYTE cbMaxCount)
{
	ASSERT(cbMaxCount >= CountArray(m_dwRight));
	if(cbMaxCount < CountArray(m_dwRight)) return 0;

	CopyMemory(dwRight,m_dwRight,sizeof(uint64)*CountArray(m_dwRight));
	return CountArray(m_dwRight);
}

//设置权位数值
bool CChiHuRight::SetRightData(const uint64 dwRight[], BYTE cbRightCount)
{
	ASSERT(cbRightCount <= CountArray(m_dwRight));
	if(cbRightCount > CountArray(m_dwRight)) return false;

	ZeroMemory(m_dwRight,sizeof(m_dwRight));
	CopyMemory(m_dwRight,dwRight,sizeof(uint64)*cbRightCount);

	return true;
}

//检查仅位是否正确
bool CChiHuRight::IsValidRight(uint64 dwRight)
{
	uint64 dwRightHead = dwRight & 0x0000000000000000;
	for(BYTE i = 0; i < CountArray(m_dwRightMask); i++)
		if(m_dwRightMask[i] == dwRightHead) return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////

//静态变量

//麻将数据
const BYTE CGameLogic::m_cbCardDataArray[MAX_REPERTORY]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
	0x31,0x31,0x31,0x31,																//东风
	0x32,0x32,0x32,0x32,																//南风
	0x33,0x33,0x33,0x33,																//西风
	0x34,0x34,0x34,0x34,																//北风
	0x35,0x35,0x35,0x35,																//红中
	0x36,0x36,0x36,0x36,																	//发财
	0x37, 0x37, 0x37, 0x37,																//白板
	//0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F                       //春夏秋冬
};

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
	m_cbMagicIndex[0] = MAX_INDEX;
	m_cbMagicIndex[1] = MAX_INDEX;
	ZeroMemory(m_userRule, sizeof(m_userRule));
	m_cbMagicIndex[0] = SwitchToCardIndex(0x35);
}

//析构函数
CGameLogic::~CGameLogic()
{
}

//混乱扑克
VOID CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//混乱准备
	BYTE cbCardDataTemp[CountArray(m_cbCardDataArray)];
	CopyMemory(cbCardDataTemp,m_cbCardDataArray,sizeof(m_cbCardDataArray));

	//混乱扑克
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(cbBufferCount-cbRandCount);
		cbCardBuffer[cbRandCount++]=cbCardDataTemp[cbPosition];
		cbCardDataTemp[cbPosition]=cbCardDataTemp[cbBufferCount-cbRandCount];
	} while (cbRandCount<cbBufferCount);

	return;
}

//混乱扑克
VOID CGameLogic::RandCardList(BYTE cbCardData[], BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//混乱准备
	BYTE cbCardDataTemp[MAX_COUNT] = {0};
	CopyMemory(cbCardDataTemp,cbCardData,sizeof(BYTE) * cbBufferCount);

	//混乱扑克
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(cbBufferCount-cbRandCount);
		cbCardBuffer[cbRandCount++]=cbCardDataTemp[cbPosition];
		cbCardDataTemp[cbPosition]=cbCardDataTemp[cbBufferCount-cbRandCount];
	} while (cbRandCount<cbBufferCount);

	return;
}

VOID CGameLogic::JianYiSeCardList(BYTE cbCardData[], BYTE cardIndex)
{
	int replaceIndex = MAX_REPERTORY - 1;
	int start = MAX_REPERTORY - 4*9-1;

	for (int i = start; i>=0; i--)
	{
		if (GetCardColor(cbCardData[i]) == cardIndex)
		{
			for (int j = replaceIndex; j >= start+1 ; j--)
			{
				
				if (GetCardColor(cbCardData[j]) != cardIndex)
				{
					replaceIndex = j;
					BYTE tempCard = cbCardData[j];
					cbCardData[j] = cbCardData[i];
					cbCardData[i] = tempCard;
					break;
				}
			}
		}
	}
}

//删除扑克
bool CGameLogic::RemoveCard(BYTE cbCardIndex[MAX_INDEX], const BYTE cbRemoveCard[], BYTE cbRemoveCount)
{
	//删除扑克
	for (BYTE i=0;i<cbRemoveCount;i++)
	{
		//效验扑克
		ASSERT(IsValidCard(cbRemoveCard[i]));
		ASSERT(cbCardIndex[SwitchToCardIndex(cbRemoveCard[i])]>0);

		//删除扑克
		BYTE cbRemoveIndex=SwitchToCardIndex(cbRemoveCard[i]);
		if (cbCardIndex[cbRemoveIndex]==0)
		{
			//错误断言
			ASSERT(FALSE);

			//还原删除
			for (BYTE j=0;j<i;j++) 
			{
				ASSERT(IsValidCard(cbRemoveCard[j]));
				cbCardIndex[SwitchToCardIndex(cbRemoveCard[j])]++;
			}

			return false;
		}
		else 
		{
			//删除扑克
			--cbCardIndex[cbRemoveIndex];
		}
	}

	return true;
}

//删除扑克
bool CGameLogic::RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard)
{
	//效验扑克
	ASSERT(IsValidCard(cbRemoveCard));
	ASSERT(cbCardIndex[SwitchToCardIndex(cbRemoveCard)] > 0);

	//删除扑克
	BYTE cbRemoveIndex = SwitchToCardIndex(cbRemoveCard);
	if (cbCardIndex[cbRemoveIndex] > 0)
	{
		cbCardIndex[cbRemoveIndex]--;
		return true;
	}

	//失败效验
	ASSERT(FALSE);

	return false;
}
//设置财神
void CGameLogic::SetMagicIndex( BYTE cbMagicIndex[] ) 
{ 
	if(cbMagicIndex != NULL)
		CopyMemory(m_cbMagicIndex, cbMagicIndex, sizeof(m_cbMagicIndex));
	else
		m_cbMagicIndex[0]= MAX_INDEX;
	    m_cbMagicIndex[1] = MAX_INDEX;
}
//财神判断
bool CGameLogic::IsMagicCard(BYTE cbCardData)
{
	for(int i=0;i<MAX_MAGIC_COUNT;i++)
	{
		if(m_cbMagicIndex[i] != MAX_INDEX && SwitchToCardIndex(cbCardData) == m_cbMagicIndex[i])
			return true;
	}
	return false;
}
BYTE CGameLogic::GetMajicCount(const BYTE cbCardIndex[MAX_INDEX])
{
	BYTE cbCount = 0;
	for(int i=0;i<MAX_MAGIC_COUNT;i++)
	{
		if(m_cbMagicIndex[i] != MAX_INDEX)
			cbCount +=  cbCardIndex[m_cbMagicIndex[i]];
		else
			break;
	}
	return cbCount;
}
//花牌判断
bool CGameLogic::IsHuaCard(BYTE cbCardData)
{
	ASSERT(IsValidCard(cbCardData));

	return cbCardData >= 0x38;
}

//花牌判断
BYTE CGameLogic::IsHuaCard(BYTE cbCardIndex[MAX_INDEX])
{
	BYTE cbHuaCardCount = 0;
	for(int i = MAX_INDEX - MAX_HUA_COUNT; i < MAX_INDEX; i++)
	{
		if(cbCardIndex[i] > 0)
		{
			cbHuaCardCount += cbCardIndex[i];
		}
	}

	return cbHuaCardCount;
}

//排序,根据牌值排序
bool CGameLogic::SortCardList(BYTE cbCardData[MAX_COUNT], BYTE cbCardCount)
{
	//数目过虑
	if (cbCardCount==0||cbCardCount>MAX_COUNT) return false;

	//排序操作
	bool bSorted=true;
	BYTE cbSwitchData=0,cbLast=cbCardCount-1;
	BYTE cbCard1, cbCard2;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<cbLast;i++)
		{
			cbCard1 = cbCardData[i];
			cbCard2 = cbCardData[i+1];

			if (cbCard1>cbCard2)
			{
				//设置标志
				bSorted=false;

				//扑克数据
				cbSwitchData=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbSwitchData;
			}	
		}
		cbLast--;
	} while(bSorted==false);

	return true;
}

//删除扑克
bool CGameLogic::RemoveCard(BYTE cbCardData[], BYTE cbCardCount, const BYTE cbRemoveCard[], BYTE cbRemoveCount)
{
	//检验数据
	ASSERT(cbCardCount<=MAX_COUNT);
	ASSERT(cbRemoveCount<=cbCardCount);

	//定义变量
	BYTE cbDeleteCount=0,cbTempCardData[MAX_COUNT];
	if (cbCardCount>CountArray(cbTempCardData))
		return false;
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

	//成功判断
	if (cbDeleteCount!=cbRemoveCount) 
	{
		ASSERT(FALSE);
		return false;
	}

	//清理扑克
	BYTE cbCardPos=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbTempCardData[i]!=0) 
			cbCardData[cbCardPos++]=cbTempCardData[i];
	}

	return true;
}

//动作等级
BYTE CGameLogic::GetUserActionRank(BYTE cbUserAction)
{
	//胡牌等级
	if (cbUserAction&WIK_CHI_HU) { return 4; }

	//杠牌等级
	if (cbUserAction&WIK_GANG) { return 3; }

	//碰牌等级
	if (cbUserAction&WIK_PENG) { return 2; }

	//上牌等级
	if (cbUserAction&(WIK_RIGHT|WIK_CENTER|WIK_LEFT)) { return 1; }

	return 0;
}

//胡牌等级
WORD CGameLogic::GetChiHuActionRank(const CChiHuRight & ChiHuRight)
{
	return 1;
}

//自动出牌
BYTE CGameLogic::AutomatismOutCard(const BYTE cbCardIndex[MAX_INDEX], const BYTE cbEnjoinOutCard[MAX_COUNT], BYTE cbEnjoinOutCardCount)
{
	// 先打财神
	for(int i=0;i<MAX_MAGIC_COUNT;i++)
	{
		if(m_cbMagicIndex[i] != MAX_INDEX)
		{
			if(cbCardIndex[m_cbMagicIndex[i]] > 0)
			{
				return SwitchToCardData(m_cbMagicIndex[i]);
			}
		}
	}


	//而后打字牌，字牌打自己多的，数目一样就按东南西北中发白的顺序
	BYTE cbCardData = 0;
	BYTE cbOutCardIndex  = MAX_INDEX;
	BYTE cbOutCardIndexCount = 0;
	for(int i = MAX_INDEX - 7; i < MAX_INDEX - 1; i++)
	{
		if(cbCardIndex[i] > cbOutCardIndexCount)
		{
			cbOutCardIndexCount = cbCardIndex[i];
			cbOutCardIndex = i;
		}
	}

	if(cbOutCardIndex != MAX_INDEX)
	{
		cbCardData = SwitchToCardData(cbOutCardIndex);
		bool bEnjoinCard = false;
		for(int k = 0; k < cbEnjoinOutCardCount; k++)
		{
			if(cbCardData == cbEnjoinOutCard[k])
			{
				bEnjoinCard = true;
			}
		}
		if(!bEnjoinCard)
		{
			return cbCardData;
		}		
	}

	//没有字牌就打边张，1或9，顺序为万筒索，2，8，而后3，7，而后4，6，而后5
	for(int i = 0; i < 5; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			cbOutCardIndex  = MAX_INDEX;
			if(cbCardIndex[j * 9 + i] > 0)
			{
				cbOutCardIndex = j * 9 + i;
			}
			else if(cbCardIndex[j * 9 + (9 - i - 1)] > 0)
			{
				cbOutCardIndex = j * 9 + (9 - i - 1);
			}

			if(cbOutCardIndex != MAX_INDEX)
			{
				BYTE cbCardDataTemp = SwitchToCardData(cbOutCardIndex);
				bool bEnjoinCard = false;
				for(int k = 0; k < cbEnjoinOutCardCount; k++)
				{
					if(cbCardDataTemp == cbEnjoinOutCard[k])
					{
						bEnjoinCard = true;
					}
				}
				if(!bEnjoinCard)
				{
					return cbCardDataTemp;
				}
				else
				{
					if(cbCardData == 0)
					{
						cbCardData = cbCardDataTemp;
					}
				}
			}
		}
	}
	return cbCardData;
}

//吃牌判断
BYTE CGameLogic::EstimateEatCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	//参数效验
	ASSERT(IsValidCard(cbCurrentCard));

	//过滤判断
	if(IsMagicCard(cbCurrentCard)) return WIK_NULL;

	if ((G_ZI_SHUN==FALSE)&&(cbCurrentCard>0x30))
	{
		return WIK_NULL;
	}
	//变量定义
	BYTE cbExcursion[3]={0,1,2};
	BYTE cbItemKind[3]={WIK_LEFT,WIK_CENTER,WIK_RIGHT};

	//吃牌判断
	BYTE cbEatKind=0,cbFirstIndex=0;
	BYTE cbCurrentIndex=SwitchToCardIndex(cbCurrentCard);
	if (cbCurrentCard<0x30)
	{
		for (BYTE i=0;i<CountArray(cbItemKind);i++)
		{
			BYTE cbValueIndex=cbCurrentIndex%9;
			if ((cbValueIndex>=cbExcursion[i])&&((cbValueIndex-cbExcursion[i])<=6))
			{
				//吃牌判断
				cbFirstIndex=cbCurrentIndex-cbExcursion[i];

				//吃牌不能包含有财神
				if(/* m_cbMagicIndex[0] != MAX_INDEX && m_cbMagicIndex[1] != MAX_INDEX &&*/
					(m_cbMagicIndex[0] >= cbFirstIndex && m_cbMagicIndex[0] <= cbFirstIndex+2 || 
					m_cbMagicIndex[1] >= cbFirstIndex && m_cbMagicIndex[1] <= cbFirstIndex+2)) continue;

				if ((cbCurrentIndex!=cbFirstIndex)&&(cbCardIndex[cbFirstIndex]==0))
					continue;
				if ((cbCurrentIndex!=(cbFirstIndex+1))&&(cbCardIndex[cbFirstIndex+1]==0))
					continue;
				if ((cbCurrentIndex!=(cbFirstIndex+2))&&(cbCardIndex[cbFirstIndex+2]==0))
					continue;

				//设置类型
				cbEatKind|=cbItemKind[i];
			}
		}
	}
	
	//东南西北任意三个算顺子,中发白算顺子
    if (G_ZI_SHUN)
    {
		//东南西北
      if ((cbCurrentIndex>=27)&&(cbCurrentIndex<=33))
      {
		  //吃牌不能包含有财神
		  BYTE cbFirstCard=GenerateShunZi(cbCurrentCard,true,1);
		  BYTE cbSecondCard=GenerateShunZi(cbCurrentCard,true,2);
		  BYTE cbFirstIndex =SwitchToCardIndex(cbFirstCard);
		  BYTE cbSecondIndex =SwitchToCardIndex(cbSecondCard);

		  if ((cbCardIndex[cbFirstIndex]!=0)&&(cbCardIndex[cbSecondIndex]!=0)&&(IsMagicCard(cbFirstCard)==false)&&(IsMagicCard(cbSecondCard)==false))
		  {
			  if (cbCurrentCard<=0x35)
			  {
				   cbEatKind|=cbItemKind[0];
			  }
             
		  }
		 cbFirstCard=GenerateShunZi(cbCurrentCard,false,1);
		 cbSecondCard=GenerateShunZi(cbCurrentCard,true,1);
		 cbFirstIndex =SwitchToCardIndex(cbFirstCard);
		 cbSecondIndex =SwitchToCardIndex(cbSecondCard);

		  if ((cbCardIndex[cbFirstIndex]!=0)&&(cbCardIndex[cbSecondIndex]!=0)&&(IsMagicCard(cbFirstCard)==false)&&(IsMagicCard(cbSecondCard)==false))
		  {
			 
			  if ((cbCurrentCard<=0x34)||(cbCurrentCard==0x36))
			  {
				  cbEatKind|=cbItemKind[1];
			  }
		  }

		  cbFirstCard=GenerateShunZi(cbCurrentCard,false,2);
		  cbSecondCard=GenerateShunZi(cbCurrentCard,false,1);
		  cbFirstIndex =SwitchToCardIndex(cbFirstCard);
		  cbSecondIndex =SwitchToCardIndex(cbSecondCard);

		  if ((cbCardIndex[cbFirstIndex]!=0)&&(cbCardIndex[cbSecondIndex]!=0)&&(IsMagicCard(cbFirstCard)==false)&&(IsMagicCard(cbSecondCard)==false))
		  {
			  if ((cbCurrentCard<=0x34)||(cbCurrentCard==0x37))
			  {
				  cbEatKind|=cbItemKind[2];
			  }
		  }
          
      }
	

    }
	return cbEatKind;
}

//碰牌判断
BYTE CGameLogic::EstimatePengCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	//参数效验
	ASSERT(IsValidCard(cbCurrentCard));

	//过滤判断
	if (IsMagicCard(cbCurrentCard) || IsHuaCard(cbCurrentCard)) 
		return WIK_NULL;

	//碰牌判断
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)]>=2)?WIK_PENG:WIK_NULL;
}

//杠牌判断
BYTE CGameLogic::EstimateGangCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	//参数效验
	ASSERT(IsValidCard(cbCurrentCard));

	//过滤判断
	if (IsMagicCard(cbCurrentCard) || IsHuaCard(cbCurrentCard)) 
		return WIK_NULL;

	//杠牌判断
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)]==3)?WIK_GANG:WIK_NULL;
}

//杠牌分析
BYTE CGameLogic::AnalyseGangCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, tagGangCardResult & GangCardResult, BYTE cbCurrentCard, BYTE cbCallCard)
{
	//设置变量
	BYTE cbActionMask = WIK_NULL;
	ZeroMemory(&GangCardResult, sizeof(GangCardResult));

	//手上杠牌
	for (BYTE i = 0; i < MAX_INDEX; i++)
	{
		if (IsMagicCard(SwitchToCardData(i))) continue;
		if (cbCardIndex[i] == 4)
		{
			if (GetCardColor(SwitchToCardData(i)) != GetCardColor(cbCallCard))
			{
				cbActionMask |= WIK_GANG;
				GangCardResult.cbCardData[GangCardResult.cbCardCount++] = SwitchToCardData(i);
				GangCardResult.GangKind = 2;
			}

		}
	}

	//组合杠牌
	for (BYTE i = 0; i < cbWeaveCount; i++)
	{
		if (WeaveItem[i].cbWeaveKind == WIK_PENG)
		{
			if (WeaveItem[i].cbCenterCard == cbCurrentCard)
			{
				cbActionMask |= WIK_GANG;
				GangCardResult.cbCardData[GangCardResult.cbCardCount++] = WeaveItem[i].cbCenterCard;
				GangCardResult.GangKind = 0;
			}
		}
	}

	return cbActionMask;
}

//吃胡分析
BYTE CGameLogic::AnalyseChiHuCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, CChiHuRight &ChiHuRight)
{
	//变量定义
	BYTE cbChiHuKind=WIK_NULL;
	vector<tagAnalyseItem> AnalyseItemArray;

	//设置变量
	AnalyseItemArray.clear();
	ChiHuRight.SetEmpty();

	//构造扑克
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	//cbCurrentCard一定不为0			!!!!!!!!!
	ASSERT(cbCurrentCard != 0);
	if(cbCurrentCard == 0) return WIK_NULL;

	//插入扑克
		if (cbCurrentCard != 0)
			cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]++;

	//分析扑克
	//AnalyseCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,AnalyseItemArray);
	CombinatoryAnalysisEx(cbCardIndexTemp, AnalyseItemArray, WeaveItem, cbWeaveCount);
	CChiHuRight MaxChr;
	CChiHuRight chr;
	int Maxfan = 0;
	//胡牌分析
	if (AnalyseItemArray.size()>0)
	{
		//牌型分析
		for (int i = 0; i<AnalyseItemArray.size(); i++)
 		{
			tagAnalyseItem * pAnalyseItem=&AnalyseItemArray[i];
			bool bLianCard=false;
			bool bPengCard=false;
			BYTE cbGangCount = 0;
			//牌型分析
			for (BYTE j=0;j<CountArray(pAnalyseItem->cbWeaveKind);j++)
			{
				BYTE cbWeaveKind=pAnalyseItem->cbWeaveKind[j];
				bPengCard=((cbWeaveKind&(WIK_GANG|WIK_PENG))!=0) ? true : bPengCard;
				bLianCard=((cbWeaveKind&(WIK_LEFT|WIK_CENTER|WIK_RIGHT)) !=0 ) ? true : bLianCard;
				if(cbWeaveKind == WIK_GANG) cbGangCount++;
			}
			chr |= CHR_PING_HU;
			//if (IsYiBanGao(pAnalyseItem) == true)
			//{
			//	chr = chr | CHR_YI_BAN_GAO;
			//}
			////-- 2番
			if (m_userRule[6]&&IsPengPeng(pAnalyseItem) == true)
			{
				chr = chr | CHR_PENG_PENG;
			}
			if (m_userRule[7]&&IsQingYiSe(cbCardIndexTemp, WeaveItem, cbWeaveCount))
			{
				chr = chr | CHR_QING_YI_SE;
			}
			//if (IsHunYiSe(cbCardIndexTemp, WeaveItem, cbWeaveCount))
			//{
			//	chr = chr | CHR_HUN_YI_SE;
			//}
			//if (IsZiYiSe(pAnalyseItem) == true)
			//{
			//	chr = chr | CHR_ZI_YI_SE;
			//}
			// if (IsYaoJiu(pAnalyseItem) == true)
			//{
			//	chr = chr | CHR_HUN_YAO_JIU;
			//}

			//if (IsShuangAnKe(pAnalyseItem, WeaveItem, cbWeaveCount, cbCurrentCard, false) == true)
			//{
			//	chr = chr | CHR_SHUANG_AN_KE;
			//}
			//if (IsBianZhang(pAnalyseItem, WeaveItem, cbWeaveCount, cbCurrentCard) == true)
			//{
			//	chr = chr | CHR_BIAN_ZHANG;
			//}
			//if (IsKanZhang(pAnalyseItem, cbCardIndexTemp, WeaveItem, cbWeaveCount, cbCurrentCard) == true)
			//{
			//	chr = chr | CHR_KAN_ZHANG;
			//}
			//if (IsDanDiaoJiang(pAnalyseItem, cbCardIndexTemp, cbCurrentCard) == true)
			//{
			//	chr = chr | CHR_DAN_DIAO_JIANG;
			//}
			//
			//if (IsDaSiXi(pAnalyseItem) == true)
			//{
			//	chr = chr | CHR_DA_SI_XI;
			//}
			//		
			//if (IsDaSanYuan(pAnalyseItem) == true)
			//{
			//	chr = chr | CHR_DA_SAN_YUAN;
			//}
			//if (IsDuanYao(pAnalyseItem) == true)
			//{
			//	chr = chr | CHR_DUAN_YAO;
			//}
			//chr = ClearRepeateFan(chr);
			int fan = GetUserHuFan(chr);

			string tempStr = GetFanString(chr);
			if (fan > Maxfan)
			{
				MaxChr = chr;
				Maxfan = fan;
			}
  		}

		
	}
	
	chr = MaxChr;
	if (m_userRule[8] && IsQiDui(cbCardIndexTemp, cbWeaveCount))
	{
		chr |= CHR_QI_DUI;
		if (IsQingYiSe(cbCardIndexTemp, WeaveItem, cbWeaveCount) == true)
		{
			chr = chr | CHR_QING_YI_SE;
		}
		
		 if (IsQiDaDui(cbCardIndexTemp, cbWeaveCount))
		{
			chr = chr | CHR_QI_DA_DUI;
		}

	}

	//////十三幺
	if (m_userRule[5]&&IsShiSanYao(cbCardIndexTemp, cbWeaveCount))
		chr |= CHR_SHI_SAN_YAO;

	if (!chr.IsEmpty())
	{
		cbChiHuKind = WIK_CHI_HU;
		ChiHuRight = chr;
	}


	return cbChiHuKind;
}

//听牌分析
BYTE CGameLogic::AnalyseTingCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, bool bNoCheckMagic)
{
	//复制数据
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));

	BYTE cbCardCount = GetCardCount(cbCardIndexTemp);
	CChiHuRight chr;

	if((cbCardCount + 1) % 3 == 0)
	{
		for(BYTE i = 0; i < MAX_INDEX - MAX_HUA_COUNT; i++)
		{
			if(cbCardIndexTemp[i] == 0) continue;
			cbCardIndexTemp[i]--;

			for(BYTE j = 0; j < MAX_INDEX-MAX_HUA_COUNT; j++)
			{
				if (bNoCheckMagic && ((j  == m_cbMagicIndex[0]) || (j  == m_cbMagicIndex[1]) ))
				{
					continue;
				}
				BYTE cbCurrentCard = SwitchToCardData(j);
				if(WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp, WeaveItem, cbWeaveCount, cbCurrentCard, chr))
					return WIK_LISTEN;
			}

			cbCardIndexTemp[i]++;
		}
	}
	else
	{
		for(BYTE j = 0; j < MAX_INDEX-MAX_HUA_COUNT; j++)
		{
			if (bNoCheckMagic && ((j == m_cbMagicIndex[0]) || (j == m_cbMagicIndex[1])))
			{
				continue;
			}
			BYTE cbCurrentCard = SwitchToCardData(j);
			if(WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,cbCurrentCard,chr))
				return WIK_LISTEN;
		}
	}

	return WIK_NULL;
}

//听牌分析
BYTE CGameLogic::GetTingData(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount,BYTE& cbOutCardCount, BYTE cbOutCardData[])
{
	//复制数据
	BYTE cbOutCount = 0;
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	BYTE cbCardCount = GetCardCount(cbCardIndexTemp);
	CChiHuRight chr;

	if((cbCardCount-2)%3==0)
	{
		for(BYTE i = 0; i < MAX_INDEX-MAX_HUA_COUNT; i++)
		{
			if(cbCardIndexTemp[i] == 0) continue;
			cbCardIndexTemp[i]--;
			
			for(BYTE j = 0; j < MAX_INDEX-MAX_HUA_COUNT; j++)
			{
				BYTE cbCurrentCard = SwitchToCardData(j);
				if(WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,cbCurrentCard,chr))
				{
					cbOutCardData[cbOutCount++] = SwitchToCardData(i);
					break;
				}
			}

			cbCardIndexTemp[i]++;
		}
	}
	cbOutCardCount = cbOutCount;

	return cbOutCount;
}

BYTE CGameLogic::GetTingDataEx(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE& cbOutCardCount,BYTE cbOutCardData[],BYTE cbHuCardCount[],BYTE cbHuCardData[][HEAP_FULL_COUNT],BYTE cbHuFan[][HEAP_FULL_COUNT])
{
	//复制数据
	BYTE cbOutCount = 0;
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	BYTE cbCardCount = GetCardCount(cbCardIndexTemp);
	CChiHuRight chr;

	if((cbCardCount-2)%3==0)
	{
		for(BYTE i = 0; i < MAX_INDEX-MAX_HUA_COUNT; i++)
		{
			if(cbCardIndexTemp[i] == 0) continue;
			cbCardIndexTemp[i]--;

			bool bAdd=false;
			BYTE nCount=0;
			for(BYTE j = 0; j < MAX_INDEX-MAX_HUA_COUNT; j++)
			{
				BYTE cbCurrentCard = SwitchToCardData(j);
				/*if (i==25 && j==24)
				{
				int a = 4;
				}*/
				if(WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,cbCurrentCard,chr))
				{
					if(bAdd==FALSE)
					{
						bAdd=true;
						cbOutCardData[cbOutCount++] = SwitchToCardData(i);
					}
					cbHuCardData[cbOutCount-1][nCount]=SwitchToCardData(j);
					cbHuFan[cbOutCount - 1][nCount] = GetUserHuFan(chr);
					nCount++;
				}
			}
			if(bAdd)
				cbHuCardCount[cbOutCount-1]=nCount;

			cbCardIndexTemp[i]++;
		}
	}
	else
	{
		BYTE cbCount=0;
		for (BYTE j = 0; j < MAX_INDEX - MAX_HUA_COUNT; j++)
		{
			BYTE cbCurrentCard = SwitchToCardData(j);
			if( WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,cbCurrentCard,chr) )
			{
				cbHuCardData[0][cbCount++] = cbCurrentCard;
			}
		}
		cbHuCardCount[0]=cbCount;
	}

	cbOutCardCount = cbOutCount;
	return cbOutCount;
}

BYTE CGameLogic::GetErXiangTingDataEx(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE& cbOutCardCount, BYTE cbOutCardData[], BYTE cbHuCardCount[], BYTE cbHuCardData[][HEAP_FULL_COUNT], BYTE cbHuFan[][HEAP_FULL_COUNT])
{
	//复制数据
	BYTE cbOutCount = 0;
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));

	BYTE cbCardCount = GetCardCount(cbCardIndexTemp);

	if ((cbCardCount - 2) % 3 == 0)
	{
		for (BYTE i = 0; i < MAX_INDEX - MAX_HUA_COUNT; i++)
		{
			if (cbCardIndexTemp[i] == 0) continue;
			cbCardIndexTemp[i]--;
			BYTE nCount = 0;
			bool bAdd = false;
				for (BYTE j = 0; j < MAX_INDEX - MAX_HUA_COUNT; j++)
				{
					if (i==j)
					{
						continue;
					}
					BYTE cbCurrentCard = SwitchToCardData(j);
					cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]++;
					BYTE cbRes = AnalyseTingCard(cbCardIndexTemp, WeaveItem, cbWeaveCount);
					if (cbRes == WIK_LISTEN)
					{
						if (bAdd == FALSE)
						{
							bAdd = true;
							cbOutCardData[cbOutCount++] = SwitchToCardData(i);
						}
						cbHuCardData[cbOutCount - 1][nCount] = SwitchToCardData(j);
						nCount++;
					}
					cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]--;
				}
				if (bAdd)
					cbHuCardCount[cbOutCount - 1] = nCount;
			cbCardIndexTemp[i]++;
		}
	}
	cbOutCardCount = cbOutCount;
	return cbOutCount;
}

BYTE CGameLogic::GetHuCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount,BYTE cbHuCardData[])
{
	//复制数据
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory( cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp) );

	BYTE cbCount = 0;
	ZeroMemory(cbHuCardData,sizeof(cbHuCardData));

	BYTE cbCardCount = GetCardCount(cbCardIndexTemp);
	CChiHuRight chr;

	if( (cbCardCount-2)%3 !=0 )
	{
		for( BYTE j = 0; j < MAX_INDEX; j++ )
		{
			BYTE cbCurrentCard = SwitchToCardData(j);
			if( WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,cbCurrentCard,chr) )
			{
				cbHuCardData[cbCount++] = cbCurrentCard;
			}
		}
		if(cbCount>0)
			return cbCount;
	}

	return 0;
}
//扑克转换
BYTE CGameLogic::SwitchToCardData(BYTE cbCardIndex)
{
	if (cbCardIndex < 27)
		return ((cbCardIndex / 9) << 4) | (cbCardIndex % 9 + 1);
	else return (0x30 | (cbCardIndex - 27 + 1));
	//return ((((cbCardIndex) / 9)) << 4) | ((cbCardIndex) % 9 + 1);
}

//扑克转换
BYTE CGameLogic::SwitchToCardIndex(BYTE cbCardData)
{
	ASSERT(IsValidCard(cbCardData));
	return ((cbCardData & MASK_COLOR) >> 4) * 9 + (cbCardData & MASK_VALUE) - 1;
}

//扑克转换
BYTE CGameLogic::SwitchToCardData(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[MAX_COUNT])
{
	//转换扑克
	BYTE cbPosition=0;
	//财神
	if(m_cbMagicIndex[0] != MAX_INDEX)
	{
		int index = cbCardIndex[m_cbMagicIndex[0]];
		for(BYTE i = 0; i < cbCardIndex[m_cbMagicIndex[0]]; i++)
			cbCardData[cbPosition++] = SwitchToCardData(m_cbMagicIndex[0]);
	}
	if(m_cbMagicIndex[1] != MAX_INDEX)
	{
		for(BYTE i = 0; i < cbCardIndex[m_cbMagicIndex[1]]; i++)
			cbCardData[cbPosition++] = SwitchToCardData(m_cbMagicIndex[1]);
	}
	int relaceCardCount = cbCardIndex[INDEX_REPLACE_CARD];
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if( (i == m_cbMagicIndex[0] && m_cbMagicIndex[0] != INDEX_REPLACE_CARD) || (i == m_cbMagicIndex[1] && m_cbMagicIndex[1] != INDEX_REPLACE_CARD) )
		{
			//如果财神有代替牌，则代替牌代替财神原来的位置
			if ((INDEX_REPLACE_CARD != MAX_INDEX)&& (relaceCardCount>0))
			{
				for (BYTE j = 0; j < cbCardIndex[INDEX_REPLACE_CARD]; j++)
				{
					cbCardData[cbPosition++] = SwitchToCardData(INDEX_REPLACE_CARD);
					relaceCardCount--;
				}
					
			}
			continue;
		}
		if(i == INDEX_REPLACE_CARD) continue;
		if (cbCardIndex[i]!=0)
		{
			for (BYTE j=0;j<cbCardIndex[i];j++)
			{
				ASSERT(cbPosition<MAX_COUNT);
				cbCardData[cbPosition++]=SwitchToCardData(i);
			}
		}
	}

	return cbPosition;
}

//扑克转换
BYTE CGameLogic::SwitchToCardIndex(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardIndex[MAX_INDEX])
{
	//设置变量
	ZeroMemory(cbCardIndex,sizeof(BYTE)*MAX_INDEX);

	//转换扑克
	for (BYTE i=0;i<cbCardCount;i++)
	{
		ASSERT(IsValidCard(cbCardData[i]));
		cbCardIndex[SwitchToCardIndex(cbCardData[i])]++;
	}

	return cbCardCount;
}


//有效判断
bool CGameLogic::IsValidCard(BYTE cbCardData)
{
	BYTE cbValue = (cbCardData & MASK_VALUE);
	BYTE cbColor = (cbCardData & MASK_COLOR) >> 4;
	return (((cbValue >= 1) && (cbValue <= 9) && (cbColor <= 2)) || ((cbValue >= 1) && (cbValue <= (7 + MAX_HUA_COUNT)) && (cbColor == 3)));
}

//扑克数目
BYTE CGameLogic::GetCardCount(const BYTE cbCardIndex[MAX_INDEX])
{
	//数目统计
	BYTE cbCardCount=0;
	for (BYTE i=0;i<MAX_INDEX;i++) 
		cbCardCount+=cbCardIndex[i];

	return cbCardCount;
}

//获取组合
BYTE CGameLogic::GetWeaveCard(BYTE cbWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4])
{
	//组合扑克
	switch (cbWeaveKind)
	{
	case WIK_LEFT:		//上牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard+1;
			cbCardBuffer[2]=cbCenterCard+2;

			return 3;
		}
	case WIK_RIGHT:		//上牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard-2;
			cbCardBuffer[1]=cbCenterCard-1;
			cbCardBuffer[2]=cbCenterCard;

			return 3;
		}
	case WIK_CENTER:	//上牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard-1;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard+1;

			return 3;
		}
	case WIK_PENG:		//碰牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard;

			return 3;
		}
	case WIK_GANG:		//杠牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard;
			cbCardBuffer[3]=cbCenterCard;

			return 4;
		}
	default:
		{
			ASSERT(FALSE);
		}
	}

	return 0;
}

bool CGameLogic::AddKindItem(tagKindItem &TempKindItem, tagKindItem KindItem[], BYTE &cbKindItemCount, bool &bMagicThree)
{
	TempKindItem.cbMagicCount =  
		(m_cbMagicIndex[0] == TempKindItem.cbValidIndex[0] ? 1 : 0) + 
		(m_cbMagicIndex[0] == TempKindItem.cbValidIndex[1] ? 1 : 0) +
		(m_cbMagicIndex[0] == TempKindItem.cbValidIndex[2] ? 1 : 0) ;

	if(TempKindItem.cbMagicCount>=3) 
	{
		if(!bMagicThree)
		{
			bMagicThree = true;
			CopyMemory(&KindItem[cbKindItemCount++],&TempKindItem,sizeof(TempKindItem));
			return true;
		}
		return false;
	}
	else if(TempKindItem.cbMagicCount == 2)
	{
		BYTE cbNoMagicIndex = 0;
		BYTE cbNoTempMagicIndex = 0;
		for(int i = 0; i < 3; i++)
		{
			if(TempKindItem.cbValidIndex[i] != m_cbMagicIndex[0])
			{
				cbNoTempMagicIndex = TempKindItem.cbValidIndex[i];
				break;
			}
		}
		bool bFind = false;
		for(int j = 0; j < cbKindItemCount; j++)
		{
			for(int i = 0; i < 3; i++)
			{
				if(KindItem[j].cbValidIndex[i] != m_cbMagicIndex[0])
				{
					cbNoMagicIndex = KindItem[j].cbValidIndex[i];
					break;
				}
			}
			if(cbNoMagicIndex == cbNoTempMagicIndex && cbNoMagicIndex != 0)
			{
				bFind = true;
			}
		}

		if(!bFind)
		{
			CopyMemory(&KindItem[cbKindItemCount++],&TempKindItem,sizeof(TempKindItem));
			return true;
		}
		return false;
	}
	else if(TempKindItem.cbMagicCount == 1)
	{
		BYTE cbTempCardIndex[2] = {0};
		BYTE cbCardIndex[2] = {0};
		BYTE cbCardCount = 0;
		for(int i = 0; i < 3; i++)
		{
			if(TempKindItem.cbValidIndex[i] != m_cbMagicIndex[0])
			{
				cbTempCardIndex[cbCardCount++] = TempKindItem.cbValidIndex[i];
			}
		}
		ASSERT(cbCardCount == 2);

		for(int j = 0; j < cbKindItemCount; j++)
		{
			if(1 == KindItem[j].cbMagicCount)
			{
				cbCardCount = 0;
				for(int i = 0; i < 3; i++)
				{
					if(KindItem[j].cbValidIndex[i] != m_cbMagicIndex[0])
					{
						cbCardIndex[cbCardCount++] = KindItem[j].cbValidIndex[i];
					}
				}
				ASSERT(cbCardCount == 2);

				if(cbTempCardIndex[0] == cbCardIndex[0] && cbTempCardIndex[1] == cbCardIndex[1])
				{
					return false;
				}
			}
		}


		CopyMemory(&KindItem[cbKindItemCount++],&TempKindItem,sizeof(TempKindItem));
		return true;
	}
	else
	{
		CopyMemory(&KindItem[cbKindItemCount++],&TempKindItem,sizeof(TempKindItem));
		return true;
	}
}


//分析扑克
bool CGameLogic::AnalyseCard(const BYTE cbCardIndex1[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, vector<tagAnalyseItem> & AnalyseItemArray)
{
	//计算数目
	BYTE cbCardCount=GetCardCount(cbCardIndex1);

	//效验数目
	ASSERT((cbCardCount>=2)&&(cbCardCount<=MAX_COUNT)&&((cbCardCount-2)%3==0));
	if ((cbCardCount<2)||(cbCardCount>MAX_COUNT)||((cbCardCount-2)%3!=0))
		return false;

	BYTE cbCardIndex[MAX_INDEX]={0};
	CopyMemory(cbCardIndex,cbCardIndex1,sizeof(cbCardIndex));

	if(m_cbMagicIndex[1] != MAX_INDEX)
	{
		cbCardIndex[m_cbMagicIndex[0]] += cbCardIndex[m_cbMagicIndex[1]];
		cbCardIndex[m_cbMagicIndex[1]]=0;
	}
	//变量定义
	BYTE cbKindItemCount=0;
	tagKindItem KindItem[27*2+28+16];
	ZeroMemory(KindItem,sizeof(KindItem));
	tagKindItem TempKindItem;
	ZeroMemory(&TempKindItem,sizeof(TempKindItem));
	bool bMagicThree=false;

	//需求判断
	BYTE cbLessKindItem=(cbCardCount-2)/3;
	ASSERT((cbLessKindItem+cbWeaveCount)==MAX_WEAVE);

	//单吊判断
	if (cbLessKindItem==0)
	{
		//效验参数
		ASSERT((cbCardCount==2)&&(cbWeaveCount==MAX_WEAVE));

		//牌眼判断
		for (BYTE i=0;i<MAX_INDEX;i++)
		{
			if (cbCardIndex[i]==2 || (m_cbMagicIndex[0] != MAX_INDEX && i != m_cbMagicIndex[0] && cbCardIndex[m_cbMagicIndex[0]]+cbCardIndex[i]==2))
			{
				//变量定义
				tagAnalyseItem AnalyseItem;
				ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));

				//设置结果
				for (BYTE j=0;j<cbWeaveCount;j++)
				{
					AnalyseItem.cbWeaveKind[j]=WeaveItem[j].cbWeaveKind;
					AnalyseItem.cbCenterCard[j]=WeaveItem[j].cbCenterCard;
					GetWeaveCard(WeaveItem[j].cbWeaveKind, WeaveItem[j].cbCenterCard, AnalyseItem.cbCardData[j]);
				}
				if(cbCardIndex[i] < 2 || i == m_cbMagicIndex[0])
					AnalyseItem.bMagicEye = true;
				else AnalyseItem.bMagicEye = false;
				AnalyseItem.cbCardEye=cbCardIndex[i]==0?SwitchToCardData(cbCardIndex[m_cbMagicIndex[0]]):SwitchToCardData(i);

				//插入结果
				AnalyseItemArray.push_back(AnalyseItem);

				return true;
			}
		}

		return false;
	}

	//拆分分析
	BYTE cbMagicCardIndex[MAX_INDEX];
	CopyMemory(cbMagicCardIndex,cbCardIndex,sizeof(cbMagicCardIndex));

	//如果有财神
	BYTE cbMagicCardCount = 0;
	BYTE cbTempMagicCount = 0;

	if(m_cbMagicIndex[0] != MAX_INDEX)
	{
		cbMagicCardCount = cbCardIndex[m_cbMagicIndex[0]];
		//如果财神有代替牌，财神与代替牌转换
	/*	if(INDEX_REPLACE_CARD != MAX_INDEX)
		{
			cbMagicCardIndex[m_cbMagicIndex[0]] = cbMagicCardIndex[INDEX_REPLACE_CARD];
			cbMagicCardIndex[INDEX_REPLACE_CARD] = cbMagicCardCount;
		}*/
	}

	if (cbCardCount>=3)
	{
		for (BYTE i=0;i<MAX_INDEX-MAX_HUA_COUNT;i++)
		{
			//同牌判断
			//如果是财神,并且财神数小于3,则不进行组合
			if(cbMagicCardIndex[i] >= 3 || (cbMagicCardIndex[i]+cbMagicCardCount >= 3 &&
				((INDEX_REPLACE_CARD!=MAX_INDEX && i != INDEX_REPLACE_CARD) || (INDEX_REPLACE_CARD==MAX_INDEX && i != m_cbMagicIndex[0])))
				)
			{
				int nTempIndex = cbMagicCardIndex[i];
				do
				{
					ASSERT(cbKindItemCount < CountArray(KindItem));
					BYTE cbIndex = i;
					BYTE cbCenterCard = SwitchToCardData(i);
					//如果是财神且财神有代替牌,则换成代替牌
					if(i == m_cbMagicIndex[0] && INDEX_REPLACE_CARD != MAX_INDEX)
					{
						cbIndex = INDEX_REPLACE_CARD;
						cbCenterCard = SwitchToCardData(INDEX_REPLACE_CARD);
					}
					TempKindItem.cbWeaveKind=WIK_PENG;
					TempKindItem.cbCenterCard=cbCenterCard;
					TempKindItem.cbValidIndex[0] = nTempIndex>0?cbIndex:m_cbMagicIndex[0];
					TempKindItem.cbValidIndex[1] = nTempIndex>1?cbIndex:m_cbMagicIndex[0];
					TempKindItem.cbValidIndex[2] = nTempIndex>2?cbIndex:m_cbMagicIndex[0];
					AddKindItem(TempKindItem, KindItem, cbKindItemCount, bMagicThree);


					//当前索引牌未与财神组合 且财神个数不为0 
					if(nTempIndex>=3 && cbMagicCardCount >0)
					{
						--nTempIndex;
						//1个财神与之组合
						TempKindItem.cbWeaveKind=WIK_PENG;
						TempKindItem.cbCenterCard=cbCenterCard;
						TempKindItem.cbValidIndex[0] = nTempIndex>0?cbIndex:m_cbMagicIndex[0];
						TempKindItem.cbValidIndex[1] = nTempIndex>1?cbIndex:m_cbMagicIndex[0];
						TempKindItem.cbValidIndex[2] = nTempIndex>2?cbIndex:m_cbMagicIndex[0];
						AddKindItem(TempKindItem, KindItem, cbKindItemCount, bMagicThree);

						//两个财神与之组合
						if(cbMagicCardCount>1)
						{
							TempKindItem.cbWeaveKind=WIK_PENG;
							TempKindItem.cbCenterCard=cbCenterCard;
							TempKindItem.cbValidIndex[0] = nTempIndex>0?cbIndex:m_cbMagicIndex[0];
							TempKindItem.cbValidIndex[1] = nTempIndex>1?cbIndex:m_cbMagicIndex[0];
							TempKindItem.cbValidIndex[2] = nTempIndex>2?cbIndex:m_cbMagicIndex[0];
							AddKindItem(TempKindItem, KindItem, cbKindItemCount, bMagicThree);
						}

						++nTempIndex;
					}

					//如果是财神,则退出
					if(i == INDEX_REPLACE_CARD || ((i == m_cbMagicIndex[0]) && INDEX_REPLACE_CARD == MAX_INDEX))
						break;

					nTempIndex -= 3;
					//如果刚好搭配全部，则退出
					if(nTempIndex == 0) break;

				}while(nTempIndex+cbMagicCardCount >= 3);
			}

			//连牌判断
			if ( ((i<(MAX_INDEX-MAX_HUA_COUNT-9))&&((i%9)<7)) || (G_ZI_SHUN && i==31) )
			{
				//只要财神牌数加上3个顺序索引的牌数大于等于3,则进行组合
				int replaceCardCount = 0;
				if (INDEX_REPLACE_CARD != MAX_INDEX )
				{
					 replaceCardCount = cbMagicCardIndex[INDEX_REPLACE_CARD];
				}
					
				if (cbMagicCardCount + cbMagicCardIndex[i] + cbMagicCardIndex[i + 1] + cbMagicCardIndex[i + 2] + replaceCardCount >= 3)
				{
					BYTE cbIndex[3] = { cbMagicCardIndex[i],cbMagicCardIndex[i+1],cbMagicCardIndex[i+2] };

					if(cbIndex[0]+cbIndex[1]+cbIndex[2]==0) continue;

					int nMagicCountTemp;
					nMagicCountTemp = cbMagicCardCount;
					BYTE cbValidIndex[3];
					while (nMagicCountTemp + cbIndex[0] + cbIndex[1] + cbIndex[2] + replaceCardCount >= 3)
					{
						for(BYTE j = 0; j < CountArray(cbIndex); j++)
						{
							if(cbIndex[j] > 0) 
							{
								cbIndex[j]--;
								if ((INDEX_REPLACE_CARD != MAX_INDEX) && (replaceCardCount > 0) && ((i + j == m_cbMagicIndex[0]) || (i + j == m_cbMagicIndex[1])))
								{
									replaceCardCount--;
									cbValidIndex[j] = INDEX_REPLACE_CARD;
								}
								else{
									cbValidIndex[j] =  i + j;
									/*	if (((i + j == m_cbMagicIndex[0]) || (i + j == m_cbMagicIndex[1])))
										{
										nMagicCountTemp--;
										}*/
								}
								
							}
							else 
							{
								if ((INDEX_REPLACE_CARD != MAX_INDEX)&&(replaceCardCount>0) && ((i + j == m_cbMagicIndex[0]) || (i + j == m_cbMagicIndex[1])))
								{
									replaceCardCount--;
									cbValidIndex[j] = INDEX_REPLACE_CARD;
								}
								else
								{
									nMagicCountTemp--;
									cbValidIndex[j] = m_cbMagicIndex[0];
								}
																					
							}
						}
						if(nMagicCountTemp >= 0)
						{
							ASSERT(cbKindItemCount < CountArray(KindItem));
							TempKindItem.cbWeaveKind=WIK_LEFT;
							TempKindItem.cbCenterCard=SwitchToCardData(i);
							CopyMemory(TempKindItem.cbValidIndex,cbValidIndex,sizeof(cbValidIndex));
							AddKindItem(TempKindItem, KindItem, cbKindItemCount, bMagicThree);
						}
						else break;
					}
				}
			}
			else if(G_ZI_SHUN && i==27)//东南西北任意三个可成顺
			{
				BYTE cbTempIndex[4][3]={0};
				cbTempIndex[0][0]=cbMagicCardIndex[i];
				cbTempIndex[0][1]=cbMagicCardIndex[i+1];
				cbTempIndex[0][2]=cbMagicCardIndex[i+2];

				cbTempIndex[1][0]=cbMagicCardIndex[i];
				cbTempIndex[1][1]=cbMagicCardIndex[i+1];
				cbTempIndex[1][2]=cbMagicCardIndex[i+3];

				cbTempIndex[2][0]=cbMagicCardIndex[i];
				cbTempIndex[2][1]=cbMagicCardIndex[i+2];
				cbTempIndex[2][2]=cbMagicCardIndex[i+3];

				cbTempIndex[3][0]=cbMagicCardIndex[i+1];
				cbTempIndex[3][1]=cbMagicCardIndex[i+2];
				cbTempIndex[3][2]=cbMagicCardIndex[i+3];

				for(int index = 0;index<4;index++)
				{
					//只要财神牌数加上3个顺序索引的牌数大于等于3,则进行组合
					if(cbMagicCardCount+cbTempIndex[index][0]+cbTempIndex[index][1]+cbTempIndex[index][2] >= 3)
					{
						BYTE cbIndex[3] = { cbTempIndex[index][0],cbTempIndex[index][1],cbTempIndex[index][2] };

						if(cbIndex[0]+cbIndex[1]+cbIndex[2]==0) continue;

						int nMagicCountTemp;
						nMagicCountTemp = cbMagicCardCount;

						BYTE cbValidIndex[3];
						while(nMagicCountTemp+cbIndex[0]+cbIndex[1]+cbIndex[2] >= 3)
						{
							for(BYTE j = 0; j < CountArray(cbIndex); j++)
							{
								if(cbIndex[j] > 0) 
								{
									cbIndex[j]--;
									if(index==0)
										cbValidIndex[j] = ((i+j==m_cbMagicIndex[0]) && INDEX_REPLACE_CARD!=MAX_INDEX)?INDEX_REPLACE_CARD:i+j;
									else if(index==1)
									{
										if(j==2)
											cbValidIndex[j] = ((i+j+1==m_cbMagicIndex[0]) && INDEX_REPLACE_CARD!=MAX_INDEX)?INDEX_REPLACE_CARD:i+j+1;
										else
											cbValidIndex[j] = ((i+j==m_cbMagicIndex[0]) && INDEX_REPLACE_CARD!=MAX_INDEX)?INDEX_REPLACE_CARD:i+j;
									}
									else if(index==2)
									{
										if(j==0)
											cbValidIndex[j] = ((i+j==m_cbMagicIndex[0]) && INDEX_REPLACE_CARD!=MAX_INDEX)?INDEX_REPLACE_CARD:i+j;
										else
											cbValidIndex[j] = ((i+j+1==m_cbMagicIndex[0]) && INDEX_REPLACE_CARD!=MAX_INDEX)?INDEX_REPLACE_CARD:i+j+1;
									}
									else if(index==3)
									{
										cbValidIndex[j] = ((i+j+1==m_cbMagicIndex[0]) && INDEX_REPLACE_CARD!=MAX_INDEX)?INDEX_REPLACE_CARD:i+j+1;
									}


								}
								else 
								{
									nMagicCountTemp--;
									cbValidIndex[j] = m_cbMagicIndex[0];														
								}
							}
							if(nMagicCountTemp >= 0)
							{
								ASSERT(cbKindItemCount < CountArray(KindItem));
								TempKindItem.cbWeaveKind=WIK_LEFT;
								TempKindItem.cbCenterCard=SwitchToCardData(i);
								CopyMemory(TempKindItem.cbValidIndex,cbValidIndex,sizeof(cbValidIndex));
								AddKindItem(TempKindItem, KindItem, cbKindItemCount, bMagicThree);
							}
							else break;
						}
					}
				}

			}

		}
	}

	//组合分析
	if (cbKindItemCount>=cbLessKindItem)
	{
		ASSERT(27*2+28+16 >= cbKindItemCount);
		//变量定义
		BYTE cbCardIndexTemp[MAX_INDEX];
		ZeroMemory(cbCardIndexTemp,sizeof(cbCardIndexTemp));

		//变量定义
		BYTE cbIndex[MAX_WEAVE];
		for(BYTE i = 0; i < CountArray(cbIndex); i++)
			cbIndex[i] = i;

		tagKindItem * pKindItem[MAX_WEAVE];
		ZeroMemory(&pKindItem,sizeof(pKindItem));

		tagKindItem KindItemTemp[CountArray(KindItem)];

		//开始组合
		do
		{
			//如果四个组合中的混牌大于手上的混牌个数则重置索引
			cbTempMagicCount = 0;
			for(int i=0;i<cbLessKindItem;i++) cbTempMagicCount +=  KindItem[cbIndex[i]].cbMagicCount; 
			if(cbTempMagicCount <= cbMagicCardCount)
			{
				//设置变量
				CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));
				CopyMemory(KindItemTemp,KindItem,sizeof(KindItem));

				for (BYTE i=0;i<cbLessKindItem;i++)
					pKindItem[i]=&KindItemTemp[cbIndex[i]];


				//数量判断
				bool bEnoughCard=true;

				for (BYTE i=0;i<cbLessKindItem*3;i++)
				{
					//存在判断
					BYTE cbCardIndex=pKindItem[i/3]->cbValidIndex[i%3]; 
					if (cbCardIndexTemp[cbCardIndex]==0)
					{
						if(m_cbMagicIndex[0] != MAX_INDEX && cbCardIndexTemp[m_cbMagicIndex[0]] > 0)
						{											
							pKindItem[i/3]->cbValidIndex[i%3] = m_cbMagicIndex[0];
							cbCardIndexTemp[m_cbMagicIndex[0]]--;
						}
						else
						{
							bEnoughCard=false;
							break;
						}
					}
					else cbCardIndexTemp[cbCardIndex]--;
				}

				//胡牌判断
				if (bEnoughCard==true)
				{
					//牌眼判断
					BYTE cbCardEye=0;
					bool bMagicEye = false;
					if(GetCardCount(cbCardIndexTemp) == 2)
					{
						if(m_cbMagicIndex[0] != MAX_INDEX && cbCardIndexTemp[m_cbMagicIndex[0]]==2)
						{
							cbCardEye = SwitchToCardData(m_cbMagicIndex[0]);
							bMagicEye = true;
						}
						else
						{
							for (BYTE i=0;i<MAX_INDEX;i++)
							{
								if (cbCardIndexTemp[i]==2)
								{
									cbCardEye=SwitchToCardData(i);
									if(m_cbMagicIndex[0] != MAX_INDEX && i == m_cbMagicIndex[0]) 
									{
										bMagicEye = true;
									}
									break;
								}
								else if(i!=m_cbMagicIndex[0] && m_cbMagicIndex[0] != MAX_INDEX && cbCardIndexTemp[i]+cbCardIndexTemp[m_cbMagicIndex[0]]==2)
								{
									cbCardEye = SwitchToCardData(i);
									bMagicEye = true;
									break;
								}
							}
						}
					}

					//组合类型
					if (cbCardEye!=0)
					{
						//变量定义
						tagAnalyseItem AnalyseItem;
						ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));

						//设置组合
						for (BYTE i=0;i<cbWeaveCount;i++)
						{
							AnalyseItem.cbWeaveKind[i]=WeaveItem[i].cbWeaveKind;
							AnalyseItem.cbCenterCard[i]=WeaveItem[i].cbCenterCard;
							GetWeaveCard(WeaveItem[i].cbWeaveKind,WeaveItem[i].cbCenterCard,AnalyseItem.cbCardData[i]);
						}

						//设置牌型
						for (BYTE i=0;i<cbLessKindItem;i++) 
						{
							AnalyseItem.cbWeaveKind[i+cbWeaveCount]=pKindItem[i]->cbWeaveKind;
							AnalyseItem.cbCenterCard[i+cbWeaveCount]=pKindItem[i]->cbCenterCard;
							AnalyseItem.cbCardData[cbWeaveCount+i][0] = SwitchToCardData(pKindItem[i]->cbValidIndex[0]);
							AnalyseItem.cbCardData[cbWeaveCount+i][1] = SwitchToCardData(pKindItem[i]->cbValidIndex[1]);
							AnalyseItem.cbCardData[cbWeaveCount+i][2] = SwitchToCardData(pKindItem[i]->cbValidIndex[2]);
						}

						//设置牌眼
						AnalyseItem.cbCardEye=cbCardEye;
						AnalyseItem.bMagicEye = bMagicEye;

						//插入结果
						AnalyseItemArray.push_back(AnalyseItem);
					}
				}
			}

			//设置索引
			if (cbIndex[cbLessKindItem-1]==(cbKindItemCount-1))
			{
				BYTE i = 0;
				for ( i=cbLessKindItem-1;i>0;i--)
				{
					if ((cbIndex[i-1]+1)!=cbIndex[i])
					{
						BYTE cbNewIndex=cbIndex[i-1];
						for (BYTE j=(i-1);j<cbLessKindItem;j++) 
							cbIndex[j]=cbNewIndex+j-i+2;
						break;
					}
				}
				if (i==0)
					break;
			}
			else
				cbIndex[cbLessKindItem-1]++;

		} while (true);
	}

	return (AnalyseItemArray.size()>0);
}
/*
// 胡法分析函数
*/

//碰碰和
bool CGameLogic::IsPengPeng(const tagAnalyseItem *pAnalyseItem)
{
	for(BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++)
	{
		if(pAnalyseItem->cbWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT))
			return false;
	}
	return true;
}

//是否七对
bool CGameLogic::IsQiDui(const BYTE cbCardIndex[MAX_INDEX], BYTE cbWeaveCount)
{
	if(cbWeaveCount!=0)
		return false;

	if(GetCardCount(cbCardIndex) != MAX_COUNT)
		return false;

  
	BYTE cbTempIndex[MAX_INDEX]={0};
	CopyMemory(cbTempIndex,cbCardIndex,sizeof(cbTempIndex));
	if(m_cbMagicIndex[1] != MAX_INDEX)
	{
		cbTempIndex[m_cbMagicIndex[0]] += cbTempIndex[m_cbMagicIndex[1]];
		cbTempIndex[m_cbMagicIndex[1]] = 0;
	}

	BYTE cbMagicCount = GetMajicCount(cbTempIndex);
	//变量定义
	for(BYTE i=0;i<MAX_INDEX;i++)
	{
		if(cbTempIndex[i] != 0 && IsMagicCard(SwitchToCardData(i)) == FALSE)
		{			
			if(cbTempIndex[i] % 2 == 1)
			{
				if(cbMagicCount >= 1)
				{
					cbMagicCount--;
				}
				else
				{
					return false;//有非对子，跳出
				}
			}
		}
	}
	return true;
}

bool CGameLogic::IsQiDaDui(const BYTE cbCardIndex[MAX_INDEX],BYTE cbWeaveCount)
{
	if(cbWeaveCount!=0)
		return false;

	if (GetCardCount(cbCardIndex) != MAX_COUNT)
		return false;

	BYTE cbTempIndex[MAX_INDEX]={0};
	CopyMemory(cbTempIndex,cbCardIndex,sizeof(cbTempIndex));
	if(m_cbMagicIndex[1] != MAX_INDEX)
	{
		cbTempIndex[m_cbMagicIndex[0]] += cbTempIndex[m_cbMagicIndex[1]];
		cbTempIndex[m_cbMagicIndex[1]] = 0;
	}

	BYTE cbMagicCount = GetMajicCount(cbTempIndex);
	int  tempCount=0;
	bool QiDaDui3=true;
	//变量定义
	for(BYTE i=0;i<MAX_INDEX;i++)
	{
		if(cbTempIndex[i] != 0 && IsMagicCard(SwitchToCardData(i)) == FALSE)
		{			
			if(cbTempIndex[i]== 1)
			{
				if(cbMagicCount >= 2)
				{
					cbMagicCount-=2;
				}
				else
				{
					QiDaDui3= false;//有非对子，跳出
					break;
				}
			}
			else if (cbTempIndex[i]==2)
			{
				tempCount++;
				if (tempCount>1)
				{
					if(cbMagicCount >= 1)
					{
						cbMagicCount-=1;
					}
					else
					{
						QiDaDui3=false;
						break;
					}
				}
			}
		}
	}

	 cbMagicCount = GetMajicCount(cbTempIndex);
	  tempCount=0;
	  	bool QiDaDui4=true;
	//变量定义
	for(BYTE i=0;i<MAX_INDEX;i++)
	{
		if(cbTempIndex[i] != 0 && IsMagicCard(SwitchToCardData(i)) == FALSE)
		{			
			if(cbTempIndex[i]== 1)
			{
				if(cbMagicCount >= 3)
				{
					cbMagicCount-=3;
				}
				else
				{
					QiDaDui4=false;
					break;
				}
			}
			else if (cbTempIndex[i]==2)
			{
				tempCount++;
				if (tempCount>1)
				{
					if(cbMagicCount >= 2)
					{
						cbMagicCount-=2;
					}
					else
					{
						QiDaDui4=false;
						break;
					}
				}
			}
		}
	}
	if (QiDaDui4||QiDaDui3)
	{
		return true;
	}
	else
	{
		return false;
	}
	return false;

}

//十三烂系列
bool CGameLogic::IsShiSanLan(const BYTE cbCardIndex[MAX_INDEX],BYTE cbWeaveCount)
{
	//组合判断
	if (cbWeaveCount!=0) return false;

	BYTE cbTempIndex[MAX_INDEX]={0};
	CopyMemory(cbTempIndex,cbCardIndex,sizeof(cbTempIndex));

	if(m_cbMagicIndex[0] != MAX_INDEX)
		cbTempIndex[m_cbMagicIndex[0]] = 0;
	if(m_cbMagicIndex[1] != MAX_INDEX)
		cbTempIndex[m_cbMagicIndex[1]] = 0;


	for(int i=0;i<MAX_INDEX;i++)
	{
		if(cbTempIndex[i]>=2 && i != m_cbMagicIndex[0]) //不能有重复牌
			return false;
	}

	for(int j=0;j<3;j++)
	{
		for(int i=0;i<9-2;i++)
		{
			int index = j*9+i;
			if(cbTempIndex[index]+cbTempIndex[index+1]+cbTempIndex[index+2]>1)//间隔必须>=3
			{
				return false;
			}
		}
	}

	return true;
}

bool CGameLogic::IsQiXing(const BYTE cbCardIndex[MAX_INDEX],BYTE cbWeaveCount)
{
   if (IsShiSanLan(cbCardIndex,cbWeaveCount))
   {
	   for (int i=27;i<MAX_INDEX;i++)
	   {
		   if ((cbCardIndex[i]!=1))
		   {
			   return false;
		   }
		   
	   }
	   return true;
   }
   else
   {
	   return false;
   }
 
}

bool CGameLogic::IsJingDiaoJiang(const BYTE cbCardIndex[MAX_INDEX],BYTE cbWeaveCount)
{

   for (int i=0;i<MAX_INDEX;i++)
   {
       if ((cbCardIndex[i]>1)&&(m_cbMagicIndex[0]!=i)&&(m_cbMagicIndex[1]!=i))
       {
		   return false;
       }
   }
   if (((m_cbMagicIndex[0]==1)&&(m_cbMagicIndex[1]==0))||((m_cbMagicIndex[0]==0)&&(m_cbMagicIndex[1]==1)))
   {
	   return true;

   }
   else
   {
	   return false;
   }
   return true;
}

bool CGameLogic::IsShouShuangYou(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX])
{
	int majicCount = GetMajicCount(cbCardIndex);
	int otherMajicCout = 0;
	if (pAnalyseItem->bMagicEye&&majicCount >= 2)
	{
		for (BYTE i = 0; i < CountArray(pAnalyseItem->cbCardData); i++)
		{
			for (BYTE j = 0; j < CountArray(pAnalyseItem->cbCardData[i]); j++)
			{
				if (pAnalyseItem->cbCardData[i][j] != 0)
				{
					BYTE index = SwitchToCardIndex(pAnalyseItem->cbCardData[i][j]);
					if (index == m_cbMagicIndex[0] || index == m_cbMagicIndex[1])
					{
						otherMajicCout++;
					}
				}
			}
		}
		if ((majicCount-otherMajicCout)==2)
		{
			return true;
		}
		else{
			return false;
		}
	}
	
	return false;
}

bool CGameLogic::IsShuangAnKe(const tagAnalyseItem *pAnalyseItem, const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, bool bZiMo)
{
	//--双暗刻：牌里有2个暗刻
	int count = 0;
	for (BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++)
	{
		if ((pAnalyseItem->cbWeaveKind[i] & (WIK_PENG | WIK_GANG)) != 0)
		{
			bool bExist = false;
			for (int j = 0; j < cbWeaveCount;j++)
			{
				if ((WeaveItem[j].cbWeaveKind & (WIK_PENG) != 0) &&
					pAnalyseItem->cbCardData[i][0] == WeaveItem[j].cbCardData[0])
				{
					bExist = true;
				}

				if ((WeaveItem[j].cbWeaveKind & (WIK_GANG) != 0) &&
					pAnalyseItem->cbCardData[i][0] == WeaveItem[j].cbCardData[0])
				{
					bExist = true;
				}
			}
			////--放炮上来那张不算暗刻
				//if (pAnalyseItem->cbCardData[i][0] == cbCurrentCard&& bZiMo != true)
				//{
				//	bExist = true;
				//}

			if (bExist == false)
			{
				count = count + 1;
			}
		}


	}
	if (count == 1)
	{
		return true;
	}
	return false;
}

bool CGameLogic::IsBianZhang(const tagAnalyseItem *pAnalyseItem, const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard)
{
	//--边张：单胡123中3、789的7，或1233胡3、7789胡7；手中有12345胡3，56789胡7则不算边张
	if (cbCurrentCard > 0x30)
	{
		return false;
	}
	BYTE currentCardVule = GetCardValue(cbCurrentCard);
	if (currentCardVule == 3 || currentCardVule == 7)
	{
		int bExist[10] = {  0 };
		//--索引1代表123是否存在, 2代表234是否存在, 3代表....类推
		for (BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++)
		{
			//--手牌上拿过来的组成的顺子只有DEF.WIK_LEFT类型
			if (pAnalyseItem->cbWeaveKind[i] & (WIK_LEFT | WIK_CENTER | WIK_RIGHT) != 0)
			{
				BYTE tempVaule = GetCardValue(pAnalyseItem->cbCardData[i][0]);
				bExist[tempVaule] = bExist[tempVaule] + 1;
			}

		}
		for (BYTE i = 0; i < cbWeaveCount;i++)
		{
			if (WeaveItem[i].cbWeaveKind & (WIK_LEFT | WIK_CENTER | WIK_RIGHT) != 0)
			{
				BYTE tempVaule = GetCardValue(WeaveItem[i].cbCardData[0]);
				bExist[tempVaule] = bExist[tempVaule] - 1;
			}
		}

		if (currentCardVule == 3 && (bExist[currentCardVule - 2] >= 1) && (bExist[currentCardVule] <= 0) &&
			bExist[currentCardVule - 1] <= 0)
		{
			if (pAnalyseItem->cbCardEye == cbCurrentCard && bExist[currentCardVule + 1] > 0)
			{
				return false;
			}
			return true;
		}
		else if
			(currentCardVule == 7 && (bExist[currentCardVule] >= 1) && (bExist[currentCardVule - 1] <= 0) &&
				bExist[currentCardVule - 2] <= 0)
		{
			if (pAnalyseItem->cbCardEye == cbCurrentCard && bExist[currentCardVule - 3] > 0)
			{
				return false;
			}
			return true;
		}
		else
		{
			return false;
		}

	}

	return false;
}

bool CGameLogic::IsKanZhang(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard)
{
	//--坎张：胡两张牌中间的一张牌；4556胡5也是坎张，手中有45567胡6不算坎张
	if (cbCurrentCard > 0x30)
	{
		return false;
	}
	BYTE currentCardVule = GetCardValue(cbCurrentCard);
	if (currentCardVule >= 2 && currentCardVule <= 8)
	{
		int bExist[10] = { 0 };
		//--索引1代表123是否存在, 2代表234是否存在, 3代表....类推
		if (currentCardVule >= 3 && currentCardVule <= 7)
		{
			BYTE cbCurrentCardIndex = SwitchToCardIndex(cbCurrentCard);
			if
				((cbCardIndex[cbCurrentCardIndex - 1] > 0) && (cbCardIndex[cbCurrentCardIndex - 2] > 0) &&
					currentCardVule != 3 &&
					cbCardIndex[cbCurrentCardIndex] != 1)
			{
				return false;
			}
			if
				((cbCardIndex[cbCurrentCardIndex + 1] > 0) && (cbCardIndex[cbCurrentCardIndex + 2] > 0) &&
					currentCardVule != 7 &&
					cbCardIndex[cbCurrentCardIndex] != 1)
			{
				return false;
			}

		}
		for (BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++)
		{
			//--手牌上拿过来的组成的顺子只有DEF.WIK_LEFT类型
			if (pAnalyseItem->cbWeaveKind[i] & (WIK_LEFT | WIK_CENTER | WIK_RIGHT) != 0)
			{
				//--判断cbCurrentCard <= 0x12防止数组越界)
				if
					((pAnalyseItem->cbCardData[i][0] == cbCurrentCard) || (pAnalyseItem->cbCardData[i][1] == cbCurrentCard) ||
					(pAnalyseItem->cbCardData[i][2] == cbCurrentCard))
				{
					BYTE tempVaule = GetCardValue(pAnalyseItem->cbCardData[i][0]);
					bExist[tempVaule] = bExist[tempVaule] + 1;
				}

			}

		}

		for (BYTE i = 0; i < cbWeaveCount;i++)
		{
			if (WeaveItem[i].cbWeaveKind & (WIK_LEFT | WIK_CENTER | WIK_RIGHT) != 0)
			{
				BYTE tempVaule = GetCardValue(WeaveItem[i].cbCardData[0]);
				bExist[tempVaule] = bExist[tempVaule] - 1;
			}
		}


		bool bSpeical1 = true;
		if ((currentCardVule > 2) && (bExist[currentCardVule - 2] > 0))
		{
			bSpeical1 = false;
		}
		bool bSpeical2 = true;
		if ((currentCardVule > 2) && (bExist[currentCardVule - 2] <= 0))
		{
			bSpeical2 = false;
		}
		if
			(bExist[currentCardVule - 1] >= 1 && bExist[currentCardVule] <= 0 && bSpeical1 &&
				cbCardIndex[currentCardVule] <= 3)
		{
			//--456, 吃5情况
			return true;
		}

		else if (bExist[currentCardVule - 1] >= 1 && bExist[currentCardVule] <= 0 && bSpeical2)
		{
			//--123, 234, 吃3情况
			if (currentCardVule == 3)
				return true;
			else
				return false;
		}

		else if (bExist[currentCardVule - 1] >= 1 && bExist[currentCardVule] >= 1 && bSpeical1)
		{
			//--678, 789, 吃7情况
			if (currentCardVule == 7)
				return true;
			else
				return false;
		}
		else
			return false;

	}
	return false;
}

bool CGameLogic::IsDanDiaoJiang(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	//--单钓将：钓单张牌作将成胡
	BYTE cbCurrentCardIndex = SwitchToCardIndex(cbCurrentCard);
	if (pAnalyseItem->cbCardEye == cbCurrentCard && cbCardIndex[SwitchToCardIndex(cbCurrentCard)] == 2)
	{
		if (cbCurrentCard < 0x30)
		{
			BYTE currentCardVule = GetCardValue(cbCurrentCard);
			if (cbCardIndex[cbCurrentCardIndex] != 2)
			{
				return false;
			}
			if
				(currentCardVule >= 4 && cbCardIndex[cbCurrentCardIndex - 3] > 0 &&
					cbCardIndex[cbCurrentCardIndex - 2] > 0 &&
					cbCardIndex[cbCurrentCardIndex - 1] > 0)
			{
				return false;
			}
			if
				(currentCardVule <= 6 && cbCardIndex[cbCurrentCardIndex + 3] > 0 &&
					cbCardIndex[cbCurrentCardIndex + 2] > 0 &&
					cbCardIndex[cbCurrentCardIndex + 1] > 0)
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

bool CGameLogic::IsDuanYao(const tagAnalyseItem *pAnalyseItem)
{
	//--断幺：牌里没有一、九及字牌
	if (pAnalyseItem->cbCardEye > 0x30 || GetCardValue(pAnalyseItem->cbCardEye) == 9 || GetCardValue(pAnalyseItem->cbCardEye) == 1)
	{
		return false;
	}
	for (BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++)
	{
		for (BYTE j = 0; j < CountArray(pAnalyseItem->cbCardData[i]); j++)
		{
			if (pAnalyseItem->cbCardData[i][j] > 0x30 || GetCardValue(pAnalyseItem->cbCardData[i][j]) == 1 ||
				GetCardValue(pAnalyseItem->cbCardData[i][j]) == 9)
			{
				return false;
			}
		}

	}

	return true;
}

bool CGameLogic::IsYiSeSanTongShun(const tagAnalyseItem *pAnalyseItem)
{
	//--一色三同顺：牌里有一种花色且序数相同的3副顺子（包括吃牌）；（不计算一色三节高，一般高的番数）例：123 123 123
	for (BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++)
	{
		int ShunCount = 0;
		if ((pAnalyseItem->cbWeaveKind[i] & (WIK_LEFT | WIK_CENTER | WIK_RIGHT)) != 0)
		{
			for (BYTE j = 0; j < CountArray(pAnalyseItem->cbWeaveKind); j++)
			{
				if ((i != j) && (pAnalyseItem->cbWeaveKind[j] & (WIK_LEFT | WIK_CENTER | WIK_RIGHT)) != 0)
				{
					if ((pAnalyseItem->cbCardData[i][0]) == pAnalyseItem->cbCardData[j][0])
					{
						ShunCount = ShunCount + 1;
					}
				}
			}

		}
		if (ShunCount == 2)
			return true;
	}
	return false;
}

bool CGameLogic::IsYiBanGao(const tagAnalyseItem *pAnalyseItem)
{
	//--一般高：由一种花色，2副相同的顺子组成的牌，如：112233或223344
	for (BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++)
	{
		if ((pAnalyseItem->cbWeaveKind[i] & (WIK_LEFT | WIK_CENTER | WIK_RIGHT)) != 0)
		{
			for (BYTE j = 0; j < CountArray(pAnalyseItem->cbWeaveKind); j++)
			{
				if ((i != j) && (pAnalyseItem->cbWeaveKind[j] & (WIK_LEFT | WIK_CENTER | WIK_RIGHT)) != 0)
				{
					if (pAnalyseItem->cbCardData[i][0] == pAnalyseItem->cbCardData[j][0])
						return true;
				}

			}

		}
	}
	return false;
}

BYTE CGameLogic::GetCallCard(const BYTE cardIndex[MAX_INDEX], BYTE callCard)
{
	if (callCard==0)
	{
		return 0;
	}
	BYTE tempCard[MAX_COUNT] = { 0 };
	SwitchToCardData(cardIndex, tempCard);
	BYTE tempCardData = 0;
	for (int i = 0; i < MAX_COUNT; i++)
	{
		if (tempCard[i] != 0)
		{
			if (GetCardColor(tempCard[i]) == GetCardColor(callCard))
			{
				tempCardData = tempCard[i];
				break;
			}
		}

	}
	return tempCardData;
}

void CGameLogic::GetACardShengYuCount(BYTE cbDiscardCard[], BYTE cbDiscardCount, BYTE cbCardIndex[])
{
	//混乱准备
	BYTE cbCardDataTemp[MAX_REPERTORY];
	CopyMemory(cbCardDataTemp, m_cbCardDataArray, sizeof(m_cbCardDataArray));
	SwitchToCardIndex(cbCardDataTemp, MAX_REPERTORY, cbCardIndex);
	RemoveCard(cbCardIndex, cbDiscardCard, cbDiscardCount);
}

void CGameLogic::CombinatoryAnalysisEx(const BYTE cbCardIndex[MAX_INDEX], vector<tagAnalyseItem>& AnalyseItemArray, const tagWeaveItem WeaveItem[], BYTE cbWeaveICount)
{
	BYTE cbCardData[MAX_COUNT] = { 0 };
	BYTE cbCardCount = 0;
	BYTE cbMagicData[MAX_MAGIC_COUNT] = { 0 };
	BYTE cbMagicDataCount = 0;
	for (int i = 0; i < MAX_MAGIC_COUNT; i++)
	{
		if ((m_cbMagicIndex[i] != INDEX_REPLACE_CARD) && (m_cbMagicIndex[i] != MAX_INDEX))
		{
			cbMagicData[i] = SwitchToCardData(m_cbMagicIndex[i]);
			cbMagicDataCount++;
		}
	}
	SwitchToCardData(cbCardIndex, cbCardData);
	cbCardCount = GetCardCount(cbCardIndex);
	AnalyseItemArray = m_fastHu.lhu_pai(cbCardData, cbCardCount, cbMagicData, cbMagicDataCount, WeaveItem, cbWeaveICount, G_ZI_SHUN);
}

bool CGameLogic::IsQingYaoJiu(tagAnalyseItem * pAnalyseItem)
{
	//扑克判断
	if (GetCardValue(pAnalyseItem->cbCardEye) == 1 || GetCardValue(pAnalyseItem->cbCardEye) == 9)
	{

	}
	else{
		return false;
	}
	for (BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++)
	{
		if (GetCardValue(pAnalyseItem->cbCenterCard[i]) == 1 || GetCardValue(pAnalyseItem->cbCenterCard[i]) == 9)
		{

		}
		else{
			return false;
		}
	}
	return true;
}

bool CGameLogic::IsYaoJiu(tagAnalyseItem * pAnalyseItem)
{
	//扑克判断
	if (pAnalyseItem->cbCardEye > 0x30 || GetCardValue(pAnalyseItem->cbCardEye) == 1 || GetCardValue(pAnalyseItem->cbCardEye) == 9)
	{

	}
	else{
		return false;
	}
	for (BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++)
	{
		if (pAnalyseItem->cbCenterCard[i] > 0x30 || GetCardValue(pAnalyseItem->cbCenterCard[i]) == 1 || GetCardValue(pAnalyseItem->cbCenterCard[i]) == 9)
		{

		}
		else{
			return false;
		}
	}

	return true;
}

CChiHuRight CGameLogic::ClearRepeateFan(CChiHuRight chrAll)
{
	if ((chrAll & CHR_SAN_QI_DA_DUI) != 0)
	{
		chrAll = chrAll & (~CHR_QI_DUI);
		chrAll = chrAll & (~CHR_QI_DA_DUI);
		chrAll = chrAll & (~CHR_SHUANG_QI_DA_DUI);
	}
	else if ((chrAll & CHR_SHUANG_QI_DA_DUI) != 0)
	{
		chrAll = chrAll & (~CHR_QI_DUI);
		chrAll = chrAll & (~CHR_QI_DA_DUI);
	}
	else if ((chrAll & CHR_QI_DA_DUI) != 0)
	{
		chrAll = chrAll & (~CHR_QI_DUI);
	}
	uint64		dwChiHuRight[MAX_RIGHT_COUNT];
	chrAll.GetRightData(dwChiHuRight, MAX_RIGHT_COUNT);
	if (((dwChiHuRight[0] > CHR_PING_HU) && (dwChiHuRight[0] < CHR_TIAN_HU)) || ((chrAll & (CHR_QUAN_QIU_REN | CHR_QI_DUI)) != 0))
	{
		chrAll = chrAll & (~CHR_PING_HU);
	}
	return chrAll;
}

int CGameLogic::GetUserHuFan(CChiHuRight chr, int lianGang)
{
	if (m_userRule[9])
	{
		return GetCommonFan(chr, lianGang);
	}
	else {
		return 1;
	}
	
}

int CGameLogic::GetCommonFan(CChiHuRight chr, int lianGang)
{
	int fan = 1;

	if ((chr & CHR_PING_HU) != 0)
	{
		fan += 1;
	}
	if ((chr & CHR_DUAN_YAO) != 0)
	{
		fan += 2;
	}
	if ((chr & CHR_DAN_DIAO_JIANG) != 0)
	{
		fan += 1;
	}
	
	if ((chr & CHR_YI_BAN_GAO) != 0)
	{
		fan += 6;
	}
	if ((chr & CHR_PENG_PENG) != 0)
	{
		fan += 10;
	}
	if ( (chr & CHR_HUN_YI_SE) != 0)
	{
		fan += 10;
	}
	if ((chr & CHR_QI_DUI) != 0 || (chr & CHR_QING_YI_SE) != 0)
	{
		fan += 12;
	}
	if ((chr & CHR_QI_DA_DUI) != 0 || (chr & CHR_HUN_YAO_JIU) != 0)
	{
		fan += 6;
	}
	if ((chr & CHR_SHUANG_QI_DA_DUI) != 0 || (chr & CHR_ZI_YI_SE) != 0)
	{
		fan += 20;
	}
	if ((chr & CHR_DA_SAN_YUAN) != 0)
	{
		fan += 25;
	}
	if ((chr & CHR_DA_SI_XI) != 0)
	{
		fan += 50;
	}

	///特殊胡

	if ((chr & (CHR_QIANG_GANG_HU)) != 0 || (chr & CHR_GANG_SHANG_HUA) != 0)
	{
		fan += 1;
	}
	if ((chr & CHR_HAI_DI_LAO_YUE) != 0)
	{
		fan += 1;
	}
	if ((chr & CHR_TIAN_HU) != 0)
	{
		fan += 24;
	}
	if ((chr & CHR_DI_HU) != 0)
	{
		fan += 12;
	}

	return fan;
}

int CGameLogic::GetColorCount(const BYTE cbCardIndex[MAX_INDEX], int color)
{
	//--1是万, 2, 是条, 3是筒, 4是字
	int count = 0;
	for (int k = (color)* 9; k <MAX_INDEX - MAX_HUA_COUNT; k++)
	{
		if (k < (color + 1) * 9)
		{
			count = count + cbCardIndex[k];
		}
	}
	return count;
}



std::string CGameLogic::GetFanString(CChiHuRight chr)
{
	uint64 tempCount = 0;
	uint64 di = 2;
	uint64 tempFanType = pow(di, 0);
	string str;
	while (tempFanType <= CHR_QI_LIAN_DUI)
	{
		//uint64 tempV = (chr & tempFanType);
		if ((chr & tempFanType) != 0)
		{
			str = str += FanString[tempFanType]+",";
		}
			
		tempCount = tempCount + 1;
		tempFanType = pow(di,tempCount);
	}
	return str;
}

//鸡胡
bool CGameLogic::IsJiHu(const tagAnalyseItem *pAnalyseItem)
{
	bool bPeng = false,bLian = false;
	for(BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++)
	{
		if(pAnalyseItem->cbWeaveKind[i]&(WIK_PENG|WIK_GANG))
			bPeng = true;
		else 
			bLian = true;
	}

	return bPeng&&bLian;
}

//平胡
bool CGameLogic::IsPingHu(const tagAnalyseItem *pAnalyseItem)
{
	//检查组合
	for(BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++)
	{
		if(pAnalyseItem->cbWeaveKind[i]&(WIK_PENG|WIK_GANG)) return false;
	}

	return true;
}


bool CGameLogic::IsQingYiSe(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveICount)
{
	//变量定义
	BYTE  cbCardColor=0xFF;
	for(BYTE i=0;i<MAX_INDEX;i++)
	{
		if (i == m_cbMagicIndex[0] || i == m_cbMagicIndex[1])
		{
			continue;
		}
		if( cbCardIndex[i]>0) 
		{
			BYTE cbColor = SwitchToCardData(i)&MASK_COLOR;
			if(cbCardColor == 0xFF)
				cbCardColor = cbColor;
			else if(cbColor != cbCardColor)
				return false;
		}
	}
	ASSERT(cbCardColor != 0xFF);

	for(BYTE i=0;i<cbWeaveICount;i++)
	{
		if( (WeaveItem[i].cbCenterCard&MASK_COLOR) != cbCardColor)
			return false;
	}

	return true;
}



bool CGameLogic::IsHunYiSe(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveICount)
{
	BYTE cbColorCount[4] = {0};
	for(BYTE i=0;i<MAX_INDEX;i++)
	{
		if (i == m_cbMagicIndex[0] || i == m_cbMagicIndex[1])
		{
			continue;
		}
		if(cbCardIndex[i]>0)
		{
			BYTE cbColor = (SwitchToCardData(i)&MASK_COLOR)>>4;
			if(0 == cbColorCount[cbColor])
			{
				cbColorCount[cbColor] = 1;
			}
		}
	}
	for(BYTE i=0;i<cbWeaveICount;i++)
	{
		BYTE cbColor = (WeaveItem[i].cbCenterCard&MASK_COLOR)>>4;
		if(0 == cbColorCount[cbColor])
		{
			cbColorCount[cbColor] = 1;
		}
	}

	if(cbColorCount[0] + cbColorCount[1] + cbColorCount[2] == 1 && cbColorCount[3] == 1)
	{
		return true;
	}

	return false;
}

//字一色牌
bool CGameLogic::IsZiYiSe(tagAnalyseItem * pAnalyseItem)
{
	//扑克判断
	for (BYTE i=0;i<CountArray(pAnalyseItem->cbWeaveKind);i++)
	{
		if ((pAnalyseItem->cbCenterCard[i]&MASK_COLOR)!=0x30)
			return false;
	}

	return true;
}

bool CGameLogic::IsZiYiSe(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveICount)
{
	//变量定义
	for(BYTE i=0;i<MAX_INDEX;i++)
	{
		if( cbCardIndex[i]>0) 
		{
			BYTE cbColor = SwitchToCardData(i)&MASK_COLOR;
			if(cbColor != 0x30)
				return false;
		}
	}

	for(BYTE i=0;i<cbWeaveICount;i++)
	{
		if( (WeaveItem[i].cbCenterCard&MASK_COLOR) != 0x30)
			return false;
	}

	return true;
}

//大三元牌
bool CGameLogic::IsDaSanYuan(tagAnalyseItem * pAnalyseItem)
{
	bool bExist[3]={false,false,false};
	for (BYTE i=0;i<CountArray(pAnalyseItem->cbWeaveKind);i++)
	{
		if (pAnalyseItem->cbCenterCard[i]==0x35) bExist[0]=true;
		if (pAnalyseItem->cbCenterCard[i]==0x36) bExist[1]=true;
		if (pAnalyseItem->cbCenterCard[i]==0x37) bExist[2]=true;
	}

	if ((bExist[0]==true)&&(bExist[1]==true)&&(bExist[2]==true)) 
		return true;

	return false;
}



bool CGameLogic::IsXiaoSanYuan(const BYTE cbCardIndex[MAX_INDEX],tagAnalyseItem * pAnalyseItem)
{
	if(cbCardIndex[31]>=2 && cbCardIndex[32]>=2 && cbCardIndex[33]>=2)
	{
		BYTE nCount = 0;
		for (BYTE i=0;i<CountArray(pAnalyseItem->cbWeaveKind);i++)
		{
			if (pAnalyseItem->cbCenterCard[i]==0x35) nCount++;
			if (pAnalyseItem->cbCenterCard[i]==0x36) nCount++;
			if (pAnalyseItem->cbCenterCard[i]==0x37) nCount++;
		}

		if(nCount == 2)
		{
			return true;
		}
	}

	BYTE cbCount = 0,cbEye = 0;
	for (BYTE i=0;i<CountArray(pAnalyseItem->cbWeaveKind);i++)
	{
		if ((pAnalyseItem->cbCenterCard[i]==0x35) || (pAnalyseItem->cbCenterCard[i]==0x36) || (pAnalyseItem->cbCenterCard[i]==0x37)) 
			cbCount++;
	}
	if(cbCount !=2 )
		return false;

	cbEye = pAnalyseItem->cbCardEye;
	if((cbEye==0x35)||(cbEye==0x36)||(cbEye==0x37))
		cbCount++;

	return (cbCount==3);
}
//大四喜
bool CGameLogic::IsDaSiXi(tagAnalyseItem * pAnalyseItem)
{
	bool bExist[4]={false,false,false,false};
	for (BYTE i=0;i<CountArray(pAnalyseItem->cbWeaveKind);i++)
	{
		if (pAnalyseItem->cbCenterCard[i]==0x31) bExist[0]=true;
		if (pAnalyseItem->cbCenterCard[i]==0x32) bExist[1]=true;
		if (pAnalyseItem->cbCenterCard[i]==0x33) bExist[2]=true;
		if (pAnalyseItem->cbCenterCard[i]==0x34) bExist[3]=true;
	}

	if ((bExist[0]==true)&&(bExist[1]==true)&&(bExist[2]==true)&&(bExist[3]==true)) 
		return true;

	return false;
}


//小四喜
bool CGameLogic::IsXiaoSiXi(tagAnalyseItem * pAnalyseItem)
{
	BYTE cbCount = 0,cbEye = 0;
	for (BYTE i=0;i<CountArray(pAnalyseItem->cbWeaveKind);i++)
	{
		if ((pAnalyseItem->cbCenterCard[i]==0x31)||(pAnalyseItem->cbCenterCard[i]==0x32)||
			(pAnalyseItem->cbCenterCard[i]==0x33)||(pAnalyseItem->cbCenterCard[i]==0x34)) 
			cbCount++;
	}
	if(cbCount !=3 )
		return false;

	cbEye = pAnalyseItem->cbCardEye;
	if((cbEye==0x31)||(cbEye==0x32)||(cbEye==0x33)||(cbEye==0x34))
		cbCount++;
	return (cbCount==4);
}
//十三幺
bool CGameLogic::IsShiSanYao( const BYTE cbCardIndex[MAX_INDEX], BYTE cbWeaveCount)
{
	//检查吃碰杠
	if( cbWeaveCount != 0 ) return false;

	//变量定义
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory( cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp) );

	BYTE cbMagicCount = GetMajicCount(cbCardIndexTemp);
	if(cbMagicCount >0)
	{
		if(m_cbMagicIndex[0] != MAX_INDEX)
			cbCardIndexTemp[m_cbMagicIndex[0]]=0;
		if(m_cbMagicIndex[1] != MAX_INDEX)
			cbCardIndexTemp[m_cbMagicIndex[1]]=0;
	}
	//扑克判断
	bool bCardEye=false;

	//一九判断
	for (BYTE i=0;i<MAX_INDEX-7;i+=9)
	{
		//无效判断
		if (cbCardIndexTemp[i]==0)
		{
			if(cbMagicCount >= 1)
				cbMagicCount--;
			else
				return false;
		}
		if (cbCardIndexTemp[i+8]==0)
		{
			if(cbMagicCount >= 1)
				cbMagicCount--;
			else
				return false;
		}

		//牌眼判断
		if ((bCardEye==false)&&(cbCardIndexTemp[i]==2)) bCardEye=true;
		if ((bCardEye==false)&&(cbCardIndexTemp[i+8]==2)) bCardEye=true;
	}

	//番子判断
	for (BYTE i=MAX_INDEX-7;i<MAX_INDEX;i++)
	{
		if (cbCardIndexTemp[i]==0)
		{
			if(cbMagicCount >= 1)
				cbMagicCount--;
			else
				return false;
		}
		if ((bCardEye==false)&&(cbCardIndexTemp[i]==2)) bCardEye=true;
	}

	if(bCardEye == false && cbMagicCount>0)
		bCardEye=true;

	return bCardEye;
}

VOID CGameLogic::SetUserRule(BYTE bRule[])
{
	
		CopyMemory(m_userRule, bRule, sizeof(m_userRule));
}

VOID CGameLogic::SetBanZiMO(bool bBanZiMo)
{
}

VOID CGameLogic::SetYouJinBiYou(bool bEanble)
{
}

BYTE CGameLogic::GenerateShunZi(BYTE cbCardData,bool bAdd,int space)
{

	BYTE tempData=0;
	BYTE cbValue=GetCardValue(cbCardData);
	BYTE cbColor = GetCardColor(cbCardData);
	if(cbCardData >= 0x35) 
	{
		if (bAdd==true) 
			tempData = 0x35+(cbValue-5+space)%3 ;
		else
			tempData = 0x35+(cbValue-5-space+3)%3 ;
	}
    else if(cbCardData >= 0x30)
	{
		if (bAdd==true)
			tempData = 0x31+(cbValue-1+space)%4;
		else
			tempData = 0x31+(cbValue-1-space+4)%4;
	}

	else
	{
		if (bAdd==true )
			tempData = cbColor+(cbValue-1+space)%9+1 ;
		else
			tempData = cbColor+(cbValue-1-space+9)%9+1 ;
	}

	return tempData;

}
int CGameLogic::GetHuaCardCount(const BYTE cbCardIndex[MAX_INDEX])
{
	int  cbHuaCardCount = 0;
	for (int i = MAX_INDEX - MAX_HUA_COUNT; i <= MAX_HUA_INDEX;i++)
		if (cbCardIndex[i] > 0) {

		cbHuaCardCount = cbHuaCardCount + cbCardIndex[i];

		}
	return cbHuaCardCount;
}


int CGameLogic::ChouGuanCount(int score)
{
      if (score>=5)
      {
          score=(score-3)*score;
      }
	  return score;
}

bool CGameLogic::IsCanHuPai(int beiShu,const BYTE cbCardIndex[MAX_INDEX], bool fangPao/*=false*/)
{
	bool bEnable=true;
	
	return bEnable;
}

//////////////////////////////////////////////////////////////////////////////////
