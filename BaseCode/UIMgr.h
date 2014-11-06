//
//  UIMgr.h
//
//  Created by Ruoqian, Chen on 2013/06/14
//
#ifndef __UI_MGR_H__
#define __UI_MGR_H__

#include "BaseDef.h"

class ISceneFactory;
class MyScrollView;
class MyLayer;
class MyCCOjbectImpl;
class MyDrawNode;
class MyFrameAnimationNode;
class MyMenu;

NS_CC_EXT_BEGIN
class CCControlSlider;
class MyEditBox;
NS_CC_EXT_END


#include "MyMenuItem.h"

USING_NS_CC;

#define CONFIG_POS

#define UI_TRACE_ENABLE 0
#if UI_TRACE_ENABLE
#define UI_TRACE(Format, ...) MyTrace(Format, ##__VA_ARGS__, "No Log")
#else
#define UI_TRACE(...)
#endif

enum
{
NODETYPE_NONE,
NODETYPE_SPRITE,
NODETYPE_TEXTLABEL,
NODETYPE_MENU,
NODETYPE_MENUITEM,
NODETYPE_DRAWNODE,
NODETYPE_GROUP,
NODETYPE_EDIT,
NODETYPE_MULTIPLE,
NODETYPE_SCROLLVIEW,
NODETYPE_PROGRESS,
NODETYPE_ANIMATION,
NODETYPE_FLASHACTOR,
NODETYPE_TABLE_VIEW,
NODETYPE_DICT_IMAGE,
NODETYPE_PARAM,
NODETYPE_PARTICLE_SYSTEM_QUAD,
NODETYPE_SLIDER,
};


enum eTransitionType
{
TRANSITION_TYPE_NONE,
TRANSITION_TYPE_SLIDEINL,
TRANSITION_TYPE_SLIDEINR,
TRANSITION_TYPE_JUMPZOOM,
TRANSITION_TYPE_ROTOZOOM,
TRANSITION_TYPE_SPLITCOLS,
TRANSITION_TYPE_FADE,
};

enum
{
	TOP_LAYER_GROUP_EDITOR = 1,

	TOP_LAYER_GROUP_MASK_ALL = 0xffff,
	TOP_LAYER_GROUP_MASK_NONE = 0x0,
};

enum
{
	ACTION_TAG_ANIMATION,
};

enum
{
	MULTIRESOLUTION_POLICY_LACE,
	MULTIRESOLUTION_POLICY_BASEPOINT,
};

class QMyGroupNode : public CCNode
{
public:
	virtual void addChild(CCNode *child);
	QMyMenuItemSprite* CreateMenuItem(void);
};

class UIMgr
{
public:
	static UIMgr* GetInstance(void);
	static void Release(void);

	void AddTopLayer(MyLayer* pLayer, int nGroup);
	std::vector<MyLayer* > GetTopLayers(void) const;
	int GetTopLayerGroup(MyLayer* pLayer) const;

	void AddModuleLayer(MyLayer* pLayer);
	void DelModuleLayer(MyLayer* pLayer);
	bool IsModuleLayerExist(void) const;

	void RemoveUnusedRes(float fDelay = -1);
	void AddTexturePackDir(const char* pszKey);
	void LoadTexturePacks(void);

	int GetConfigValue(const char *pszConfig, const char *pszKey, int nDefaultValue);
	const char* GetConfigValue(const char *pszConfig, const char *pszKey, const char *pszDefault);
	CCNode* CreateConfigNode(const char *pszConfig, int *pType = NULL, CCObject *pTarget = NULL, SEL_MenuHandler selector = NULL);
	CCNode* CreateConfigNodeWithChlid(const char *pszConfig, const char *pszChildConfig);
	MyDrawNode* CreateConfigDrawNode(const char *pszConfig);
	CCSprite* CreateConfigSprite(const char *pszConfig, const char *pszValue = NULL);
	CCLabelTTF* CreateConfigTextLabel(const char *pszConfig, const char *pszValue = NULL, int nWidth = -1);
	cocos2d::extension::MyEditBox* CreateConfigEditBox(const char *pszEditBox);
	MyScrollView* CreateConfigScrollView(const char *pszConfig);
	CCMenuItemSprite* CreateConfigMenuItem(const char *pszConfig, CCObject *pTarget = NULL, SEL_MenuHandler selector = NULL);
	QMyGroupNode* CreateConfigGroupNode(const char *pszConfig);
	CCNode* CreateConfigMultipleNode(const char *pszConfig, const char *pszModel = NULL);
	CCProgressTimer* CreateConfigProgress(const char *pszConfig);
	MyFrameAnimationNode* CreateConfigAnimation(const char *pszConfig, float fDelay = 0);
	CCSprite* CreateConfigDictImage(const char *pszConfig, const char *pszText = NULL);
	CCFiniteTimeAction* CreateConfigFiniteTimeAction(const char *pszConfig);
	CCParticleSystemQuad* CreateConfigParticleSystemQuad(const char *pszConfig);
	CCControlSlider* CreateConfigSlider(const char *pszConfig);
	
	std::vector<CCNode*> AddConfigChild(CCNode* pParent, const char *pszConfigChild);
	void AddConfigStateChild(CCMenuItemSprite* pBtn, const char *pszConfigChild, BOOL bbStaticBG = TRUE);
	CCNode* CreateConfigSingleChild(CCNode* pParent, const char *pszNoSepTagConfigChild);
	CCNode* CreateConfigSingleBranchChild(CCNode* pParent, const char *pszNoBrotherTagConfigChild);
	CCNode* AddConfigSingleBranchChild(CCNode* pParent, const char *pszNoBrotherTagConfigChild);

	void SetConfigDictImage(CCNode* pNode, const char *pszConfig, const char *pszText);
	void SetDictImage(CCNode* pSprite, const char *pszText, const char *pszDictImage, float fCharWitdh, const char *pszDict, const CCPoint rPtAnchor);

	void ManageArgvs(CCNode* pNode, CCNode* pParent, const char *pszArgv);
	void ManageNode(CCNode* pNode, const char *pszConfig);
	void ManageMyMenu(MyMenu* pMyMenu, const char *pszConfig);
	void ManageMyLayer(MyLayer* pMyLayer, const char *pszConfig);
	void ManageCCLayer(CCLayer* pCCLayer, const char *pszConfig, int nDefaultTouchPriority = 0);
private:
	void ManageRotation(CCNode* pNode, const char *pszConfig);
	void ManagePos(CCNode* pNode, const char *pszConfig);
	void ManageSize(CCNode* pNode, const char *pszConfig);
	void ManageZOrder(CCNode* pNode, const char *pszConfig);
	void ManageAlpha(CCRGBAProtocol* pNode, const char *pszConfig);
	void ManageColor(CCNodeRGBA *pNode, const char *pszConfig);
	void ManageAddChild(CCNode* pNode, const char *pszConfig);
	void ManageNodeAndAddChild(CCNode* pNode, const char *pszConfig);

private:
	UIMgr(void);
	~UIMgr(void);

	MyCCOjbectImpl* m_pCCImpl;
	std::string m_strConfigFileName;
	std::vector<std::string> m_vecTexturePackDir;
	std::vector<MyLayer* > m_vecTopLayer;
	std::set<MyLayer*> m_setModuleLayer;
	std::map<MyLayer*, int> m_mapTopLayerGroupInfo;

#ifdef CONFIG_POS
private:
	std::map<int, std::string> m_mapObjSection;
	std::map<int, CCPoint> m_mapObjOffsetXY;
	std::map<int, CCPoint> m_mapAnchorPoint;
public:
	CCNode* FindConfigNode(CCNode* pNodeRoot, const char *pszConfig) const;
	const char* GetNodeConfig(int uId) const;

	bool SaveConfigValue(const char *pszConfig, const char *pszKey, int nValue);
	void ReloadPos(void);
#endif
	
	void OnNodePositionMove(CCNode* pNode);
	void NodePositionMove(CCNode* pNode, const CCPoint& rPtOffset);
	void NodeSetPos(CCNode* pNode, CCPoint ptPos);
	void NodeSetPos(CCNode* pNode, float fX, float fY);
	void NodeSetPosX(CCNode* pNode, float fX);
	void NodeSetPosY(CCNode* pNode, float fY);

	void NodeAnchorChange(CCNode* pNode, int nType);
	void NodeAnchorChange(CCNode* pNode, CCPoint pt);
	void OnNodeAnchorChange(CCNode* pNode);

// Multi Resolution
public:
	static void CalcuNewPos(const CCPoint& rPtOld, CCPoint& rPtNew, int nBasePoint, const CCSize& sizeConfig, const CCSize& sizeDevice);
	void CalcuNewPos(const CCPoint& rPtOld, CCPoint& rPtNew, int nBasePoint);
	void SetConfigSize(float fWidth, float fHeight);
	void SetDeviceSize(float fScrWidth, float fScrHeight);
	void SetMultiResolutionPolicy(int nType);
	void ManageNodeResolution(CCNode *pNode, int nBasePoint);
	
	int GetMultiResolutionPolicy(void) const;
	CCSize GetSizeConfig(void) const;
	CCSize GetWinSize(void) const;
private:
	int m_nMultiResolutionPolicy;
	CCSize m_sizeConfig;
	CCSize m_sizeDevice;

// Scene Factory
public:
	ISceneFactory* m_pSceneFactoryImpl;
	void SetSceneFactory(ISceneFactory* pSceneFactory);

// Scene Stack
public:
	void ResetSceneStack(void);
	void SetSceneInitData(int nInitData);
	void PopScene(void);
	void ReplaceScene(int nSceneType, eTransitionType nTransitionType = TRANSITION_TYPE_NONE, int nInitData = 0,bool bNeedPushLastScene = true);
	void RunWithScene(int nSceneType, int nInitData = 0);
    int  GetCurrentSceneType()const;
	bool IsCurrentSceneStable(void) const;
private:
	bool SwitchScene(int nSceneType, eTransitionType nTransitionType = TRANSITION_TYPE_NONE, int nInitData = 0);
private:
	struct SCENE_PARAM
	{
		int m_nSceneType;
		int m_nInitData;
		eTransitionType m_nTransitionType;
	};
	CCScene* m_pSceneLogic;
	SCENE_PARAM m_infoCurrentScene;
	std::stack<SCENE_PARAM> m_stackScene;
};

#define RUN_WITH_SCENE(className) UIMgr::GetInstance()->RunWithScene(className::GetSceneType())
#define RUN_WITH_SCENE2(className, nInitData) UIMgr::GetInstance()->RunWithScene(className::GetSceneType(), nInitData)
#define REPLACE_SCENE(className) UIMgr::GetInstance()->ReplaceScene(className::GetSceneType())
#define REPLACE_SCENE2(className, nTransitionType) UIMgr::GetInstance()->ReplaceScene(className::GetSceneType(), nTransitionType)
#define REPLACE_SCENE3(className, nTransitionType, nInitData) UIMgr::GetInstance()->ReplaceScene(className::GetSceneType(), nTransitionType, nInitData)
#define REPLACE_SCENE4(className) UIMgr::GetInstance()->ReplaceScene(className::GetSceneType(),TRANSITION_TYPE_NONE,0,false)

#endif
