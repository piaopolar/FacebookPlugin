//
//  MyLayer.cpp
//
//  Created by Ruoqian, Chen on 2013/06/14
//
#include "StdAfx.h"

#include "MyScrollView.h"
#include "MyMenu.h"
#include "MyLayer.h"

#include "UIMgr.h"

//#define CHECK_CLICK_EVENT

MyLayer::MyLayer(void)
{
	m_bUpdateOnEnterEnabled = true;
	m_pSprite = NULL;
	m_pSpriteBack = NULL;
	m_pMenuLayer = NULL;
	m_pSpriteFront = NULL;
	CCSize sizeWin = UIMgr::GetInstance()->GetSizeConfig();
	m_rect.setRect(0, 0, sizeWin.width, sizeWin.height);
	m_ptPosition.setPoint(sizeWin.width / 2, sizeWin.height / 2);
	m_ptAnchor.setPoint(0.5f, 0.5f);
	m_bSwallowsTouches = false;
	this->setKeypadEnabled(true);
}

bool MyLayer::init()
{
	auto sizeTmp = m_rect;
    if (!CCLayer::init()) {
		return false;
	}

	m_rect = sizeTmp;

    if (NULL == m_pSpriteBack) {
        if (m_strFileBG.empty()) {
            m_pSpriteBack = SpriteCreate();
        } else {
			m_strFileBG = GetImageFileName(m_strFileBG.c_str());
            m_pSpriteBack = SpriteCreate(m_strFileBG.c_str());
        }

		MY_ASSERT(m_pSpriteBack, "spriteBack create err!!");
        m_pSpriteBack->setAnchorPoint(m_ptAnchor);
        this->addChild(m_pSpriteBack);
        m_pSpriteBack->retain();
    }
        
    if (NULL == m_pMenuLayer) {
        m_pMenuLayer = MyMenu::create();
        MY_ASSERT(m_pMenuLayer, "menuLayer create err!!");
        m_pMenuLayer->setAnchorPoint(m_ptAnchor);
        //m_pMenuLayer->setCascadeOpacityEnabled(true);
        //m_pMenuLayer->setCascadeColorEnabled(true);
        this->addChild(m_pMenuLayer);
        m_pMenuLayer->retain();
    }
        
    if (NULL == m_pSpriteFront) {
        m_pSpriteFront = SpriteCreate();
        CCAssert(m_pSpriteFront, "spriteFront create err!!");
        m_pSpriteFront->setAnchorPoint(m_ptAnchor);
        this->addChild(m_pSpriteFront);
        //m_pSpriteFront->setCascadeOpacityEnabled(true);
        //m_pSpriteFront->setCascadeColorEnabled(true);
        m_pSpriteFront->retain();
    }
        
    this->setContentSize(m_rect.size);
    this->SetAddFrontMode(false);

	this->AdjustResolution();

#ifdef CHECK_CLICK_EVENT
	if (m_pMenuLayer) {
		m_pMenuLayer->setTouchEnabled(true);
	}
#endif

	return true;
}

void MyLayer::AdjustResolution( void )
{
#if 0
	auto sizeDevice = CCDirector::sharedDirector()->getWinSize();
	NodePositionMove(this, ccp(0, (sizeDevice.height - m_rect.size.height) / 2));
#endif
}

MyLayer::~MyLayer(void)
{
	CC_SAFE_RELEASE_NULL(m_pSpriteFront);
	CC_SAFE_RELEASE_NULL(m_pSpriteBack);
	CC_SAFE_RELEASE_NULL(m_pMenuLayer);
	
	m_pSprite = NULL;
}

bool MyLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	return m_bSwallowsTouches;
}

void MyLayer::setSwallowTouches(bool bSwallowsTouches)
{
	m_bSwallowsTouches = bSwallowsTouches;
	if (m_bSwallowsTouches) {
		this->setTouchMode(kCCTouchesOneByOne);
	} else {
		this->setTouchMode(kCCTouchesAllAtOnce);
	}
}

bool MyLayer::getSwallowTouches(void) const
{
	return m_bSwallowsTouches;
}

void MyLayer::onExit()
{
#ifdef CHECK_CLICK_EVENT
	if (m_pMenuLayer) {
		m_pMenuLayer->setTouchEnabled(false);
	}
#endif

	CCLayer::onExit();
}

void MyLayer::setContentSize( const CCSize& contentSize )
{
	CCLayer::setContentSize(contentSize);

	m_rect = CCRect(0, 0, contentSize.width, contentSize.height);
	m_ptPosition.setPoint(contentSize.width / 2, contentSize.height / 2);

	if (m_pSpriteBack) {
		m_pSpriteBack->setPosition(m_ptPosition);
		m_pSpriteBack->setContentSize(m_rect.size);
	}

	if (m_pMenuLayer) {
 		m_pMenuLayer->setPosition(ANCHOR_LEFT_BOTTOM);
 		m_pMenuLayer->setContentSize(m_rect.size);
	}

	if (m_pSpriteFront) {
		m_pSpriteFront->setPosition(m_ptPosition);
		m_pSpriteFront->setContentSize(m_rect.size);
	}
}

void MyLayer::onEnter()
{	
	CCLayer::onEnter();
    
    if (m_bUpdateOnEnterEnabled) {
        
        UpdateView();
	}
}

void MyLayer::SetAddFrontMode( bool bAddFront /*= true*/ )
{
	m_pSprite = bAddFront ? m_pSpriteFront : m_pSpriteBack;
}

void MyLayer::onEnterTransitionDidFinish()
{
	CCLayer::onEnterTransitionDidFinish();
}

void MyLayer::onExitTransitionDidStart()
{
	CCLayer::onExitTransitionDidStart();
}

void MyLayer::UpdateView()
{

}

CCSprite* MyLayer::AddSprite( CCSprite* pSprite )
{
	if (NULL == m_pSprite || NULL == pSprite) {
		return NULL;
	}

	m_pSprite->addChild(pSprite);
	return pSprite;
}

void MyLayer::SetImage( const char *pszFileName )
{
	m_strFileBG = pszFileName ? pszFileName : "";
	if (m_pSpriteBack && !m_strFileBG.empty()) {
		m_strFileBG = GetImageFileName(m_strFileBG.c_str());
		m_pSpriteBack->initWithFile(m_strFileBG.c_str());
	}
}

void MyLayer::ClearFrontSprite( void )
{
	if (m_pSpriteFront) {
		m_pSpriteFront->removeAllChildren();
	}
}

void MyLayer::ApplyConfig(const char *pszConfigSection)
{
	UIMgr::GetInstance()->ManageMyLayer(this, pszConfigSection);

	std::string strMenu = pszConfigSection;
	strMenu += "_Menu";
	UIMgr::GetInstance()->ManageMyMenu(m_pMenuLayer, strMenu.c_str());
}

CCNode* MyLayer::AddConfigNode(CCNode* pParent, const char *pszConfigSection, SEL_MenuHandler selector)
{
	int nType;
	CCNode* pNode = UIMgr::GetInstance()->CreateConfigNode(pszConfigSection, &nType);
	if (pNode) {
		MY_ASSERT(NODETYPE_MENUITEM != nType || dynamic_cast<CCMenu*>(pParent), "try to add MENUITEM [%s] to node NOT with MENU type", pszConfigSection);
		pParent->addChild(pNode);
	}

	return pNode;
}

void MyLayer::AddConfigChild(const char *pszConfigChild)
{
	std::vector<std::string> vecStrChild = StringSplit(pszConfigChild, '|');
	std::vector<std::string>::const_iterator it(vecStrChild.begin());
	for (; it != vecStrChild.end(); ++it) {
		this->AddConfigNode(it->c_str());
	}
}

void MyLayer::AddConfigChild( CCNode* pParent, const char *pszConfigChild )
{
	std::vector<std::string> vecStrChild = StringSplit(pszConfigChild, '|');
	std::vector<std::string>::const_iterator it(vecStrChild.begin());
	for (; it != vecStrChild.end(); ++it) {
		this->AddConfigNode(pParent,it->c_str());
	}
}

CCNode* MyLayer::AddConfigNode(const char *pszConfigSection, SEL_MenuHandler selector)
{
	int nType = 0;
	CCNode* pNode = UIMgr::GetInstance()->CreateConfigNode(pszConfigSection, &nType, this, selector);

	MY_ASSERT(m_pSprite, "you must call AddConfigNode [%s] after parent node init", pszConfigSection);

	switch (nType) {
	case NODETYPE_MENUITEM:
		m_pMenuLayer->addChild(pNode);
		break;
	case NODETYPE_SPRITE:
	case NODETYPE_TEXTLABEL:
	default:
		m_pSprite->addChild(pNode);
		break;
	}

	return pNode;
}

MyEditBox * MyLayer::AddConfigEditBox( const char *pszConfigSection, MyEditBoxDelegate* pDelegate )
{
	if (NULL == pDelegate) {
		return NULL;
	}
	
	MyEditBox *pEditBox = UIMgr::GetInstance()->CreateConfigEditBox(pszConfigSection);
	if (pEditBox) {
		pEditBox->setDelegate(pDelegate);
		this->addChild(pEditBox);
	}
	
	return pEditBox;
}

MyScrollView* MyLayer::AddToConfigScrollView( const char *pszConfig, CCScrollViewDelegate* pDelegate )
{
	MyScrollView* pScrollView = this->AddToConfigScrollViewWithNoDelegate(pszConfig);
	MY_ASSERT(pDelegate, "call AddToConfigScrollViewWithNoDelegate if no need callback");

	if (pScrollView && pDelegate) {
		pScrollView->setDelegate(pDelegate);
	}

	UI_TRACE("MyLayer::AddToConfigScrollView %s %x", pszConfig, pScrollView);
	return pScrollView;
}

MyScrollView* MyLayer::AddToConfigScrollViewWithNoDelegate( const char *pszConfig )
{
	MyScrollView* pScrollView = UIMgr::GetInstance()->CreateConfigScrollView(pszConfig);
	if (pszConfig) {
		pScrollView->setContainer(this);
	}

	UI_TRACE("MyLayer::AddToConfigScrollViewWithNoDelegate %s %x", pszConfig, pScrollView);
	return pScrollView;
}

CCMenuItemSprite* MyLayer::AddConfigMenuItem( const char *pszConfigSection, SEL_MenuHandler selector /*= NULL*/ )
{
	return this->AddConfigMenuItem(m_pMenuLayer, pszConfigSection, selector);
}

CCMenuItemSprite* MyLayer::AddConfigMenuItem( CCNode* pParent, const char *pszConfigSection, SEL_MenuHandler selector /*= NULL*/ )
{
	if (NULL == pParent) {
		return NULL;
	}

	MY_ASSERT(dynamic_cast<CCMenu*>(pParent), "try to add MENUITEM [%s] to node NOT with MENU type", pszConfigSection);

	CCMenuItemSprite* pMenuItem = UIMgr::GetInstance()->CreateConfigMenuItem(pszConfigSection, this, selector);
	if (pMenuItem) {
		pParent->addChild(pMenuItem);
	}

	return pMenuItem;
}

CCSprite* MyLayer::AddConfigSprite( const char *pszConfigSection, const char *pszImageFile /*= NULL*/ )
{
	return this->AddConfigSprite(m_pSprite, pszConfigSection, pszImageFile);
}

CCSprite* MyLayer::AddConfigSprite( CCNode* pParent, const char *pszConfigSection, const char *pszImageFile /*= NULL*/ )
{
	if (NULL == pParent) {
		return NULL;
	}

	auto* pSprite = UIMgr::GetInstance()->CreateConfigSprite(pszConfigSection, pszImageFile);
	if (pSprite) {
        pSprite->setCascadeColorEnabled(true);
        pSprite->setCascadeOpacityEnabled(true);
		pParent->addChild(pSprite);
	}

	return pSprite;
}

CCLabelTTF* MyLayer::AddConfigTextLabel( const char *pszConfigSection, const char *pszString /*= NULL*/, int nWidth )
{
	return this->AddConfigTextLabel(m_pSprite, pszConfigSection, pszString, nWidth);
}

CCLabelTTF* MyLayer::AddConfigTextLabel( CCNode* pParent, const char *pszConfigSection, const char *pszString /*= NULL*/, int nWidth )
{
	if (NULL == pParent) {
		return NULL;
	}

	auto* pTextLabel = UIMgr::GetInstance()->CreateConfigTextLabel(pszConfigSection, pszString, nWidth);
	if (pTextLabel) {
		pParent->addChild(pTextLabel);
	}

	return pTextLabel;
}

bool MyLayer::MakeSureShowMe(void)
{
	if (UIMgr::GetInstance()->IsCurrentSceneStable()) {
		auto* pRunningScene = CCDirector::sharedDirector()->getRunningScene();
		if (pRunningScene) {
			pRunningScene->addChild(this);
			return true;
		}
	}

	CCScheduler* pScheduler = CCDirector::sharedDirector()->getScheduler();
	MY_ASSERT(pScheduler);

	if (NULL == pScheduler) {
		return false;
	}

	pScheduler->scheduleSelector(schedule_selector(MyLayer::AddMeProperlyTime), this, 0, false);
	return false;
}

void MyLayer::AddMeProperlyTime( float f )
{
	if (UIMgr::GetInstance()->IsCurrentSceneStable()) {
//		CCLOG("MyLayer::AddMeProperlyTime  CurrentScene %d", UIMgr::GetInstance()->GetCurrentSceneType());

		auto pRunningScene = CCDirector::sharedDirector()->getRunningScene();
		if (NULL == pRunningScene) {
			return;
		}

		pRunningScene->addChild(this);

		CCScheduler* pScheduler = CCDirector::sharedDirector()->getScheduler();
		if (NULL == pScheduler) {
			return;
		}

		pScheduler->unscheduleSelector(schedule_selector(MyLayer::AddMeProperlyTime), this);
	}
}

CCNode* MyLayer::AddConfigNodeWithChild( const char *pszConfigSection, const char *pszChildConfigSection )
{
	CCNode* pNode = this->AddConfigNode(pszConfigSection);
	if (NULL == pNode) {
		return NULL;
	}

	UIMgr::GetInstance()->AddConfigChild(pNode, pszChildConfigSection);
	return pNode;
}

CCNode* MyLayer::AddConfigNodeWithChild( const char *pszConfigSection, CCNode* pNodeChild )
{
	CCNode* pNode = this->AddConfigNode(pszConfigSection);
	if (NULL == pNode) {
		return NULL;
	}

	if (pNodeChild) {
		pNode->addChild(pNodeChild);
	}

	return pNode;
}

void MyLayer::SpriteSetImage( CCSprite* pSprite, const char *pszFile )
{
	if (NULL == pSprite) {
		return;
	}

	CCPoint ptAnchor = pSprite->getAnchorPoint();
	pSprite->initWithFile(pszFile);
	pSprite->setAnchorPoint(ptAnchor);
}

void MyLayer::EnabledUpdateOnEnter( bool bEnabled )
{
	m_bUpdateOnEnterEnabled = bEnabled;
}

CCSprite* MyLayer::AddConfigDictImage( CCNode* pParent, const char *pszConfigSection, const char *pszText )
{
	CCSprite* pSprite = UIMgr::GetInstance()->CreateConfigDictImage(pszConfigSection, pszText);
	pParent->addChild(pSprite);
	return pSprite;
}

CCNode* MyLayer::FindConfigNode( const char *pszConfig ) const
{
	auto it = m_mapConfigNode.find(pszConfig);
	if (it == m_mapConfigNode.end()) {
		return NULL;
	}

	return it->second;
}

void MyLayer::MapConfigNode( void )
{
	m_mapConfigNode.clear();
	std::vector<CCNode*> vecNodes;
	GetNodeAllChild(this, vecNodes);
	for (auto it(vecNodes.begin()); it != vecNodes.end(); ++it) {
		CCNode *pNode = *it;
		const char *pszConfig = UIMgr::GetInstance()->GetNodeConfig(pNode->m_uID);
		if (NULL == pszConfig) {
			continue;
		}

		m_mapConfigNode[pszConfig] = pNode;
	}
}

void MyLayer::keyBackClicked()
{
	if (UIMgr::GetInstance()->IsModuleLayerExist()) {
		return;
	}

	this->OnOperBack();
}

void MyLayer::OnBtnBack( CCObject* pObj )
{
	this->OnOperBack();
}

void MyLayer::OnOperBack( void )
{

}
