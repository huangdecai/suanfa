#ifndef CMD_SPARROW_HEAD_FILE
#define CMD_SPARROW_HEAD_FILE
typedef	unsigned	__int64		uint64;
///////////////////////功能开关
#define G_SHOW_TING_DATA	  TRUE		//听牌辅助信息

#define G_SHOW_HU_DATA		 TRUE			//胡字提示

#define G_CAN_CHI_CARD 		 TRUE			//吃牌

#define G_CAN_TING_CARD		 FALSE			//听牌

#define G_QIANG_GANG_HU		 TRUE			//抢杠胡

#define G_GANG_SHANG_PAO	 FALSE			//杠上炮

#define G_CHI_HU						 TRUE			//吃胡

#define G_ZI_SHUN					FALSE			//字牌成顺 风牌：东南西北任意三个成顺 箭牌：中发白成顺

#define G_GEN_ZHUANG			TRUE			//跟庄

#define G_ONLY_ONE_HU			TRUE			//只能一个人胡

#define G_HUANG_ZHUANG_CARD_NUM			17			//荒庄牌数
//////////////////////////////////////////////////////////////////////////

#define KIND_ID						302									//游戏 I D
#define GAME_NAME					TEXT("血战麻将")					//游戏名字

//组件属性
#define GAME_PLAYER					4									//游戏人数
#define MAX_HUA_INDEX			41									//花牌索引
#define MAX_HUA_COUNT			8									//花牌个数
#define MAX_MA_COUNT			8									//最大抓码数
#define ZI_PAI_COUNT	7								//花牌个数

//常量定义
#define MAX_WEAVE					4									//最大组合
#define MAX_INDEX					42									//最大索引
#define MAX_COUNT					14									//最大数目
#define MAX_REPERTORY				136									//最大库存

#define MAX_MAGIC_COUNT            2
//扑克定义
#define HEAP_FULL_COUNT				26									//堆立全牌

#define MAX_RIGHT_COUNT				1									//最大权位DWORD个数	
#define MAX_CHANGE_CARDCOUNT		3									//--换三张牌张数
//--对局流水清单
#define MAX_CHART_COUNT             40                                 //
#define MAX_RECORD_COUNT            32                                 //
//--换三张
#define CLOCKWISE_CHANGE			0									//--顺时针换牌
#define ANTI_CLOCKWISE_CHANGE		1									//--逆时针换牌
#define OPPOSITE_CHANGE				2									//--对家换牌
#define MAX_CHANGE_TYPE				3									//--换三张方式个数
#define MAX_CHANGE_CARDCOUNT		3									//--换三张牌张数
//缺门定义
#define M_WAN						0x00
#define M_SUO						0x10
#define M_TONG						0x20
//////////////////////////////////////////////////////////////////////////
//数组维数
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))
#define ASSERT(f)    ((void)0)
#define VERIFY(f)          ((void)(f))
//组合子项
//组合子项
struct tagWeaveItem
{
	BYTE							cbWeaveKind;						//组合类型
	BYTE							cbCenterCard;						//中心扑克
	BYTE							cbPublicCard;						//公开标志
	WORD							wProvideUser;						//供应用户
	BYTE							cbCardData[4];								//麻将数据
};

//杠牌结果
struct tagGangCardResult
{
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[4];						//扑克数据
	int                             GangKind;
};


//分析子项
struct tagAnalyseItem
{
	BYTE							cbCardEye;							//牌眼扑克
	bool                          bMagicEye;                          //牌眼是否是王霸
	BYTE							cbWeaveKind[MAX_WEAVE];				//组合类型
	BYTE						    cbCenterCard[MAX_WEAVE];			//中心扑克
	BYTE                        cbCardData[MAX_WEAVE][4];           //实际扑克
};
//听牌辅助信息
struct CMD_S_TING_DATA
{
	//出哪几张能听
	BYTE							cbOutCardCount;
	BYTE							cbOutCardData[MAX_COUNT];
	//听后能胡哪几张牌
	BYTE							cbHuCardCount[MAX_COUNT];
	BYTE							cbHuCardData[MAX_COUNT][HEAP_FULL_COUNT];
	//胡牌剩余数
	BYTE							cbHuCardRemainingCount[MAX_COUNT][HEAP_FULL_COUNT];
	//胡牌翻数
	BYTE							cbHuFan[MAX_COUNT][HEAP_FULL_COUNT];
};
//听牌辅助信息
struct CMD_S_ER_XING_TING_DATA
{
	//出哪几张能听
	BYTE							cbOutCardCount;
	BYTE							cbOutCardData[200];
	//听后能胡哪几张牌
	BYTE							cbHuCardCount[200];
	BYTE							cbHuCardData[200][HEAP_FULL_COUNT];
	//胡牌剩余数
	BYTE							cbHuCardRemainingCount[200][HEAP_FULL_COUNT];
	//胡牌翻数
	BYTE							cbHuFan[200][HEAP_FULL_COUNT];
};
//////////////////////////////////////////////////////////////////////////
//出牌结果
struct tagInPyhonNew
{
	BYTE							cbHandCardData[20];
	BYTE							cbHandCardCount;
	BYTE							DiscardCard[144];
	BYTE                            cbDiscardCardCount;
	BYTE							cbWeaveKind[GAME_PLAYER*MAX_WEAVE];						//组合类型
	BYTE							cbCenterCard[GAME_PLAYER*MAX_WEAVE];						//中心扑克
	BYTE							cbPublicCard[GAME_PLAYER*MAX_WEAVE];						//公开标志
	BYTE							wProvideUser[GAME_PLAYER*MAX_WEAVE];						//供应用户
	BYTE                            cbWeaveCount[GAME_PLAYER];
	BYTE                            cbCardDataEx[20];
	BYTE                            wMeChairId;
	BYTE                            wCurrentUser;
	BYTE                            cbActionMask;
	BYTE                            cbActionCard;
	BYTE							cbOperateCode;						//扑克数目
	BYTE							cbResultCard[20];			//结果扑克
};
//定缺结果
struct tagInPyhonCallCard
{
	BYTE							cbHandCardData[20];
	BYTE							cbHandCardCount;
	BYTE							cbType;
};
enum CHARTTYPE
{
	    INVALID_CHARTTYPE = 0,
		GUAFENG_TYPE = 22,
		BEIGUAFENG_TYPE = 23,
		XIAYU_TYPE = 24,
		BEIXIAYU_TYPE = 25,
		DIANPAO_TYPE = 26,
		BEIDIANPAO_TYPE = 27,
		ZIMO_TYPE = 28,
		BEIZIMO_TYPE = 29,
		HUJIAOZHUANYI_TYPE = 30,
		TUISHUI_TYPE = 31,
		CHAHUAZHU_TYPE = 32,
		CHADAJIAO_TYPE = 33,
};

//出牌结果
struct tagOutCardResult
{
	BYTE							cbOperateCode;						//出牌代码
	BYTE							cbOperateCard;						//出牌值
};

//听牌分析结果
struct tagTingCardResult
{
	BYTE bAbandonCount;													//可丢弃牌数
	BYTE bAbandonCard[MAX_COUNT];										//可丢弃牌
	BYTE bTingCardCount[MAX_COUNT];										//听牌数目
	BYTE bTingCard[MAX_COUNT][MAX_COUNT - 1];								//听牌
	BYTE bRemainCount[MAX_COUNT];										//听的牌的剩余数
};

//////////////////////////////////////////////////////////////////////////

#endif
