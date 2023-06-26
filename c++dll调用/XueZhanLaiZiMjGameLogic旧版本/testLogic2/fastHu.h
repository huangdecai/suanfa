#ifndef GAME_FASTHU_HEAD_FILE
#define GAME_FASTHU_HEAD_FILE

#pragma once
#include "Stdafx.h"
//#include "GameLogic.h"
#include <vector>
using namespace std;

#define WEAVE_TYPE_CHI				0x01
#define WEAVE_TYPE_PENG				0x08
#define WEAVE_TYPE_DUI				0xFF				
#define CARD_NUM 					42
#define WEAVE_MAX_NUM 				5
#define MAGIC_INDEX					100
#define MAGIC_MAX_NUM 				(16 + 1)
#define CARD_INDEX(card) 			((((card & 0xF0) >> 4) - 1) * 9 + (card & 0x0F) - 1)
#define CARD_DATA(index) 			(((index / 9 + 1) << 4) | (index % 9 + 1))
#define CAN_CHI(index) 				(index < CARD_INDEX(0x41))
#define CAN_CHI_LOGIC(index)		(!CARD_IS_ZI(index) && CAN_CHI(index) && (index % 9 < 7))
#define CARD_IS_ZI(index) 			(index >= CARD_INDEX(0x41) && index <= CARD_INDEX(0x47))
#define CARD_ZI_END(index)			((index <= CARD_INDEX(0x44)) ? CARD_INDEX(0x44) : CARD_INDEX(0x47))
#define ROLLBACK_START				int _magic_num = *magic_num; \
									int _weave_index = *weave_index; \
									int _card1, _card2, _card3; \
									_card1 = 0; _card2 = 0; _card3 = 0; \
									if (index < CARD_NUM) {	\
										_card1 = cards[index]; \
																											} \
									if (index < CARD_NUM - 1) { \
										_card2 = cards[index + 1]; \
																											} \
									if (index < CARD_NUM - 2) { \
										_card3 = cards[index + 2]; \
																											}
#define ROLLBACK_END				*magic_num = _magic_num; \
									*weave_index = _weave_index; \
									if (index < CARD_NUM) {	\
										cards[index] =_card1; \
																											} \
									if (index < CARD_NUM - 1) { \
										cards[index + 1] =_card2; \
																											} \
									if (index < CARD_NUM - 2) { \
										cards[index + 2] = _card3; \
																											}
#define ROLLBACK_START_4			ROLLBACK_START; \
									int _card4 = 0; \
									if (index < CARD_NUM - 3) { \
										_card4 = cards[index + 3]; \
																											}
#define ROLLBACK_END_4				ROLLBACK_END \
									if (index < CARD_NUM - 3) { \
										cards[index + 3] = _card4; \
																											}
struct mj_info_t {
	// temporary data for calculation
	int last_out;     					/*上一次检测听牌时所出的牌*/
	int last_ting;    					/*上一次检测听牌进庄的牌*/
	int cur_out;		  				/*当前检测听牌时所出的牌*/
	int cur_ting;	    				/*当前检测听牌进庄的牌*/
	int ting_num;						/*可听牌次数*/
	int ting_weave_num;					/*当前听牌后，组合数*/
	// state and return
	int magic[MAGIC_MAX_NUM];			/*当前手中拥有实际的赖子牌   card 相同定义*/
	int magic_num;						/*上面参数的数量*/
	int magic_cfg[MAGIC_MAX_NUM];		/*游戏玩家规定赖子牌     card 相同定义, */
	int magic_cfg_num;					/*上面数据的数据*/
	int search_weave_num;				/*当前要胡牌的组合数*/
	bool b_zi_sun;        				/*是否能吃字*/
	// statistics
	int hu_times;						/*内部使用的，调试用*/
};
class fastHu
{
public:
	fastHu();
	~fastHu();

	vector <tagAnalyseItem> m_huResult;
	tagWeaveItem m_WeaveItem[MAX_WEAVE];
	BYTE         m_cbWeaveICount;
	bool is_magic(struct mj_info_t *info, int card);

	/*
	删除持有的赖子牌
	*/
	bool try_remove_magic(struct mj_info_t *info, int card);

	/*
	吃牌
	使用牌索引为 		index1 	index2 	index3
	对应使用数量为	num1	num2	num3
	其余用赖子补
	*/
	bool chi(int *cards, int *weave, int *magic_num, int *weave_index, int index1, int index2, int index3, int num1, int num2, int num3);

	/*
	碰牌
	使用牌索引为 		index
	对应使用数量为	normal_num
	其余用赖子补
	*/
	bool peng(int *cards, int *weave, int *magic_num, int *weave_index, int index, int normal_num);

	/*
	碰牌
	全使用赖子
	*/
	bool peng_magic(int *weave, int *magic_num, int *weave_index);

	/*
	组合眼
	使用牌索引为 		index
	对应使用数量为	normal_num
	其余用赖子补
	*/
	bool dui(int *cards, int *weave, int *magic_num, int *weave_index, int index, int normal_num);

	/*
	组合眼
	全使用赖子
	*/
	bool dui_magic(int *weave, int *magic_num, int *weave_index);

	/*
	尝试所有碰牌组合
	cards 	 	当前手牌(除赖子)
	weave 		已取出组合
	magic_num 	当前剩余赖子数
	weave_index 已取出组合数
	index 		当前访问到的牌索引
	find_dui 	是否已组合出眼
	info 		其他信息 见定义
	*/
	void try_peng(int *cards, int *weave, int *magic_num, int *weave_index, int index, bool find_dui, struct mj_info_t *info);

	/*
	尝试所有眼组合
	cards 	 	当前手牌(除赖子)
	weave 		已取出组合
	magic_num 	当前剩余赖子数
	weave_index 已取出组合数
	index 		当前访问到的牌索引
	find_dui 	是否已组合出眼
	info 		其他信息 见定义
	*/
	void try_dui(int *cards, int *weave, int *magic_num, int *weave_index, int index, bool find_dui, struct mj_info_t *info);

	/*
	尝试所有吃组合
	cards 	 	当前手牌(除赖子)
	weave 		已取出组合
	magic_num 	当前剩余赖子数
	weave_index 已取出组合数
	index 		当前访问到的牌索引
	find_dui 	是否已组合出眼
	info 		其他信息 见定义
	*/
	void try_chi(int *cards, int *weave, int *magic_num, int *weave_index, int index, bool find_dui, struct mj_info_t *info);

	void hu_pai_cb(struct mj_info_t *info, int *weave, int *weave_index);

	bool init_data(int *cards, int *weave, struct mj_info_t *info, BYTE cbCardData[], BYTE cbCardCount, BYTE cbMagicData[], BYTE cbMagicDataCount, bool bZiShun);

	vector <tagAnalyseItem>  lhu_pai(BYTE cbCardData[], BYTE cbCardCount, BYTE cbMagicData[], BYTE cbMagicDataCount, const tagWeaveItem WeaveItem[], BYTE cbWeaveICount, bool bZiShun = false);
};
#endif
