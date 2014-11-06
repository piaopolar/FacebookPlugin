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
const char* FBJavaFriendClassName = "org/FBPlugin/FacebookPickFriendPlugin";
const char* FBJavaLoginClassName = "org/FBPlugin/FacebookConnectPlugin";
const char* FBJavaPostClassName = "org/FBPlugin/FacebookPostPlugin";
const char* FBJavaSendRequestsClassName = "org/FBPlugin/FacebookSendRequestsPlugin";
const char* FBJavaClassName = "org/FBPlugin/FacebookConnectPlugin";
extern "C"
{
    void Java_org_FBPlugin_FacebookConnectPlugin_nativeCallback(JNIEnv* jEnv, jobject jObj, jint cbIndex, jstring params)
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
                                                , FBJavaLoginClassName
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

const char* FacebookInterface::getStatus(void)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)	
	cocos2d::JniMethodInfo t;
	if (cocos2d::JniHelper::getStaticMethodInfo(t
                                                , FBJavaClassName
                                                , "getStatus"
                                                , "(I)Ljava/lang/String;"))
	{
		jstring ret = (jstring)(t.env->CallStaticObjectMethod(t.classID, t.methodID, FBACTION_GET_STATUS));
        t.env->DeleteLocalRef(t.classID);
        const char* aStr= t.env->GetStringUTFChars(ret, NULL);
        return aStr;
	}
#endif
	return "";
}

void FacebookInterface::pickFriend(void)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	cocos2d::JniMethodInfo t;
	if (cocos2d::JniHelper::getStaticMethodInfo(t
		, FBJavaFriendClassName
		, "pickFriend"
        , "(I)V"))
	{
		t.env->CallStaticVoidMethod(t.classID, t.methodID, FBACTION_PICK_FRIEND);
		t.env->DeleteLocalRef(t.classID);
	}
#endif
}

void FacebookInterface::postStatus(const char* name,const char* caption,const char* description,const char* link,const char* picture)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	cocos2d::JniMethodInfo t;
	if (cocos2d::JniHelper::getStaticMethodInfo(t
		, FBJavaPostClassName
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
		, FBJavaSendRequestsClassName
		, "sendRequests"
		, "(I)V"))
	{
		t.env->CallStaticVoidMethod(t.classID, t.methodID, FBACTION_SEND_REQUESTS);
		t.env->DeleteLocalRef(t.classID);
	}
#endif
}

