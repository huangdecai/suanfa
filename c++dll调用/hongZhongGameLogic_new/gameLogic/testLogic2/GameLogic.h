#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once
#include "Stdafx.h"
#include <vector>
#include <map>
using namespace std;
//////////////////////////////////////////////////////////////////////////
#include "fastHu.h"

//用于财神的转换，如果有牌可以代替财神本身牌使用，则设为该牌索引，否则设为MAX_INDEX. 注:如果替换牌是序数牌,将出错.
#define	INDEX_REPLACE_CARD					MAX_INDEX//33 (有些地方是白板变金的就是33
   
//////////////////////////////////////////////////////////////////////////
//逻辑掩码

#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE						0x0F								//数值掩码

//////////////////////////////////////////////////////////////////////////
//动作定义

//动作类型
#define WIK_GANERAL					0x00								//普通操作
#define WIK_MING_GANG				0x01								//明杠（碰后再杠）
#define WIK_FANG_GANG				0x02								//放杠
#define WIK_AN_GANG					0x03								//暗杠

//动作标志
#define WIK_NULL					0x00								//没有类型
#define WIK_LEFT					0x01								//左吃类型
#define WIK_CENTER				0x02								//中吃类型
#define WIK_RIGHT				0x04								//右吃类型
#define WIK_PENG					0x08								//碰牌类型
#define WIK_GANG				0x10								//杠牌类型
#define WIK_LISTEN				0x20								//听牌类型
#define WIK_CHI_HU				0x40								//吃胡类型
#define WIK_FANG_PAO		0x80								//放炮

#define WIK_YOU_JIN		0x01								//游金
#define WIK_ER_YOU		0x02								//二游
#define WIK_SAN_YOU		0x04								//三游
//#define WIK_QIANG_JIN		0x08								//二游
#define WIK_SAN_JIN_DAO		0x10								//三游
//////////////////////////////////////////////////////////////////////////
//胡牌定义
#define CHK_MASK_SMALL			0x000000FF							//小胡掩码
#define CHK_MASK_BIG				0xFFFFFF00							//大胡掩码


//大胡牌型

//1番牌型
#define CHR_YI_BAN_GAO  0x0000000000000001					//		--//一般高
#define CHR_LIAN_LIU  0x0000000000000002					//		--//连六
#define CHR_LAO_SHAO_FU  0x0000000000000004				//		    --//老少副
#define CHR_YAO_JIU_KE  0x0000000000000008					//		--幺九刻
#define CHR_SHI_SAN_YAO  0x0000000000000010					//		--//明杠
#define CHR_BIAN_ZHANG  0x0000000000000020					//		--//边张
#define CHR_KAN_ZHANG  0x0000000000000040					//		--//坎张
#define CHR_DAN_DIAO_JIANG  0x0000000000000080				//			--//单钓将
#define CHR_ZI_MO  0x0000000000000100						//	    --//自摸
#define CHR_ER_WU_BA_JIANG  0x0000000000000200				//			--//二五八将	
#define CHR_YAO_JIU_TOU  0x0000000000000400				//			--//幺九头
//2番牌型													 //
#define CHR_JIAN_KE  0x0000000000000800					//		--//箭刻
#define CHR_MEN_QIANG_QING  0x0000000000001000				//			--//门前清
#define CHR_PING_HU  0x0000000000002000					//		--//平胡
#define CHR_SI_GUI_YI  0x0000000000004000					//		--//四归一
#define CHR_SHUANG_AN_KE  0x0000000000008000				//			--//双暗刻
#define CHR_AN_GANG  0x0000000000010000					//		--//暗杠
#define CHR_DUAN_YAO  0x0000000000020000					//		--//断幺
#define CHR_TING_PAI  0x0000000000040000					//		--//听牌
//--4番牌型												 //
#define CHR_QUANG_DAI_YAO  0x0000000000080000				//			--//全带幺
#define CHR_BU_QIU_REN  0x0000000000100000					//		--//不求人
#define CHR_SHUANG_MING_GANG  0x0000000000200000			//				--//双明杠
#define CHR_HU_JUE_ZHANG  0x0000000000400000				//			--//胡绝张
//--6番牌型												 //
#define CHR_PENG_PENG  0x0000000000800000					//		--//碰碰胡
#define CHR_HUN_YI_SE  0x0000000001000000					//		--//混一色
#define CHR_QUAN_QIU_REN  0x0000000002000000				//			--//全求人
#define CHR_SHUANG_AN_GANG  0x0000000004000000				//			--//双暗杠
#define CHR_SHUANG_JIAN_KE  0x0000000008000000				//			--//双箭刻
//--8番牌型												 //
#define CHR_MIAO_SHOU_HUI_CHUN  0x0000000010000000			//				--//妙手回春
#define CHR_HAI_DI_LAO_YUE  0x0000000020000000				//			--//海底捞月
#define CHR_GANG_SHANG_HUA  0x0000000040000000				//			--//杠上开花
#define CHR_QIANG_GANG_HU  0x0000000080000000				//			--//抢杠胡
//--12番牌型												 //
#define CHR_QI_DA_DUI  0x0000000100000000					//		--//大于五
#define CHR_SHUANG_QI_DA_DUI  0x0000000200000000					//		--//小于五
#define CHR_SAN_FENG_KE  0x0000000400000000				//			--//三风刻
//--16番牌型												 //
#define CHR_TIAN_TING  0x0000000800000000					//		--//天听
#define CHR_QING_LONG  0x0000001000000000					//		--//清龙
#define CHR_YI_SE_SAN_BU_GAO  0x0000002000000000			//				--//一色三步高
#define CHR_SAN_AN_KE  0x0000004000000000					//		--//三暗刻
//--24番牌型												 //
#define CHR_QI_DUI  0x0000008000000000						//	   --//七对
#define CHR_QING_YI_SE  0x0000010000000000					//		--//清一色
#define CHR_YI_SE_SAN_TONG_SHUN  0x0000020000000000		//					--//一色三同顺
#define CHR_YI_SE_SAN_JIE_GAO  0x0000040000000000			//				--//一色三节高
#define CHR_SAN_QI_DA_DUI  0x0000080000000000						//	--//人胡
//--32番牌型												 //
#define CHR_YI_SE_SI_BU_GAO  0x0000100000000000			//				--//一色四步高
#define CHR_SAN_GANG  0x0000200000000000					//		--//三杠
#define CHR_HUN_YAO_JIU  0x0000400000000000				//			--//混幺九
#define CHR_DI_HU  0x0000800000000000						//	--//地胡
//--48番牌型												 //
#define CHR_TIAN_HU  0x0001000000000000					//		--//天胡
#define CHR_YI_SE_SI_TONG_SHUN  0x0002000000000000			//				--//一色四同顺
#define CHR_YI_SE_SI_JIE_GAO  0x0004000000000000			//				--//一色四节高
//--64番牌型												 //
#define CHR_XIAO_SI_XI  0x0008000000000000					//		--//小四喜
#define CHR_XIAO_SAN_YUAN  0x0010000000000000				//			--//小三元
#define CHR_ZI_YI_SE  0x0020000000000000					//		--//字一色
#define CHR_YI_SS_LONG_HUI  0x0040000000000000				//			--//一色双龙会
#define CHR_SI_AN_KE  0x0080000000000000					//		--//四暗刻
//--88番牌型												 //
#define CHR_DA_SI_XI  0x0100000000000000					//		--//大四喜
#define CHR_DA_SAN_YUAN  0x0200000000000000				//			--//大三元
#define CHR_JIU_LIAN_BAO_DENG  0x0400000000000000			//				--//九连宝灯
#define CHR_SI_GANG  0x0800000000000000					//		--//四杠
#define CHR_QI_LIAN_DUI  0x1000000000000000				//			--//七连对
//未使用

//
//#define CHR_SHI_SAN_LAN			    0x04000000							//十三烂
//#define CHR_DA_QI_DUI				0x08000000							//大七对

//////////////////////////////////////////////////////////////////////////////////

//类型子项
struct tagKindItem
{
	BYTE							cbWeaveKind;						//组合类型
	BYTE							cbCenterCard;						//中心扑克
	BYTE							cbValidIndex[3];					//实际扑克索引
	BYTE							cbMagicCount;						//财神牌数
};


//////////////////////////////////////////////////////////////////////////


//#define MASK_CHI_HU_RIGHT			0x0fffffff
#define MASK_CHI_HU_RIGHT			0xffffffffffffffff

/*
//	权位类。
//  注意，在操作仅位时最好只操作单个权位.例如
//  CChiHuRight chr;
//  chr |= (chr_zi_mo|chr_peng_peng)，这样结果是无定义的。
//  只能单个操作:
//  chr |= chr_zi_mo;
//  chr |= chr_peng_peng;
*/
class CChiHuRight
{	
	//静态变量
private:
	static bool						m_bInit;
	static uint64					m_dwRightMask[MAX_RIGHT_COUNT];

	//权位变量
private:
	uint64							m_dwRight[MAX_RIGHT_COUNT];

public:
	//构造函数
	CChiHuRight();

	//运算符重载
public:
	//赋值符
	CChiHuRight & operator = (uint64 dwRight);

	//与等于
	CChiHuRight & operator &= (uint64 dwRight);
	//或等于
	CChiHuRight & operator |= (uint64 dwRight);

	//与
	CChiHuRight operator & (uint64 dwRight);
	CChiHuRight operator & (uint64 dwRight) const;

	//或
	CChiHuRight operator | (uint64 dwRight);
	CChiHuRight operator | (uint64 dwRight) const;

	//相等
	bool operator == (uint64 dwRight) const;
	bool operator == (const CChiHuRight chr) const;

	//不相等
	bool operator != (uint64 dwRight) const;
	bool operator != (const CChiHuRight chr) const;

	//功能函数
public:
	//是否权位为空
	bool IsEmpty();

	//设置权位为空
	void SetEmpty();

	//获取权位数值
	int GetRightData(uint64 dwRight[], BYTE cbMaxCount);

	//设置权位数值
	bool SetRightData(const uint64 dwRight[], BYTE cbRightCount);

private:
	//检查权位是否正确
	bool IsValidRight(uint64 dwRight);
};

//////////////////////////////////////////////////////////////////////////////////

//数组说明


//游戏逻辑类
class CGameLogic
{
	//变量定义
protected:
	static const BYTE				m_cbCardDataArray[MAX_REPERTORY];	//扑克数据
	BYTE									m_cbMagicIndex[2];						//钻牌索引
	BOOL								m_bHuQiDui;									//能否胡七对
	fastHu                              m_fastHu;
	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//类型函数
public:
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&MASK_COLOR; }

public:
	//混乱扑克
	VOID RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//混乱扑克
	VOID RandCardList(BYTE cbCardData[], BYTE cbCardBuffer[], BYTE cbBufferCount);
	VOID JianYiSeCardList(BYTE cbCardData[], BYTE cardIndex);
	//删除扑克
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], const BYTE cbRemoveCard[], BYTE cbRemoveCount);
	//删除扑克
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard);
	//排序,根据牌值排序
	bool SortCardList(BYTE cbCardData[MAX_COUNT], BYTE cbCardCount);
	//删除扑克
	bool RemoveCard(BYTE cbCardData[], BYTE cbCardCount, const BYTE cbRemoveCard[], BYTE cbRemoveCount);
	//设置财神
	void SetMagicIndex( BYTE cbMagicIndex[] ); 
	//财神判断
	bool IsMagicCard(BYTE cbCardData);
	//获取财神个数
	BYTE GetMajicCount(const BYTE cbCardIndex[MAX_INDEX]);
	//花牌判断
	bool IsHuaCard(BYTE cbCardData);
	//花牌判断
	BYTE IsHuaCard(BYTE cbCardIndex[MAX_INDEX]);
	//设置自定义规则
	VOID SetUserRule(BOOL bHuQiDui);
	VOID SetBanZiMO(BOOL bBanZiMo);
	VOID SetYouJinBiYou(BOOL bEanble);
	//等级函数
public:
	//动作等级
	BYTE GetUserActionRank(BYTE cbUserAction);
	//胡牌等级
	WORD GetChiHuActionRank(const CChiHuRight & ChiHuRight);

	//自动出牌
	BYTE AutomatismOutCard(const BYTE cbCardIndex[MAX_INDEX], const BYTE cbEnjoinOutCard[MAX_COUNT], BYTE cbEnjoinOutCardCount);

	//动作判断
public:
	//吃牌判断
	BYTE EstimateEatCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
//碰牌判断
BYTE EstimatePengCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
//杠牌判断
BYTE EstimateGangCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);

public://动作判断
//杠牌分析
BYTE AnalyseGangCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, tagGangCardResult & GangCardResult, BYTE cbCurrentCard, BYTE cbCallCard);
//吃胡分析
BYTE AnalyseChiHuCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, CChiHuRight &ChiHuRight);
//听牌分析
BYTE AnalyseTingCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, bool bNoCheckMagic = false);
//获取胡牌数据，听牌后调用
BYTE GetHuCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbHuCardData[]);
//获取听牌数据,打哪些牌能听
BYTE GetTingData(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE& cbOutCardCount, BYTE cbOutCardData[]);
//获取辅助数据，打哪些牌能听，以及能胡哪些牌
BYTE GetTingDataEx(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE& cbOutCardCount, BYTE cbOutCardData[], BYTE cbHuCardCount[], BYTE cbHuCardData[][HEAP_FULL_COUNT], BYTE cbHuFan[][HEAP_FULL_COUNT]);
//二向听
BYTE GetErXiangTingDataEx(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE& cbOutCardCount, BYTE cbOutCardData[], BYTE cbHuCardCount[], BYTE cbHuCardData[][HEAP_FULL_COUNT], BYTE cbHuFan[][HEAP_FULL_COUNT]);
public://转换函数
	//扑克转换
	BYTE SwitchToCardData(BYTE cbCardIndex);
	//扑克转换
	BYTE SwitchToCardIndex(BYTE cbCardData);
	//扑克转换
	BYTE SwitchToCardData(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[MAX_COUNT]);
	//扑克转换
	BYTE SwitchToCardIndex(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardIndex[MAX_INDEX]);

public://辅助函数
	//有效判断
	bool IsValidCard(BYTE cbCardData);
	//扑克数目
	BYTE GetCardCount(const BYTE cbCardIndex[MAX_INDEX]);
	//组合扑克
	BYTE GetWeaveCard(BYTE cbWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4]);
	//江西麻将要用
	bool IsCanHuPai(int beiShu, const BYTE cbCardIndex[MAX_INDEX], bool fangPao = false);
	//生成相隔的牌
	BYTE GenerateShunZi(BYTE cbCardData, bool bAdd, int space);
	//冲关算法
	int ChouGuanCount(int score);
	int GetHuaCardCount(const BYTE cbCardIndex[MAX_INDEX]);

	//金倒
private://内部函数
	bool AddKindItem(tagKindItem &TempKindItem, tagKindItem KindItem[], BYTE &cbKindItemCount, bool &bMagicThree);
	//分析扑克
	bool AnalyseCard(const BYTE cbCardIndex1[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveICount, vector<tagAnalyseItem> & AnalyseItemArray);

protected://胡法分析

	//平胡
	bool IsPingHu(const tagAnalyseItem *pAnalyseItem);
	//鸡胡
	bool IsJiHu(const tagAnalyseItem *pAnalyseItem);
	//清一色
	bool IsQingYiSe(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveICount);
	//混一色
	bool IsHunYiSe(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveICount);
	//字一色牌
	bool IsZiYiSe(tagAnalyseItem * pAnalyseItem);
	bool IsZiYiSe(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveICount);
	//大三元
	bool IsDaSanYuan(tagAnalyseItem * pAnalyseItem);
	//小三元
	bool IsXiaoSanYuan(const BYTE cbCardIndex[MAX_INDEX], tagAnalyseItem * pAnalyseItem);
	//大四喜
	bool IsDaSiXi(tagAnalyseItem * pAnalyseItem);
	//小四喜
	bool IsXiaoSiXi(tagAnalyseItem * pAnalyseItem);
	//十三幺
	bool IsShiSanYao(const BYTE cbCardIndex[MAX_INDEX], BYTE cbWeaveCount);
	//碰碰和
	bool IsPengPeng(const tagAnalyseItem *pAnalyseItem);
	//七对系列
	bool IsQiDui(const BYTE cbCardIndex[MAX_INDEX], BYTE cbWeaveCount);
	//七大对
	bool IsQiDaDui(const BYTE cbCardIndex[MAX_INDEX], BYTE cbWeaveCount);
	//十三烂系列
	bool IsShiSanLan(const BYTE cbCardIndex[MAX_INDEX], BYTE cbWeaveCount);
	//七星
	bool IsQiXing(const BYTE cbCardIndex[MAX_INDEX], BYTE cbWeaveCount);
	//精钓将
	bool IsJingDiaoJiang(const BYTE cbCardIndex[MAX_INDEX], BYTE cbWeaveCount);
	//是否双游金
	bool IsShouShuangYou(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX]);
	//是否是双暗刻
	bool IsShuangAnKe(const tagAnalyseItem *pAnalyseItem, const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, bool bZiMo);
	//是否是边张
	bool IsBianZhang(const tagAnalyseItem *pAnalyseItem, const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard);
	bool IsKanZhang(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard);
	bool IsDanDiaoJiang(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	bool IsDuanYao(const tagAnalyseItem *pAnalyseItem);
	bool IsYiSeSanTongShun(const tagAnalyseItem *pAnalyseItem);
	bool IsYiBanGao(const tagAnalyseItem *pAnalyseItem);
		
	public:
	BYTE GetCallCard(const  BYTE cardIndex[MAX_INDEX], BYTE callCard);
	void GetACardShengYuCount(BYTE cbDiscardCard[], BYTE cbDiscardCount, BYTE cbCardIndex[]);

	void CombinatoryAnalysisEx(const  BYTE cbCardIndex[MAX_INDEX], vector<tagAnalyseItem>& AnalyseItemArray, const tagWeaveItem WeaveItem[], BYTE cbWeaveICount);
	bool IsQingYaoJiu(tagAnalyseItem * pAnalyseItem);
	bool IsYaoJiu(tagAnalyseItem * pAnalyseItem);
	CChiHuRight ClearRepeateFan(CChiHuRight chrAll);
	int GetUserHuFan(CChiHuRight chr, int lianGang=0);
	int GetCommonFan(CChiHuRight chr, int lianGang = 0);

	int GetColorCount(const  BYTE cbCardIndex[MAX_INDEX],  int color);
	string GetFanString(CChiHuRight chr);
		
		
};

//////////////////////////////////////////////////////////////////////////////////

#endif