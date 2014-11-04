//
//  StringMgr.h
//
//  Created by Ruoqian, Chen on 2013/08/28
//
#ifndef __STRING_MGR_H__
#define __STRING_MGR_H__

class CStringMgr
{
private:
	CStringMgr(void);
public:
	~ CStringMgr(void);
	static CStringMgr* Instance(void);
	static const CStringMgr& Singleton(void);
	static void Release(void);

	void Reload(void);
	const char *GetStr(const char *pszIndex) const;
private:
	std::map<std::string, std::string> m_mapString;
};

#define g_objStrMgr CStringMgr::Singleton()

#endif
