//
//  SceneFactoryImpl.cpp
//
//  Created by Ruoqian, Chen on 2014/09/25
//
#include "MyScene.h"

#include "BaseFunc.h"
#include "HelloWorldScene.h"

#include "SceneFactoryImpl.h"

#define CASE_CREATE_SCENE(className) case SCENE_TYPE_##className: pMyScene = new className; break;

MyScene* CSceneFactory::createScene( int nType )
{
	MyScene* pMyScene = NULL;
	switch (nType) {
	CASE_CREATE_SCENE(HelloWorldScene);
	default:
		MY_ASSERT(false, "Scene Type(%d) create failed", nType);
		break;
	}

	if (pMyScene) {
		pMyScene->autorelease();
	}

	return pMyScene;
}

namespace
{
	static CSceneFactory* s_pMgr = NULL;
}

CSceneFactory* CSceneFactory::GetInstance( void )
{
	if (NULL == s_pMgr) {
		s_pMgr = new CSceneFactory;
	}

	return s_pMgr;
}

void CSceneFactory::Release( void )
{
	if (NULL == s_pMgr) {
		return;
	}

	CC_SAFE_DELETE(s_pMgr);
}
