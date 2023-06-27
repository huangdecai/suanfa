#include "stdafx.h"
#include "fastHu.h"

#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE						0x0F								//数值掩码
//扑克转换
BYTE SwitchToCardData(BYTE cbCardIndex)
{
	if (cbCardIndex < 27)
		return ((cbCardIndex / 9) << 4) | (cbCardIndex % 9 + 1);
	else return (0x30 | (cbCardIndex - 27 + 1));
	//return ((((cbCardIndex) / 9)) << 4) | ((cbCardIndex) % 9 + 1);
}
//扑克转换
BYTE SwitchToCardIndex(BYTE cbCardData)
{
	return ((cbCardData & MASK_COLOR) >> 4) * 9 + (cbCardData & MASK_VALUE) - 1;
}
#define WEAVE_TYPE_CHI				0x01
#define WEAVE_TYPE_PENG				0x08
#define WEAVE_TYPE_DUI				0xFF				
#define CARD_NUM 					42
#define WEAVE_MAX_NUM 				5
#define MAGIC_INDEX					100



#define CARD_INDEX(card) 			SwitchToCardIndex(card)
#define CARD_DATA(index) 			SwitchToCardData(index)
//#define CARD_INDEX(card) 			((((card & 0xF0) >> 4) - 1) * 9 + (card & 0x0F) - 1)
//#define CARD_DATA(index) 			(((index / 9 + 1) << 4) | (index % 9 + 1))
#define CAN_CHI(index) 				(index < CARD_INDEX(0x31))
#define CAN_CHI_LOGIC(index)		(!CARD_IS_ZI(index) && CAN_CHI(index) && (index % 9 < 7))
#define CARD_IS_ZI(index) 			(index >= CARD_INDEX(0x31) && index <= CARD_INDEX(0x37))
#define CARD_ZI_END(index)			((index <= CARD_INDEX(0x34)) ? CARD_INDEX(0x34) : CARD_INDEX(0x37))
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

fastHu::fastHu()
{
}


fastHu::~fastHu()
{
}

bool fastHu::is_magic(struct mj_info_t *info, int card)
{
	int i;
	for (i = 0; i < info->magic_cfg_num; i++) {
		if (info->magic_cfg[i] == card) {
			return true;
		}
	}
	return false;
}

bool fastHu::try_remove_magic(struct mj_info_t *info, int card)
{
	int i = 0;
	bool bfine = false;
	for (i = 0; i < info->magic_num; i++) {
		if (!bfine && info->magic[i] == card) {
			bfine = true;
		}
		if (bfine) {
			info->magic[i] = info->magic[i + 1];
		}
	}
	if (bfine) {
		info->magic_num -= 1;
	}
	return bfine;
}

bool fastHu::chi(int *cards, int *weave, int *magic_num, int *weave_index, int index1, int index2, int index3, int num1, int num2, int num3)
{
	//最后一个牌越界
	if (index3 >= CARD_NUM) {
		return false;
	}
	//是否3张连续的牌
	if (cards[index1] < num1
		|| cards[index2] < num2
		|| cards[index3] < num3) {
		return false;
	}
	//不能全部为0，
	if (num1 == 0
		&& num2 == 0
		&& num3 == 0) {
		return false;
	}
	//参数检查
	if (num1 + num2 + num3 + *magic_num < 3) {
		return false;
	}
	//把现在吃的牌的组合，放在吃碰牌堆里，方便后面计算
	if (num1 > 0) {
		weave[*weave_index * 4] = index1;
		cards[index1] -= 1;
	}
	else {
		weave[*weave_index * 4] = MAGIC_INDEX;
		*magic_num -= 1;
	}
	if (num2 > 0) {
		weave[*weave_index * 4 + 1] = index2;
		cards[index2] -= 1;
	}
	else {
		weave[*weave_index * 4 + 1] = MAGIC_INDEX;
		*magic_num -= 1;
	}
	if (num3 > 0) {
		weave[*weave_index * 4 + 2] = index3;
		cards[index3] -= 1;
	}
	else {
		weave[*weave_index * 4 + 2] = MAGIC_INDEX;
		*magic_num -= 1;
	}
	//记下牌的类型
	weave[*weave_index * 4 + 3] = WEAVE_TYPE_CHI;
	*weave_index += 1;
	return true;
}

bool fastHu::peng(int *cards, int *weave, int *magic_num, int *weave_index, int index, int normal_num)
{
	if (index >= CARD_NUM) {
		return false;
	}
	if (cards[index] < normal_num
		|| normal_num > 3) {
		return false;
	}
	if (normal_num + *magic_num < 3) {
		return false;
	}
	int i;
	for (i = 0; i < normal_num; i++) {
		weave[*weave_index * 4 + i] = index;
	}
	for (i = normal_num; i < 3; i++) {
		weave[*weave_index * 4 + i] = MAGIC_INDEX;
	}
	cards[index] -= normal_num;
	*magic_num -= 3 - normal_num;
	weave[*weave_index * 4 + 3] = WEAVE_TYPE_PENG;
	*weave_index += 1;
	return true;
}

bool fastHu::peng_magic(int *weave, int *magic_num, int *weave_index)
{
	if (*magic_num < 3) {
		return false;
	}
	weave[*weave_index * 4 + 0] = MAGIC_INDEX;
	weave[*weave_index * 4 + 1] = MAGIC_INDEX;
	weave[*weave_index * 4 + 2] = MAGIC_INDEX;
	weave[*weave_index * 4 + 3] = WEAVE_TYPE_PENG;
	*magic_num -= 3;
	*weave_index += 1;
	return true;
}

bool fastHu::dui(int *cards, int *weave, int *magic_num, int *weave_index, int index, int normal_num)
{
	if (index >= CARD_NUM) {
		return false;
	}
	if (cards[index] < normal_num
		|| normal_num > 2) {
		return false;
	}
	if (normal_num + *magic_num < 2) {
		return false;
	}
	int i;
	for (i = 0; i < normal_num; i++) {
		weave[*weave_index * 4 + i] = index;
	}
	for (i = normal_num; i < 2; i++) {
		weave[*weave_index * 4 + i] = MAGIC_INDEX;
	}
	cards[index] -= normal_num;
	*magic_num -= 2 - normal_num;
	weave[*weave_index * 4 + 3] = WEAVE_TYPE_DUI;
	*weave_index += 1;
	return true;
}

bool fastHu::dui_magic(int *weave, int *magic_num, int *weave_index)
{
	if (*magic_num < 2) {
		return false;
	}
	weave[*weave_index * 4 + 0] = MAGIC_INDEX;
	weave[*weave_index * 4 + 1] = MAGIC_INDEX;
	weave[*weave_index * 4 + 3] = WEAVE_TYPE_DUI;
	*magic_num -= 2;
	*weave_index += 1;
	return true;
}

void fastHu::try_peng(int *cards, int *weave, int *magic_num, int *weave_index, int index, bool find_dui, struct mj_info_t *info)
{
	/*递归结束条件*/
	if (index >= CARD_NUM || *weave_index >= info->search_weave_num) {
		ROLLBACK_START;
		if (*weave_index < info->search_weave_num) {
			while (peng_magic(weave, magic_num, weave_index)) {
			}
			if (!find_dui && dui_magic(weave, magic_num, weave_index)) {
				find_dui = true;
			}
		}
		/*有可能多次满足下面条件，可以返回多个结果，利用lua表结构返回结果*/
		if (*weave_index == info->search_weave_num && *magic_num == 0 && find_dui) {
			/*快要结束时，把计算后的参数返回LUA调用*/
			hu_pai_cb(info, weave, weave_index);
		}
		ROLLBACK_END;
		return;
	}

	int num, magic, i, j, k;
	num = cards[index];
	magic = *magic_num;
	// 含有当前牌的碰只有0个情况
	{
		ROLLBACK_START;
		try_dui(cards, weave, magic_num, weave_index, index, find_dui, info);
		ROLLBACK_END;
	}

	// 含有当前牌的碰只有1个情况
	for (i = 1; i <= 3; i++) {
		if (num >= i && num + magic >= 3) {
			ROLLBACK_START;
			if (peng(cards, weave, magic_num, weave_index, index, i)) {
				try_dui(cards, weave, magic_num, weave_index, index, find_dui, info);
			}
			ROLLBACK_END;
		}
	}
	// 含有当前牌的碰只有2个情况
	for (i = 1; i <= 3; i++) {
		for (j = i; j <= 3; j++) {
			if (num >= i + j &&  num + magic >= 6) {
				ROLLBACK_START;
				if (peng(cards, weave, magic_num, weave_index, index, i)
					&& peng(cards, weave, magic_num, weave_index, index, j)) {
					try_dui(cards, weave, magic_num, weave_index, index, find_dui, info);
				}
				ROLLBACK_END;
			}
		}
	}
	// 含有当前牌的碰只有3个情况
	for (i = 1; i <= 3; i++) {
		for (j = i; j <= 3; j++) {
			for (k = j; k <= 3; k++) {
				if (num >= i + j + k && num + magic >= 9) {
					ROLLBACK_START;
					if (peng(cards, weave, magic_num, weave_index, index, i)
						&& peng(cards, weave, magic_num, weave_index, index, j)
						&& peng(cards, weave, magic_num, weave_index, index, k)) {
						try_dui(cards, weave, magic_num, weave_index, index, find_dui, info);
					}
					ROLLBACK_END;
				}
			}
		}
	}
	// 含有当前牌的碰只有4个情况
	if (num >= 4 && num + magic >= 12) {
		ROLLBACK_START;
		if (peng(cards, weave, magic_num, weave_index, index, 1)
			&& peng(cards, weave, magic_num, weave_index, index, 1)
			&& peng(cards, weave, magic_num, weave_index, index, 1)
			&& peng(cards, weave, magic_num, weave_index, index, 1)) {
			try_dui(cards, weave, magic_num, weave_index, index, find_dui, info);
		}
		ROLLBACK_END;
	}
}

void fastHu::try_dui(int *cards, int *weave, int *magic_num, int *weave_index, int index, bool find_dui, struct mj_info_t *info)
{
	int num, magic, i;
	num = cards[index];
	magic = *magic_num;
	// 已经有眼了
	if (find_dui) {
		ROLLBACK_START;
		try_chi(cards, weave, magic_num, weave_index, index, find_dui, info);
		ROLLBACK_END;
		return;
	}
	// 不组成眼
	{
		ROLLBACK_START;
		try_chi(cards, weave, magic_num, weave_index, index, find_dui, info);
		ROLLBACK_END;
	}
	// 组成1个眼
	for (i = 1; i <= 2; i++) {
		if (num >= i && num + magic >= 2) {
			ROLLBACK_START;
			if (dui(cards, weave, magic_num, weave_index, index, i)) {
				try_chi(cards, weave, magic_num, weave_index, index, true, info);
			}
			ROLLBACK_END;
		}
	}
}

void fastHu::try_chi(int *cards, int *weave, int *magic_num, int *weave_index, int index, bool find_dui, struct mj_info_t *info)
{
	// 消耗掉所有当前牌索引才往下面进行 否则视这次搜索为不能胡牌(有牌不能进行组合)
	if (cards[index] <= 0) {
		try_peng(cards, weave, magic_num, weave_index, index + 1, find_dui, info);
	}
	// 字牌 有些麻将可以吃字 分别与后面的字牌组合求出所有情况
	if (info->b_zi_sun && CARD_IS_ZI(index)) {
		int n, m, i, j, k, end, magic;
		end = CARD_ZI_END(index);
		magic = *magic_num;
		for (n = index + 1; n <= end - 1; n++) {
			for (m = n + 1; m <= end; m++) {
				for (i = 0; i <= 1; i++) {
					for (j = 0; j <= 1; j++) {
						for (k = 0; k <= 1; k++) {
							if (i + k + j > 0 && i + j + k + magic >= 3) {
								ROLLBACK_START_4;
								if (chi(cards, weave, magic_num, weave_index, index, n, m, i, j, k)) {
									try_chi(cards, weave, magic_num, weave_index, index, find_dui, info);
								}
								ROLLBACK_END_4;
							}
						}
					}
				}
			}
		}
		// 普通牌
	}
	else if (CAN_CHI_LOGIC(index)) {
		int i, j, k;
		for (i = 0; i <= 1; i++) {
			for (j = 0; j <= 1; j++) {
				for (k = 0; k <= 1; k++) {
					if (i + k + j > 0 && i + j + k + *magic_num >= 3) {
						ROLLBACK_START;
						if (chi(cards, weave, magic_num, weave_index, index, index + 1, index + 2, i, j, k)) {
							try_chi(cards, weave, magic_num, weave_index, index, find_dui, info);
						}
						ROLLBACK_END;
					}
				}
			}
		}

	}
}

void fastHu::hu_pai_cb(struct mj_info_t *info, int *weave, int *weave_index)
{
	tagAnalyseItem anslyseItem;
	ZeroMemory(&anslyseItem, sizeof(anslyseItem));
	int i, j, type, index;
	index = 0;
	int itemCount = 0;
	for (i = 0; i < *weave_index; i++) {
		
		type = weave[i * 4 + 3];
		if ( type == WEAVE_TYPE_DUI) {
			anslyseItem.cbCardEye = CARD_DATA(weave[i * 4 + 1]);
			if (weave[i * 4 + 1] == MAGIC_INDEX)
			{
				anslyseItem.bMagicEye = true;
				anslyseItem.cbCardEye = (info->magic[index]);
				index += 1;
			}
			continue;
		}
		for (j = 0; j < 4; j++) {
			 if (j == 3) {
				anslyseItem.cbWeaveKind[i] = type;
			}
			else {
				
				if (weave[i * 4 + j] == MAGIC_INDEX) {
					anslyseItem.cbCardData[itemCount][j] = info->magic[index];
					index += 1;
				}
				else {
					anslyseItem.cbCardData[itemCount][j] = CARD_DATA(weave[i * 4 + j]);
				}
				if (j==0 )
				{
					anslyseItem.cbCenterCard[itemCount] = anslyseItem.cbCardData[itemCount][j];
				}
			}
		}
		itemCount++;
	}
	int tmpIndex = itemCount;
	for (int i =0 ; i < m_cbWeaveICount;i++)
	{
		CopyMemory(anslyseItem.cbCardData[i + tmpIndex], m_WeaveItem[i].cbCardData, sizeof(m_WeaveItem[i].cbCardData));
		anslyseItem.cbCenterCard[i + tmpIndex] = m_WeaveItem[i].cbCenterCard;
		anslyseItem.cbWeaveKind[i + tmpIndex] = m_WeaveItem[i].cbWeaveKind;
	}
	m_huResult.push_back(anslyseItem);
}

bool fastHu::init_data(int *cards, int *weave, struct mj_info_t *info,  BYTE cbCardData[], BYTE cbCardCount, BYTE cbMagicData[], BYTE cbMagicDataCount, bool bZiShun)
{
	for (int i = 0; i < CARD_NUM; i++) {
		cards[i] = 0;
	}
	for (int i = 0; i < WEAVE_MAX_NUM * 4; i++) {
		weave[i] = -1;
	}

	info->last_out = -1;
	info->last_ting = -1;
	info->cur_out = -1;
	info->cur_ting = -1;
	info->ting_num = 0;
	info->ting_weave_num = 0;
	info->magic_num = 0;
	info->magic_cfg_num = 0;
	info->search_weave_num = 0;
	info->b_zi_sun = bZiShun ? true : false;
	info->hu_times = 0;
	for (int i = 0; i < MAGIC_MAX_NUM; i++) {
		info->magic[i] = 0;
	}
	for (int i = 0; i < MAGIC_MAX_NUM; i++) {
		info->magic_cfg[i] = 0;
	}
	//BYTE		cbMagicIndex[MAX_MAGIC_COUNT] = { MAX_INDEX, MAX_INDEX };
	for (int i = 0; i < cbMagicDataCount; i++) {
		if (cbMagicData[i]!=0)
		{
			//cbMagicIndex[0] = m_GameLogic.SwitchToCardIndex(cbMagicData[i]);//钻牌索引
			info->magic_cfg[info->magic_cfg_num] = cbMagicData[i];
			info->magic_cfg_num += 1;
		}
	}
	//m_GameLogic.SetMagicIndex(cbMagicIndex);
	int card_num = 0;
	for (int i = 0; i < cbCardCount;i++)
	{
		if (is_magic(info, cbCardData[i])) {
			info->magic[info->magic_num] = cbCardData[i];
			info->magic_num += 1;
		}
		else {
			int card_index = CARD_INDEX(cbCardData[i]);
			cards[card_index] += 1;
		}
		card_num += 1;
	}
	

	info->search_weave_num = card_num / 3 + 1;
	if (info->search_weave_num > WEAVE_MAX_NUM) {
		return false;
	}

	return true;
}

vector <tagAnalyseItem>  fastHu::lhu_pai(BYTE cbCardData[], BYTE cbCardCount, BYTE cbMagicData[], BYTE cbMagicDataCount, const tagWeaveItem WeaveItem[], BYTE cbWeaveICount, bool bZiShun/*=false*/)
{
	int cards[CARD_NUM] = { 0 };
	int weave[WEAVE_MAX_NUM * 4] = { 0 };
	struct mj_info_t info;
	m_huResult.clear();
	if (!init_data(cards, weave, &info, cbCardData, cbCardCount, cbMagicData, cbMagicDataCount, bZiShun)) {
		return m_huResult;
	}
	CopyMemory(m_WeaveItem, WeaveItem,sizeof(m_WeaveItem));
	m_cbWeaveICount = cbWeaveICount;
	int weave_index = 0;
	int magic_num = info.magic_num;
	info.cur_out = 0;
	info.cur_ting = 0;
	try_peng(cards, weave, &magic_num, &weave_index, 0, false, &info);
	return m_huResult;
}

