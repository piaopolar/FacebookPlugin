//
//  StringMgr.cpp
//
//  Created by Ruoqian, Chen on 2013/08/28
//
#include "StdAfx.h"

#include "StringMgr.h"

namespace
{
static CStringMgr *s_pStringMgr = NULL;
const char *STR_RES = "ini/strRes.ini";
}

CStringMgr::CStringMgr(void)
{
	this->Reload();
}

CStringMgr::~CStringMgr(void)
{
}

CStringMgr *CStringMgr::Instance(void)
{
	if (NULL == s_pStringMgr) {
		s_pStringMgr = new CStringMgr;
	}

	return s_pStringMgr;
}

void CStringMgr::Release(void)
{
	CC_SAFE_DELETE(s_pStringMgr);
}

void CStringMgr::Reload(void)
{
	m_mapString.clear();

    auto strPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(STR_RES);
    
	unsigned long uSize = 0;
	unsigned char *pData = CCFileUtils::sharedFileUtils()->getFileData(strPath.c_str(), "r", &uSize);

	MY_CHECK(pData, "fopen %s failed", strPath.c_str());
	
	bool bCommentFlag = false;
	char szLine[MAX_STRING];
	unsigned char *pPos = pData;
	
	GetLineFromData(szLine, sizeof(szLine), pPos, pData, uSize);
	
	while (GetLineFromData(szLine, sizeof(szLine), pPos, pData, uSize)) {
		char cFirst = szLine[0];

		switch (cFirst) {
		case 0:
		case '\r':
		case '\n':
		case ';':
			continue;
			break;
		case '/':
			switch (szLine[1]) {
			case '/':	continue; break;
			case '*':	bCommentFlag = true; break;
			default:	break;
			}
			break;
		case '*':
			if (szLine[1] == '/') {
				bCommentFlag = false;
			}
			break;
		default:
			break;
		}

		if (bCommentFlag) {
			continue;
		}

		char *pPos = strstr(szLine, "=");
		if (pPos) {
			*pPos = 0;

			std::string strValue = pPos + 1;
			ReplaceStdString(strValue, "\n", "");
			ReplaceStdString(strValue, "\r", "");
			MyTrim(szLine);
			
			ReplaceStdString(strValue, "\\n", "\n");

			std::string strIndex = szLine;
			MY_ASSERT(m_mapString.find(strIndex) == m_mapString.end(), "警告: %s 含有两项以上 Index 为 %s 的数据",
					  strPath.c_str(), strIndex.c_str());
			m_mapString.insert(make_pair(strIndex, strValue));
		}
	}

	delete[] pData;
}

const char *CStringMgr::GetStr(const char *pszIndex) const
{
	if (NULL == pszIndex) {
		return "";
	}

	std::map < std::string, std::string >::const_iterator it(m_mapString.find(pszIndex));
	if (it == m_mapString.end()) {
		return "";
	}

	return it->second.c_str();
}

const CStringMgr &CStringMgr::Singleton(void)
{
	CStringMgr *pInstance = Instance();
	MY_ASSERT(pInstance);
	return(*pInstance);
}

