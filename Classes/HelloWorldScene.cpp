#include "HelloWorldScene.h"

#include "UIMgr.h"
#include "UIEditor.h"

USING_NS_CC;

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !MyLayer::init() )
    {
        return false;
    }

	m_pBtnLogin = this->AddConfigMenuItem("FacebookBtnLogin", menu_selector(HelloWorld::OnBtnLogin));
	m_pBtnLogout = this->AddConfigMenuItem("FacebookBtnLogout", menu_selector(HelloWorld::OnBtnLogout));
	this->AddConfigNode("FacebookBtnSendRequest", menu_selector(HelloWorld::OnBtnSendRequest));
	this->AddConfigNode("FacebookBtnShareScreenShot", menu_selector(HelloWorld::OnBtnShareScreenShot));
	this->AddConfigNode("FacebookBtnReqMyselfInfo", menu_selector(HelloWorld::OnBtnReqMyselfInfo));
	this->AddConfigNode("FacebookBtnReqFriendInfo", menu_selector(HelloWorld::OnBtnReqFriendInfo));
    
    return true;
}


void HelloWorld::menuCloseCallback(CCObject* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
#else
    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
#endif
}

void HelloWorld::OnBtnLogin( CCObject* pObj )
{

}

void HelloWorld::OnBtnLogout( CCObject* pObj )
{

}

void HelloWorld::OnBtnSendRequest( CCObject* pObj )
{

}

void HelloWorld::OnBtnShareScreenShot( CCObject* pObj )
{

}

void HelloWorld::OnBtnReqMyselfInfo( CCObject* pObj )
{

}

void HelloWorld::OnBtnReqFriendInfo( CCObject* pObj )
{

}



bool HelloWorldScene::OnInit( void )
{
	if (!MyScene::OnInit()) {
		return false;
	}

	auto* pLayer = HelloWorld::create();
	this->addChild(pLayer);
	return true;
}
