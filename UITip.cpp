#include "StdAfx.h"
#include "UITip.h"

namespace
{
const char *CONFIG_INI = "ini/TipBox.ini";
};

TipLayer::TipLayer(const char *pszMsg)
{
	m_strMsg = pszMsg ? pszMsg : NULL;
	m_strConfig = "";
	m_strMsgExtra = "";
    
    init();
}

TipLayer::TipLayer( const char *pszMsgConfig, const char *pszMsgMain, const char *pszMsgExtra /*= NULL*/ )
{
	m_strConfig = pszMsgConfig ? pszMsgConfig : "";
	m_strMsg = pszMsgMain ? pszMsgMain : "";
	m_strMsgExtra = pszMsgExtra ? pszMsgExtra : "";
    
    init();
}

bool TipLayer::init()
{
    if (!MyLayer::init()) {
		return false;
	}

	const char *pszConfig = m_strConfig.empty() ? "Stardard" : m_strConfig.c_str();
	const char *pszTipBg = GET_INI_VALUE("Bg", "TipBg");
	CCNode* pNodeBg = this->AddConfigNode(pszTipBg);

	const char *pszMsg = GET_INI_VALUE("Msg", m_strMsgExtra.empty() ? "TipText" : "TipTextMain");
	this->AddConfigTextLabel(pNodeBg, pszMsg, m_strMsg.c_str());

	if (!m_strMsgExtra.empty()) {
		const char *pszMsgExtra = GET_INI_VALUE("MsgExtra", "TipTextExtra");
		this->AddConfigTextLabel(pNodeBg, pszMsgExtra, m_strMsgExtra.c_str());
	}

	return true;
}

void TipLayer::onExit()
{
    this->stopAllActions();
    
    MyLayer::onExit();
}

void TipLayer::onEnter()
{
	MyLayer::onEnter();
    
	this->runAction(CCSequence::create(CCDelayTime::create(1.0f), CCCallFunc::create(this, callfunc_selector(TipLayer::AutoClose)), NULL));
// 	float fDelay = CCDirector::sharedDirector()->getDeltaTime();
// 	this->scheduleOnce(schedule_selector(TipLayer::AutoClose), fDelay + 1.0f);
// 	CCLog("\Tip %x Msg %s %s %f %f\n", this, m_strMsg.c_str(), GetTimeStr().c_str(), fDelay, fDelay + 1.0f);
}


void TipLayer::AutoClose(void)
{
// 	float fDelay = CCDirector::sharedDirector()->getDeltaTime();
// 	CCLog("\Tip %x AutoClose %s %f %f\n", this, GetTimeStr().c_str(), fDelay, f);
	this->removeFromParent();
}


void TipBox( const char *pszMsg )
{
	TipLayer *pTipLayer = new TipLayer(pszMsg);
	pTipLayer->autorelease();
	pTipLayer->MakeSureShowMe();
}

void TipBox( const char *pszConfig, const char *pszMsgMain, const char *pszExtra )
{
	TipLayer *pTipLayer = new TipLayer(pszConfig, pszMsgMain, pszExtra);
	pTipLayer->autorelease();
	pTipLayer->MakeSureShowMe();
}
