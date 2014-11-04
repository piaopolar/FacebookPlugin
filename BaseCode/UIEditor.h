//
//  UIEditor.h
//
//  Created by Ruoqian, Chen on 2014/06/09
//


#ifndef __UI_EDITOR__
#define __UI_EDITOR__

#if WIN32
#define EDITOR_UI 1
#else
#define EDITOR_UI 0
#endif

#include "BaseDef.h"
#include "MyLayer.h"

class MyDrawNode;

enum
{
	EDITOR_MODE_NONE = 0,
	EDITOR_MODE_UI_CATCH = 1,
};

class EditorLayer : public MyLayer
{
public:
	EditorLayer(void);
	virtual ~EditorLayer(void);

	CREATE_FUNC(EditorLayer);
	virtual bool init();

	bool IsHide(void) const;
	CCNode* GetFocusNode(void) const;
	bool TestMode(int nMode) const;
	void SetMode(int nMode);
	
	void OnBtnCatch(CCObject* pObj);
	void OnBtnReset(CCObject* pObj);
	void OnBtnSwitch(CCObject* pObj);
	void OnBtnSave(CCObject* pObj);

	void OnBtnPause(CCObject* pObj);
	void OnBtnRoot(CCObject* pObj);
	void OnBtnTest(CCObject* pObj);

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void didAccelerate(CCAcceleration* pAccelerationValue);

	void OnBtnUp(CCObject* pObj);
	void OnBtnDown(CCObject* pObj);
	void OnBtnLeft(CCObject* pObj);
	void OnBtnRight(CCObject* pObj);

	void MoveFocus(CCPoint ptDelta);
	bool SelectNode(CCNode* pNode);

	void UpdateFocusConfig(void);
	void UpdateFocusInfo(void);

	CCMenuItem* BindConfigSelector(const char *pszConfig, SEL_MenuHandler selector);

	virtual void UpdateView();
private:
	int m_nMode;
	bool m_bHide;
	CCPoint m_ptFocusOrg;
	CCNode* m_pNodeFocus;
	CCNode* m_pNodeFocusInfo;
	CCNode* m_pNodeFocusConfig;
	MyDrawNode* m_pDrawFocus;
	CCMenuItem* m_pBtnSwitch;
	CCMenuItem* m_pBtnCatch;
	CCMenuItem* m_pBtnPause;
	std::vector<MyLayer*> m_vecPauseLayers;
};

#endif
