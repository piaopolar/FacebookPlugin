#include "StdAfx.h"

#include "MyMenu.h"

MyMenu* MyMenu::create()
{
	return MyMenu::create(NULL, NULL);
}

MyMenu* MyMenu::create(CCMenuItem* item, ...)
{
	va_list args;
	va_start(args,item);

	MyMenu *pRet = MyMenu::createWithItems(item, args);

	va_end(args);

	return pRet;
}

MyMenu* MyMenu::createWithArray(CCArray* pArrayOfItems)
{
	MyMenu *pRet = new MyMenu();
	if (pRet && pRet->initWithArray(pArrayOfItems))
	{
		pRet->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}

	return pRet;
}

MyMenu* MyMenu::createWithItems(CCMenuItem* item, va_list args)
{
	CCArray* pArray = NULL;
	if( item )
	{
		pArray = CCArray::create(item, NULL);
		CCMenuItem *i = va_arg(args, CCMenuItem*);
		while(i)
		{
			pArray->addObject(i);
			i = va_arg(args, CCMenuItem*);
		}
	}

	return MyMenu::createWithArray(pArray);
}

MyMenu* MyMenu::createWithItem(CCMenuItem* item)
{
	return MyMenu::create(item, NULL);
}

void MyMenu::setSwallowTouches(bool bSwallowsTouches)
{
	m_bSwallowsTouches = bSwallowsTouches;
}

bool MyMenu::getSwallowTouches()
{
	return m_bSwallowsTouches;
}

void MyMenu::registerWithTouchDispatcher()
{
	CCTouchDispatcher* pDispatcher = CCDirector::sharedDirector()->getTouchDispatcher();

	// Using LuaBindings
	if (getScriptTouchHandlerEntry())
	{
		if (getScriptTouchHandlerEntry()->isMultiTouches())
		{
			pDispatcher->addStandardDelegate(this, 0);
			LUALOG("[LUA] Add multi-touches event handler: %d", getScriptTouchHandlerEntry()->getHandler());
		}
		else
		{
			pDispatcher->addTargetedDelegate(this,
				getScriptTouchHandlerEntry()->getPriority(),
				getScriptTouchHandlerEntry()->getSwallowsTouches());
			LUALOG("[LUA] Add touch event handler: %d", getScriptTouchHandlerEntry()->getHandler());
		}
	}
	else
	{
		if( getTouchMode() == kCCTouchesAllAtOnce ) {
			pDispatcher->addStandardDelegate(this, 0);
		} else {
			pDispatcher->addTargetedDelegate(this, getTouchPriority(), m_bSwallowsTouches);
		}
	}
}

void MyMenu::ccTouchEnded( CCTouch* touch, CCEvent* event )
{
	CC_UNUSED_PARAM(touch);
	CC_UNUSED_PARAM(event);
	CCAssert(m_eState == kCCMenuStateTrackingTouch, "[Menu ccTouchEnded] -- invalid state");
	if (m_pSelectedItem)
	{
		m_pSelectedItem->retain();
		m_pSelectedItem->autorelease();
		m_pSelectedItem->unselected();
		m_pSelectedItem->activate();
//		if (m_pSelectedItem) {
//			m_pSelectedItem->release();
//		}
	}
	m_eState = kCCMenuStateWaiting;
}

