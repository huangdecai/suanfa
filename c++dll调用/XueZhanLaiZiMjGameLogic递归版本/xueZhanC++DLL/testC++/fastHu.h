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
	int last_out;     					/*��һ�μ������ʱ��������*/
	int last_ting;    					/*��һ�μ�����ƽ�ׯ����*/
	int cur_out;		  				/*��ǰ�������ʱ��������*/
	int cur_ting;	    				/*��ǰ������ƽ�ׯ����*/
	int ting_num;						/*�����ƴ���*/
	int ting_weave_num;					/*��ǰ���ƺ������*/
	// state and return
	int magic[MAGIC_MAX_NUM];			/*��ǰ����ӵ��ʵ�ʵ�������   card ��ͬ����*/
	int magic_num;						/*�������������*/
	int magic_cfg[MAGIC_MAX_NUM];		/*��Ϸ��ҹ涨������     card ��ͬ����, */
	int magic_cfg_num;					/*�������ݵ�����*/
	int search_weave_num;				/*��ǰҪ���Ƶ������*/
	bool b_zi_sun;        				/*�Ƿ��ܳ���*/
	// statistics
	int hu_times;						/*�ڲ�ʹ�õģ�������*/
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
	ɾ�����е�������
	*/
	bool try_remove_magic(struct mj_info_t *info, int card);

	/*
	����
	ʹ��������Ϊ 		index1 	index2 	index3
	��Ӧʹ������Ϊ	num1	num2	num3
	���������Ӳ�
	*/
	bool chi(int *cards, int *weave, int *magic_num, int *weave_index, int index1, int index2, int index3, int num1, int num2, int num3);

	/*
	����
	ʹ��������Ϊ 		index
	��Ӧʹ������Ϊ	normal_num
	���������Ӳ�
	*/
	bool peng(int *cards, int *weave, int *magic_num, int *weave_index, int index, int normal_num);

	/*
	����
	ȫʹ������
	*/
	bool peng_magic(int *weave, int *magic_num, int *weave_index);

	/*
	�����
	ʹ��������Ϊ 		index
	��Ӧʹ������Ϊ	normal_num
	���������Ӳ�
	*/
	bool dui(int *cards, int *weave, int *magic_num, int *weave_index, int index, int normal_num);

	/*
	�����
	ȫʹ������
	*/
	bool dui_magic(int *weave, int *magic_num, int *weave_index);

	/*
	���������������
	cards 	 	��ǰ����(������)
	weave 		��ȡ�����
	magic_num 	��ǰʣ��������
	weave_index ��ȡ�������
	index 		��ǰ���ʵ���������
	find_dui 	�Ƿ�����ϳ���
	info 		������Ϣ ������
	*/
	void try_peng(int *cards, int *weave, int *magic_num, int *weave_index, int index, bool find_dui, struct mj_info_t *info);

	/*
	�������������
	cards 	 	��ǰ����(������)
	weave 		��ȡ�����
	magic_num 	��ǰʣ��������
	weave_index ��ȡ�������
	index 		��ǰ���ʵ���������
	find_dui 	�Ƿ�����ϳ���
	info 		������Ϣ ������
	*/
	void try_dui(int *cards, int *weave, int *magic_num, int *weave_index, int index, bool find_dui, struct mj_info_t *info);

	/*
	�������г����
	cards 	 	��ǰ����(������)
	weave 		��ȡ�����
	magic_num 	��ǰʣ��������
	weave_index ��ȡ�������
	index 		��ǰ���ʵ���������
	find_dui 	�Ƿ�����ϳ���
	info 		������Ϣ ������
	*/
	void try_chi(int *cards, int *weave, int *magic_num, int *weave_index, int index, bool find_dui, struct mj_info_t *info);

	void hu_pai_cb(struct mj_info_t *info, int *weave, int *weave_index);

	bool init_data(int *cards, int *weave, struct mj_info_t *info, BYTE cbCardData[], BYTE cbCardCount, BYTE cbMagicData[], BYTE cbMagicDataCount, bool bZiShun);

	vector <tagAnalyseItem>  lhu_pai(BYTE cbCardData[], BYTE cbCardCount, BYTE cbMagicData[], BYTE cbMagicDataCount, const tagWeaveItem WeaveItem[], BYTE cbWeaveICount, bool bZiShun = false);
};
#endif
