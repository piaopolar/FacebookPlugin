//
//  MyFrameAnimationNode.h
//
//  Created by Ruoqian, Chen on 2014/01/08
//
#include "BaseDef.h"

#ifndef __MY_FRAME_ANIMATION_NODE_H__
#define __MY_FRAME_ANIMATION_NODE_H__

class MyFrameAnimationNode : public CCSprite
{
public:
	virtual ~MyFrameAnimationNode(void);
	static MyFrameAnimationNode* create(const char *pszConfig);

	virtual bool initWithConfig(const char *pszConfig);
	virtual void StopAnimation(void);
	virtual void RunAnimation(float fDelay = -1, const char *pszConfig = NULL);
private:
	std::string m_strConfig;
	CCAction* m_pFrameAnimation;
};

#endif
