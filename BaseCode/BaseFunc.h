//
//  BaseFunc.h
//
//  Created by Ruoqian, Chen on 2013/07/11
//

#ifndef __BASEFUNC_H__
#define __BASEFUNC_H__

#ifdef WIN32
#include "atlstr.h"
#include "atlconv.h"
#include "crtdefs.h"
#endif

#include "BaseDef.h"
#include "MyEditBox.h"

#define GET_INI_VALUE(key, defaultValue) (CIniMgr::GetInstance()->GetValue(CONFIG_INI, pszConfig, (key), (defaultValue)));
#define LOG_JSON(json) LogJson(json, #json)

std::vector<std::string> StringSplit(const char *pszStr, char cSep);
std::vector<int> StringSplitInt(const char *pszStr, char cSep = ' ');
std::string ReplaceString(const char *pszOrg, const char *pszOld, const char *pszNew);
void MyTrim(char sz[]);
void MyTrim(std::string& str);
void ReplaceStdString(std::string &str, const std::string &strSrc, const std::string &strDest);
bool GetLineFromData(char* szBuf, int nBufSize, unsigned char*& pPos, unsigned char *pData, int nDataSize);
int MyCeil(double dValue);
int MyFloor(double dValue);
long long MyFloorLL(double dValue);
int MyRound(double dValue);
int CmpFloat(float fValue1, float fValue2);

float GetKeyFrameTime(int nKeyFrame);
float GetKeyFrameInterval(int nKeyFrameBegin, int nKeyFrameEnd);
std::string Value2Str(long long i64Value);
int GetDateValue(void);
std::string GetTimeStr(void);
std::string GetDebugTime(void);
std::string UTF8ToAnsi(const std::string &strIn);
void MyTrace(const char* pszFormat, ... );
void ConTrace(const char *pszFormat, ...);
void ConTraceLn(const char *pszFormat, ...);
void ConTimeTraceLn(const char *pszFormat, ...);
void TimeTraceLn(const char *pszFormat, ...);
void LogIntVec(const std::vector<int>& rVec, const char* pszFormat, ...);

ccColor3B GetColor3B(DWORD dwColor);
ccColor4B GetColor4B(DWORD dwColor);
void GetNodeAllChild(CCNode *pNode, std::vector<CCNode*>& rVecNodeChild);
void SetAllChildCascadeOpacityEnabled(CCNode* pNode, bool able);
void SetHandlerPri(CCTouchDelegate* pDelegate, int nPri);
void SetHandlerSwallows(CCTouchDelegate* pDelegate, int nPri, bool bSwa);

void TestReplaceString(void);
void TestStringSplit(void);
void TestValue2Str(void);
void TestMyTrim(void);

std::string SimNewParagraphTab(const char *pszFont, int nFontSize);
std::string GetImageFileName(const char *pszFileName);
CCAction* RunCommonShakeAction(CCNode* pNode,float fScale = 1.0f, float fDelay = 0);
CCBezierTo* CreateSimpleBezierToAction(float fDuration, CCPoint ptFrom, CCPoint ptTo, CCPoint ptReference);
bool IsTouchInNode(CCNode *pNode, CCTouch *touch);
std::string GetObjType(CCObject* pObj);
void NodeSetText(CCLabelTTF* pNode, int nValue, bool bShowZero = true, bool bNeedSign = false);
void NodeSetText(cocos2d::extension::MyEditBox* pNode, int nValue, bool bShowZero = true, bool bNeedSign = false);
CCSprite* SpriteCreate(const char *pszName = NULL);
CCScale9Sprite* Scale9SpriteCreate(const char *pszName);
bool ScreenShot( const char *pszFileName );

#define CREATE_FUNC_INT(__TYPE__) \
static __TYPE__* create(int __Argv) \
{ \
    __TYPE__ *pRet = new __TYPE__(__Argv); \
    if (pRet && pRet->init()) \
    { \
        pRet->autorelease(); \
        return pRet; \
    } \
    else \
    { \
        delete pRet; \
        pRet = NULL; \
        return NULL; \
    } \
}

#ifdef DEBUG
#define SHOW_ASSERT_DETAIL
#endif

#if (defined(WIN32) && defined(SHOW_ASSERT_DETAIL))
#define MY_ASSERT(_Expression, ...)  if(!(_Expression)) \
{\
	CString cstrMsg;\
	char szExtraMsg[1024]; \
	sprintf(szExtraMsg, ##__VA_ARGS__, "No ExtraMsg"); \
	cstrMsg.Format("%s\nExtraMsg: %s\nFunction: %s", #_Expression, szExtraMsg, __FUNCTION__); \
	_set_error_mode(_OUT_TO_MSGBOX); \
	USES_CONVERSION; \
	_wassert(A2W(cstrMsg), _CRT_WIDE(__FILE__), __LINE__); \
}
#elif (defined(SHOW_ASSERT_DETAIL))
#define MY_ASSERT(_Expression, ...)  if (!(_Expression)) \
{\
	char szExtraMsg[1024]; \
	sprintf(szExtraMsg, ##__VA_ARGS__, "No ExtraMsg"); \
	CCAssert(_Expression, szExtraMsg); \
}
#else
#define MY_ASSERT(_Expression, ...) 
#endif


#if (defined(WIN32) && defined(SHOW_ASSERT_DETAIL))
#define MY_CHECK(_Expression, ...)  if(!(_Expression)) \
{\
	CString cstrMsg;\
	char szExtraMsg[1024]; \
	sprintf(szExtraMsg, ##__VA_ARGS__, "No ExtraMsg"); \
	cstrMsg.Format("%s\nExtraMsg: %s\nFunction: %s", #_Expression, szExtraMsg, __FUNCTION__); \
	_set_error_mode(_OUT_TO_MSGBOX); \
	USES_CONVERSION; \
	_wassert(A2W(cstrMsg), _CRT_WIDE(__FILE__), __LINE__); \
	return; \
}
#elif (defined(SHOW_ASSERT_DETAIL))
#define MY_CHECK(_Expression, ...)  if (!(_Expression)) \
{\
	char szExtraMsg[1024]; \
	sprintf(szExtraMsg, ##__VA_ARGS__, "No ExtraMsg"); \
	CCAssert(_Expression, szExtraMsg); \
	return; \
}
#else
#define MY_CHECK(_Expression, ...)  if (!(_Expression)) \
{\
	return; \
}
#endif

#if (defined(WIN32) && defined(SHOW_ASSERT_DETAIL))
#define MY_CHECK_V(_Expression, ...)  if(!(_Expression)) \
{\
	CString cstrMsg;\
	char szExtraMsg[1024]; \
	sprintf(szExtraMsg, ##__VA_ARGS__, "No ExtraMsg"); \
	cstrMsg.Format("%s\nExtraMsg: %s\nFunction: %s", #_Expression, szExtraMsg, __FUNCTION__); \
	_set_error_mode(_OUT_TO_MSGBOX); \
	USES_CONVERSION; \
	_wassert(A2W(cstrMsg), _CRT_WIDE(__FILE__), __LINE__); \
	return 0; \
}
#elif (defined(SHOW_ASSERT_DETAIL))
#define MY_CHECK_V(_Expression, ...)  if (!(_Expression)) \
{\
	char szExtraMsg[1024]; \
	sprintf(szExtraMsg, ##__VA_ARGS__, "No ExtraMsg"); \
	CCAssert(_Expression, szExtraMsg); \
	return 0; \
}
#else
#define MY_CHECK_V(_Expression, ...)  if (!(_Expression)) \
{\
	return 0; \
}
#endif
#endif





