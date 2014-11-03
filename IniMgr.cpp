//
//  IniMgr.cpp
//
//  Created by Ruoqian, Chen on 2013/06/26
//
#include "StdAfx.h"

#include "IniMgr.h"

CIniMgr::CIniMgr(void)
{
}

CIniMgr::~CIniMgr(void)
{
}

bool CIniMgr::IsSectionExist(const char *pszFile, const char *pszSection)
{
	if (m_mapIni.find(pszFile) == m_mapIni.end()) {
		m_mapIni[pszFile] = CMyIni(pszFile);
	}

	return m_mapIni[pszFile].IsSectionExist(pszSection);
}

const std::map<std::string, std::string> & CIniMgr::GetSectionInfo( const char *pszFile, const char *pszApp )
{
	if (m_mapIni.find(pszFile) == m_mapIni.end()) {
		m_mapIni[pszFile] = CMyIni(pszFile);
	}

	return m_mapIni[pszFile].GetSectionInfo(pszApp);
}

const char *CIniMgr::GetValue(const char *pszFile, const char *pszSection, const char *pszKey, const char *pszDefault)
{
	if (m_mapIni.find(pszFile) == m_mapIni.end()) {
		m_mapIni[pszFile] = CMyIni(pszFile);
	}

	return m_mapIni[pszFile].GetValue(pszSection, pszKey, pszDefault);
}

int CIniMgr::GetValue(const char *pszFile, const char *pszSection, const char *pszKey, int nDefault)
{
	const char *pszValue = this->GetValue(pszFile, pszSection, pszKey, "");
	if (NULL == pszValue || pszValue[0] == 0) {
		return nDefault;
	}

	int nRet = nDefault;
	if (pszValue[0] == '0' && (pszValue[1] == 'x' || pszValue[1] == 'X')) {
		sscanf(pszValue, "%x", &nRet);
	} else {
		sscanf(pszValue, "%d", &nRet);
	}

	return nRet;
}

CIniMgr* CIniMgr::GetInstance( void )
{
	static CIniMgr* s_pIniMgr = NULL;
	if (NULL == s_pIniMgr) {
		s_pIniMgr = new CIniMgr;
	}

	return s_pIniMgr;
}

void CIniMgr::Reload( const char *pszFile )
{
	std::map<std::string, CMyIni>::iterator it(m_mapIni.find(pszFile));
	if (it != m_mapIni.end()) {
		m_mapIni.erase(it);
	}
}

void CIniMgr::OnUpdateFin( void )
{
	for (auto it(m_mapIni.begin()); it != m_mapIni.end(); ++it) {
 		it->second.OnUpdateFin();
 	}
}

int CIniMgr::SetValue( const char *pszFile, const char *pszSection, const char *pszKey, const char *pszValue )
{
	if (m_mapIni.find(pszFile) == m_mapIni.end()) {
		m_mapIni[pszFile] = CMyIni(pszFile);
	}

	return m_mapIni[pszFile].SetValue(pszSection, pszKey, pszValue);
}
