//
//  MyScrollView .h
//
//  Created by Ruoqian, Chen on 2014/05/09
//

#include "BaseDef.h"

#include "GUI/CCScrollView/CCScrollView.h"

class UIMgr;

class MyScrollView : public CCScrollView
{
public:
	MyScrollView(): m_nSpace(0), m_bSwallow(true), m_bWantClick(true), m_bTouchMoveEnabled(true) {}
	virtual void onEnter();
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);

	void MakeSureIndexVisable(int nIndex);
	void NodePositionMove(CCNode* pNode, int nIndex);
	void ScrollToHead(void);
	void SetContentSizeByItemAmount(float fAmount);

	bool IsWantClick(void) const;
	bool IsVaildView(void);

	friend MyScrollView* UIMgr::CreateConfigScrollView(const char *pszConfig);

	virtual void registerWithTouchDispatcher();
	void setSwallow(bool bSwallow);
	bool getSwallow();
protected:
	void SetSpace(int nSpace);
	void EnableTouchMove(bool bEnabled);
private:
	int m_nSpace;
	bool m_bSwallow;
	bool m_bTouchMoveEnabled;
	bool m_bWantClick;
	CCPoint m_ptTouchBegan;
};

