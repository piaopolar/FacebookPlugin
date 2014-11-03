//
//  MyFrameAnimationNode.cpp
//
//  Created by Ruoqian, Chen on 2014/01/08
//
#include "StdAfx.h"

#include "MyFrameAnimationNode.h"

MyFrameAnimationNode* MyFrameAnimationNode::create(const char *pszConfig)
{
    MyFrameAnimationNode *pNode = new MyFrameAnimationNode();
    if (pNode && pNode->initWithConfig(pszConfig))
    {
        pNode->autorelease();
        return pNode;
    }

    CC_SAFE_DELETE(pNode);
    return NULL;
}

bool MyFrameAnimationNode::initWithConfig( const char *pszConfig )
{
	CCSprite::init();

	m_strConfig = pszConfig;
	m_pFrameAnimation = NULL;

	return true;
}


void MyFrameAnimationNode::StopAnimation( void )
{
//	CCLog("StopAnimation m_pFrameAnimation : %x", m_pFrameAnimation);
	if (m_pFrameAnimation && m_pFrameAnimation == this->getActionByTag(ACTION_TAG_ANIMATION)) {
		this->stopAction(m_pFrameAnimation);
		m_pFrameAnimation = NULL;
	}
}

void MyFrameAnimationNode::RunAnimation( float fDelay /*= 0*/, const char *pszConfig /*= NULL*/ )
{
	this->StopAnimation();

	if (NULL == pszConfig) {
		pszConfig = m_strConfig.c_str();
	}

	CCFiniteTimeAction* pActionWithRepeat = UIMgr::GetInstance()->CreateConfigFiniteTimeAction(pszConfig);
	
	const char *pszValue = UIMgr::GetInstance()->GetConfigValue(pszConfig, "Value", "");
	const char *pszFrame = UIMgr::GetInstance()->GetConfigValue(pszConfig, "frame", "");
	CCSpriteFrame *pFrame = NULL;
	if (pszFrame[0]) {
		pFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(pszFrame);
	} else if (pszValue[0]) {
		auto pStr = CCString::createWithFormat("%s0000", pszValue);
		pFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(pStr->getCString());
	}
	
	this->setDisplayFrame(pFrame);

	if (NULL == pActionWithRepeat) {
		return;
	}

	CCSequence* pActionSeq = NULL;
	BOOL bRemoveAfterAct = UIMgr::GetInstance()->GetConfigValue(pszConfig, "RemoveAfterAct", TRUE);
	
	CCFiniteTimeAction* pActionAfterAct = NULL;
	if (bRemoveAfterAct) {
		pActionAfterAct = CCRemoveSelf::create();
	} else {
		pActionAfterAct = CEmptyAction::create();
	}
	
	if (fDelay <= 0) {
		int nDelayMs = UIMgr::GetInstance()->GetConfigValue(pszConfig, "Delay_ms", 0);
		fDelay = nDelayMs / 1000.0f;
	}

	if (fDelay > 0) {
		BOOL bShowFirst = UIMgr::GetInstance()->GetConfigValue(pszConfig, "ShowFirst", FALSE);
		if (bShowFirst) {
			pActionSeq = CCSequence::create(CCDelayTime::create(fDelay), pActionWithRepeat, pActionAfterAct, NULL);
		} else {
			this->setVisible(false);
			pActionSeq = CCSequence::create(CCDelayTime::create(fDelay), CCShow::create(), pActionWithRepeat, pActionAfterAct, NULL);
		}
	} else {
		pActionSeq = CCSequence::create(pActionWithRepeat, pActionAfterAct, NULL);
	}
	
	pActionSeq->setTag(ACTION_TAG_ANIMATION);
	this->runAction(pActionSeq);
	m_pFrameAnimation = pActionSeq;
//	CCLog("RunAnimation m_pFrameAnimation : %x", m_pFrameAnimation);
}

MyFrameAnimationNode::~MyFrameAnimationNode( void )
{
//	CCLOG("~QMyFrameAnimationNode %s", m_strConfig.c_str());
}