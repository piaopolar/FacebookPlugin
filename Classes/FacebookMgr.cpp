//
//  FacebookMgr.cpp
//
//  Created by Ruoqian, Chen on 2014/10/29
//

#include "network/HttpClient.h"
#include "network/HttpRequest.h"
#include "network/HttpResponse.h"

#include "BaseFunc.h"
#include "FacebookInterface.h"
#include "FacebookMgr.h"
#include "spine/Json.h"

USING_NS_CC;
USING_NS_CC_EXT;

FB_USERINFO::FB_USERINFO() : m_i64Id(0)
{
}

namespace
{
static FacebookMgr* s_pInstance = NULL;
}

FacebookMgr * FacebookMgr::GetInstance( void )
{
	if (NULL == s_pInstance) {
		s_pInstance = new FacebookMgr;
	}

	return s_pInstance;
}

void FacebookMgr::Release( void )
{
	CC_SAFE_DELETE(s_pInstance);
}

void FacebookMgr::OnGetResponse( int nIndex, const char *pszStr )
{
	CCLog("FacebookMgr::OnGetResponse %d [%s]", nIndex, pszStr ? pszStr : "NULL");
	bool bLogin = FacebookInterface::isLogin();

	switch (nIndex)
	{
	case FBACTION_REQ_MYINFO:
	case FBACTION_REQ_FRIENDINFO:
		this->ProcessJsonResponse(nIndex, pszStr);
		break;
	case FBACTION_LOGIN_TO_SHARE:
		if (this->IsLogin()) {
			this->ShareScreenShot();
		}
		break;
	default:
		break;
	}

	this->NotifyObserver(nIndex);
}

void FacebookMgr::ProcessJsonResponse( int nIndex, const char *pszStr )
{
	Json* pJsonValue = Json_create(pszStr);

	switch (nIndex)
	{
	case FBACTION_REQ_MYINFO:
		{
			const char *pszId = Json_getString(pJsonValue, "id", "0");
			
			FB_USERINFO info;
			sscanf(pszId, "%lld", &m_infoMyself.m_i64Id);
			m_infoMyself.m_strName = Json_getString(pJsonValue, "name", "");
		}
		break;
	case FBACTION_REQ_FRIENDINFO:
		{
			auto pJsonData = Json_getItem(pJsonValue, "data");
			m_vecFriends.clear();

			int nSize = Json_getSize(pJsonData);
			for (int i = 0; i < nSize; ++i) {
				auto pJsonEach = Json_getItemAt(pJsonData, i);

				const char *pszId = Json_getString(pJsonEach, "id", "0");

				FB_USERINFO info;
				sscanf(pszId, "%lld", &info.m_i64Id);
				info.m_strName = Json_getString(pJsonEach, "name", "");
				m_vecFriends.push_back(info);
			}
		}
		break;
	default:
		break;
	}

	Json_dispose(pJsonValue);

	if (FBACTION_REQ_FRIENDINFO == nIndex) {
		this->DownloadImage();
	}
}

std::vector<FB_USERINFO> FacebookMgr::GetFriendsInfo( void ) const
{
	return m_vecFriends;
}

const FB_USERINFO& FacebookMgr::GetMyselfInfo( void ) const
{
	return m_infoMyself;
}

void FacebookMgr::CommonRequest( int nIndex )
{
	const char *pszRequest = NULL;
	const char *pszPermission = NULL;
	switch (nIndex)
	{
	case FBACTION_REQ_FRIENDINFO:
		pszRequest = "/me/friends";
		pszPermission = "user_friends";
		break;
	case FBACTION_REQ_MYINFO:
		pszRequest = "/me/";
		break;
	default:
		break;
	}

	FacebookInterface::request(nIndex, pszRequest, pszPermission);
}

void FacebookMgr::RequestMyselfInfo( void )
{
	FacebookMgr::CommonRequest(FBACTION_REQ_MYINFO);
}

void FacebookMgr::RequestFriendInfo( void )
{
	FacebookMgr::CommonRequest(FBACTION_REQ_FRIENDINFO);
}

void FacebookMgr::DownloadImage( void )
{
	CCLog("FacebookMgr::DownloadImage FriendSize: %d", m_vecFriends.size());
	for (auto info : m_vecFriends) {
		auto i64id = info.m_i64Id;
		auto* pStrFileName = CCString::createWithFormat("%lld.jpg", i64id);
		CCLog("FacebookMgr::DownloadImage try file: [%s]", pStrFileName->getCString());

		std::string strPath = CCFileUtils::sharedFileUtils()->getWritablePath();
		strPath += pStrFileName->getCString();
		if (CCFileUtils::sharedFileUtils()->isFileExist(strPath.c_str())) {
			continue;
		}

		auto* pStrUrl = CCString::createWithFormat("https://graph.facebook.com/%lld/picture?type=square", i64id);
		CCLog("FacebookMgr::DownloadImage download file: [%s] from [%s]", pStrFileName->getCString(), pStrUrl->getCString());

		CCHttpRequest* request = new CCHttpRequest();
		request->setUrl(pStrUrl->getCString());
		request->setRequestType(CCHttpRequest::kHttpGet);
		request->setResponseCallback(this, httpresponse_selector(FacebookMgr::OnDownloadCallback));

		request->setTag(pStrFileName->getCString());
		CCHttpClient::getInstance()->send(request);
		request->release();
	}

	this->NotifyObserver(FBACTION_IMAGE_UPDATE);
}

void FacebookMgr::OnDownloadCallback(CCHttpClient *sender, CCHttpResponse *response)
{
	if (NULL == response) {
		CCLog("FacebookMgr::OnDownloadCallback response NULL");
		return;
	}
    
	int statusCode = response->getResponseCode();
    
	if (!response->isSucceed()) {
		CCLog("FacebookMgr::OnDownloadCallback Failed [%s]", response->getErrorBuffer());
		return;
	}
    
	auto *vecBuf = response->getResponseData();
	std::string strPath = CCFileUtils::sharedFileUtils()->getWritablePath();
	std::string strBuf(vecBuf->begin(), vecBuf->end());
	strPath += response->getHttpRequest()->getTag();

	FILE *fp = fopen(strPath.c_str(), "wb+");
	fwrite(strBuf.c_str(), 1, vecBuf->size(), fp);
	fclose(fp);

	FILE *fpTest = fopen(strPath.c_str(), "r");
	int nSize = 0;
	if (fpTest) {
		fseek(fpTest, 0, SEEK_END);
		nSize = ftell(fpTest);
		fclose(fpTest);
	}

	CCLog("FacebookMgr::OnDownloadCallback %x [%s] Size %d %d", fpTest, strPath.c_str(), vecBuf->size(), nSize);
	this->NotifyObserver(FBACTION_IMAGE_UPDATE);
}

void FacebookMgr::TryShareScreenShot( void )
{
	const char *IMAGE_FILE = "screenshot.jpg";
	if (!ScreenShot(IMAGE_FILE)) {
		return;
	}

	std::string strFullPath = CCFileUtils::sharedFileUtils()->getWritablePath() + IMAGE_FILE;
	std::string strUrl = "file://";
	strUrl += strFullPath;
	strUrl = strFullPath;
	m_strScreenShotPath = strFullPath;

	if (FacebookInterface::isLogin()) {
		this->ShareScreenShot();
	} else {
		FacebookInterface::login(FBACTION_LOGIN_TO_SHARE);
	}
}

void FacebookMgr::ShareScreenShot( void )
{	
	CCLog("FacebookMgr::ShareScreenShot %s", m_strScreenShotPath.c_str());
	FacebookInterface::postStatus("Name", "Caption", "Desc", "www.yile.com", m_strScreenShotPath.c_str());
}

void FacebookMgr::InviteFriends(const char *pszAppLink, const char *pszImgUrl)
{
	FacebookInterface::inviteFriends(pszAppLink, pszImgUrl);
}

void FacebookMgr::TrySendRequest( void )
{
	this->SendRequest();
}

void FacebookMgr::SendRequest( void )
{
	FacebookInterface::sendRequests();
}

void FacebookMgr::Login( void )
{
	FacebookInterface::login(FBACTION_LOGIN);
}

void FacebookMgr::Logout( void )
{
	FacebookInterface::logout();
}

bool FacebookMgr::IsLogin( void ) const
{
	return FacebookInterface::isLogin();
}
