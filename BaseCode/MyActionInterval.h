//
//  MyActionInterval.h
//
//  Created by Ruoqian, Chen on 2013/09/11
//

#ifndef __MY_ACTION_INTERVAL_H__
#define __MY_ACTION_INTERVAL_H__

#include "BaseDef.h"
#include "actions/CCActionInterval.h"

class CEmptyAction : public CCFiniteTimeAction
{
public:
    static CEmptyAction* create();
    virtual CCObject* copyWithZone(CCZone* pZone);
    virtual CEmptyAction* reverse(void);
    virtual void update(float time);
    virtual void step(float dt);
};

class CConTraceLn : public CCActionInstant
{
public:
    CConTraceLn(const char *pszInfo);
    virtual ~CConTraceLn(){}
    virtual void update(float time);
    virtual CCObject* copyWithZone(CCZone *pZone);
public:
    static CConTraceLn * create(const char *pszFormat, ...);
protected:
	std::string m_strInfo;
};

class CElasticActionBy : public CCActionInterval
{
public:
    /** initializes the action */
    bool initWithDuration(float duration, const CCPoint& posDelta, unsigned int nTimes);
	
    virtual CCObject* copyWithZone(CCZone* pZone);
    virtual void startWithTarget(CCNode *pTarget);
    virtual void update(float time);
    virtual CCActionInterval* reverse(void);
	
public:
    /** creates the action */
    static CElasticActionBy* create(float duration, const CCPoint& posDelta, unsigned int nTimes = 1);
protected:
    CCPoint         m_startPosition;
    CCPoint         m_delta;
    unsigned int    m_nTimes;
    CCPoint         m_previousPos;
};

class CParabolicActionTo : public CCActionInterval
{
public:
    bool initWithDuration(float duration, const CCPoint& rPosTo, const CCPoint& rPosVertex);
	
    virtual void startWithTarget(CCNode *pTarget);
    virtual CCObject* copyWithZone(CCZone* pZone);
    virtual void update(float fTime);

public:
    static CParabolicActionTo* create(float duration, const CCPoint& rPosTo, const CCPoint& rPosVertex); 
protected:
	CCPoint m_ptSrc;
	CCPoint m_ptTo;
	CCPoint m_ptVertex;
	CCPoint m_ptSpeed;
	CCPoint m_ptAccSpeed;
};

class CMyProgressTo : public CCProgressTo
{
public:
    virtual CCObject* copyWithZone(CCZone *pZone);
    virtual void startWithTarget(CCNode *pTarget);
public:
    /** Creates and initializes with a duration and a percent */
    static CMyProgressTo* create(float duration, float fPercent);
};

#endif
