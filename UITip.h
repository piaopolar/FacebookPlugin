#ifndef __UI_TIP_H__
#define __UI_TIP_H__

#include "MyLayer.h"

class TipLayer : public MyLayer
{
public:
    bool init();
	TipLayer(const char *pszMsg);
	TipLayer(const char *pszMsgConfig, const char *pszMsgMain, const char *pszMsgExtra);
    virtual void onEnter();
    virtual void onExit();
private:
	void AutoClose(void);
	std::string m_strConfig;
	std::string m_strMsg;
	std::string m_strMsgExtra;
};

void TipBox(const char *pszMsg);
void TipBox(const char *pszConfig, const char *pszMsgMain, const char *pszExtra = NULL);

#endif