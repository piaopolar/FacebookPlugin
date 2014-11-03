//
//  MyIni.h
//
//  Created by Ruoqian, Chen on 2013/06/26
//

#ifndef __MYINI_H__
#define __MYINI_H__

#include <map>
#include <string>

#define KEY_CLONE  "clone"

class CMyIni
{
public:
	CMyIni(void);
	explicit CMyIni(const char *pszFile);
	~ CMyIni(void);

	void OnUpdateFin(void);

	bool Open(const char *pszFileName);
	bool IsSectionExist(const char *pszApp) const;
	const char *GetValue(const char *pszApp, const char *pszKey, const char *pszDefault) const;
	const std::map<std::string, std::string> & GetSectionInfo(const char *pszApp) const;

	bool SetValue(const char *pszApp, const char *pszKey, const char *pszValue);
private:
	std::map<std::string, std::map<std::string, std::string> > m_mapValue;
	std::string m_strFileName;
	std::string m_strFullPath;
};

#endif
