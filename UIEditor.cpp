//
//  UIEditor.cpp
//
//  Created by Ruoqian, Chen on 2014/06/09
//

#include "StdAfx.h"

#include "UIEditor.h"

typedef CCLabelTTF OrgValueControl;

EditorLayer* s_pEditorLayer = NULL;

void TurnBtnTextColor(CCNode* pNode, ccColor3B color)
{
	if (NULL == pNode) {
		return;
	}

	auto pMenuItem = dynamic_cast<CCMenuItem*>(pNode);
	if (NULL == pMenuItem) {
		return;
	}

	auto pChilds = pMenuItem->getChildren();
	CCObject* pObj;
	CCARRAY_FOREACH(pChilds, pObj) {
		auto* pText = dynamic_cast<CCLabelTTF*>(pObj);
		pText->setColor(color);
	}
}

#if WIN32
HHOOK s_hHookOrg = NULL;

bool CopyTextToClipboard(const char *pszStr)
{
	if (NULL == pszStr) {
		return false;
	}

	CCEGLView* pMainWnd = CCEGLView::sharedOpenGLView();
	if (NULL == pMainWnd) {
		return false;
	}

	if (!OpenClipboard(pMainWnd->getHWnd())) {
		return false;
	}

	EmptyClipboard();
	HGLOBAL hClipBuf = GlobalAlloc(GMEM_DDESHARE, strlen(pszStr) + 1);
	char *pBuf = (char*)GlobalLock(hClipBuf);
	strcpy(pBuf, pszStr);
	GlobalUnlock(hClipBuf);
	SetClipboardData(CF_TEXT, hClipBuf);
	CloseClipboard();
	return true;
}

std::string GetTextFromClipboard(void)
{
	std::string strRet;
	CCEGLView* pMainWnd = CCEGLView::sharedOpenGLView();
	if (NULL == pMainWnd) {
		return strRet;
	}

	if (!OpenClipboard(pMainWnd->getHWnd())) {
		return strRet;
	}

	HANDLE hClipBuf = GetClipboardData(CF_TEXT);
	char *pBuf = (char*)GlobalLock(hClipBuf);
	if (NULL == pBuf) {
		return strRet; 
	}

	strRet = pBuf;
	GlobalUnlock(hClipBuf);
	CloseClipboard();
	return strRet;
}

LRESULT CALLBACK MyKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (s_pEditorLayer && !s_pEditorLayer->IsHide() && HC_ACTION == nCode) {
		//CCLog("KeyBoardProc %d %x %x", nCode, wParam, lParam);
		int nKeyState = (lParam >> 30) & 1;
		int nKeyTurnState = (lParam >> 31) & 1;
		if (!nKeyState && !nKeyTurnState) {
			int nCtrlState = HIBYTE(GetAsyncKeyState(VK_CONTROL));	
			switch (wParam)
			{
			case 'C':
				if (nCtrlState) {
					CCNode* pNodeFocus = s_pEditorLayer->GetFocusNode();
					if (NULL == pNodeFocus) {
						break;
					}
						
					const char *pszConfigFocus = UIMgr::GetInstance()->GetNodeConfig(pNodeFocus->m_uID);

					CopyTextToClipboard(pszConfigFocus);

					CCString* pStrMsg = CCString::createWithFormat("\"%s\" copied ok", pszConfigFocus);
					TipBox(pStrMsg->getCString());
				} else {
					s_pEditorLayer->OnBtnCatch(NULL);
				}
				break;
			case 'V':
				if (nCtrlState && s_pEditorLayer->TestMode(EDITOR_MODE_UI_CATCH)) {
					std::string strCfg = GetTextFromClipboard();
					if (strCfg.empty()) {
						break;
					}

					CCNode* pNodeFocus = s_pEditorLayer->GetFocusNode();
					CCNode* pRoot = pNodeFocus ? pNodeFocus : CCDirector::sharedDirector()->getRunningScene();
					CCNode* pNodeFind = UIMgr::GetInstance()->FindConfigNode(pRoot, strCfg.c_str());

					if (NULL == pNodeFind) {
						std::string strType = GetObjType(pRoot);
						auto *pStrMsg = CCString::createWithFormat("cannot find [%s] in %s", strCfg.c_str(), strType.c_str());
						TipBox(pStrMsg->getCString());
					} else {
						s_pEditorLayer->SelectNode(pNodeFind);
					}
				}
				break;
			case 'R':
				s_pEditorLayer->OnBtnRoot(NULL);
				break;
			case 'T':
				s_pEditorLayer->OnBtnTest(NULL);
				break;
			case 'Q':
				s_pEditorLayer->OnBtnSwitch(NULL);
				break;
			case 'S':
				if (nCtrlState) {
					s_pEditorLayer->OnBtnSave(NULL);
				}
				break;
			case 'L':
				s_pEditorLayer->OnBtnReset(NULL);
				break;
			case 'P':
				s_pEditorLayer->OnBtnPause(NULL);
				break;
			default:
				break;
			}
		}
	}

	return CallNextHookEx(s_hHookOrg, nCode, wParam, lParam);
}

#endif

EditorLayer::EditorLayer( void )
{
	m_bHide = true;
	m_nMode = EDITOR_MODE_NONE;
	m_pBtnCatch = NULL;
	m_pDrawFocus = NULL;
	m_pNodeFocus = NULL;
	m_pNodeFocusInfo = NULL;
	s_pEditorLayer = this;
}

EditorLayer::~EditorLayer( void )
{
	s_pEditorLayer = NULL;
	CC_SAFE_RELEASE_NULL(m_pDrawFocus);
	CC_SAFE_RELEASE_NULL(m_pNodeFocus);
}

CCMenuItem* EditorLayer::BindConfigSelector( const char *pszConfig, SEL_MenuHandler selector )
{
	auto pBtn = dynamic_cast<CCMenuItem*>(this->FindConfigNode(pszConfig));
	if (pBtn) {
		pBtn->setTarget(this, selector);
	}

	return pBtn;
}

bool EditorLayer::init()
{
	if (!MyLayer::init()) {
		return false;
	}

	this->setAccelerometerEnabled(true);

	this->ApplyConfig("EditorLayer");

	this->MapConfigNode();

	m_pBtnSwitch = this->BindConfigSelector("EditorBtnSwitch", menu_selector(EditorLayer::OnBtnSwitch));
	m_pBtnCatch = this->BindConfigSelector("EditorBtnCatch", menu_selector(EditorLayer::OnBtnCatch));
	this->BindConfigSelector("EditorBtnReset", menu_selector(EditorLayer::OnBtnReset));
	this->BindConfigSelector("EditorBtnSave", menu_selector(EditorLayer::OnBtnSave));
	this->BindConfigSelector("EditorBtnUp", menu_selector(EditorLayer::OnBtnUp));
	this->BindConfigSelector("EditorBtnDown", menu_selector(EditorLayer::OnBtnDown));
	this->BindConfigSelector("EditorBtnLeft", menu_selector(EditorLayer::OnBtnLeft));
	this->BindConfigSelector("EditorBtnRight", menu_selector(EditorLayer::OnBtnRight));

	m_pNodeFocusInfo = this->FindConfigNode("EditorFocusInfo");
	m_pNodeFocusConfig = this->FindConfigNode("EditorFocusConfig");

	m_pBtnPause = this->BindConfigSelector("EditorBtnPause", menu_selector(EditorLayer::OnBtnPause));

	this->BindConfigSelector("EditorBtnRoot", menu_selector(EditorLayer::OnBtnRoot));
	this->BindConfigSelector("EditorBtnTest", menu_selector(EditorLayer::OnBtnTest));

#if WIN32
	if (NULL == s_hHookOrg) {
		HMODULE pModule = GetModuleHandle(NULL);
		CCEGLView* pMainWnd = CCEGLView::sharedOpenGLView();
		DWORD dwThreadId = GetWindowThreadProcessId(pMainWnd->getHWnd(), NULL);
		s_hHookOrg = (HHOOK)SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)MyKeyboardProc, pModule, dwThreadId);
	}
#endif

	return true;
}

void EditorLayer::OnBtnSwitch( CCObject* pObj )
{
	m_bHide = !m_bHide;

	this->UpdateView();
}

void EditorLayer::OnBtnReset( CCObject* pObj )
{
	UIMgr::GetInstance()->ReloadPos();
	CStringMgr::Instance()->Reload();

	TipBox("Configs reload ok");
}

void EditorLayer::OnBtnCatch( CCObject* pObj )
{
	this->SetMode(this->TestMode(EDITOR_MODE_UI_CATCH) ? EDITOR_MODE_NONE : EDITOR_MODE_UI_CATCH);

	this->UpdateView();
}

bool EditorLayer::ccTouchBegan( CCTouch *pTouch, CCEvent *pEvent )
{
	if (!this->TestMode(EDITOR_MODE_UI_CATCH)) {
		return false;
	}
	
	CCScene* pRunningScene = CCDirector::sharedDirector()->getRunningScene();
	std::vector<CCNode*> vecChild;
	GetNodeAllChild(pRunningScene, vecChild);

	const char *pszConfigFocus = NULL;
	CCNode* pNodeFocus = NULL;
	float fMinLength = FLOAT_MAX_VALUE;
	for (auto it(vecChild.begin()); it != vecChild.end(); ++it) {
		CCNode* pNodeChild = *it;
		if (NULL == pNodeChild || !pNodeChild->isVisible()) {
			continue;
		}

		auto* pNodeParent = pNodeChild->getParent();
		if (pNodeParent == m_pMenuLayer || pNodeParent && pNodeParent->getParent() == m_pMenuLayer) {
			continue;
		}
		
		if (dynamic_cast<MyLayer*>(pNodeChild)) {
			continue;;
		}
		
		if (IsTouchInNode(pNodeChild, pTouch)) {
			CCPoint ptTo = pNodeChild->getContentSize();
			float fLength = ptTo.getLength();
			if (fLength >= fMinLength) {
				continue;
			}

			const char *pszConfig = UIMgr::GetInstance()->GetNodeConfig(pNodeChild->m_uID);
			if (pszConfig) {
				pNodeFocus = pNodeChild;
				pszConfigFocus = pszConfig;
				fMinLength = fLength;
			}
		}
	}

	return SelectNode(pNodeFocus);
}

bool EditorLayer::SelectNode( CCNode* pNodeFocus )
{
	CC_SAFE_RELEASE_NULL(m_pNodeFocus);
	m_pNodeFocus = pNodeFocus;
	CC_SAFE_RETAIN(m_pNodeFocus);
	if (m_pNodeFocus) {
		m_ptFocusOrg = m_pNodeFocus->getPosition();
	}

	this->UpdateView();

	if (m_pDrawFocus) {
		m_pDrawFocus->setVisible(false);
	}

	CC_SAFE_RELEASE_NULL(m_pDrawFocus);

	if (pNodeFocus) {
		m_pDrawFocus = MyDrawNode::create(CCPointZero, pNodeFocus->getContentSize(), 0xffffffff);
		m_pDrawFocus->SetDrawStyle(DRAW_STYLE_RECT);
		CC_SAFE_RETAIN(m_pDrawFocus);

		pNodeFocus->addChild(m_pDrawFocus);
	}

	return true;
}

void EditorLayer::UpdateFocusConfig( void )
{
	CCNode* pNodeRoot = m_pNodeFocusConfig;
	if (NULL == m_pNodeFocus) {
		pNodeRoot->setVisible(false);
		return;
	}
	
	pNodeRoot->setVisible(true);
	
	auto pSta = dynamic_cast<CCLabelTTF*>(UIMgr::GetInstance()->FindConfigNode(pNodeRoot, "EditorStaFocusTitle"));

	const char *pszConfigFocus = UIMgr::GetInstance()->GetNodeConfig(m_pNodeFocus->m_uID);
	if (NULL == pszConfigFocus) {
		if (pSta) {
			pSta->setString("NO CONFIG");
		}

		return;
	}

	if (pSta) {
		pSta->setString(pszConfigFocus);
	}

	OrgValueControl* pEdt = NULL;
	int nValue = 0;
	pEdt = dynamic_cast<OrgValueControl*>(UIMgr::GetInstance()->FindConfigNode(pNodeRoot, "EditorEdtX"));
	nValue = UIMgr::GetInstance()->GetConfigValue(pszConfigFocus, "x", 0);
	NodeSetText(pEdt, nValue);

	pSta = dynamic_cast<CCLabelTTF*>(UIMgr::GetInstance()->FindConfigNode(pNodeRoot, "EditorStaDeltaX"));
	int nDeltaX = MyCeil(m_pNodeFocus->getPositionX() - m_ptFocusOrg.x);
	if (nDeltaX) {
		NodeSetText(pSta, nValue + nDeltaX);
	} else {
		pSta->setString("");
	}

	pEdt = dynamic_cast<OrgValueControl*>(UIMgr::GetInstance()->FindConfigNode(pNodeRoot, "EditorEdtY"));
	nValue = UIMgr::GetInstance()->GetConfigValue(pszConfigFocus, "y", 0);
	NodeSetText(pEdt, nValue);

	pSta = dynamic_cast<CCLabelTTF*>(UIMgr::GetInstance()->FindConfigNode(pNodeRoot, "EditorStaDeltaY"));
	int nDeltaY = MyCeil(m_pNodeFocus->getPositionY() - m_ptFocusOrg.y);
	if (nDeltaY) {
		NodeSetText(pSta, nValue + nDeltaY);
	} else {
		pSta->setString("");
	}

	pEdt = dynamic_cast<OrgValueControl*>(UIMgr::GetInstance()->FindConfigNode(pNodeRoot, "EditorEdtW"));
	if (pEdt) {
		int nValue = UIMgr::GetInstance()->GetConfigValue(pszConfigFocus, "w", -1);
		if (nValue > 0) {
			NodeSetText(pEdt, nValue, true);
		} else {
			NodeSetText(pEdt, 0, false);
		}
	}

	pEdt = dynamic_cast<OrgValueControl*>(UIMgr::GetInstance()->FindConfigNode(pNodeRoot, "EditorEdtH"));
	if (pEdt) {
		int nValue = UIMgr::GetInstance()->GetConfigValue(pszConfigFocus, "h", -1);
		if (nValue > 0) {
			NodeSetText(pEdt, nValue, true);
		} else {
			NodeSetText(pEdt, 0, false);
		}
	}

	pEdt = dynamic_cast<OrgValueControl*>(UIMgr::GetInstance()->FindConfigNode(pNodeRoot, "EditorEdtZ"));
	if (pEdt) {
		int nValue = UIMgr::GetInstance()->GetConfigValue(pszConfigFocus, "z", INT_MAX);
		if (INT_MAX != nValue) {
			NodeSetText(pEdt, nValue, true);
		} else {
			NodeSetText(pEdt, 0, false);
		}
	}
}

void EditorLayer::UpdateFocusInfo( void )
{
	CCNode* pNodeRoot = m_pNodeFocusInfo;
	if (NULL == m_pNodeFocus) {
		pNodeRoot->setVisible(false);
		return;
	}

	pNodeRoot->setVisible(true);

	auto pSta = dynamic_cast<CCLabelTTF*>(UIMgr::GetInstance()->FindConfigNode(pNodeRoot, "EditorStaFocusTitle"));
	if (pSta) {
		std::string strType = GetObjType(m_pNodeFocus);
		auto* pStrMsg = CCString::createWithFormat("%s*(0x%x)", strType.c_str(), m_pNodeFocus);
		pSta->setString(pStrMsg->getCString());
	}

	CCPoint ptDelta = m_pNodeFocus->getPosition() - m_ptFocusOrg;
	pSta = dynamic_cast<CCLabelTTF*>(UIMgr::GetInstance()->FindConfigNode(pNodeRoot, "EditorStaDeltaX"));
	NodeSetText(pSta, MyCeil(ptDelta.x), false, true);
	
	pSta = dynamic_cast<CCLabelTTF*>(UIMgr::GetInstance()->FindConfigNode(pNodeRoot, "EditorStaDeltaY"));
	NodeSetText(pSta, MyCeil(ptDelta.y), false, true);

	OrgValueControl* pEdt = NULL;
	pEdt = dynamic_cast<OrgValueControl*>(UIMgr::GetInstance()->FindConfigNode(pNodeRoot, "EditorEdtX"));
	NodeSetText(pEdt, m_ptFocusOrg.x);

	pEdt = dynamic_cast<OrgValueControl*>(UIMgr::GetInstance()->FindConfigNode(pNodeRoot, "EditorEdtY"));
	NodeSetText(pEdt, m_ptFocusOrg.y);

	pEdt = dynamic_cast<OrgValueControl*>(UIMgr::GetInstance()->FindConfigNode(pNodeRoot, "EditorEdtW"));
	NodeSetText(pEdt, m_pNodeFocus->getContentSize().width);

	pEdt = dynamic_cast<OrgValueControl*>(UIMgr::GetInstance()->FindConfigNode(pNodeRoot, "EditorEdtH"));
	NodeSetText(pEdt, m_pNodeFocus->getContentSize().height);

	pEdt = dynamic_cast<OrgValueControl*>(UIMgr::GetInstance()->FindConfigNode(pNodeRoot, "EditorEdtZ"));
	NodeSetText(pEdt, m_pNodeFocus->getZOrder());
}

void EditorLayer::UpdateView()
{
	if (m_bHide) {
		this->SetMode(EDITOR_MODE_NONE);
	}

	bool bCatchMode = this->TestMode(EDITOR_MODE_UI_CATCH);
	if (!bCatchMode) {
		m_pNodeFocus = NULL;
	}

	TurnBtnTextColor(m_pBtnCatch, bCatchMode ? ccRED : ccWHITE);
	TurnBtnTextColor(m_pBtnPause, m_vecPauseLayers.empty() ? ccWHITE : ccRED);

	bool bShowExtra = !m_bHide;
	m_pSpriteFront->setVisible(bShowExtra);
	m_pSpriteBack->setVisible(bShowExtra);
	std::vector<CCNode*> vecChild;
	GetNodeAllChild(this, vecChild);
	for (auto it(vecChild.begin()); it != vecChild.end(); ++it) {
		auto *pBtn = dynamic_cast<CCMenuItem*>(*it);
		if (NULL == pBtn) {
			continue;
		}

		if (pBtn == m_pBtnSwitch) {
			continue;
		}

		pBtn->setVisible(bShowExtra);
	}

	if (m_pDrawFocus) {
		m_pDrawFocus->setVisible(m_pNodeFocus ? true : false);
	}
	
	this->UpdateFocusInfo();
	this->UpdateFocusConfig();
}


void EditorLayer::MoveFocus( CCPoint ptDelta )
{
	if (!this->TestMode(EDITOR_MODE_UI_CATCH) || NULL == m_pNodeFocus) {
		return;
	}

	CCPoint ptPos = m_pNodeFocus->getPosition() + ptDelta;
	m_pNodeFocus->setPosition(ptPos);
	this->UpdateView();
}


void EditorLayer::didAccelerate( CCAcceleration* pAccelerationValue )
{
	this->MoveFocus(ccp(pAccelerationValue->x, pAccelerationValue->y));
//	CCLOG("x: %lf y: %lf z: %lf time: %lf", pAccelerationValue->x, pAccelerationValue->y, pAccelerationValue->z, pAccelerationValue->timestamp);
}

void EditorLayer::OnBtnSave( CCObject* pObj )
{
	if (!this->TestMode(EDITOR_MODE_UI_CATCH)) {
		TipBox("only for catch mode(click 'C')");
		return;
	}

	if (NULL == m_pNodeFocus) {
		TipBox("no node selected");
		return;
	}

	CCPoint ptDelta = m_pNodeFocus->getPosition() - m_ptFocusOrg;
	int nDeltaX = MyCeil(ptDelta.x);
	int nDeltaY = MyCeil(ptDelta.y);
	if (!nDeltaX && !nDeltaY) {
		TipBox("no changes");
		return;
	}
	
#ifdef WIN32
	int nSucAmount = 0;
	const char *pszConfigFocus = UIMgr::GetInstance()->GetNodeConfig(m_pNodeFocus->m_uID);
	if (nDeltaX) {
		int nValue = UIMgr::GetInstance()->GetConfigValue(pszConfigFocus, "x", 0) + nDeltaX;
		if (UIMgr::GetInstance()->SaveConfigValue(pszConfigFocus, "x", nValue)) {
			++nSucAmount;
		}
	}

	if (nDeltaY) {
		int nValue = UIMgr::GetInstance()->GetConfigValue(pszConfigFocus, "y", 0) + nDeltaY;
		if (UIMgr::GetInstance()->SaveConfigValue(pszConfigFocus, "y", nValue)) {
			++nSucAmount;
		}
	}

	auto pStr = CCString::createWithFormat("Save %d Item(s) Suc", nSucAmount);
	TipBox(pStr->getCString());

	m_ptFocusOrg = m_pNodeFocus->getPosition();
	UpdateView();
#else
	TipBox("save operation only for win32 ver");
	return;
#endif
}

void EditorLayer::OnBtnUp( CCObject* pObj )
{
	this->MoveFocus(ccp(0, 1));
}

void EditorLayer::OnBtnDown( CCObject* pObj )
{
	this->MoveFocus(ccp(0, -1));
}

void EditorLayer::OnBtnLeft( CCObject* pObj )
{
	this->MoveFocus(ccp(-1, 0));
}

void EditorLayer::OnBtnRight( CCObject* pObj )
{
	this->MoveFocus(ccp(1, 0));
}

bool EditorLayer::TestMode( int nMode ) const
{
	return (m_nMode & nMode) ? true : false;
}

void EditorLayer::SetMode( int nMode )
{
	m_nMode = nMode;
}

void EditorLayer::OnBtnPause( CCObject* pObj )
{
	auto pScene = CCDirector::sharedDirector()->getRunningScene();
	auto pChilds = pScene->getChildren();

	if (!m_vecPauseLayers.empty()) {
		for (auto it(m_vecPauseLayers.begin()); it != m_vecPauseLayers.end(); ++it) {
			MyLayer* pLayer = *it;
			pLayer->onEnter();
		}

		m_vecPauseLayers.clear();
	} else {
		CCObject* pObjLayer;
		CCARRAY_FOREACH(pChilds, pObjLayer) {
			MyLayer* pLayer = dynamic_cast<MyLayer*>(pObjLayer);
			if (NULL == pLayer || pLayer == this) {
				continue;
			}

			if (pLayer->isRunning()) {
				pLayer->onExit();
				m_vecPauseLayers.push_back(pLayer);
			}
		}
	}

	this->UpdateView();
}

void EditorLayer::OnBtnRoot( CCObject* pObj )
{
	CCNode* pNodeFocus = s_pEditorLayer->GetFocusNode();
	if (NULL == pNodeFocus) {
		return;
	}

	s_pEditorLayer->SelectNode(pNodeFocus->getParent());
}

#ifdef LUA_ENABLED
TOLUA_API int tolua_ToLuaTest_open (lua_State* tolua_S);
#endif

void EditorLayer::OnBtnTest( CCObject* pObj )
{
#ifdef LUA_ENABLED
	lua_State *pState = luaL_newstate();
	luaopen_base(pState);
	tolua_ToLuaTest_open(pState);
	std::string strPath = CCFileUtils::sharedFileUtils()->fullPathForFilename("script/uieditor.lua");
	MY_ASSERT(!luaL_loadfile(pState, strPath.c_str()), "luaL_loadfile failed: [%s]", lua_tostring(pState, -1));
	MY_ASSERT(!lua_pcall(pState, 0, 0, 0), "lua_pcall failed: [%s]", lua_tostring(pState, -1));
#endif
}

bool EditorLayer::IsHide( void ) const
{
	return m_bHide;
}

CCNode* EditorLayer::GetFocusNode( void ) const
{
	return m_pNodeFocus;
}
