//
//  BaseFunc.cpp
//
//  Created by Ruoqian, Chen on 2013/07/11
//
#include "StdAfx.h"

#include "StringMgr.h"

namespace
{
const char *DOC_EXT = ".data";
}

void TestStringSplit(void)
{
	std::vector<std::string> vecRet;
	vecRet = StringSplit("a|b|c", '|');
	vecRet = StringSplit("||", '|');
	vecRet = StringSplit("|", '|');
	vecRet = StringSplit("", '|');
	vecRet = StringSplit("|a|", '|');
	vecRet = StringSplit("|a||b", '|');
	vecRet = StringSplit("|a||b", '|');
	vecRet = StringSplit("||ab", '|');
	vecRet = StringSplit("ab||", '|');
}

std::vector<std::string> StringSplit(const char *pszStr, char cSep)
{
	std::vector<std::string> vecRet;
	if (NULL == pszStr) {
		return vecRet;
	}

	std::string str = pszStr ? pszStr : "";
	std::string::size_type stEnd = 0;
	std::string::size_type stBegin = 0;
	while (true) {
		stBegin = str.find_first_not_of(cSep, stEnd);
		if (std::string::npos == stBegin) {
			return vecRet;
		}

		stEnd = str.find_first_of(cSep, stBegin);
		vecRet.push_back(str.substr(stBegin, stEnd - stBegin));

		if (stEnd == std::string::npos) {
			return vecRet;
		}
	}
}

std::vector<int> StringSplitInt(const char *pszStr, char cSep)
{
	auto vecStr = StringSplit(pszStr, cSep);
	std::vector<int> vecRet;
	for (auto itStr(vecStr.begin()); itStr != vecStr.end(); ++itStr) {
		int nData = 0;
		if (1 == sscanf(itStr->c_str(), "%d", &nData)) {
			vecRet.push_back(nData);
		}
	}

	return vecRet;
}

std::string ReplaceString(const char *pszOrg, const char *pszOld, const char *pszNew)
{
	const std::string strOrg = pszOrg ? pszOrg : "";
	if (NULL == pszOld || pszOld[0] == 0) {
		return strOrg;
	}

	int nModelLength = strlen(pszOld);
	std::string::size_type stBegin = 0;
	std::string::size_type stFind = 0;

	std::string strRet;
	while (true) {
		stFind = strOrg.find(pszOld, stBegin);
		if (std::string::npos == stFind) {
			strRet += strOrg.substr(stBegin);
			return strRet;
		}

		if (stFind > stBegin) {
			strRet += strOrg.substr(stBegin, stFind - stBegin);
		}

		if (pszNew && pszNew[0]) {
			strRet += pszNew;
		}

		stBegin = stFind + nModelLength;
	}
}

void ReplaceStdString(std::string &str, const std::string &strSrc, const std::string &strDest)
{
	str = ReplaceString(str.c_str(), strSrc.c_str(), strDest.c_str());
}

void TestReplaceString(void)
{
	std::string strRet;
	strRet = ReplaceString(NULL, NULL, "test");
	strRet = ReplaceString("aa", "a", "aa");
	strRet = ReplaceString("abba", "ab", "a");
	strRet = ReplaceString("abc\\n", "\\n", "\n");
}

bool GetLineFromData(char* szBuf, int nBufSize, unsigned char*& pPos, unsigned char *pData, int nDataSize)
{
	unsigned char *pDataEnd = pData + nDataSize;
	if (pPos >= pDataEnd) {
		return false;
	}

	int i = 0;
	for (; i < nBufSize && pPos < pDataEnd; ++i, ++pPos) {
		szBuf[i] = *pPos;
		if (*pPos == '\n') {
			break;
		}
	}

	for (; pPos < pDataEnd && *pPos != '\n'; ++pPos);
	if (i == nBufSize) {
		--i;
	}

	szBuf[i] = 0;
	++pPos;
	return true;
}

ccColor3B GetColor3B(DWORD dwColor)
{
	return ccc3((dwColor & 0xff0000) >> 16, (dwColor & 0x00ff00) >> 8, dwColor & 0xff);
}

ccColor4B GetColor4B(DWORD dwColor)
{
	return ccc4((dwColor & 0x00ff0000) >> 16, (dwColor & 0x0000ff00) >> 8, dwColor & 0xff, (dwColor & 0xff000000) >> 24);
}
std::string Value2Str( long long i64Value )
{
	char szTmp[32] = { 0 };
#ifdef WIN32
	sprintf(szTmp, "%I64d", i64Value);
#else
	sprintf(szTmp, "%lld", i64Value);
#endif
	const int PART_LENTH = 3;
	int nOrgLength = strlen(szTmp);
	int nSeps = (nOrgLength - 1) / PART_LENTH;
	int nNewLength = nOrgLength + nSeps;
	int nFillTag = PART_LENTH;
	--nNewLength;
	--nOrgLength;
	while (nNewLength >= 0) {
		if (!nFillTag) {
			nFillTag = PART_LENTH;
			szTmp[nNewLength--] = ',';
			continue;
		}

		--nFillTag;
		szTmp[nNewLength--] = szTmp[nOrgLength--];
	}

	return std::string(szTmp);
}

void TestValue2Str( void )
{
	std::string strRet;
	strRet = Value2Str(0);
	strRet = Value2Str(1);
	strRet = Value2Str(12);
	strRet = Value2Str(123);
	strRet = Value2Str(1234);
	strRet = Value2Str(12345);
	strRet = Value2Str(123456);
	strRet = Value2Str(1234567);
	strRet = Value2Str(12345678);
	strRet = Value2Str(9223372036854775807);
}

std::string UTF8ToAnsi(const std::string &strIn)
{
	std::string strOut;

#ifdef  WIN32
	WCHAR *strSrc = NULL;
	TCHAR *szRes = NULL;
	int i = MultiByteToWideChar(CP_UTF8, 0, strIn.c_str(), -1, NULL, 0);

	strSrc = new WCHAR[i + 1];
	MultiByteToWideChar(CP_UTF8, 0, strIn.c_str(), -1, strSrc, i);

	i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);

	szRes = new TCHAR[i + 1];
	WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);

	strOut = szRes;

	delete[] strSrc;
	delete[] szRes;
#else
	strOut = strIn;
#endif

	return strOut;
}

void MyTrace(const char* pszFormat, ... )
{
	char szExtraMsg[1024];
	va_list ap;
	va_start(ap, pszFormat);
	vsnprintf(szExtraMsg, sizeof(szExtraMsg), pszFormat, ap);
	va_end(ap);
	
	szExtraMsg[sizeof(szExtraMsg) - 1] = 0;

	OutputDebugString(szExtraMsg);
}

void ConTrace( const char *pszFormat, ... )
{
	char szExtraMsg[1024];
	va_list ap;
	va_start(ap, pszFormat);
	vsnprintf(szExtraMsg, sizeof(szExtraMsg), pszFormat, ap);
	va_end(ap);
	
	szExtraMsg[sizeof(szExtraMsg) - 1] = 0;
	std::string strInfo = szExtraMsg;

#if WIN32
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut) {
		DWORD dwTmp = 0;
		WriteConsole(hOut, strInfo.c_str(), strInfo.length(), &dwTmp, NULL);
	} else {
		OutputDebugString(strInfo.c_str());
	}
#else
	CCLog("%s", strInfo.c_str());
#endif
}


void LogIntVec( const std::vector<int>& rVec, const char* pszFormat, ... )
{
	char szExtraMsg[1024];
	va_list ap;
	va_start(ap, pszFormat);
	vsnprintf(szExtraMsg, sizeof(szExtraMsg), pszFormat, ap);
	va_end(ap);
	
	szExtraMsg[sizeof(szExtraMsg) - 1] = 0;
	int nSize = rVec.size();
	MyTrace("----- \n %s Vec size %d", szExtraMsg, nSize);
	for (int i = 0; i < nSize; ++i) {
		MyTrace("%d ", rVec[i]);
	}
	OutputDebugString("\n");
}

bool IsTouchInNode( CCNode *pNode, CCTouch *touch )
{
	if (NULL == pNode) {
		return false;
	}

	CCPoint pt = pNode->convertTouchToNodeSpace(touch);
	CCSize size = pNode->getContentSize();
	CCRect rect(0, 0, size.width, size.height);
	return rect.containsPoint(pt);
}

void SetAllChildCascadeOpacityEnabled(CCNode* pNode, bool able)
{
	if (pNode) {
		CCRGBAProtocol* proto = dynamic_cast<CCRGBAProtocol*>(pNode);
		if(proto) {
			proto->setCascadeOpacityEnabled(able);
		}

		CCObject* obj;
		CCARRAY_FOREACH(pNode->getChildren(), obj) {
			CCNode* pChild = dynamic_cast<CCNode*>(obj);
			SetAllChildCascadeOpacityEnabled(pChild, able);
		}
	}
}

void SetHandlerPri(CCTouchDelegate* pDelegate, int nPri)
{
    CCTouchDispatcher* pTouchDispatcher = CCDirector::sharedDirector()->getTouchDispatcher();
    CCTouchHandler* pManaged = pTouchDispatcher->findHandler(pDelegate);
    if (NULL == pManaged) {
        pTouchDispatcher->addTargetedDelegate(pDelegate, nPri, false);
    } else {
        pTouchDispatcher->setPriority(nPri, pDelegate);
    }
}

void SetHandlerSwallows(CCTouchDelegate* pDelegate, int nPri, bool bSwa)
{
    CCTouchDispatcher* pTouchDispatcher = CCDirector::sharedDirector()->getTouchDispatcher();
    CCTouchHandler* pManaged = pTouchDispatcher->findHandler(pDelegate);
    if (pManaged) {
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(pDelegate);
    }
    
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(pDelegate, nPri, bSwa);
}

void MyTrim( char sz[] )
{
	std::string str = sz;
	MyTrim(str);
	strcpy(sz, str.c_str());
}

void MyTrim( std::string& str )
{
	int nBegin = 0;
	int nEnd = str.length();

	if (nEnd == 0) {
		return;
	}

	bool bFind = false;
	for (; nBegin < nEnd; ++nBegin) {
		switch (str[nBegin])
		{
		case '\r':
		case '\n':
		case '\t':
		case ' ':
			break;
		default:
			bFind = true;
			break;
		}

		if (bFind) {
			break;
		}
	}

	bFind = false;

	for (--nEnd; nEnd > nBegin; --nEnd) {
		switch (str[nEnd])
		{
		case '\r':
		case '\n':
		case '\t':
		case ' ':
			break;
		default:
			bFind = true;
			break;
		}

		if (bFind) {
			break;
		}
	}

	str = str.substr(nBegin, nEnd - nBegin + 1);
}

std::string GetTimeStr( void )
{
#ifdef _WIN32
	SYSTEMTIME st;
	GetLocalTime(&st);
	auto pStr = CCString::createWithFormat("%04d-%02d-%02d %02d:%02d:%02d:%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return pStr->getCString();
#else
	cc_timeval tv;
	CCTime::gettimeofdayCocos2d(&tv, NULL);
	time_t timeSec = tv.tv_sec;
	tm* pTime = localtime(&timeSec);

	auto pStr = CCString::createWithFormat("%04d-%02d-%02d %02d:%02d:%02d:%03d", pTime->tm_year + 1900, pTime->tm_mon + 1, pTime->tm_mday, 
		pTime->tm_hour, pTime->tm_min, pTime->tm_sec, tv.tv_usec / 1000);
#endif
}

std::string GetDebugTime( void )
{
#ifdef WIN32
	SYSTEMTIME st;
	GetLocalTime(&st);
	auto pStr = CCString::createWithFormat("%02d:%02d:%02d:%03d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return pStr->getCString();
#else
	cc_timeval tv;
	CCTime::gettimeofdayCocos2d(&tv, NULL);
	time_t timeSec = tv.tv_sec;
	tm* pTime = localtime(&timeSec);

	auto pStr = CCString::createWithFormat("%02d:%02d:%02d:%03d:%03d", pTime->tm_hour, pTime->tm_min, pTime->tm_sec, tv.tv_usec / 1000, tv.tv_usec % 1000);
	return pStr->getCString();
#endif
}

int GetDateValue( void )
{
    time_t tTime;
    time(&tTime);
    struct tm *pTime  = localtime(&tTime);
	int nValue = (pTime->tm_year + 1900) * 10000 + (pTime->tm_mon + 1) * 100 + pTime->tm_mday;
    return nValue;
}

int MyCeil( double dValue )
{
	double dRet = ceil(dValue);
	return (int) dRet;
}

int MyFloor( double dValue )
{
	double dRet = floor(dValue);
	return (int) dRet;
}

long long MyFloorLL( double dValue )
{
	double dRet = floor(dValue);
	return (long long) dRet;
}

int MyRound( double dValue )
{
	double dRet = floor(dValue + 0.5);
	return (int) dRet;
}

int CmpFloat( float fValue1, float fValue2 )
{
	const float FLOAT_EPSILON = 0.0001f; 
	if (fabs(fValue1 - fValue2) < FLOAT_EPSILON) {
		return 0;
	}

	if (fValue1 < fValue2) {
		return -1;
	}

	if (fValue1 > fValue2) {
		return 1;
	}

	return 0;
}

void TimeTraceLn( const char *pszFormat, ... )
{
	char szExtraMsg[1024];
	va_list ap;
	va_start(ap, pszFormat);
	vsnprintf(szExtraMsg, sizeof(szExtraMsg), pszFormat, ap);
	va_end(ap);

	szExtraMsg[sizeof(szExtraMsg) - 1] = 0;

	char *pLE = strstr(szExtraMsg, "\n");
#ifndef WIN32
	if (pLE) {
		*pLE = ' ';
	}
#endif
	
	std::string strInfo = GetDebugTime();

	static cc_timeval s_ccTimeLast;
	cc_timeval ccTimeCurrent;
	CCTime::gettimeofdayCocos2d(&ccTimeCurrent, NULL);
	double dDeltaTime = CCTime::timersubCocos2d(&s_ccTimeLast, &ccTimeCurrent);
	s_ccTimeLast = ccTimeCurrent;
	
	auto* pStrTimeDelta = CCString::createWithFormat(" (Delta:%7.2lfms) ", dDeltaTime);
	strInfo += pStrTimeDelta->getCString();
	
	strInfo += szExtraMsg;

#ifdef WIN32
	if (NULL == pLE) {
		strInfo += "\n";
	}

	OutputDebugString(strInfo.c_str());
#else
	CCLog("%s", strInfo.c_str());
#endif
}


void ConTraceLn( const char *pszFormat, ... )
{
	char szExtraMsg[1024];
	va_list ap;
	va_start(ap, pszFormat);
	vsnprintf(szExtraMsg, sizeof(szExtraMsg), pszFormat, ap);
	va_end(ap);

	szExtraMsg[sizeof(szExtraMsg) - 1] = 0;

	char *pLE = strstr(szExtraMsg, "\n");
#ifndef WIN32
	if (pLE) {
		*pLE = ' ';
	}
#endif
	
	std::string strInfo = szExtraMsg;

#if WIN32 && !FIGHT_PREDICT_DEBUG
	if (NULL == pLE) {
		strInfo += "\n";
	}

	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut) {
		DWORD dwTmp = 0;
		WriteConsole(hOut, strInfo.c_str(), strInfo.length(), &dwTmp, NULL);
	} else {
		OutputDebugString(strInfo.c_str());
	}
#else
	CCLog("%s", strInfo.c_str());
#endif
}


void ConTimeTraceLn( const char *pszFormat, ... )
{
	char szExtraMsg[1024];
	va_list ap;
	va_start(ap, pszFormat);
	vsnprintf(szExtraMsg, sizeof(szExtraMsg), pszFormat, ap);
	va_end(ap);

	szExtraMsg[sizeof(szExtraMsg) - 1] = 0;

	char *pLE = strstr(szExtraMsg, "\n");
#ifndef WIN32
	if (pLE) {
		*pLE = ' ';
	}
#endif
	
	std::string strInfo = GetDebugTime();

	static cc_timeval s_ccTimeLast;
	cc_timeval ccTimeCurrent;
	CCTime::gettimeofdayCocos2d(&ccTimeCurrent, NULL);
	double dDeltaTime = CCTime::timersubCocos2d(&s_ccTimeLast, &ccTimeCurrent);
	s_ccTimeLast = ccTimeCurrent;
	
	auto pStrTimeDelta = CCString::createWithFormat(" (Delta:%7.2lfms) ", dDeltaTime);
	strInfo += pStrTimeDelta->getCString();
	
	strInfo += szExtraMsg;

#if WIN32 && !FIGHT_PREDICT_DEBUG
	if (NULL == pLE) {
		strInfo += "\n";
	}

	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut) {
		DWORD dwTmp = 0;
		WriteConsole(hOut, strInfo.c_str(), strInfo.length(), &dwTmp, NULL);
	} else {
		OutputDebugString(strInfo.c_str());
	}
#else
	CCLog("%s", strInfo.c_str());
#endif
}

CCBezierTo* CreateSimpleBezierToAction( float fDuration, CCPoint ptFrom, CCPoint ptTo, CCPoint ptReference )
{
	ccBezierConfig config;
	config.endPosition = ptTo;

	CCPoint ptOffset = (ptTo - ptFrom);
	config.controlPoint_1 = ptFrom + ptOffset / 3.0f;
	config.controlPoint_2 = ptFrom + ptOffset * 0.6f;

	float fLength = ptOffset.getLength();
	float fOffset = fLength / 3;

	if (fabs(ptOffset.x) > fabs(ptOffset.y)) {
		if (ptReference.y > ptTo.y) {
			fOffset = -fOffset;
		}

		config.controlPoint_1.y += fOffset;
		config.controlPoint_2.y += fOffset;
	} else {
		if (ptReference.x > ptTo.x) {
			fOffset = -fOffset;
		}

		config.controlPoint_1.x += fOffset;
		config.controlPoint_2.x += fOffset;
	}

	return CCBezierTo::create(fDuration, config);
}

void GetNodeAllChild( CCNode *pNode, std::vector<CCNode*>& rVecNodeChild )
{
	if (NULL == pNode) {
		return;
	}

	rVecNodeChild.push_back(pNode);

	CCArray* pChildArray = pNode->getChildren();

	if (NULL == pChildArray) {
		return;
	}

	CCObject* pObj;
	int nCount = pChildArray->count();
	CCARRAY_FOREACH(pChildArray, pObj)
	{
		CCNode *pChild = dynamic_cast<CCNode*>(pObj);
		if (NULL == pChild) {
			continue;
		}

		std::vector<CCNode*> vecChildChild;
		GetNodeAllChild(pChild, vecChildChild);
		rVecNodeChild.insert(rVecNodeChild.end(), vecChildChild.begin(), vecChildChild.end());
	}
}

void TestMyTrim( void )
{
	std::string str = "a";
	MyTrim(str);
	MY_ASSERT(str == "a");

	str = " abc ";
	MyTrim(str);
	MY_ASSERT(str == "abc");

	str = "abc";
	MyTrim(str);
	MY_ASSERT(str == "abc");

	str = "  ";
	MyTrim(str);
	MY_ASSERT(str == "");

	str = " ";
	MyTrim(str);
	MY_ASSERT(str == "");
	
	str = "";
	MyTrim(str);
	MY_ASSERT(str == "");

}

float GetKeyFrameTime( int nKeyFrame )
{
	return (nKeyFrame - 1) / 24.0f;
}

float GetKeyFrameInterval( int nKeyFrameBegin, int nKeyFrameEnd )
{
	return (nKeyFrameEnd - nKeyFrameBegin) / 24.0f;
}

std::string SimNewParagraphTab(const char *pszFont, int nFontSize)
{
	const char *pszTab = g_objStrMgr.GetStr("STR_NEW_PARAGRAPH_TAB");
	auto* pLabelTab = CCLabelTTF::create(pszTab, pszFont, nFontSize);
	float fWidthTab = pLabelTab ? pLabelTab->getContentSize().width : 0;
	
	const char *pszFill = " ";
	auto* pLabelFill = CCLabelTTF::create(pszFill, pszFont, nFontSize);
	float fWidthFill = pLabelFill ? pLabelFill->getContentSize().width : 0;

	std::string strFill;
	if (fWidthFill > 0) {
		int nCount = MyRound(fWidthTab / fWidthFill);
		strFill = std::string(nCount, pszFill[0]);
	}
	
	return strFill;
}

std::string GetObjType( CCObject* pObj )
{
	std::string strType = typeid(*pObj).name();
	ReplaceStdString(strType, "class ", "");
	ReplaceStdString(strType, "eb::", "");
	ReplaceStdString(strType, "cmwar::", "");
	return strType;
}

void NodeSetText( CCLabelTTF* pNode, int nValue, bool bShowZero /*= true*/, bool bNeedSign /*= false*/ )
{
	if (NULL == pNode) {
		return;
	}

	if (!nValue && !bShowZero) {
		pNode->setString("");
		return;
	}

	if (nValue > 0 && bNeedSign) {
		auto* pStr = CCString::createWithFormat("+%d", nValue);
		pNode->setString(pStr->getCString());
		return;
	}

	auto* pStr = CCString::createWithFormat("%d", nValue);
	pNode->setString(pStr->getCString());
}

void NodeSetText( CCEditBox* pNode, int nValue, bool bShowZero /*= true*/, bool bNeedSign /*= false*/ )
{
	if (NULL == pNode) {
		return;
	}

	if (!nValue && !bShowZero) {
		pNode->setText("");
		return;
	}

	if (nValue > 0 && bNeedSign) {
		auto* pStr = CCString::createWithFormat("+%d", nValue);
		pNode->setText(pStr->getCString());
		return;
	}
	
	auto* pStr = CCString::createWithFormat("%d", nValue);
	pNode->setText(pStr->getCString());
}

std::string GetImageFileName( const char *pszFileName )
{
	std::string strFileName = pszFileName ? pszFileName : "";
	std::string strPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(pszFileName);
	bool bExist = CCFileUtils::sharedFileUtils()->isFileExist(strPath.c_str());
	if (!bExist) {
		auto nPos = strFileName.find_last_of('.');
		if (nPos != std::string::npos) {
			strFileName = strFileName.substr(0, nPos);
			strFileName += ".jpg";
		}
	}

	return strFileName;
}

CCAction* RunCommonShakeAction( CCNode* pNode,float fScale, float fDelay /*= 0*/)
{
	CCArray* pArrAction = CCArray::create();
    
	pArrAction->addObject(CCDelayTime::create(fDelay));
    pArrAction->addObject(CCJumpTo::create(0.3f, ccp(0, 0), 4.0f * fScale, 4.0f));
    
//	pArrAction->addObject(CCMoveTo::create(0.04, ccp(0, -4*fScale)));
//	pArrAction->addObject(CCMoveTo::create(0.08f, CCPointZero));
//	pArrAction->addObject(CCMoveTo::create(0.08f, ccp(0, -4*fScale)));
//	pArrAction->addObject(CCMoveTo::create(0.08f, CCPointZero));
	CCSequence *pSeq = CCSequence::create(pArrAction);
	pNode->runAction(pSeq);

	return pSeq;
}

CCScale9Sprite* Scale9SpriteCreate( const char *pszName )
{
	CCSpriteFrame *pFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(pszName);
	if (pFrame) {
		return CCScale9Sprite::createWithSpriteFrameName(pszName);
	}

	return CCScale9Sprite::create(pszName);
}

CCSprite* SpriteCreate( const char *pszName )
{
	if (NULL == pszName || pszName[0] == 0) {
		return CCSprite::create();
	}

	return CCSprite::create(pszName);
}

bool ScreenShot( const char *pszFileName )
{
	CCSize size = CCDirector::sharedDirector()->getWinSize();
	CCRenderTexture* texture = CCRenderTexture::create((int)size.width, (int)size.height, kCCTexture2DPixelFormat_RGBA8888);      
	texture->setPosition(ccp(size.width/2, size.height/2));      
	texture->begin();  
	CCDirector::sharedDirector()->getRunningScene()->visit();  
	texture->end();
	return texture->saveToFile(pszFileName, kCCImageFormatJPEG);   
}
