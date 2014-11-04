//
//  MyCCImpl.h
//
//  Created by Ruoqian, Chen on 2014/01/26
//

#ifndef __MY_CC_IMPL__
#define __MY_CC_IMPL__

class MyCCOjbectImpl : public CCObject
{
public:
	void RemoveUnusedRes(float fDelay);
private:
	void RemoveUnusedResImm(float f);
};

#endif
