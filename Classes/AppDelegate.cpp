#include "AppDelegate.h"
#include "UIMgr.h"
#include "HelloWorldScene.h"
#include "UIEditor.h"

USING_NS_CC;


AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    CCDirector* pDirector = CCDirector::sharedDirector();
    CCEGLView* pEGLView = CCEGLView::sharedOpenGLView();

    pDirector->setOpenGLView(pEGLView);
	
    // turn on display FPS
//  pDirector->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    pDirector->setAnimationInterval(1.0 / 60);

	UIMgr::GetInstance()->SetSceneFactory(CSceneFactory::GetInstance());
	UIMgr::GetInstance()->SetMultiResolutionPolicy(MULTIRESOLUTION_POLICY_BASEPOINT);

    // run
	RUN_WITH_SCENE(HelloWorldScene);
	
#if EDITOR_UI
	auto pLayer = EditorLayer::create();
	UIMgr::GetInstance()->AddTopLayer(pLayer, TOP_LAYER_GROUP_EDITOR);
#endif

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    CCDirector::sharedDirector()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    CCDirector::sharedDirector()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}
