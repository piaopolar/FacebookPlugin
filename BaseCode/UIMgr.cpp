//
//  UIMgr.cpp
//
//  Created by Ruoqian, Chen on 2013/06/14
//
#include "StdAfx.h"

#include "UIMgr.h"

#include "IniMgr.h"
#include "MyMenu.h"
#include "MyCCImpl.h"
#include "MyDrawNode.h"
#include "MyScrollView.h"
#include "MyFrameAnimationNode.h"

#include "SceneFactoryInterface.h"

namespace
{
const char *CONFIG_INI = "ini/ui_element.ini";

const char *GetStrValue(const char *pszConfig, const char *pszKey, const char *pszDefault)
{
	const char *pszValue = GET_INI_VALUE(pszKey, pszDefault);
	if (pszValue && strstr(pszValue, "STR_")) {
		pszValue = g_objStrMgr.GetStr(pszValue);
	}

	return pszValue;
}
}

USING_NS_CC_EXT;

void QMyGroupNode::addChild( CCNode *pAddNode )
{
	if (NULL == pAddNode) {
		return;
	}

#ifdef DEBUG_MODE
	QMyGroupNode *pGroupNodeAddNode = dynamic_cast<QMyGroupNode*>(pAddNode);
	MY_ASSERT(pGroupNodeAddNode, "try to add NOT GROUPNODE to node with GROUPNODE type");
#endif

	CCArray* pMyChildArray = this->getChildren();

	if (NULL == pMyChildArray) {
		return;
	}

	CCArray* pAddChildArray = pAddNode->getChildren();

	if (NULL == pAddChildArray) {
		return;
	}

	MY_ASSERT(pMyChildArray->count() == pAddChildArray->count());

	std::vector<CCNode*> vecAddChild;

	CCObject* pObj;
	CCARRAY_FOREACH(pAddChildArray, pObj)
	{
		CCNode *pAddChild = dynamic_cast<CCNode*>(pObj);
		if (NULL == pAddChild) {
			continue;
		}

		vecAddChild.push_back(pAddChild);
	}

	pAddNode->removeAllChildren();

	std::vector<CCNode*>::iterator it(vecAddChild.begin());
	CCARRAY_FOREACH(pMyChildArray, pObj)
	{
		CCNode *pAddChild = *it;
		CCNode *pChild = dynamic_cast<CCNode*>(pObj);
		pChild->addChild(pAddChild);
		++it;
	}
}

QMyMenuItemSprite* QMyGroupNode::CreateMenuItem( void )
{
	CCArray* pMyChildArray = this->getChildren();

	if (NULL == pMyChildArray) {
		return NULL;
	}

	MY_ASSERT(pMyChildArray->count() == 3);

	CCObject* pObj;
	std::vector<CCNode*> vecMyChild;

	CCARRAY_FOREACH(pMyChildArray, pObj)
	{
		CCNode *pChild = dynamic_cast<CCNode*>(pObj);
		if (NULL == pChild) {
			continue;
		}

		vecMyChild.push_back(pChild);
	}

	this->removeAllChildren();

	QMyMenuItemSprite* pMenuItem = QMyMenuItemSprite::create(vecMyChild[0], vecMyChild[1], vecMyChild[2]);
	return pMenuItem;
}


static UIMgr* s_pUIMgr = NULL;

UIMgr::UIMgr(void)
{
	m_strConfigFileName = "ini/ui_element.ini";
	m_sizeConfig.setSize(640, 960);
	m_sizeDevice.setSize(640, 960);
	m_pCCImpl = new MyCCOjbectImpl;
	m_pSceneFactoryImpl = NULL;
	m_nMultiResolutionPolicy = MULTIRESOLUTION_POLICY_LACE;
}

UIMgr::~UIMgr(void)
{
	CC_SAFE_DELETE(m_pCCImpl);
	std::vector<MyLayer* >::iterator it(m_vecTopLayer.begin());
	for (; it != m_vecTopLayer.end(); ++it) {
		CCLayer* pLayer = *it;
		pLayer->release();
	}
}

UIMgr* UIMgr::GetInstance( void )
{
	if (NULL == s_pUIMgr) {
		s_pUIMgr = new UIMgr;
	}
	return s_pUIMgr;
}

void UIMgr::Release( void )
{
	CC_SAFE_DELETE(s_pUIMgr);
}

void UIMgr::RemoveUnusedRes( float fDelay /*= -1*/ )
{
	m_pCCImpl->RemoveUnusedRes(fDelay);
}

void UIMgr::AddTopLayer(MyLayer* pLayer, int nGroup)
{
	if (pLayer) {
		pLayer->retain();
		m_vecTopLayer.push_back(pLayer);
		m_mapTopLayerGroupInfo[pLayer] = nGroup;
	}
}

std::vector<MyLayer* > UIMgr::GetTopLayers( void ) const
{
	return m_vecTopLayer;
}

int UIMgr::GetTopLayerGroup( MyLayer* pLayer ) const
{
	std::map<MyLayer*, int>::const_iterator it(m_mapTopLayerGroupInfo.find(pLayer));
	if (it == m_mapTopLayerGroupInfo.end()) {
		return 0;
	}

	return it->second;
}

void UIMgr::AddModuleLayer( MyLayer* pLayer )
{
	m_setModuleLayer.insert(pLayer);
}

void UIMgr::DelModuleLayer( MyLayer* pLayer )
{
	m_setModuleLayer.erase(pLayer);
}

bool UIMgr::IsModuleLayerExist( void ) const
{
	return !m_setModuleLayer.empty();
}

#define GET_VALUE(key, defaultValue) (CIniMgr::GetInstance()->GetValue(m_strConfigFileName.c_str(), pszConfig, (key), (defaultValue)));
#define GET_GROUP_VALUE(group, defaultValue) (CIniMgr::GetInstance()->GetValue(m_strConfigFileName.c_str(), (group), pszConfig, (defaultValue)));

int UIMgr::GetConfigValue(const char *pszConfig, const char *pszKey, int nDefaultValue)
{
	return GET_VALUE(pszKey, nDefaultValue);
}

const char* UIMgr::GetConfigValue( const char *pszConfig, const char *pszKey, const char *pszDefault )
{
	return GET_VALUE(pszKey, pszDefault);
}

CCFiniteTimeAction* UIMgr::CreateConfigFiniteTimeAction(const char *pszConfig)
{
	const char *pszValue = GET_VALUE("Value", "");
	const char *pszPicPlist = GET_VALUE("plistPic", "");
	if (pszPicPlist[0]) {
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(pszPicPlist);
	} else if (pszValue[0]) {
		auto* pStr = CCString::createWithFormat("ani/%s.plist", pszValue);
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(pStr->getCString());
	}

	const char *pszAniPlist = GET_VALUE("plistAni", "");
	if (pszAniPlist[0]) {
		CCAnimationCache::sharedAnimationCache()->addAnimationsWithFile(pszAniPlist);
	} else if (pszValue[0]) {
		auto* pStr = CCString::createWithFormat("ani/%s_ani.plist", pszValue);
		CCAnimationCache::sharedAnimationCache()->addAnimationsWithFile(pStr->getCString());
	}
	
	int nRepeat = GET_VALUE("Repeat", 1);
	if (0 == nRepeat) {
		return NULL;
	}

	const char *pszAni = GET_VALUE("ani", "");
	CCAnimation* pAni = NULL;
	if (pszAni[0]) {
		pAni = CCAnimationCache::sharedAnimationCache()->animationByName(pszAni);
	} else {
		auto* pStr = CCString::createWithFormat("%s_ani", pszValue);
		pAni = CCAnimationCache::sharedAnimationCache()->animationByName(pStr->getCString());
	}

	CCFiniteTimeAction* pActionWithRepeat = NULL;
	switch (nRepeat) {
	case 0:
		break;
	case 1:
		pActionWithRepeat = CCAnimate::create(pAni);
		break;
	case -1:
		//pActionWithRepeat = CCRepeatForever::create(CCAnimate::create(pAni));
		pActionWithRepeat = CCRepeat::create(CCAnimate::create(pAni), UINT32_MAX);
		break;
	default:
		pActionWithRepeat = CCRepeat::create(CCAnimate::create(pAni), nRepeat);
		break;
	}
	
	return pActionWithRepeat;
}

MyFrameAnimationNode* UIMgr::CreateConfigAnimation(const char *pszConfig, float fDelay)
{	
	MyFrameAnimationNode* pSpriteAni = MyFrameAnimationNode::create(pszConfig);
	
	if (fDelay <= 0) {
		int nDelayMs = GET_VALUE("Delay_ms", 0);
		fDelay = nDelayMs / 1000.0f;
	}

	pSpriteAni->RunAnimation(fDelay);

	int nSrcFunc = GET_VALUE("src", GL_SRC_ALPHA);
	int nDstFunc = GET_VALUE("dst", GL_ONE_MINUS_SRC_ALPHA);
	if (GL_SRC_ALPHA != nSrcFunc || GL_ONE_MINUS_SRC_ALPHA != nDstFunc) {
		ccBlendFunc func = { static_cast<GLenum>(nSrcFunc), static_cast<GLenum>(nDstFunc) };
		pSpriteAni->setBlendFunc(func);
	}

	this->ManageColor(pSpriteAni, pszConfig);
	this->ManageNodeAndAddChild(pSpriteAni, pszConfig);
	
	return pSpriteAni;
}

CCParticleSystemQuad* UIMgr::CreateConfigParticleSystemQuad( const char *pszConfig )
{
	const char *pszValue = GET_VALUE("Value", "");
	auto* pStrPath = CCString::createWithFormat("particle_system/%s.plist", pszValue);
	CCParticleSystemQuad* pSystem = CCParticleSystemQuad::create(pStrPath->getCString());

	if (NULL == pSystem) {
		return NULL;
	}
		
	this->ManageNodeAndAddChild(pSystem, pszConfig);
	return pSystem;
}

CCControlSlider* UIMgr::CreateConfigSlider( const char *pszConfig )
{
	CCSprite* pSpriteBG = NULL;
	const char *pszBg = GET_VALUE("bg", "");
	if (strstr(pszBg, ".png")) {
		pSpriteBG = SpriteCreate(pszBg);
	} else {
		pSpriteBG = this->CreateConfigSprite(pszBg);
	}

	const char *pszProgress = GET_VALUE("progress", "");

	CCSprite* pSpriteThumb = NULL;
	const char *pszThumb = GET_VALUE("thumb", "");
	if (strstr(pszThumb, ".png")) {
		pSpriteThumb = SpriteCreate(pszThumb);
	} else {
		pSpriteThumb = this->CreateConfigSprite(pszThumb);
	}

	CCControlSlider* pSlider = CCControlSlider::create(pSpriteBG, SpriteCreate(pszProgress), pSpriteThumb);
	if (NULL == pSlider) {
		return NULL;
	}

	int nMaxValue = GET_VALUE("max", 100);
	pSlider->setMaximumValue(nMaxValue);

	int nValue = GET_VALUE("Value", 0);
	pSlider->setValue(nValue);

	this->ManageNodeAndAddChild(pSlider, pszConfig);
	return pSlider;
}

void UIMgr::SetDictImage( CCNode* pSprite, const char *pszText, const char *pszDictImage, float fCharWitdh, const char *pszDict, const CCPoint rPtAnchor )
{
	MY_CHECK(pSprite);
	pSprite->removeAllChildren();

	if (NULL == pszText || NULL == pszDictImage || NULL == pszDict) {
		return;
	}

	if (!pszText[0]) {
		return;
	}

	int nDictLen = strlen(pszDict);
	if (nDictLen == 0) {
		return;
	}

	CCSprite *pSpriteDict = CCSprite::create(pszDictImage);
	if (NULL == pSpriteDict) {
		return;
	}

	float fGridWitdh = pSpriteDict->getContentSize().width / nDictLen;
	float fHight = pSpriteDict->getContentSize().height;
	float fX = 0;
	
	std::map<char, CCRect> mapRect;
	for (int i = 0; i < nDictLen; ++i) {
		CCRect rect(fX, 0,fGridWitdh, fHight);
		fX += fGridWitdh;
		mapRect.insert(std::make_pair(pszDict[i], rect));
	}

	float fOffset = (fCharWitdh - fGridWitdh) / 2;

	for (int i = 0; pszText[i]; ++i) {
		CCSprite *pScriptGrid = CCSprite::create(pszDictImage, mapRect[pszText[i]]);
		pScriptGrid->setPosition(ccp(fCharWitdh * i + fOffset, 0));
		pScriptGrid->setAnchorPoint(rPtAnchor);// ANCHOR_LEFT
		pSprite->addChild(pScriptGrid);
	}

	SetAllChildCascadeOpacityEnabled(pSprite, true);
}

void UIMgr::SetConfigDictImage( CCNode* pSpriteRet, const char *pszConfig, const char *pszText )
{
	MY_CHECK(pSpriteRet);

	std::string strText;
	if (pszText && pszText[0]) {
		strText = pszText;
	} else {
		strText = GET_VALUE("Value", "");
	}

	if (!strText.length()) {
		return;
	}
	
	std::string strDictImage = GET_VALUE("DictImage", "tab_word_4.png");
	std::string strDict = GET_VALUE("Dict", "");
	if (strDict.empty()) {
		auto nPos = strDictImage.find_last_of('.');
		auto strName = strDictImage.substr(0, nPos);
		strDict = this->GetConfigValue("DictImage", strName.c_str(), "0123456789/()");
	}

	CCSprite* pDictImage = SpriteCreate(strDictImage.c_str());
	float fCharWidthInDict = pDictImage->getContentSize().width / strDict.length();
	float fCharWidth = fCharWidthInDict;

	if (strText.length() > 1) {
		fCharWidth = GET_VALUE("CharWidth", fCharWidthInDict);
	}

	this->SetDictImage(pSpriteRet,strText.c_str(), strDictImage.c_str(), fCharWidth, strDict.c_str(), ANCHOR_LEFT_BOTTOM);

	this->ManageNode(pSpriteRet, pszConfig);
	CCSize size = pSpriteRet->getContentSize();
	size.width = fCharWidth * strText.length();
	size.height = pDictImage->getContentSize().height;
	pSpriteRet->setContentSize(size);
}

CCSprite* UIMgr::CreateConfigDictImage(const char *pszConfig, const char *pszText)
{
	CCSprite* pSprite = CCSprite::create();
	this->SetConfigDictImage(pSprite, pszConfig,pszText);
	return pSprite;
}

CCSprite* UIMgr::CreateConfigSprite( const char *pszConfig, const char *pszValue)
{
	CCSprite* pSprite = NULL;
	std::string strValue = pszValue ? pszValue : GET_VALUE("Value", "");
	if (!strValue.empty()) {
		int nTextureX = GET_VALUE("TextureX", 0);
		int nTextureY = GET_VALUE("TextureY", 0);
		int nTextureW = GET_VALUE("TextureW", 0);
		int nTextureH = GET_VALUE("TextureH", 0);
		bool bInitSuc = true;
		if (nTextureX || nTextureY || nTextureW || nTextureH) {
			CCRect rectTexture(nTextureX, nTextureY, nTextureW, nTextureH);
			pSprite = CCSprite::create(strValue.c_str(), rectTexture);
		} else {
			pSprite = CCSprite::create(strValue.c_str());
		}
		MY_ASSERT(pSprite, "ebShaderSprite::create [%s] failed",strValue.c_str());
	} else {
		pSprite = CCSprite::create();
	}

	int nFlipX = GET_VALUE("FlipX", 0);
	if (nFlipX) {
		pSprite->setFlipX(true);
	}

	int nFilpY = GET_VALUE("FlipY", 0);
	if (nFilpY) {
		pSprite->setFlipY(true);
	}

	int nSrcFunc = GET_VALUE("src", GL_ONE);
	int nDestFunc = GET_VALUE("dst", GL_ONE_MINUS_SRC_ALPHA);
	ccBlendFunc blendFunc = { static_cast<GLenum>(nSrcFunc), static_cast<GLenum>(nDestFunc) };
	pSprite->setBlendFunc(blendFunc);

	this->ManageColor(pSprite, pszConfig);
	this->ManageNodeAndAddChild(pSprite, pszConfig);

	return pSprite;
}

CCLabelTTF* UIMgr::CreateConfigTextLabel( const char *pszConfig, const char *pszValue /*= NULL*/, int nWidth )
{
	CCLabelTTF* pTextLabel = NULL;	
	std::string strValue;

	if (NULL == pszValue) {
		strValue = GET_VALUE("Value", "");
		
		if (!strValue.find("STR_")) {
			strValue = g_objStrMgr.GetStr(strValue.c_str());
		}

		pszValue = strValue.c_str();
	}
	
	if (-1 == nWidth) {
		nWidth = GET_VALUE("Width", -1);
	}

	std::string strFontName = GET_VALUE("FontName", "");		
	int nFontSize = GET_VALUE("FontSize", 24);
	pTextLabel = CCLabelTTF::create(pszValue, strFontName.c_str(), nFontSize);

	if (-1 != nWidth) {
		int nAlignType = GET_VALUE("align", -1);
		if (nAlignType > 0) {
			pTextLabel->setHorizontalAlignment((CCTextAlignment)nAlignType);
		}
	}

	this->ManageColor(pTextLabel, pszConfig);
	this->ManageNodeAndAddChild(pTextLabel, pszConfig);

	return pTextLabel;
}

void UIMgr::ManageAlpha( CCRGBAProtocol* pNode, const char *pszConfig )
{
	int nAlphaP = GET_VALUE("AlphaP", 100);
	if (nAlphaP != 100) {
		pNode->setOpacity(nAlphaP * 255 / 100);
	}
}

void UIMgr::ManageColor( CCNodeRGBA *pNode, const char *pszConfig )
{
	int nColor = GET_VALUE("Color", -1);
	if (-1 != nColor) {
		if (!pNode->isCascadeColorEnabled()) {
			pNode->setCascadeColorEnabled(true);
		}

		pNode->setColor(GetColor3B(nColor));
	}

	this->ManageAlpha(pNode, pszConfig);
}


CCMenuItemSprite* UIMgr::CreateConfigMenuItem( const char *pszConfig, CCObject *pTarget /*= NULL*/, SEL_MenuHandler selector /*= NULL*/ )
{
	QMyMenuItemSprite* pMenuItem = NULL;
	BOOL bStaticBG = GET_VALUE("static_bg", TRUE);

	const char *pszNodeGroup = GET_VALUE("NodeGroup", "");
	if (pszNodeGroup && pszNodeGroup[0]) {
		QMyGroupNode* pNodeGroup = this->CreateConfigGroupNode(pszNodeGroup);
		if (NULL == pszNodeGroup) {
			return pMenuItem;
		}

		pMenuItem = pNodeGroup->CreateMenuItem();
	} else {
		const char *pszNormalNode = GET_VALUE("NormalNode", "");
		if (pszNormalNode && pszNormalNode[0]) {
			CCNode* pNodeNormal = this->CreateConfigNode(pszNormalNode);
			const char *pszNodeSelected = GET_VALUE("SelectedNode", pszNormalNode);
			CCNode* pNodeSelected = this->CreateConfigNode(pszNodeSelected);
			const char *pszNodeDisabled = GET_VALUE("DisabledNode", pszNormalNode);
			CCNode* pNodeDisabled = this->CreateConfigNode(pszNodeDisabled);
			pMenuItem = QMyMenuItemSprite::create(pNodeNormal,pNodeSelected, pNodeDisabled);
		} else {
			std::string strValue = GET_VALUE("Value", "");
			const char *pszNormalImage = GET_VALUE("NormalImage", strValue.c_str());
			const char *pszSelectedImage = GET_VALUE("SelectedImage", pszNormalImage);
			const char *pszDisableImage = GET_VALUE("DisabledImage", pszNormalImage);
			if (pszNormalImage[0]) {
				pMenuItem = QMyMenuItemSprite::create(pszNormalImage, pszSelectedImage, pszDisableImage);
			} else {
				pMenuItem = QMyMenuItemSprite::create();
			}
		}
	}

	if (NULL == pMenuItem) {
		return NULL;
	}

	const char *pszStateChild = GET_VALUE("StateChild", "");
	if (pszStateChild && pszStateChild[0]) {
		this->AddConfigStateChild(pMenuItem, pszStateChild, bStaticBG);
	}

	if (!bStaticBG) {
		CCNode* pSelectBG = pMenuItem->getSelectedImage();
		if (pSelectBG) {
			float fY = pSelectBG->getPositionY();
			pSelectBG->setPositionY(fY - 2);
		}
	}

	BOOL bEnabled = GET_VALUE("Enabled", TRUE);
	if (!bEnabled) {
		pMenuItem->setEnabled(false);
	}

	if (pTarget) {
		pMenuItem->setTarget(pTarget, selector);
	}

	const char *pszSound = GET_VALUE("Sound", "SOUND_CLICK_OK");
	if (pszSound && pszSound[0]) {
		pMenuItem->SetClickSound(pszSound);
	}

	this->ManageNodeAndAddChild(pMenuItem, pszConfig);
	return pMenuItem;
}

void UIMgr::AddConfigStateChild( CCMenuItemSprite* pMenuItem, const char *pszStateChild, BOOL bStaticBG )
{
	const int STATE_AMOUNT = 3;
	CCNode* pArrNode[] = { pMenuItem->getNormalImage(), pMenuItem->getSelectedImage(), pMenuItem->getDisabledImage() };
		
	std::vector<std::string> vecStateChild = StringSplit(pszStateChild, ':');
	int nStateConfigAmount = vecStateChild.size();
	for (int i = 0; i < STATE_AMOUNT; ++i) {
		const char *pszChildConfig = NULL;
		if (i < nStateConfigAmount) {
			pszChildConfig = vecStateChild.at(i).c_str();
		} else {
			pszChildConfig = vecStateChild.at(0).c_str();
		}

		if (pszChildConfig[0] == 0 ||!strcmp(pszChildConfig, "null")) {
			continue;
		}

		bool bNeedSetSize = false;
		bool bBg = false;
		if (NULL == pArrNode[i]) {
			bBg = true;
			pArrNode[i] = CCSprite::create(); 
			switch (i)
			{
			case 0:
				pMenuItem->setNormalImage(pArrNode[i]);
				bNeedSetSize = true;
				break;
			case 1:
				pMenuItem->setSelectedImage(pArrNode[i]);
				break;
			case 2:
				pMenuItem->setDisabledImage(pArrNode[i]);
				break;
			default:
				break;
			}
		}

		std::vector<CCNode*> vecChilds = this->AddConfigChild(pArrNode[i], pszChildConfig);
		if (bNeedSetSize && !vecChilds.empty()) {
			pMenuItem->setContentSize(vecChilds.front()->getContentSize());
		}

		if (i == 1 && bStaticBG && !bBg) {
			std::vector<CCNode*>::iterator it(vecChilds.begin());
			for (; it != vecChilds.end(); ++it) {
				CCNode* pNode = *it;
				if (NULL == pNode) {
					continue;
				}

				float fY = pNode->getPositionY();
				pNode->setPositionY(fY - 2);	
			}
		}
	}
}

MyEditBox* UIMgr::CreateConfigEditBox( const char *pszConfig )
{
	int nW = GET_VALUE("w", 0);
	int nH = GET_VALUE("h", 0);
	CCScale9Sprite* pNormal9SpriteBg = NULL;
	std::string strValue = GET_VALUE("Value", "");
	if (strValue.empty()) {
		pNormal9SpriteBg = CCScale9Sprite::create();
	} else {
		pNormal9SpriteBg = Scale9SpriteCreate(strValue.c_str());
	}
	
	MyEditBox* pEditBox = MyEditBox::create(CCSize(nW, nH), pNormal9SpriteBg);
	
	if (NULL == pEditBox) {
		return pEditBox;
	}
	
	const char *pszPlaceHolder = GetStrValue(pszConfig, "PlaceHolder", "");
	if (pszPlaceHolder && pszPlaceHolder[0]) {
		pEditBox->setPlaceHolder(pszPlaceHolder);
	}
	
	int nFontColor = GET_VALUE("FontColor", -1);
	if (-1 != nFontColor) {
		pEditBox->setFontColor(GetColor3B(nFontColor));
	}
	
	int nPlaceholderFontColor = GET_VALUE("PlaceholderFontColor", -1);
	if (-1 != nPlaceholderFontColor) {
		pEditBox->setPlaceholderFontColor(GetColor3B(nPlaceholderFontColor));
	}
	
	int nFontSize = GET_VALUE("FontSize", 0);
	if (nFontSize) {
		pEditBox->setFontSize(nFontSize);
	}

	int nMaxLength = GET_VALUE("MaxLength", -1);
	if (-1 != nMaxLength) {
		pEditBox->setMaxLength(nMaxLength);
	}
	
	ManageNodeAndAddChild(pEditBox, pszConfig);
	return pEditBox;
}

MyScrollView* UIMgr::CreateConfigScrollView( const char *pszConfig )
{
	MyScrollView* pScrollView = new MyScrollView;
	pScrollView->autorelease();

	CCSize sizeView = m_sizeDevice;
	sizeView.width = GET_VALUE("viewW", m_sizeDevice.width);
	sizeView.height = GET_VALUE("viewH", m_sizeDevice.height);

	pScrollView->initWithViewSize(sizeView);
	pScrollView->setContentSize(m_sizeDevice);
	pScrollView->setTouchEnabled(true);

//	bounceable true need touchEnded with schedule
 	int nBounceable = GET_VALUE("Bounceable", 0);
 	pScrollView->setBounceable(nBounceable ? true : false);

	int nSwallowsTouches = GET_VALUE("SwallowsTouches", 0);
    
    pScrollView->setSwallow(nSwallowsTouches ? true : false);
	
	int nDir = GET_VALUE("dir", -1);
	switch (nDir) {
	case 0:
		pScrollView->setDirection(kCCScrollViewDirectionHorizontal);
		break;
	case 1:
		pScrollView->setDirection(kCCScrollViewDirectionVertical);
		break;
	case 2:
		pScrollView->setDirection(kCCScrollViewDirectionBoth);
		break;
	default:
		break;
	}

	int nSpace = GET_VALUE("space", -1);
	if (nSpace >= 0) {
		pScrollView->SetSpace(nSpace);
	}

	int nTouchMoveEnabled = GET_VALUE("touchMoveEnabled", -1);
	if (nTouchMoveEnabled >= 0) {
		pScrollView->EnableTouchMove(nTouchMoveEnabled ? true : false);
	}

	this->ManageNodeAndAddChild(pScrollView, pszConfig);
	return pScrollView;
}

QMyGroupNode* UIMgr::CreateConfigGroupNode( const char *pszConfig )
{
	QMyGroupNode* pMyGroupNode = new QMyGroupNode;
	pMyGroupNode->autorelease();

	std::string strMember;
	
	if (strchr(pszConfig, '@')) {
		strMember = pszConfig;
	} else {
		strMember = GET_VALUE("Member", "");
	}

	if (!strMember.empty()) {
		std::vector<std::string> vecStrMember = StringSplit(strMember.c_str(), '|');
		std::vector<std::string>::const_iterator it(vecStrMember.begin());
		for (; it != vecStrMember.end(); ++it) {
			CCNode* pMember = this->CreateConfigSingleBranchChild(pMyGroupNode, it->c_str());
			if (pMember) {
				pMyGroupNode->CCNode::addChild(pMember);
				this->ManageNode(pMember, pszConfig);
			}
		}
	}

	this->ManageAddChild(pMyGroupNode, pszConfig);
	return pMyGroupNode;
}

CCNode* UIMgr::CreateConfigMultipleNode( const char *pszConfig, const char *pszModel /*= NULL*/ )
{
	int nMultiple = GET_VALUE("Multiple", 0);
	BOOL bBrotherMode = GET_VALUE("BrotherMode", 1);
	std::string strModel = GET_VALUE("Model", "");

	if (pszModel && pszModel[0]) {
		strModel = pszModel;
	}

	if (strModel.empty() || nMultiple < 1) {
		return NULL;
	}

	int nLayout = GET_VALUE("Layout", 0);

	CCNode* pNodeFirst = this->CreateConfigNode(strModel.c_str());
	CCSize size = pNodeFirst->getContentSize();
	CCNode* pNodeRoot = bBrotherMode ? CCNode::create() : pNodeFirst;
	CCNode *pNodeToAdd = pNodeRoot;
	if (bBrotherMode) {
		pNodeRoot->addChild(pNodeFirst);
	}

	CCPoint ptOffset;
	switch (nLayout) {
	case 0:
		ptOffset.x = GET_VALUE("OffsetX", 0);
		ptOffset.y = GET_VALUE("OffsetY", 0);
		break;
	case 1:
		ptOffset = ccp(size.width, 0);
		break;
	case 2:
		ptOffset = ccp(0, -size.height);
		break;
	default:
		break;
	}

	for (int i = 1; i < nMultiple; ++i) {
		CCNode *pNode = this->CreateConfigNode(strModel.c_str());
		CCPoint ptMove = ptOffset;
		if (bBrotherMode) {
			ptMove.x *= i;
			ptMove.y *= i;
		}

		this->NodePositionMove(pNode, ptMove);

		pNodeToAdd->addChild(pNode);
		if (!bBrotherMode) {
			pNodeToAdd = pNode;
		}
	}

	this->ManageNodeAndAddChild(pNodeRoot, pszConfig);
	return pNodeRoot;
}

CCProgressTimer* UIMgr::CreateConfigProgress( const char *pszConfig )
{
	const char *pszImage = GET_VALUE("Image", "");
	CCSprite* pSprite = SpriteCreate(pszImage);

	MY_ASSERT(pSprite);

	if (NULL == pSprite) {
		return NULL;
	}

	CCProgressTimer* pProgress = CCProgressTimer::create(pSprite);
	if (NULL == pProgress) {
		return NULL;
	}

	int nType = GET_VALUE("Type", kCCProgressTimerTypeBar);
	pProgress->setType(CCProgressTimerType(nType));

	int nMidX = GET_VALUE("MidPointX", 0);
	int nMidY = GET_VALUE("MidPointY", 0);
	pProgress->setMidpoint(ccp(nMidX, nMidY));

	int nBarChangeRateX = GET_VALUE("BarChangeRateX", 1);
	int nBarChangeRateY = GET_VALUE("BarChangeRateY", 0);
	pProgress->setBarChangeRate(ccp(nBarChangeRateX, nBarChangeRateY));

	this->ManageNodeAndAddChild(pProgress, pszConfig);
	return pProgress;
}

MyDrawNode* UIMgr::CreateConfigDrawNode( const char *pszConfig )
{
	int nFromX = GET_VALUE("FromX", 0);
	int nFromY = GET_VALUE("FromY", 0);
	int nToX = GET_VALUE("ToX", 640);
	int nToY = GET_VALUE("ToY", 960);
	DWORD dwColor = GET_VALUE("Color", 0xffffffff);
	MyDrawNode* pNode = MyDrawNode::create(ccp(nFromX, nFromY), ccp(nToX, nToY), dwColor);

	int nStyle = GET_VALUE("Style", DRAW_STYLE_DEFAULT);
	if (DRAW_STYLE_DEFAULT != nStyle) {
		pNode->SetDrawStyle(nStyle);
	}

	this->ManageAlpha(pNode, pszConfig);
	this->ManageNodeAndAddChild(pNode, pszConfig);
	return pNode;
}

CCNode* UIMgr::CreateConfigNode( const char *pszConfig, int *pType /*= NULL*/ ,CCObject *pTarget /*= NULL*/, SEL_MenuHandler selector /*= NULL*/ )
{
	if (NULL == pszConfig || pszConfig[0] == 0) {
		return NULL;
	}

	CCNode* pNode = NULL;

	// for quick config child
	if (strchr(pszConfig, '<')) {
		std::string strOrg = pszConfig;
		std::string::size_type stPos = strOrg.find_first_of('<');
		std::string strParent = strOrg.substr(0, stPos);
		pNode = this->CreateConfigNode(strParent.c_str());
		std::string strChild = strOrg.substr(stPos + 1);
		this->AddConfigSingleBranchChild(pNode, strChild.c_str());
	} else if (strchr(pszConfig, '@')) {
		const char *pPos = strchr(pszConfig, '.');
		if (pPos) {
			pNode = SpriteCreate(pszConfig + 1);
		} else {
			pNode = this->CreateConfigTextLabel(NULL, pszConfig + 1);
		}

		if (pNode) {
			pNode->setAnchorPoint(ANCHOR_DEFAULT);
		}
	}

	if (pNode) {
		return pNode;
	}

	int nNodeType = GET_VALUE("NodeType", NODETYPE_NONE);
	if (selector) {
		MY_ASSERT(NODETYPE_MENUITEM == nNodeType, "argv [%s]: you set selector but config nodetype %d != %d", pszConfig, nNodeType, NODETYPE_MENUITEM);
	}

	if (pType) {
		*pType = nNodeType;
	}

	std::string strValue = GET_VALUE("Value", "");
	switch (nNodeType) {
	case NODETYPE_SPRITE:
		pNode = this->CreateConfigSprite(pszConfig);
		break;
	case NODETYPE_TEXTLABEL:
		pNode = this->CreateConfigTextLabel(pszConfig);
		break;
	case NODETYPE_MENUITEM:
		pNode = this->CreateConfigMenuItem(pszConfig, pTarget, selector);
		break;
	case NODETYPE_DRAWNODE:
		pNode = this->CreateConfigDrawNode(pszConfig);
		break;
	case NODETYPE_GROUP:
		pNode = this->CreateConfigGroupNode(pszConfig);
		break;
	case NODETYPE_MULTIPLE:
		pNode = this->CreateConfigMultipleNode(pszConfig);
		break;
	case NODETYPE_SCROLLVIEW:
		pNode = this->CreateConfigScrollView(pszConfig);
		break;
	case NODETYPE_PROGRESS:
		pNode = this->CreateConfigProgress(pszConfig);
		break;
	case NODETYPE_ANIMATION:
		pNode = this->CreateConfigAnimation(pszConfig);
		break;
	case NODETYPE_DICT_IMAGE:
		pNode = this->CreateConfigDictImage(pszConfig);
		break;
	case NODETYPE_PARTICLE_SYSTEM_QUAD:
		pNode = this->CreateConfigParticleSystemQuad(pszConfig);
		break;
	case NODETYPE_SLIDER:
		pNode = this->CreateConfigSlider(pszConfig);
		break;
	case NODETYPE_EDIT:
		pNode = this->CreateConfigEditBox(pszConfig);
		break;
	default:
		break;
	}

	MY_ASSERT(pNode, "argv: [%s]", pszConfig);

	return pNode;
}

void UIMgr::ManageRotation( CCNode* pNode, const char *pszConfig )
{
	int nRotate = GET_VALUE("rotate", 0);
	if (nRotate) {
		pNode->setRotation(nRotate);
	}
}


void UIMgr::ManagePos( CCNode* pNode, const char *pszConfig )
{
	int nX = GET_VALUE("x", 0);
	int nY = GET_VALUE("y", 0);
	int nAnchorTypeDefault = ANCHOR_TYPE_DEFAULT;

	int nNodeType = GET_VALUE("NodeType", NODETYPE_NONE);
	switch (nNodeType) {
	case NODETYPE_PARTICLE_SYSTEM_QUAD:
		{
			nAnchorTypeDefault = ANCHOR_TYPE_CENTER;
			CCPoint ptOrg = pNode->getPosition();
			pNode->setPosition(ptOrg.x + nX, ptOrg.y + nY);
		}
		break;
	default:
		pNode->setPosition(ccp(nX, nY));
		break;
	}

	CCSize sizeOrg = pNode->getContentSize();
	CCSize sizeConfig = sizeOrg;
	int nW = GET_VALUE("w", 0);
	if (nW) {
		sizeConfig.width = nW;
	}

	int nH = GET_VALUE("h", 0);
	if (nH) {
		sizeConfig.height = nH;
	}

	if (nW || nH) {
		pNode->setContentSize(sizeConfig);
	}

	int nAnchorType = GET_VALUE("AnchorType", nAnchorTypeDefault);
	MY_ASSERT(1 <= nAnchorType && nAnchorType <= 9);
	pNode->setAnchorPoint(ANCHOR_POINT_GROUP[nAnchorType]);

	int nBasePoint = GET_VALUE("BasePoint", 0);
	if (nBasePoint) {
		ManageNodeResolution(pNode, nBasePoint);
	}
}

void UIMgr::ManageSize( CCNode* pNode, const char *pszConfig )
{
	int wScaleP = GET_VALUE("wScaleP", 0);
	if (wScaleP) {
		pNode->setScaleX(wScaleP / 100.0f);
	}

	int hScaleP = GET_VALUE("hScaleP", 0);
	if (hScaleP) {
		pNode->setScaleY(hScaleP / 100.0f);
	}

	int wScaleTo = GET_VALUE("wScaleTo", 0);
	int hScaleTo = GET_VALUE("hScaleTo", 0);
	if (wScaleTo || hScaleTo) {
		CCSize sizeOrg = pNode->getContentSize();
		if (wScaleTo) {
			pNode->setScaleX(sizeOrg.width ? (wScaleTo / sizeOrg.width) : 1);
		}

		if (hScaleTo) {
			pNode->setScaleY(sizeOrg.height ? (hScaleTo / sizeOrg.height) : 1);
		}
	}
}

void UIMgr::ManageZOrder( CCNode* pNode, const char *pszConfig )
{
	int nZOrder = GET_VALUE("zOrder", 0);
	if (nZOrder) {
		pNode->setZOrder(nZOrder);
	}
}

void UIMgr::ManageNode( CCNode* pNode, const char *pszConfig )
{
	if (NULL == pNode || NULL == pszConfig) {
		return;
	}

#ifdef CONFIG_POS
	m_mapObjSection[pNode->m_uID] = pszConfig;
#endif

	this->ManagePos(pNode, pszConfig);
	this->ManageRotation(pNode, pszConfig);
	this->ManageSize(pNode, pszConfig);
	this->ManageZOrder(pNode, pszConfig);
}


void UIMgr::ManageCCLayer(CCLayer* pCCLayer, const char *pszConfig, int nDefaultTouchPriority)
{
	if (NULL == pCCLayer) {
		return;
	}
	
	int nTouchEnabled = GET_GROUP_VALUE("TouchEnabled", 1);
	pCCLayer->setTouchEnabled(nTouchEnabled ? true : false);
	
	int nTouchPriority = GET_GROUP_VALUE("TouchPriority", nDefaultTouchPriority);
	pCCLayer->setTouchPriority(nTouchPriority);
}

void UIMgr::ManageMyMenu(MyMenu* pMyMenu, const char *pszConfig)
{
	if (NULL == pMyMenu) {
		return;
	}
	
	this->ManageCCLayer(pMyMenu, pszConfig, kCCMenuHandlerPriority);
	int nSwa = GET_GROUP_VALUE("SwallowTouches", 1);
	pMyMenu->setSwallowTouches(nSwa ? true : false);
}

void UIMgr::ManageMyLayer(MyLayer* pMyLayer, const char *pszConfig)
{
	if (NULL == pMyLayer) {
		return;
	}
	
	this->ManageCCLayer(pMyLayer, pszConfig, 0);
	int nSwa = GET_GROUP_VALUE("SwallowTouches", 1);
	pMyLayer->setSwallowTouches(nSwa ? true : false);
	
	std::string strBG = GET_VALUE("BG", "");
	if (!strBG.empty()) {
		pMyLayer->SetImage(strBG.c_str());
	}

	std::string strNodes = GET_VALUE("Node", "");
	if (!strNodes.empty()) {
		auto vecStrChild = StringSplit(strNodes.c_str(), '|');
		std::vector<std::string>::const_iterator it(vecStrChild.begin());
		for (; it != vecStrChild.end(); ++it) {
			pMyLayer->AddConfigNode(it->c_str());
		}
	}

	this->ManageNodeAndAddChild(pMyLayer, pszConfig);
	pMyLayer->AdjustResolution();
}

std::vector<CCNode*> UIMgr::AddConfigChild(CCNode* pParent, const char *pszConfigChild )
{	
	std::vector<CCNode*> vecChilds;
	std::vector<std::string> vecStrChild = StringSplit(pszConfigChild, '|');
	std::vector<std::string>::const_iterator it(vecStrChild.begin());
	for (; it != vecStrChild.end(); ++it) {
		CCNode* pChild = AddConfigSingleBranchChild(pParent, it->c_str());
		if (pChild) {
			vecChilds.push_back(pChild);
		}
	}

	return vecChilds;
}

CCNode* UIMgr::CreateConfigSingleBranchChild( CCNode* pParent, const char *pszNoBrotherTagConfigChild )
{
	std::string strChildConfig = pszNoBrotherTagConfigChild ? pszNoBrotherTagConfigChild : "";
	std::vector<std::string> vecStringChildGeneration = StringSplit(strChildConfig.c_str(), '<');
	CCNode* pNode = NULL;
	CCNode* pFather = pParent;
	std::vector<std::string>::const_iterator it(vecStringChildGeneration.begin());
	for (; it != vecStringChildGeneration.end(); ++it) {
		CCNode* pChild = this->CreateConfigSingleChild(pFather, it->c_str());
		if (pNode) {
			pFather->addChild(pChild);
		} else {
			pNode = pChild;
		}

		pFather = pChild;
	}

	return pNode;
}

CCNode* UIMgr::AddConfigSingleBranchChild( CCNode* pParent, const char *pszNoBrotherTagConfigChild )
{
	CCNode* pNode = this->CreateConfigSingleBranchChild(pParent, pszNoBrotherTagConfigChild);
	if (pNode) {
		pParent->addChild(pNode);
	}

	return pNode;
}

CCNode* UIMgr::CreateConfigSingleChild(CCNode* pNode, const char *pszNoSepTagConfigChild)
{
	if (NULL == pNode) {
		return NULL;
	}

	std::string strChildConfig = pszNoSepTagConfigChild ? pszNoSepTagConfigChild : "";
	if (strChildConfig.empty()) {
		return NULL;
	}
	
	std::string::size_type stArgv = strChildConfig.find_first_of(" -");
	CCNode* pChild = NULL;
	if (std::string::npos == stArgv) {
		pChild = this->CreateConfigNode(strChildConfig.c_str());
		return pChild;
	}

	std::string strChild = strChildConfig.substr(0, stArgv);
	pChild = this->CreateConfigNode(strChild.c_str());
	if (NULL == pChild) {
		return NULL;
	}

	std::string strArgvs = strChildConfig.substr(stArgv);
	this->ManageArgvs(pChild, pNode, strArgvs.c_str());

	return pChild;
}

void UIMgr::ManageArgvs( CCNode* pChild, CCNode* pNode, const char *pszArgvs )
{
	int nParam = 0;
	std::vector<std::string> vecArgv = StringSplit(pszArgvs, ' ');
	std::vector<std::string>::const_iterator it(vecArgv.begin());
	for (; it != vecArgv.end(); ++it) {
		const char *pszArgv = it->c_str();
		if (pszArgv[0] != '-') {
			return;
		}

		++pszArgv;
	
		bool bDone = false;
		for (int i = 0; pszArgv[i]; ++i) {
			if (bDone) {
				break;
			}

			switch (pszArgv[i]) {
			case 'p':
				{
					const char *pszConfig = pszArgv + 1;
					const char *pszArgvTemplete = GET_VALUE("Value", "");
					this->ManageArgvs(pChild, pNode, pszArgvTemplete);
					bDone = true;
					break;	
				}
				break;
			case 'c':
				{
					CCSize size = pNode->getContentSize();
					this->NodeSetPos(pChild, size.width / 2, size.height / 2);
					this->NodeAnchorChange(pChild, ANCHOR_CENTER);
					bDone = true;
				}
				break;
			case 't':
				{
					CCSize size = pNode->getContentSize();
					this->NodeSetPos(pChild, size.width / 2, size.height);
					this->NodeAnchorChange(pChild, ANCHOR_TOP);
					bDone = true;
				}
				break;
			case 'T':
				{
					CCSize size = pNode->getContentSize();
					this->NodeSetPos(pChild, size.width / 2, size.height);
					this->NodeAnchorChange(pChild, ANCHOR_BOTTOM);
					bDone = true;
				}
				break;
			case 'b':
				{
					CCSize size = pNode->getContentSize();
					this->NodeSetPos(pChild, size.width / 2, 0);
					this->NodeAnchorChange(pChild, ANCHOR_BOTTOM);
					bDone = true;
				}
				break;
			case 'B':
				{
					CCSize size = pNode->getContentSize();
					this->NodeSetPos(pChild, size.width / 2, 0);
					this->NodeAnchorChange(pChild, ANCHOR_TOP);
					bDone = true;
				}
				break;
			case 'l':
				{
					CCSize size = pNode->getContentSize();
					this->NodeSetPos(pChild, 0, size.height / 2);
					this->NodeAnchorChange(pChild, ANCHOR_LEFT);
					bDone = true;
				}
				break;
			case 'L':
				{
					CCSize size = pNode->getContentSize();
					this->NodeSetPos(pChild, 0, size.height / 2);
					this->NodeAnchorChange(pChild, ANCHOR_RIGHT);
					bDone = true;
				}
				break;
			case 'r':
				{
					CCSize size = pNode->getContentSize();
					this->NodeSetPos(pChild, size.width, size.height / 2);
					this->NodeAnchorChange(pChild, ANCHOR_RIGHT);
					bDone = true;
				}
				break;
			case 'R':
				{
					CCSize size = pNode->getContentSize();
					this->NodeSetPos(pChild, size.width, size.height / 2);
					this->NodeAnchorChange(pChild, ANCHOR_LEFT);
					bDone = true;
				}
				break;
			case 'd':
				switch (pszArgv[i + 1])	{
				case 'x':
					if (1 == sscanf(pszArgv + i + 2, "%d", &nParam)) {
						float fX = pChild->getPositionX();
						this->NodeSetPosX(pChild, fX + nParam);
						bDone = true;
					}
					break;
				case 'y':
					if (1 == sscanf(pszArgv + i + 2, "%d", &nParam)) {
						float fY = pChild->getPositionY();
						this->NodeSetPosY(pChild, fY + nParam);
						bDone = true;
					}
					break;
				default:
					break;
				}
				break;
			case 'x':
				if (1 == sscanf(pszArgv + i + 1, "%d", &nParam)) {
					this->NodeSetPosX(pChild, nParam);
					bDone = true;
				}
				break;
			case 'y':
				if (1 == sscanf(pszArgv + i + 1, "%d", &nParam)) {
					this->NodeSetPosY(pChild, nParam);
					bDone = true;
				}
				break;
			case 'a':
				if (1 == sscanf(pszArgv + i + 1, "%d", &nParam)) {
					MY_ASSERT(1 <= nParam && nParam <= 9);
					this->NodeAnchorChange(pChild, nParam);
					bDone = true;
				}
				break;
			default:
				break;
			}
		}
	}
}

void UIMgr::ManageAddChild( CCNode *pNode, const char *pszConfig )
{
	if (NULL == pNode) {
		return;
	}

	std::string strChild = GET_VALUE("Child", "");
	if (strChild.empty()) {
		return;
	}

	this->AddConfigChild(pNode, strChild.c_str());
}

void UIMgr::ManageNodeAndAddChild( CCNode* pNode, const char *pszConfig )
{
	this->ManageNode(pNode, pszConfig);
	this->ManageAddChild(pNode, pszConfig);
}

#ifdef CONFIG_POS
const char* UIMgr::GetNodeConfig( int uId ) const
{
	auto it = m_mapObjSection.find(uId);
	if (it == m_mapObjSection.end()) {
		return NULL;
	}

	return it->second.c_str();
}

CCNode* UIMgr::FindConfigNode( CCNode* pNodeRoot, const char *pszConfig ) const
{
	std::vector<CCNode*> vecNodeChild;
	GetNodeAllChild(pNodeRoot, vecNodeChild);

	for (auto it(vecNodeChild.begin()); it != vecNodeChild.end(); ++it) {
		CCNode* pNode = *it;
		if (NULL == pNode) {
			continue;
		}

		const char *pszNodeConfig = this->GetNodeConfig(pNode->m_uID);
		if (NULL == pszNodeConfig) {
			continue;
		}

		if (!strcmp(pszNodeConfig, pszConfig)) {
			return pNode;
		}
	}

	return NULL;
}

bool UIMgr::SaveConfigValue( const char *pszConfig, const char *pszKey, int nValue )
{
#if WIN32
	auto* pStr = CCString::createWithFormat("%d", nValue);
	std::string strFilePath = CCFileUtils::sharedFileUtils()->fullPathForFilename(m_strConfigFileName.c_str());
	BOOL bRet = WritePrivateProfileString(pszConfig, pszKey, pStr->getCString(), strFilePath.c_str());
	if (bRet) {
		CIniMgr::GetInstance()->SetValue(m_strConfigFileName.c_str(), pszConfig, pszKey, pStr->getCString());
	}

	return bRet ? true : false;
#else
	return false;
#endif
}

void UIMgr::ReloadPos( void )
{
	CCScene* pRunningScene = CCDirector::sharedDirector()->getRunningScene();
	if (NULL == pRunningScene) {
		return;
	}

	//MY_ASSERT(pRunningScene == NULL, "test is a test %s %x ", "hahahaha", pRunningScene);

	CIniMgr::GetInstance()->Reload(m_strConfigFileName.c_str());
	CIniMgr::GetInstance()->Reload(INFO_INI);
	CIniMgr::GetInstance()->Reload(MSGBOX_INI);
	std::vector<CCNode*> vecChild;
	GetNodeAllChild(pRunningScene, vecChild);

	int nCount = vecChild.size();

	std::vector<MyLayer*> vecActiveLayer;

	std::vector<CCNode*>::iterator itNode(vecChild.begin());
	for (; itNode != vecChild.end(); ++itNode) {
		CCNode *pNode = *itNode;
		if (NULL == pNode) {
			continue;
		}

		MyLayer *pMyLayer = dynamic_cast<MyLayer*>(pNode);
		if (pMyLayer) {
			vecActiveLayer.push_back(pMyLayer);
		}

		std::map<int, std::string>::const_iterator it(m_mapObjSection.find(pNode->m_uID));
		if (it != m_mapObjSection.end()) {
			const char *pszConfig = it->second.c_str();
			ManagePos(pNode, pszConfig);
			ManageRotation(pNode, pszConfig);

			int nX = GET_VALUE("x", 0);
			int nY = GET_VALUE("y", 0);
			CCPoint pt = ccp(nX, nY);
            
            pt = ccpAdd(pt, m_mapObjOffsetXY[pNode->m_uID]);

			pNode->setPosition(pt);

			auto itAnchor = m_mapAnchorPoint.find(pNode->m_uID);
			if (itAnchor != m_mapAnchorPoint.end()) {
				pNode->setAnchorPoint(itAnchor->second);
			}
		}
	}

	std::vector<MyLayer*>::iterator itLayer(vecActiveLayer.begin());
	for (; itLayer != vecActiveLayer.end(); ++itLayer) {
		(*itLayer)->UpdateView();
	}
}

void UIMgr::OnNodePositionMove( CCNode* pNode )
{
#ifdef CONFIG_POS
	if (NULL == pNode) {
		return;
	}

	std::map<int, std::string>::const_iterator it(m_mapObjSection.find(pNode->m_uID));
	if (it == m_mapObjSection.end()) {
		return;
	}

	const char *pszConfig = it->second.c_str();
	int nX = GET_VALUE("x", 0);
	int nY = GET_VALUE("y", 0);
	CCPoint ptOrg = ccp(nX, nY);
	m_mapObjOffsetXY[pNode->m_uID] = pNode->getPosition() - ptOrg;
#endif
}

void UIMgr::NodePositionMove( CCNode* pNode, const CCPoint& rPtOffset )
{
	if (NULL == pNode) {
		return;
	}

	CCPoint pt = pNode->getPosition();
    
    pt = ccpAdd(pt, rPtOffset);
    
	pNode->setPosition(pt);

#ifdef CONFIG_POS
	UIMgr::GetInstance()->OnNodePositionMove(pNode);
#endif
}

void UIMgr::NodeSetPos( CCNode* pNode, CCPoint ptPos )
{
	this->NodeSetPos(pNode, ptPos.x, ptPos.y);
}

void UIMgr::NodeSetPos( CCNode* pNode, float fX, float fY )
{
	if (NULL == pNode) {
		return;
	}

	pNode->setPosition(fX, fY);
	this->OnNodePositionMove(pNode);
}

void UIMgr::NodeSetPosX( CCNode* pNode, float fX )
{
	if (NULL == pNode) {
		return;
	}

	pNode->setPositionX(fX);
	this->OnNodePositionMove(pNode);
}

void UIMgr::NodeSetPosY( CCNode* pNode, float fY )
{
	if (NULL == pNode) {
		return;
	}

	pNode->setPositionY(fY);
	this->OnNodePositionMove(pNode);
}

void UIMgr::NodeAnchorChange( CCNode* pNode, int nAnchorType )
{
	MY_ASSERT(1 <= nAnchorType && nAnchorType <= 9);
	this->NodeAnchorChange(pNode, ANCHOR_POINT_GROUP[nAnchorType]);
}

void UIMgr::NodeAnchorChange( CCNode* pNode, CCPoint pt )
{
	if (NULL == pNode) {
		return;
	}
	
	pNode->setAnchorPoint(pt);
	this->OnNodeAnchorChange(pNode);
}

void UIMgr::OnNodeAnchorChange( CCNode* pNode )
{
#ifdef CONFIG_POS
	if (NULL == pNode) {
		return;
	}

	m_mapAnchorPoint[pNode->m_uID] = pNode->getAnchorPoint();
#endif
}

CCNode* UIMgr::CreateConfigNodeWithChlid( const char *pszConfig, const char *pszChildConfig )
{
	CCNode* pNode = this->CreateConfigNode(pszConfig);
	if (NULL == pNode) {
		return NULL;
	}

	AddConfigChild(pNode, pszChildConfig);
	return pNode;
}

void UIMgr::CalcuNewPos( const CCPoint& rPtOld, CCPoint& rPtNew, int nBasePoint, const CCSize& sizeConfig, const CCSize& sizeDevice )
{
	if (nBasePoint <= 0 || nBasePoint > 9) {
		return;
	}

	CCPoint ptBaseConfig = ccp((nBasePoint + 2) % 3 * sizeConfig.width / 2, (nBasePoint - 1) / 3 * sizeConfig.height / 2);
	CCPoint ptBaseDevice = ccp((nBasePoint + 2) % 3 * sizeDevice.width / 2, (nBasePoint - 1) / 3 * sizeDevice.height / 2);
	rPtNew = rPtOld - ptBaseConfig + ptBaseDevice;
}

void UIMgr::CalcuNewPos( const CCPoint& rPtOld, CCPoint& rPtNew, int nBasePoint )
{
	CalcuNewPos(rPtOld, rPtNew, nBasePoint, m_sizeConfig, m_sizeDevice);
}

void UIMgr::SetConfigSize( float fWidth, float fHeight )
{
	m_sizeConfig.setSize(fWidth, fHeight);
}

void UIMgr::SetDeviceSize(float fScrWidth, float fScrHeight)
{
	m_sizeDevice.setSize(fScrWidth, fScrHeight);
}

void UIMgr::ManageNodeResolution( CCNode *pNode, int nBasePoint )
{
	if (NULL == pNode) {
		return;
	}

	CCPoint pt = pNode->getPosition();
	CCPoint ptNew;
	this->CalcuNewPos(pt, ptNew, nBasePoint);
	this->NodePositionMove(pNode, ptNew - pt);
}

int UIMgr::GetMultiResolutionPolicy( void ) const
{
	return m_nMultiResolutionPolicy;
}

void UIMgr::SetMultiResolutionPolicy( int nType )
{
	m_nMultiResolutionPolicy = nType;

	CCEGLView* pEGLView = CCEGLView::sharedOpenGLView();
	auto sizeDevice = pEGLView->getFrameSize();
	UIMgr::GetInstance()->SetDeviceSize(sizeDevice.width, sizeDevice.height);

	float fDesignWidth = this->GetConfigValue("DesignResolution", "w", 960);
	float fDesignHeight = this->GetConfigValue("DesignResolution", "h", 640);

	if (MULTIRESOLUTION_POLICY_LACE == nType) {
		if (sizeDevice.height / sizeDevice.width >= fDesignHeight / fDesignHeight) {
			float fNewHeight = fDesignWidth * sizeDevice.height / sizeDevice.width;
			pEGLView->setDesignResolutionSize(fDesignWidth, fNewHeight, kResolutionShowAll);
		} else {
			float fNewWidth = fDesignHeight * sizeDevice.width / sizeDevice.height;
			pEGLView->setDesignResolutionSize(fNewWidth, fDesignHeight, kResolutionShowAll);
		}
	} else {
		pEGLView->setDesignResolutionSize(fDesignWidth, fDesignHeight, kResolutionExactFit);
	}

	UIMgr::GetInstance()->SetConfigSize(fDesignWidth, fDesignHeight);
}

void UIMgr::SetSceneFactory( ISceneFactory* pSceneFactory )
{
	m_pSceneFactoryImpl = pSceneFactory;
}


void UIMgr::SetSceneInitData( int nInitData )
{
	m_infoCurrentScene.m_nInitData = nInitData;
}

bool UIMgr::IsCurrentSceneStable(void) const
{
	CCScene* pScene = CCDirector::sharedDirector()->getRunningScene();
	if (NULL == pScene) {
		return false;
	}

	if (!pScene->isRunning()) {
		return false;
	}

	return (pScene == m_pSceneLogic);
}

bool UIMgr::SwitchScene( int nSceneType, eTransitionType nTransitionType /*= TRANSITION_TYPE_NONE*/, int nInitData /*= 0*/ )
{
	if (NULL == m_pSceneFactoryImpl) {
		return false;
	}

	MyScene* pMyScene = m_pSceneFactoryImpl->createScene(nSceneType);
	if (NULL == pMyScene) {
		return false;
	}
	
	m_pSceneLogic = pMyScene;
	pMyScene->setInitData(nInitData);
	pMyScene->OnInit();

	CCScene *pScene = NULL;
	switch (nTransitionType) {
	case TRANSITION_TYPE_NONE:
		pScene = pMyScene;
		break;
	case TRANSITION_TYPE_SLIDEINL:
		pScene = CCTransitionSlideInL::create(0.5f, pMyScene);
		break;
	case TRANSITION_TYPE_SLIDEINR:
		pScene = CCTransitionSlideInR::create(0.5f, pMyScene);
		break;
	case TRANSITION_TYPE_JUMPZOOM:
		pScene = CCTransitionJumpZoom::create(2.0f, pMyScene);
		break;
	case TRANSITION_TYPE_ROTOZOOM:
		pScene = CCTransitionRotoZoom::create(2.0f, pMyScene);
		break;
	case TRANSITION_TYPE_SPLITCOLS:
		pScene = CCTransitionSplitCols::create(2.0f, pMyScene);
		break;
	case TRANSITION_TYPE_FADE:
		pScene = CCTransitionFade::create(2.0f, pMyScene);
		break;
	default:
		break;
	}
	CCDirector::sharedDirector()->replaceScene(pScene);
	return true;
}

void UIMgr::RunWithScene( int nSceneType, int nInitData /*= 0*/ )
{
	if (NULL == m_pSceneFactoryImpl) {
		return;
	}

	MyScene* pMyScene = m_pSceneFactoryImpl->createScene(nSceneType);
	if (NULL == pMyScene) {
		return;
	}

	pMyScene->setInitData(nInitData);
	pMyScene->OnInit();
	CCDirector::sharedDirector()->runWithScene(pMyScene);
	m_pSceneLogic = pMyScene;
	m_infoCurrentScene.m_nTransitionType = TRANSITION_TYPE_NONE;
	m_infoCurrentScene.m_nSceneType = nSceneType;
	m_infoCurrentScene.m_nInitData = nInitData;
}

int UIMgr::GetCurrentSceneType() const
{
    return m_infoCurrentScene.m_nSceneType;
}

void UIMgr::ReplaceScene( int nSceneType, eTransitionType nTransitionType /*= TRANSITION_TYPE_NONE*/, int nInitData /*= 0*/ ,bool bNeedPushLastScene)
{
	if (!this->SwitchScene(nSceneType, nTransitionType, nInitData)) {
		return;
	}

	if (m_infoCurrentScene.m_nSceneType == nSceneType) {
		bNeedPushLastScene = false;
	}

	if (bNeedPushLastScene) {
		m_stackScene.push(m_infoCurrentScene);
	}

	m_infoCurrentScene.m_nInitData = nInitData;
	m_infoCurrentScene.m_nSceneType = nSceneType;
	m_infoCurrentScene.m_nTransitionType = nTransitionType;
}

void UIMgr::PopScene( void )
{
	if (m_stackScene.empty()) {
		return;
	}

	eTransitionType nTransitionType;
	switch (m_infoCurrentScene.m_nTransitionType) {
	case TRANSITION_TYPE_SLIDEINL:
		nTransitionType = TRANSITION_TYPE_SLIDEINR;
		break;
	case TRANSITION_TYPE_SLIDEINR:
		nTransitionType = TRANSITION_TYPE_SLIDEINL;
		break;
	default:
		nTransitionType = m_infoCurrentScene.m_nTransitionType;
		break;
	}

	auto infoSceneToBePop = m_infoCurrentScene;
	while (true) {
		m_infoCurrentScene = m_stackScene.top();
		m_stackScene.pop();
		
		if (m_stackScene.empty()) {
			break;
		}

		if (m_infoCurrentScene.m_nSceneType != infoSceneToBePop.m_nSceneType) {
			break;
		}

		if (m_infoCurrentScene.m_nInitData != infoSceneToBePop.m_nInitData) {
			break;
		}
	}

	this->SwitchScene(m_infoCurrentScene.m_nSceneType, nTransitionType, m_infoCurrentScene.m_nInitData);
}

void UIMgr::ResetSceneStack(void)
{
	if (!m_stackScene.empty()) {
		m_stackScene.pop();
	}

	m_infoCurrentScene.m_nInitData = 0;
	m_infoCurrentScene.m_nSceneType = 0;
}

void UIMgr::LoadTexturePacks( void )
{
	for (auto it(m_vecTexturePackDir.begin()); it != m_vecTexturePackDir.end(); ++it) {
		for (int i = 1;; ++i) {
			auto* pStrPackPlist = CCString::createWithFormat("pic_pack/%s__%d.plist", it->c_str(), i);
			if (!CCFileUtils::sharedFileUtils()->isFileExist(pStrPackPlist->getCString())) {
				break;
			}

			CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(pStrPackPlist->getCString());
		}
	}
}

void UIMgr::AddTexturePackDir( const char* pszKey )
{
	std::string strPath = pszKey ? pszKey : "";
	strPath += "/";
	CCFileUtils::sharedFileUtils()->addSearchResolutionsOrder(strPath.c_str());
	const char *pPos = strrchr(pszKey, '/');
	if (NULL == pPos) {
		m_vecTexturePackDir.push_back(pszKey);
	} else {
		m_vecTexturePackDir.push_back(pPos + 1);
	}
}

CCSize UIMgr::GetSizeConfig( void ) const
{
	return m_sizeConfig;
}

cocos2d::CCSize UIMgr::GetWinSize( void ) const
{
	return m_sizeConfig;
}

#endif
