//
//  BaseDef.h
//
//  Created by Ruoqian, Chen on 2014/09/24
//

#ifndef __BASE_DEF_H__
#define __BASE_DEF_H__

#include <stack>
#include <vector>

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

const int ANCHOR_TYPE_CENTER = 5;
const int ANCHOR_TYPE_DEFAULT = 5;
const CCPoint ANCHOR_LEFT_TOP(0, 1.0f);
const CCPoint ANCHOR_TOP(0.5, 1.0);
const CCPoint ANCHOR_RIGHT_TOP(1, 1);
const CCPoint ANCHOR_LEFT(0, 0.5);
const CCPoint ANCHOR_CENTER(0.5f, 0.5f);
const CCPoint ANCHOR_RIGHT(1, 0.5);
const CCPoint ANCHOR_LEFT_BOTTOM(0, 0);
const CCPoint ANCHOR_BOTTOM(0.5, 0);
const CCPoint ANCHOR_RIGHT_BOTTOM(1, 0);
const CCPoint ANCHOR_POINT_GROUP[10] = { CCPointZero, ANCHOR_LEFT_TOP, ANCHOR_TOP, ANCHOR_RIGHT_TOP, ANCHOR_LEFT, ANCHOR_CENTER, ANCHOR_RIGHT, ANCHOR_LEFT_BOTTOM, ANCHOR_BOTTOM, ANCHOR_RIGHT_BOTTOM };
const CCPoint ANCHOR_DEFAULT = ANCHOR_POINT_GROUP[ANCHOR_TYPE_DEFAULT];

const int MAX_STRING = 1024;

#define FLOAT_MAX_VALUE  3.402823466E38f

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

#ifdef small
#undef small
#endif


#else

#define OutputDebugString printf
typedef unsigned int       DWORD;
#ifndef INT_MAX
#define INT_MAX       2147483647    /* maximum (signed) int value */
#endif
#ifndef UINT_MAX
#define UINT_MAX      0xffffffff    /* maximum unsigned int value */
#endif

#ifndef __int64
#define __int64       long long
#endif

#ifndef OUT
#define OUT
#endif

#ifndef OBJC_BOOL_DEFINED
typedef int BOOL;
#endif

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif
#endif

#endif
