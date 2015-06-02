//
//  FacebookInterface.cpp
//
//  Created by Ruoqian, Chen on 2014/10/17
//
//  forked from https://github.com/Ryeeeeee/FacebookTutorial

#include "FacebookInterface.h"
#include "FacebookMgr.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <android/log.h>
const char* FBJavaClassName = "org/Plugin/Facebook/FacebookPlugin";
extern "C"
{
    void Java_org_Plugin_Facebook_FacebookPlugin_nativeCallback(JNIEnv* jEnv, jobject jObj, jint cbIndex, jstring params)
    {
		const char *pszParams = params ? jEnv->GetStringUTFChars(params, 0) : NULL;
		FacebookMgr::GetInstance()->OnGetResponse(cbIndex, pszParams);
		if (pszParams) {
			jEnv->ReleaseStringUTFChars(params, pszParams);
		}
    }
};
#endif


void FacebookInterface::request( int cbIndex, const char*pszRequest, const char *pszPermission )
{	
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)	
	cocos2d::JniMethodInfo t;
	if (cocos2d::JniHelper::getStaticMethodInfo(t
                                                , FBJavaClassName
                                                , "request"
                                                , "(ILjava/lang/String;Ljava/lang/String;)V"))
	{
		jstring jstrRequest = t.env->NewStringUTF(pszRequest ? pszRequest : "");
		jstring jstrPermission = t.env->NewStringUTF(pszPermission ? pszPermission : "");

		t.env->CallStaticVoidMethod(t.classID, t.methodID, cbIndex, jstrRequest, jstrPermission);

		t.env->DeleteLocalRef(jstrRequest);
		t.env->DeleteLocalRef(jstrPermission);
		t.env->DeleteLocalRef(t.classID);
	}
#endif	
}

void FacebookInterface::login(int cbIndex, const char* scope)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	cocos2d::JniMethodInfo t;
	if (cocos2d::JniHelper::getStaticMethodInfo(t
                                                , FBJavaClassName
                                                , "login"
                                                , "(ILjava/lang/String;)V"))
	{
		if (scope)
		{
			jstring jeventId = t.env->NewStringUTF(scope);
			t.env->CallStaticVoidMethod(t.classID, t.methodID, cbIndex, jeventId);
			t.env->DeleteLocalRef(jeventId);
		}
		else
		{
			t.env->CallStaticVoidMethod(t.classID, t.methodID, cbIndex, NULL);
		}
		t.env->DeleteLocalRef(t.classID);
	}
#endif
}

void FacebookInterface::logout(void)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	cocos2d::JniMethodInfo t;
	if (cocos2d::JniHelper::getStaticMethodInfo(t
                                                , FBJavaClassName
                                                , "logout"
                                                , "(I)V"))
	{
		t.env->CallStaticVoidMethod(t.classID, t.methodID, FBACTION_LOGOUT);
		t.env->DeleteLocalRef(t.classID);
	}
#endif
}

bool FacebookInterface::isLogin( void )
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)	
	cocos2d::JniMethodInfo t;
	if (cocos2d::JniHelper::getStaticMethodInfo(t
                                                , FBJavaClassName
                                                , "isLogin"
                                                , "()Z"))
	{
		jboolean ret = (jboolean)(t.env->CallStaticBooleanMethod(t.classID, t.methodID));
        t.env->DeleteLocalRef(t.classID);
		return ret ? true : false;
	}
#endif
	return false;
}

void FacebookInterface::postStatus(const char* name,const char* caption,const char* description,const char* link,const char* picture)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	cocos2d::JniMethodInfo t;
	if (cocos2d::JniHelper::getStaticMethodInfo(t
		, FBJavaClassName
		, "postStatus"
		, "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"))
	{
		if (name && caption && description && link && picture)
		{
			jstring nameId = t.env->NewStringUTF(name);
			jstring captionId = t.env->NewStringUTF(caption);
			jstring descriptionId = t.env->NewStringUTF(description);
			jstring linkId = t.env->NewStringUTF(link);
			jstring pictureId = t.env->NewStringUTF(picture);

			t.env->CallStaticVoidMethod(t.classID, t.methodID, FBACTION_POST,nameId,captionId,descriptionId,linkId,pictureId);
			t.env->DeleteLocalRef(nameId);
			t.env->DeleteLocalRef(captionId);
			t.env->DeleteLocalRef(descriptionId);
			t.env->DeleteLocalRef(linkId);
			t.env->DeleteLocalRef(pictureId);
		}
		else
		{
			t.env->CallStaticVoidMethod(t.classID, t.methodID, FBACTION_POST, NULL);
		}
		
		t.env->DeleteLocalRef(t.classID);
	}
#endif
}

void FacebookInterface::sendRequests(void)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	cocos2d::JniMethodInfo t;
	if (cocos2d::JniHelper::getStaticMethodInfo(t
		, FBJavaClassName
		, "gameRequests"
		, "(I)V"))
	{
		t.env->CallStaticVoidMethod(t.classID, t.methodID, FBACTION_SEND_REQUESTS);
		t.env->DeleteLocalRef(t.classID);
	}
#endif
}

void FacebookInterface::inviteFriends(const char *pszAppLink, const char *pszImgUrl)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	cocos2d::JniMethodInfo t;
	if (cocos2d::JniHelper::getStaticMethodInfo(t
		, FBJavaClassName
		, "inviteFriends"
		, "(Ljava/lang/String;Ljava/lang/String;)V"))
	{
		jstring jstrAppLink = t.env->NewStringUTF(pszAppLink);
		jstring jstrImgUrl = t.env->NewStringUTF(pszImgUrl);
		t.env->CallStaticVoidMethod(t.classID, t.methodID, jstrAppLink, jstrImgUrl);
		t.env->DeleteLocalRef(jstrImgUrl);
		t.env->DeleteLocalRef(jstrAppLink);
		t.env->DeleteLocalRef(t.classID);
	}
#endif
}


