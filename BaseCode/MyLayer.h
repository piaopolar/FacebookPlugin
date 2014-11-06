//
//  MyLayer.h
//
//  Created by Ruoqian, Chen on 2013/06/14
//

#ifndef __MY_LAYER_H__
#define __MY_LAYER_H__

#include "BaseDef.h"

NS_CC_EXT_BEGIN
class MyEditBox;
class MyEditBoxDelegate;
NS_CC_EXT_END

class MyMenu;
class MyScrollView;

class MyLayer : public CCLayer
{
public:
	MyLayer(void);
	virtual ~MyLayer(void);

	CREATE_FUNC(MyLayer);
    virtual bool init();
    virtual void onExit();
    virtual void onEnter();
	virtual void setContentSize(const CCSize& contentSize);
	virtual void onEnterTransitionDidFinish();
	virtual void onExitTransitionDidStart();

	virtual void OnBtnBack(CCObject* pObj);
	virtual void OnOperBack(void);
	virtual void keyBackClicked();

	virtual void UpdateView();
    
    virtual void runOpenAction() {}

    virtual void runCloseAction(){}
	
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    void setSwallowTouches(bool bSwallowsTouches);
	bool getSwallowTouches(void) const;
	
	void ApplyConfig(const char *pszConfigSection);
    
	void AddConfigChild(const char *pszConfigChild);
	void AddConfigChild(CCNode* pParent, const char *pszConfigChild);
	CCNode* AddConfigNode(const char *pszConfigSection, SEL_MenuHandler selector = NULL);
	CCNode* AddConfigNode(CCNode* pParent, const char *pszConfigSection, SEL_MenuHandler selector = NULL);
	CCNode* AddConfigNodeWithChild(const char *pszConfigSection, const char *pszChildConfigSection);
	CCNode* AddConfigNodeWithChild(const char *pszConfigSection, CCNode* pNodeChild);
	MyScrollView* AddToConfigScrollView(const char *pszConfig, CCScrollViewDelegate* pDelegate);
	MyScrollView* AddToConfigScrollViewWithNoDelegate(const char *pszConfig);
	CCSprite* AddConfigSprite(const char *pszConfigSection, const char *pszImageFile = NULL);
	CCSprite* AddConfigSprite(CCNode* pParent, const char *pszConfigSection, const char *pszImageFile = NULL);
	cocos2d::extension::MyEditBox* AddConfigEditBox(const char *pszConfigSection, cocos2d::extension::MyEditBoxDelegate* pDelegate);
	CCMenuItemSprite* AddConfigMenuItem(const char *pszConfigSection, SEL_MenuHandler selector = NULL);
	CCMenuItemSprite* AddConfigMenuItem(CCNode* pParent, const char *pszConfigSection, SEL_MenuHandler selector = NULL);
	CCLabelTTF* AddConfigTextLabel(const char *pszConfigSection, const char *pszString = NULL, int nWidth = -1);
	CCLabelTTF* AddConfigTextLabel(CCNode* pParent, const char *pszConfigSection, const char *pszString = NULL, int nWidth = -1);
	static CCSprite* AddConfigDictImage(CCNode* pParent, const char *pszConfigSection, const char *pszText);
	
	void MapConfigNode(void);
	CCNode* FindConfigNode(const char *pszConfig) const;

	CCSprite* AddSprite(CCSprite* pSprite);

	void SetImage(const char *pszFileName);
	void SetAddFrontMode(bool bAddFront = true);
	void ClearFrontSprite(void);
	void AdjustResolution(void);

	// if return false, layer init next frame or later, so put init codes in OnEnter() instead of after this function.
	bool MakeSureShowMe(void);
	void EnabledUpdateOnEnter(bool bEnabled);

	static void SpriteSetImage(CCSprite* pSprite, const char *pszFile);
private:
	void AddMeProperlyTime(float f);
protected:
    bool m_bSwallowsTouches;
	bool m_bUpdateOnEnterEnabled;
	CCRect m_rect;
	MyMenu* m_pMenuLayer;
	CCPoint m_ptPosition;
	CCPoint m_ptAnchor;
	CCSprite *m_pSpriteBack;
	CCSprite *m_pSpriteFront;
	CCSprite *m_pSprite;
	std::string m_strFileBG;
	std::map<std::string, CCNode*> m_mapConfigNode;
};

#endif