//
//  MyIni.cpp
//
//  Created by Ruoqian, Chen on 2013/06/26
//

#include "StdAfx.h"

#include "MyIni.h"

CMyIni::CMyIni(void)
{
}

CMyIni::CMyIni(const char *pszFile)
{
	this->Open(pszFile);
}

CMyIni::~CMyIni(void)
{
}

bool CMyIni::Open(const char *pszFileName)
{
	m_strFileName = pszFileName ? pszFileName : "";
	m_strFullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(pszFileName);
	const char *pszFilePath = m_strFullPath.c_str();

	unsigned long uSize = 0;
	unsigned char *pData = CCFileUtils::sharedFileUtils()->getFileData(pszFilePath, "r", &uSize);

	bool bMustExist = true;
	if (strstr(pszFileName, ".debugini")) {
		bMustExist = false;
	}

	MY_ASSERT(pData || !bMustExist, "fopen %s failed", pszFilePath);
	if (NULL == pData) {
		CCLOG("打开文件 %s 失败", pszFilePath);
		return false;
	}

	bool bCommentFlag = false;
	char szLine[MAX_STRING];
	std::string strSection;
	std::map<std::string, int> mapSectionError;
	unsigned char *pPos = pData;
	
	while (GetLineFromData(szLine, sizeof(szLine), pPos, pData, uSize)) {

		//~~~~~~~~~~~~~~~~~~~~
		char cFirst = szLine[0];
		//~~~~~~~~~~~~~~~~~~~~

		switch (cFirst) {
		case 0:
		case '\r':
		case '\n':
		case ';':
			continue;
			break;
		case '/':
			switch (szLine[1]) {
			case '/':
				continue;
				break;
			case '*':
				bCommentFlag = true;
				break;
			default:
				break;
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

		MyTrim(szLine);

		if (cFirst == '[') {
			char *pszEnd = strstr(szLine, "]");

			if (pszEnd) {
				*pszEnd = 0;
				strSection = szLine + 1;

				if (m_mapValue.find(strSection) != m_mapValue.end()) {
					if (mapSectionError.find(strSection) == mapSectionError.end()) {
						CCLOG("		警告: ini文件 %s 含有两项以上 Section 为 %s 的数据, ", pszFilePath,
								  strSection.c_str());
						MY_ASSERT(0, "muti same value section [%s] in ui_element.ini", strSection.c_str());
						mapSectionError[strSection] = 1;
					}

					strSection = "重复项";
				}
			}

			continue;
		}

		char *pPos = strstr(szLine, "=");

		if (pPos) {
			*pPos = 0;

			std::string strValue = pPos + 1;

			ReplaceStdString(strValue, "\n", "");
			ReplaceStdString(strValue, "\r", "");
			m_mapValue[strSection][szLine] = strValue;
		}
	}

	delete[] pData;
	return true;
}

const char *CMyIni::GetValue(const char *pszApp, const char *pszKey, const char *pszDefault) const
{
	if (NULL == pszApp) {
		return pszDefault;
	}

	std::map<std::string, std::map<std::string, std::string> >::const_iterator it = m_mapValue.find(pszApp);

	if (it == m_mapValue.end()) {
		return pszDefault;
	}

	std::map<std::string, std::string>::const_iterator itIn = it->second.find(pszKey);

	if (itIn == it->second.end()) {
		std::map<std::string, std::string>::const_iterator itClone = it->second.find(KEY_CLONE);
		if (itClone != it->second.end()) {
			return this->GetValue(itClone->second.c_str(), pszKey, pszDefault);
		}

		return pszDefault;
	}

	return itIn->second.c_str();
}

bool CMyIni::IsSectionExist( const char *pszApp ) const
{
	std::map<std::string, std::map<std::string, std::string> >::const_iterator it = m_mapValue.find(pszApp);

	if (it == m_mapValue.end()) {
		return false;
	}

	return true;
}

const std::map<std::string, std::string> & CMyIni::GetSectionInfo( const char *pszApp ) const
{
	auto it = m_mapValue.find(pszApp);

	if (it == m_mapValue.end()) {
		static std::map<std::string, std::string> s_mapEmpty;
		return s_mapEmpty;
	}

	return it->second;
}

bool CMyIni::SetValue( const char *pszApp, const char *pszKey, const char *pszValue )
{
	auto& rMap = m_mapValue[pszApp];
	rMap[pszKey] = pszValue;
	return true;
}

void CMyIni::OnUpdateFin( void )
{
	std::string strFullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(m_strFileName.c_str());
	if (strFullPath == m_strFullPath) {
		return;
	}

	m_mapValue.clear();
	this->Open(m_strFileName.c_str());
}
