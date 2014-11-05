//
//  SceneFactoryImpl.h
//
//  Created by Ruoqian, Chen on 2014/09/25
//

#ifndef __SCENE_FACTORY_IMPL_H__
#define __SCENE_FACTORY_IMPL_H__

#include "SceneFactoryInterface.h"

class MyScene;

enum
{
	SCENE_TYPE_HelloWorldScene,
};

#define FUN_GET_SCENE_TYPE(className) static int GetSceneType(void) { return SCENE_TYPE_##className; }

class CSceneFactory : public ISceneFactory
{
public:
	static CSceneFactory* GetInstance(void);
	static void Release(void);

	virtual MyScene* createScene(int nType);
};

#endif
