//
//  FacebookMgr.h
//
//  Created by Ruoqian, Chen on 2014/10/29
//

#ifndef  __FACEBOOK_MGR_H__
#define  __FACEBOOK_MGR_H__

#include <map>
#include "cocos2d.h"
#include "ObserverPattern.h"

enum
{
	FBACTION_LOGIN,
	FBACTION_LOGIN_TO_SHARE,
	FBACTION_LOGOUT,
	FBACTION_GET_STATUS,
	FBACTION_PICK_FRIEND,
	FBACTION_POST,
	FBACTION_SEND_REQUESTS,
	FBACTION_REQ_MYINFO,
	FBACTION_REQ_FRIENDINFO,
	FBACTION_IMAGE_UPDATE,
};

struct FB_USERINFO
{
	FB_USERINFO();
	long long m_i64Id;
	std::string m_strName;
};

namespace cocos2d {
	namespace extension {
		class CCHttpClient;
		class CCHttpResponse;
	}
}

class FacebookMgr : public Subject, public cocos2d::CCObject
{
public:
	static FacebookMgr *GetInstance(void);
	static void Release(void);

	void OnGetResponse(int nIndex, const char *pszStr);

	void Login(void);
	void Logout(void);
	bool IsLogin(void) const;

	void TryShareScreenShot(void);
	void TrySendRequest(void);
	
	void InviteFriends(const char *pszAppLink, const char *pszImgUrl);
	void RequestMyselfInfo(void);
	void RequestFriendInfo(void);
	void CommonRequest(int nIndex);

	void DownloadImage(void);
	void OnDownloadCallback(cocos2d::extension::CCHttpClient *sender, cocos2d::extension::CCHttpResponse *response);

	std::vector<FB_USERINFO> GetFriendsInfo(void) const;
	const FB_USERINFO& GetMyselfInfo(void) const;
private:
	void ShareScreenShot(void);
	void SendRequest(void);
	void ProcessJsonResponse(int nIndex, const char *pszStr);
private:
	std::string m_strScreenShotPath;
	std::vector<FB_USERINFO> m_vecFriends;
	FB_USERINFO m_infoMyself;
};
#endif  //__INTERFACE_FACEBOOK_H__
