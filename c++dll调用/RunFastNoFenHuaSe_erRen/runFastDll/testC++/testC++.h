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
#include "GameLogicNew.h"



extern "C" TESTC_API int __cdecl fntestC(BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardData[], BYTE cbTurnCardCount, BYTE	DiscardCard[], BYTE cbDiscardCardCount, BYTE cbOtherCardData[], BYTE cbOtherCardCount, BYTE cbCardDataEx[], BYTE cbMaxCard[], BYTE &cbCardCount, BYTE cbResultCard[]);
extern "C" TESTC_API int __cdecl countScore(BYTE cbHandCardData[], BYTE cbHandCardCount, int &cbCardScore);
extern "C" TESTC_API int fntestPython2(tagInPyhonNew *pythonIn);
extern "C" TESTC_API int fntestPythonType(tagInPyhonCardType *pythonIn);