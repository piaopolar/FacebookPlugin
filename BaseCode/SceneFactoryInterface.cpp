//
//  SceneFactoryInterface.cpp
//
//  Created by Ruoqian, Chen on 2013/07/23
//
#include "StdAfx.h"
#include "SceneFactoryInterface.h"

MyScene* ISceneFactory::createScene( int nType )
{
	MY_ASSERT(false, "Scene Type(%d) create failed", nType);
	return NULL;
}