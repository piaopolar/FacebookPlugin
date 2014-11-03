//
//  ObserverPattern.h
//
//  Created by Ruoqian, Chen on 2013/06/18
//
#ifndef __OBSERVER_PATTERN_H__
#define __OBSERVER_PATTERN_H__

class Observer
{
public:
	virtual void OnNotify(int nEvent, int nParam) = 0;
};

class Subject
{
public:
	void AddObserver(Observer* pObserver);
	void DelObserver(Observer* pObserver);
	void NotifyObserver(int nEvent = 0, int nParam = 0);
private:
	std::vector<Observer* > m_vecObserver;
};

#endif
