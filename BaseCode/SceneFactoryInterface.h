//
//  SceneFactoryInterface.h
//
//  Created by Ruoqian, Chen on 2013/07/23
//

#ifndef __SCENE_FACTORY_INTERFACE_H__
#define __SCENE_FACTORY_INTERFACE_H__

class MyScene;

class ISceneFactory
{
public:
	virtual MyScene* createScene(int nType);
};

#endif
