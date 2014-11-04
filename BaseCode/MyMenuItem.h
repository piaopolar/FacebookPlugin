//
//  MyMenuItem.h
//
//  Created by Ruoqian, Chen on 2014/05/09
//

#ifndef __MY_MENU_ITEM_H__
#define __MY_MENU_ITEM_H__

#include "cocos2d.h"

class QMyMenuItemSprite : public CCMenuItemSprite
{
public:
	static QMyMenuItemSprite* create();
	static QMyMenuItemSprite* create(CCNode* normalSprite, CCNode* selectedSprite, CCNode* disabledSprite);
	static QMyMenuItemSprite* create(const char *normalImage, const char *selectedImage, const char *disabledImage);
	virtual void activate();

	void SetClickSound(std::string val);
private:
	std::string m_strClickSound;
};

#endif
