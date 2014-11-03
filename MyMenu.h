#ifndef __MY_MENU_H__
#define __MY_MENU_H__

#include "BaseDef.h"

class MyMenu:public CCMenu
{
public:
    MyMenu():m_bSwallowsTouches(true){}
    
    /** creates an empty CCMenu */
    static MyMenu* create();
    
    /** creates a CCMenu with CCMenuItem objects */
    static MyMenu* create(CCMenuItem* item, ...);
    
    /** creates a CCMenu with a CCArray of CCMenuItem objects */
    static MyMenu* createWithArray(CCArray* pArrayOfItems);
    
    /** creates a CCMenu with it's item, then use addChild() to add
     * other items. It is used for script, it can't init with undetermined
     * number of variables.
     */
    static MyMenu* createWithItem(CCMenuItem* item);
    
    /** creates a CCMenu with CCMenuItem objects */
    static MyMenu* createWithItems(CCMenuItem *firstItem, va_list args);
    
    void registerWithTouchDispatcher();
    virtual void ccTouchEnded(CCTouch* touch, CCEvent* event);
    
    void setSwallowTouches(bool bSwallowsTouches);
    
    bool getSwallowTouches();
    
private:
    bool m_bSwallowsTouches;
};

#endif
