//
//  ObserverPattern.cpp
//
//  Created by Ruoqian, Chen on 2013/06/18
//
#include "StdAfx.h"
#include "ObserverPattern.h"

#include <algorithm>
#include <functional>

void Subject::AddObserver( Observer* pObserver )
{
	m_vecObserver.push_back(pObserver);
}

void Subject::DelObserver( Observer* pObserver )
{
	std::vector<Observer*>::iterator it(std::find(m_vecObserver.begin(), m_vecObserver.end(), pObserver));
	if (it != m_vecObserver.end()) {
		m_vecObserver.erase(it);
	}
}

void Subject::NotifyObserver(int nEvent, int nParam)
{
//	for_each(m_vecObserver.begin(), m_vecObserver.end(), std::bind2nd(std::mem_fun(&Observer::OnNotify), nEvent));
	std::vector<Observer*>::iterator it(m_vecObserver.begin());
	for (; it != m_vecObserver.end(); ++it) {
		Observer* pObserver = *it;
		if (NULL == pObserver) {
			continue;
		}

		pObserver->OnNotify(nEvent, nParam);
	}
}
