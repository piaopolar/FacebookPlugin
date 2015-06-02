#include "HelloWorldScene.h"

#include "UIMgr.h"
#include "UIEditor.h"
#include "UITip.h"
#include "FacebookMgr.h"

USING_NS_CC;

HelloWorld::HelloWorld()
{
	FacebookMgr::GetInstance()->AddObserver(this);
}

HelloWorld::~HelloWorld()
{
	FacebookMgr::GetInstance()->DelObserver(this);
}


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
	this->AddConfigNode("FacebookBtnInvite", menu_selector(HelloWorld::OnBtnInvite));
	m_pNodeUpdate = CCNode::create();
	this->addChild(m_pNodeUpdate);

	this->UpdateView();
    return true;
}

void HelloWorld::OnNotify(int nEvent, int nParam)
{
	switch (nEvent) {
	case FBACTION_LOGIN:
	case FBACTION_LOGIN_TO_SHARE:
	case FBACTION_LOGOUT:
	case FBACTION_IMAGE_UPDATE:
	case FBACTION_REQ_MYINFO:
		this->UpdateView();
		break;
	default:
		break;
	}
}

void HelloWorld::UpdateView()
{
	bool bLogin = FacebookMgr::GetInstance()->IsLogin();
	m_pBtnLogin->setEnabled(bLogin ? false : true);
	m_pBtnLogout->setEnabled(bLogin ? true : false);

	m_pNodeUpdate->removeAllChildren();

	CCPoint ptOffset;
	auto vecFriend = FacebookMgr::GetInstance()->GetFriendsInfo();
	int nCount = 0;
	for (auto it(vecFriend.begin()); it != vecFriend.end(); ++it, ++nCount) {		
		auto pName = this->AddConfigTextLabel(m_pNodeUpdate, "FacebookFriendName", it->m_strName.c_str());
		UIMgr::GetInstance()->NodePositionMove(pName, ptOffset);

		auto id = it->m_i64Id;
		auto cstrFileName = CCString::createWithFormat("%lld.jpg", id);
		std::string strPath = CCFileUtils::sharedFileUtils()->getWritablePath();
		strPath += cstrFileName->getCString();
		bool bFileExist = CCFileUtils::sharedFileUtils()->isFileExist(strPath.c_str());

		CCLog("UpdateView %lld %s %s %s Exist %d", id, it->m_strName.c_str(), cstrFileName->getCString(), strPath.c_str(), bFileExist ? 1 : 0);

		if (!bFileExist) {
			continue;
		}

		auto pNodeImage = this->AddConfigSprite(m_pNodeUpdate, "FacebookFriendImage", strPath.c_str());
		UIMgr::GetInstance()->NodePositionMove(pNodeImage, ptOffset);
		ptOffset.y += 50;
	}
	
	auto myInfo = FacebookMgr::GetInstance()->GetMyselfInfo();
	this->AddConfigTextLabel(m_pNodeUpdate, "FacebookMyName", myInfo.m_strName.c_str());
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
	FacebookMgr::GetInstance()->Login();
}

void HelloWorld::OnBtnLogout( CCObject* pObj )
{
	FacebookMgr::GetInstance()->Logout();
}

void HelloWorld::OnBtnSendRequest( CCObject* pObj )
{
	FacebookMgr::GetInstance()->TrySendRequest();
}

void HelloWorld::OnBtnShareScreenShot( CCObject* pObj )
{
	FacebookMgr::GetInstance()->TryShareScreenShot();
}

void HelloWorld::OnBtnReqMyselfInfo( CCObject* pObj )
{
	if (FacebookMgr::GetInstance()->IsLogin()) {
		FacebookMgr::GetInstance()->RequestMyselfInfo();
	} else {
		TipBox("Please login first");
	}
}

void HelloWorld::OnBtnReqFriendInfo( CCObject* pObj )
{
	if (FacebookMgr::GetInstance()->IsLogin()) {
		FacebookMgr::GetInstance()->RequestFriendInfo();
	} else {
		TipBox("Please login first");
	}
}

void HelloWorld::OnBtnInvite(CCObject* pObj)
{
	FacebookMgr::GetInstance()->InviteFriends("https://fb.me/1602961223294531", "http://cdn.cgmdigi.com/kc/res/pc/1/images/icon.png");
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
