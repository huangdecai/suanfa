// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� TESTC_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// TESTC_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef TESTC_EXPORTS
#define TESTC_API __declspec(dllexport)
#else
#define TESTC_API __declspec(dllimport)
#endif
#include "AndroidAI.h"


extern "C" TESTC_API tagOutCardResult* __stdcall fntestC(tagOutCardResult &OutCardResult, WORD wMeChairId, WORD wCurrentUser, BYTE cbCardDataEx[],  BYTE cbActionMask, BYTE cbActionCard, BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE	DiscardCard[], BYTE cbDiscardCardCount, tagWeaveItem WeaveItemArray[GAME_PLAYER][MAX_WEAVE], BYTE cbWeaveCount[]);
extern "C" TESTC_API int fntestPython2(tagInPyhonNew *pythonIn);
extern "C" TESTC_API int fntestPythonCallCard(tagInPyhonCallCard *pythonIn);