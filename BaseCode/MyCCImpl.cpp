//
//  MyCCImpl.cpp
//
//  Created by Ruoqian, Chen on 2014/01/26
//
#include "StdAfx.h"

#include "MyCCImpl.h"

void MyCCOjbectImpl::RemoveUnusedRes(float fDelay)
{
	if (fDelay <= 0) {
		this->RemoveUnusedResImm(0);
		return;
	}

	CCScheduler* pScheduler = CCDirector::sharedDirector()->getScheduler();
	if (NULL == pScheduler) {
		return;
	}

	pScheduler->scheduleSelector(schedule_selector(MyCCOjbectImpl::RemoveUnusedResImm), this, 0, 0, fDelay, false);
}

void MyCCOjbectImpl::RemoveUnusedResImm(float f)
{
	TimeTraceLn("MyCCOjbectImpl::RemoveUnusedResImm BEGIN");
	CCAnimationCache::sharedAnimationCache()->purgeSharedAnimationCache();
	CCSpriteFrameCache::sharedSpriteFrameCache()->removeUnusedSpriteFrames();
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();
	UIMgr::GetInstance()->LoadTexturePacks();
	TimeTraceLn("MyCCOjbectImpl::RemoveUnusedResImm END");
}

