//
//  MyScene.h
//
//  Created by Ruoqian, Chen on 2013/06/14
//
#ifndef __MY_SCENE_H__
#define __MY_SCENE_H__

#include "cocos2d.h"

class MyScene : public cocos2d::CCScene
{
public:
	MyScene();

	virtual void onEnter();
	virtual void onEnterTransitionDidFinish();
	virtual void onExitTransitionDidStart();
	virtual void onExit();
	virtual bool OnInit(void);

	virtual void addChild(CCNode * child);
	virtual void addChild(CCNode * child, int zOrder);
	virtual void addChild(CCNode* child, int zOrder, int tag);

	virtual void removeChild(CCNode * child);
	virtual void removeChild(CCNode* child, bool cleanup);

	virtual CCNode* getChildByTag(int tag);
	
	void EnableLace(bool bLaceEnabled);
	void EnableTopLayer(int nEnabledGroupMask);
protected:
	int m_nTopLayerEnabledMask;
	bool m_bLaceEnabled;
	CCNode* m_pLayout;

// Scene Stack
public:
	CC_SYNTHESIZE(int, m_nInitData, InitData);
};

#endif
