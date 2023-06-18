#ifndef GAME_FASTHU_HEAD_FILE
#define GAME_FASTHU_HEAD_FILE

#pragma once
#include "Stdafx.h"
//#include "GameLogic.h"
#include <vector>
using namespace std;
#define MAGIC_MAX_NUM 				(16 + 1)

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
