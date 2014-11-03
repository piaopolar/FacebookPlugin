//
//  MyScene.cpp
//
//  Created by Ruoqian, Chen on 2013/06/14
//
#include "StdAfx.h"

#include "MyScene.h"

#include "UIMgr.h"

MyScene::MyScene()
{
	m_nTopLayerEnabledMask = TOP_LAYER_GROUP_MASK_ALL;
	m_nInitData = 0;
	m_bLaceEnabled = true;

	m_pLayout = CCNode::create();
}

void MyScene::onEnter()
{
	CCScene::onEnter();
}

void MyScene::onEnterTransitionDidFinish()
{
	CCScene::onEnterTransitionDidFinish();

	std::vector<MyLayer* > vecTopLayers = UIMgr::GetInstance()->GetTopLayers();
	std::vector<MyLayer*>::iterator it(vecTopLayers.begin());
	for (; it != vecTopLayers.end(); ++it)
    {
		MyLayer* pLayer = *it;
		int nGroup = UIMgr::GetInstance()->GetTopLayerGroup(pLayer);
		if (pLayer->getParent())
        {
			pLayer->removeFromParent();
			if (m_nTopLayerEnabledMask & nGroup)
            {
				this->addChild(pLayer, 1);
			}
		}
        else if (m_nTopLayerEnabledMask & nGroup)
        {
			this->addChild(pLayer, 1);
		}
	}
}

void MyScene::onExitTransitionDidStart()
{
	CCScene::onExitTransitionDidStart();
}

void MyScene::onExit()
{
	CCScene::onExit();
}

void MyScene::EnableTopLayer(int nEnabledGroupMask)
{
	m_nTopLayerEnabledMask = nEnabledGroupMask;
}

bool MyScene::OnInit( void )
{
	CCSize sizeDevice = CCDirector::sharedDirector()->getWinSize();
	CCSize sizeCong = UIMgr::GetInstance()->GetSizeConfig();

	if (m_pLayout) {
		CCNode::addChild(m_pLayout, m_pLayout->getZOrder(), m_pLayout->getTag());
	}

	if (m_bLaceEnabled) {
		if (sizeDevice.height / sizeDevice.width >= 1.5) {
			auto pTop = UIMgr::GetInstance()->CreateConfigNode("LaceTop");
			UIMgr::GetInstance()->NodePositionMove(pTop, ccp(sizeDevice.width / 2, (sizeDevice.height + sizeCong.height) / 2));
			CCNode::addChild(pTop, pTop->getZOrder(), pTop->getTag());
	
			auto pButtom = UIMgr::GetInstance()->CreateConfigNode("LaceBottom");
			UIMgr::GetInstance()->NodePositionMove(pButtom, ccp(sizeDevice.width / 2, (sizeDevice.height - sizeCong.height) / 2));
			CCNode::addChild(pButtom, pButtom->getZOrder(), pButtom->getTag());
		} else {
			auto pLeft = UIMgr::GetInstance()->CreateConfigNode("LaceLeft");
			UIMgr::GetInstance()->NodePositionMove(pLeft, ccp((sizeDevice.width - sizeCong.width) / 2, sizeDevice.height / 2));
			CCNode::addChild(pLeft, pLeft->getZOrder(), pLeft->getTag());

			auto pRight = UIMgr::GetInstance()->CreateConfigNode("LaceRight");
			UIMgr::GetInstance()->NodePositionMove(pRight, ccp((sizeDevice.width + sizeCong.width) / 2, sizeDevice.height / 2));
			CCNode::addChild(pRight, pRight->getZOrder(), pRight->getTag());
		}
	}
	
	UIMgr::GetInstance()->NodePositionMove(m_pLayout, ccp((sizeDevice.width - sizeCong.width) / 2, (sizeDevice.height - sizeCong.height) / 2));

	return true;
}

void MyScene::addChild( CCNode * child )
{
	MY_CHECK(child);
	this->addChild(child, child->getZOrder(), child->getTag());
}

void MyScene::addChild( CCNode * child, int zOrder )
{
	MY_CHECK(child);
	this->addChild(child, zOrder, child->getTag());
}

void MyScene::addChild( CCNode* child, int zOrder, int tag )
{
	if (m_pLayout) {
		m_pLayout->addChild(child, zOrder, tag);
	}
}

void MyScene::removeChild(CCNode * child)
{
	this->removeChild(child, true);
}

void MyScene::removeChild(CCNode* child, bool cleanup)
{
	if (m_pLayout == NULL)
	{
		return;
	}

	m_pLayout->removeChild(child, cleanup);
}

void MyScene::removeChildByTag(int tag)
{
	this->removeChildByTag(tag, true);
}

void MyScene::removeChildByTag(int tag, bool cleanup)
{
	if (m_pLayout == NULL)
	{
		return;
	}

	m_pLayout->removeChildByTag(tag, cleanup);
}

void MyScene::EnableLace( bool bLaceEnabled )
{
	m_bLaceEnabled = bLaceEnabled;
}
