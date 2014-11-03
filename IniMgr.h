//
//  IniMgr.h
//
//  Created by Ruoqian, Chen on 2013/06/26
//
#ifndef __INIMGR_H__
#define __INIMGR_H__

#include "MyIni.h"

#define INFO_INI "ini/info.ini"
#define MSGBOX_INI "ini/MsgBox.ini"
#define DEBUG_INI "ini/debug_config.debugini"

class CIniMgr
{
private:
	CIniMgr(void);
public:
	~ CIniMgr(void);
	static CIniMgr* GetInstance(void);

	void OnUpdateFin(void);
	void Reload(const char *pszFile);
	bool IsSectionExist(const char *pszFile, const char *pszSection);
	const std::map<std::string, std::string> & GetSectionInfo(const char *pszFile, const char *pszApp);
	const char *GetValue(const char *pszFile, const char *pszSection, const char *pszKey, const char *pszDefault);
	int GetValue(const char *pszFile, const char *pszSection, const char *pszKey, int nDefault);

	int SetValue(const char *pszFile, const char *pszSection, const char *pszKey, const char *pszValue);
private:
	std::map<std::string, CMyIni> m_mapIni;
};

#endif
