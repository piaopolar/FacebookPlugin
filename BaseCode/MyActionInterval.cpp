//
//  MyActionInterval.cpp
//
//  Created by Ruoqian, Chen on 2013/09/11
//
#include "StdAfx.h"

#include "MyActionInterval.h"

CEmptyAction* CEmptyAction::create()
{
    CEmptyAction* pRet = new CEmptyAction();
    if (pRet)
    {
        pRet->autorelease();
    }
    return pRet;
}

CCObject* CEmptyAction::copyWithZone(CCZone* pZone)
{
    CC_UNUSED_PARAM(pZone);
    CEmptyAction* pRet = new CEmptyAction();
    return pRet;
}

CEmptyAction* CEmptyAction::reverse(void)
{
    return CEmptyAction::create();
}

void CEmptyAction::update(float time)
{
    CC_UNUSED_PARAM(time);
}

void CEmptyAction::step(float dt)
{
    CC_UNUSED_PARAM(dt);
}

CConTraceLn::CConTraceLn( const char *pszInfo )
{
	m_strInfo = pszInfo ? pszInfo : "";
}

CConTraceLn* CConTraceLn::create(const char *pszFormat, ...)
{
	char szExtraMsg[1024];
	va_list ap;
	va_start(ap, pszFormat);
	vsnprintf(szExtraMsg, sizeof(szExtraMsg), pszFormat, ap);
	va_end(ap);

	szExtraMsg[sizeof(szExtraMsg) - 1] = 0;

    CConTraceLn* pRet = new CConTraceLn(szExtraMsg);

    if (pRet) {
        pRet->autorelease();
    }

    return pRet;
}

void CConTraceLn::update(float time)
{
    CC_UNUSED_PARAM(time);
	std::string strTime = GetDebugTime();
	ConTraceLn("%s %s", strTime.c_str(), m_strInfo.c_str());
}

CCObject* CConTraceLn::copyWithZone(CCZone *pZone) {

    CCZone *pNewZone = NULL;
    CConTraceLn *pRet = NULL;
    if (pZone && pZone->m_pCopyObject) {
        pRet = (CConTraceLn*) (pZone->m_pCopyObject);
    } else {
        pRet = new CConTraceLn(m_strInfo.c_str());
        pZone = pNewZone = new CCZone(pRet);
    }

    CCActionInstant::copyWithZone(pZone);
    CC_SAFE_DELETE(pNewZone);
    return pRet;
}

bool CParabolicActionTo::initWithDuration( float duration, const CCPoint& rPosTo, const CCPoint& rPosVertex )
{
    if (CCActionInterval::initWithDuration(duration)) {
		m_ptTo = rPosTo;
		m_ptVertex = rPosVertex;
		return true;
	}
	
    return false;
}

void CParabolicActionTo::startWithTarget( CCNode *pTarget )
{
	CCActionInterval::startWithTarget(pTarget);
	m_ptSrc = pTarget->getPosition();
	m_ptSpeed.x = (m_ptTo.x - m_ptSrc.x) / m_fDuration;

	float fSimpleTime = m_fDuration / 4;
	m_ptAccSpeed.y = -(m_ptVertex.y - m_ptSrc.y) * 2 / (fSimpleTime * fSimpleTime);
	m_ptAccSpeed.x = 0;
	m_ptSpeed.y = (m_ptTo.y - m_ptSrc.y - m_ptAccSpeed.y * m_fDuration * m_fDuration / 2) / m_fDuration;
}

CCObject* CParabolicActionTo::copyWithZone( CCZone* pZone )
{
	CCZone* pNewZone = NULL;
	CParabolicActionTo* pCopy = NULL;

	if(pZone && pZone->m_pCopyObject) {
		pCopy = (CParabolicActionTo*)(pZone->m_pCopyObject);
    } else {
		pCopy = new CParabolicActionTo();
		pZone = pNewZone = new CCZone(pCopy);
	}
	
    CCActionInterval::copyWithZone(pZone);
	
    pCopy->initWithDuration(m_fDuration, m_ptTo, m_ptVertex);
    
    CC_SAFE_DELETE(pNewZone);
    return pCopy;
}

void CParabolicActionTo::update( float f )
{
	float fTime = f * m_fDuration;
	CCPoint ptPos = m_ptSrc + m_ptSpeed * fTime + m_ptAccSpeed / 2 * fTime * fTime;
	m_pTarget->setPosition(ptPos);

//	CCLOG("CParabolicActionTo::update %f %.2f %2f (%.2f, %.2f)", fTime, m_ptSpeed.y * fTime, m_ptAccSpeed.y / 2 * fTime * fTime, ptPos.x, ptPos.y);
}

CParabolicActionTo* CParabolicActionTo::create( float duration, const CCPoint& rPosTo, const CCPoint& rPosVertex )
{
	CParabolicActionTo* pAction = new CParabolicActionTo;
	pAction->initWithDuration(duration, rPosTo, rPosVertex);
	pAction->autorelease();

	return pAction;
}

CElasticActionBy* CElasticActionBy::create(float duration, const CCPoint& posDelta, unsigned int nTimes)
{
    CElasticActionBy *pAction = new CElasticActionBy();
    pAction->initWithDuration(duration, posDelta, nTimes);
    pAction->autorelease();
	
    return pAction;
}

bool CElasticActionBy::initWithDuration(float duration, const CCPoint& posDelta, unsigned int nTimes)
{
    if (CCActionInterval::initWithDuration(duration))
    {
        m_delta = posDelta;
        m_nTimes = nTimes;
		
        return true;
    }
	
    return false;
}

CCObject* CElasticActionBy::copyWithZone(CCZone *pZone)
{
    CCZone* pNewZone = NULL;
    CElasticActionBy* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject)
    {
        //in case of being called at sub class
        pCopy = (CElasticActionBy*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new CElasticActionBy();
        pZone = pNewZone = new CCZone(pCopy);
    }
	
    CCActionInterval::copyWithZone(pZone);
	
    pCopy->initWithDuration(m_fDuration, m_delta, m_nTimes);
    
    CC_SAFE_DELETE(pNewZone);
    return pCopy;
}

void CElasticActionBy::startWithTarget(CCNode *pTarget)
{
    CCActionInterval::startWithTarget(pTarget);
    m_previousPos = m_startPosition = pTarget->getPosition();
}

void CElasticActionBy::update(float t)
{
    if (m_pTarget)
    {
        float frac = fmodf( t * m_nTimes, 1.0f );
        float x = m_delta.x * 4 * frac * (1 - frac);
        float y = m_delta.y * 4 * frac * (1 - frac);
#if CC_ENABLE_STACKABLE_ACTIONS
        CCPoint currentPos = m_pTarget->getPosition();
		
        CCPoint diff = ccpSub( currentPos, m_previousPos );
        m_startPosition = ccpAdd( diff, m_startPosition);
		
        CCPoint newPos = ccpAdd( m_startPosition, ccp(x,y));
        m_pTarget->setPosition(newPos);
		
        m_previousPos = newPos;
#else
        m_pTarget->setPosition(ccpAdd( m_startPosition, ccp(x,y)));
#endif // !CC_ENABLE_STACKABLE_ACTIONS
    }
}

CMyProgressTo* CMyProgressTo::create(float duration, float fPercent)
{
    CMyProgressTo *pProgressTo = new CMyProgressTo();
    pProgressTo->initWithDuration(duration, fPercent);
    pProgressTo->autorelease();

    return pProgressTo;
}

CCObject* CMyProgressTo::copyWithZone(CCZone *pZone)
{
    CCZone* pNewZone = NULL;
    CMyProgressTo* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (CMyProgressTo*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new CMyProgressTo();
        pZone = pNewZone = new CCZone(pCopy);
    }

    CCActionInterval::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_fTo);

    CC_SAFE_DELETE(pNewZone);
    return pCopy;
}

void CMyProgressTo::startWithTarget(CCNode *pTarget)
{
    CCActionInterval::startWithTarget(pTarget);
    m_fFrom = ((CCProgressTimer*)(pTarget))->getPercentage();
}

CCActionInterval* CElasticActionBy::reverse(void)
{
    return CElasticActionBy::create(m_fDuration, ccp(-m_delta.x, -m_delta.y),
							m_nTimes);
}

