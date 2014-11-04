//
//  MyMenuItem.cpp
//
//  Created by Ruoqian, Chen on 2014/05/09
//
#include "StdAfx.h"

#include "MyMenuItem.h"


void QMyMenuItemSprite::SetClickSound( std::string val )
{
	m_strClickSound = val;
}

void QMyMenuItemSprite::activate()
{
	CCMenuItemSprite::activate();
}

QMyMenuItemSprite * QMyMenuItemSprite::create( CCNode* normalSprite, CCNode* selectedSprite, CCNode* disabledSprite )
{
    QMyMenuItemSprite *pRet = new QMyMenuItemSprite();
    pRet->initWithNormalSprite(normalSprite, selectedSprite, disabledSprite, NULL, NULL); 
    pRet->autorelease();
    return pRet;
}

QMyMenuItemSprite* QMyMenuItemSprite::create( const char *normalImage, const char *selectedImage, const char *disabledImage )
{
	CCSprite* normalSprite = SpriteCreate(normalImage);
	CCSprite* selectedSprite = SpriteCreate(selectedImage);
	CCSprite* disabledSprite = SpriteCreate(disabledImage);
	return QMyMenuItemSprite::create(normalSprite, selectedSprite, disabledSprite);
}

QMyMenuItemSprite* QMyMenuItemSprite::create()
{
	CCNode* pNodeNull = NULL;
	return QMyMenuItemSprite::create(pNodeNull, pNodeNull, pNodeNull);
}
