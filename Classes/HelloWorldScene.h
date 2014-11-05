#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "MyLayer.h"
#include "MyScene.h"
#include "SceneFactoryImpl.h"

class HelloWorld : public MyLayer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);

	void OnBtnLogin(CCObject* pObj);
	void OnBtnLogout(CCObject* pObj);
	void OnBtnSendRequest(CCObject* pObj);
	void OnBtnShareScreenShot(CCObject* pObj);
	void OnBtnReqMyselfInfo(CCObject* pObj);
	void OnBtnReqFriendInfo(CCObject* pObj);
    
    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);

public:
	CCMenuItem* m_pBtnLogin;
	CCMenuItem* m_pBtnLogout;
};

class HelloWorldScene : public MyScene
{
public:
	FUN_GET_SCENE_TYPE(HelloWorldScene);
	virtual bool OnInit(void);
};

#endif // __HELLOWORLD_SCENE_H__
