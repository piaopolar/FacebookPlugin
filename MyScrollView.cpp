//
//  MyScrollView.cpp
//
//  Created by Ruoqian, Chen on 2014/05/09
//
#include "StdAfx.h"

#include "MyScrollView.h"

void MyScrollView::onEnter()
{
	UI_TRACE("QMyScrollView::onEnter %x %d %d", this, CCLayer::getTouchPriority(), m_bSwallow ? 1 : 0);
	CCScrollView::onEnter();
}

void MyScrollView::registerWithTouchDispatcher()
{
	UI_TRACE("QMyScrollView::registerWithTouchDispatcher %x %d %d", this, CCLayer::getTouchPriority(), m_bSwallow ? 1 : 0);
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, CCLayer::getTouchPriority(), m_bSwallow);
}

void MyScrollView::setSwallow(bool bSwallow)
{
	m_bSwallow = bSwallow;
}

bool MyScrollView::getSwallow()
{
	return m_bSwallow;
}

bool MyScrollView::IsWantClick( void ) const
{
	if (!m_bTouchMoveEnabled) {
		return true;
	}

	return m_bWantClick;
}

bool MyScrollView::ccTouchBegan( CCTouch *pTouch , CCEvent *pEvent)
{
	if (!m_bTouchMoveEnabled) {
		return m_bSwallow;
	}

	UI_TRACE("QMyScrollView::ccTouchBegan %x");
	m_ptTouchBegan = this->convertTouchToNodeSpace(pTouch);
	m_bWantClick = true;
	float MaxH =  this->getViewSize().height;
	if(m_ptTouchBegan.y<0 || (m_ptTouchBegan.y >=MaxH))
		m_bWantClick = false;

	//CCPoint scrollPos;
	//scrollPos = this->convertTouchToNodeSpace(pTouch);

	return CCScrollView::ccTouchBegan(pTouch, pEvent);
}

void MyScrollView::ccTouchMoved( CCTouch *pTouch , CCEvent *pEvent)
{
	if (!m_bTouchMoveEnabled) {
		return;
	}

	if (m_bWantClick) {
		CCPoint ptOffset = this->convertTouchToNodeSpace(pTouch) - m_ptTouchBegan;
		const int MAX_CLICK_ABERRATIONS = 5;
		if (fabs(ptOffset.x) > MAX_CLICK_ABERRATIONS || fabs(ptOffset.y) > MAX_CLICK_ABERRATIONS) {
			m_bWantClick = false;
		}
	}


	//	QPo	int InArea = this->convertTouchToNodeSpace(pTouch);
	//	if(m_bWantClick)
	CCScrollView::ccTouchMoved(pTouch,pEvent);
}

void MyScrollView::ccTouchEnded( CCTouch* touch, CCEvent *pEvent )
{
	if (!m_bTouchMoveEnabled) {
		return;
	}

	m_tTouchPoint = touch->getLocation();

	if (this->isBounceable()) {
		return CCScrollView::ccTouchEnded(touch, pEvent);
	}

	if (!this->isVisible()) {
		return;
	}

	if (m_pTouches->containsObject(touch)) {
		if (m_pTouches->count() == 1 && m_bTouchEnabled) {
			//			this->schedule(schedule_selector(CCScrollView::deaQelerateScrolling));
		}

		m_pTouches->removeObject(touch);
	}

	if (!m_pTouches->count()) {
		m_bDragging = false;
		m_bTouchMoved = false;
	}
}

void MyScrollView::ccTouchCancelled( CCTouch *pTouch, CCEvent *pEvent )
{
	m_bWantClick = false;
}

void MyScrollView::ScrollToHead( void )
{
	CCScrollViewDirection dir = this->getDirection();
	switch (dir) {
	case kCCScrollViewDirectionVertical:
		this->setContentOffset(ccp(0, this->getViewSize().height - this->getContentSize().height));
		break;
	case kCCScrollViewDirectionHorizontal:
		this->setContentOffset(ccp(0, 0));
		break;
	default:
		break;
	}
}

void MyScrollView::SetContentSizeByItemAmount( float fAmount )
{
	CCScrollViewDirection dir = this->getDirection();
	switch (dir) {
	case kCCScrollViewDirectionVertical:
		this->setContentSize(CCSizeMake(this->getContentSize().width, m_nSpace * fAmount));
		break;
	case kCCScrollViewDirectionHorizontal:
		{
			// CCScrollView(horizontal) force align right
			CCSize sizeView = this->getViewSize();
			if (m_nSpace * fAmount < sizeView.width) {
				this->setContentSize(CCSizeMake(sizeView.width, this->getContentSize().height));
			} else {
				this->setContentSize(CCSizeMake(m_nSpace * fAmount, this->getContentSize().height));
			}
		}
		break;
	default:
		break;
	}
}

void MyScrollView::SetSpace( int nSpace )
{
	m_nSpace = nSpace;
}

void MyScrollView::EnableTouchMove( bool bEnabled )
{
	m_bTouchMoveEnabled = bEnabled;
}

void MyScrollView::NodePositionMove( CCNode* pNode, int nIndex )
{
	CCScrollViewDirection dir = this->getDirection();
	switch (dir) {
	case kCCScrollViewDirectionVertical:
		UIMgr::GetInstance()->NodePositionMove(pNode, ccp(0, this->getContentSize().height - nIndex * m_nSpace));
		break;
	case kCCScrollViewDirectionHorizontal:
		UIMgr::GetInstance()->NodePositionMove(pNode, ccp(nIndex * m_nSpace, 0));
		break;
	default:
		break;
	}
}

void MyScrollView::MakeSureIndexVisable( int nIndex )
{
	CCSize sizeView = this->getViewSize();
	CCPoint ptOffsetOrg = this->getContentOffset();
	int nOffsetX = (int) floor(ptOffsetOrg.x);
	int nVisableFirst = -nOffsetX / m_nSpace + 1;
	int nVisableLast = (-nOffsetX + sizeView.width) / m_nSpace - 1;
	if (nIndex < nVisableFirst) {
		this->setContentOffset(ccp(-nIndex * m_nSpace, ptOffsetOrg.y), false);
	} else if (nIndex > nVisableLast) {
		this->setContentOffset(ccp(sizeView.width - (nIndex + 1) * m_nSpace, ptOffsetOrg.y), false);
	}
}

bool MyScrollView::IsVaildView( void )
{
	float fOffsetY = this->getContentOffset().y;
	float fViewHeight = this->getViewSize().height;
	float fContentHeight = this->getContentSize().height;
	if (fContentHeight > fViewHeight) {
		if (fOffsetY > 0 || fOffsetY < fViewHeight - fContentHeight) {
			return false;
		}
	} else {
		if (fOffsetY < 0 || fOffsetY > fContentHeight - fContentHeight) {
			return false;
		}
	}

	return true;
}