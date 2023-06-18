// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 TESTC_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// TESTC_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
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