/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2012 James Chen
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#include "StdAfx.h"

#include "MyEditBox.h"
#include "MyEditBoxImpl.h"

MyEditBox::MyEditBox(void)
: m_pEditBoxImpl(NULL)
, m_pDelegate(NULL)
, m_eEditBoxInputMode(kEditBoxInputModeSingleLine)
, m_eEditBoxInputFlag(kEditBoxInputFlagInitialCapsAllCharacters)
, m_eKeyboardReturnType(kKeyboardReturnTypeDefault)
, m_nFontSize(-1)
, m_nPlaceholderFontSize(-1)
, m_colText(ccWHITE)
, m_colPlaceHolder(ccGRAY)
, m_nMaxLength(0)
, m_fAdjustHeight(0.0f)
, m_nScriptEditBoxHandler(0)
{
}

MyEditBox::~MyEditBox(void)
{
    CC_SAFE_DELETE(m_pEditBoxImpl);
    unregisterScriptEditBoxHandler();
}


void MyEditBox::touchDownAction(CCObject *sender, CCControlEvent controlEvent)
{
    m_pEditBoxImpl->openKeyboard();
}

MyEditBox* MyEditBox::create(const CCSize& size, CCScale9Sprite* pNormal9SpriteBg, CCScale9Sprite* pPressed9SpriteBg/* = NULL*/, CCScale9Sprite* pDisabled9SpriteBg/* = NULL*/)
{
    MyEditBox* pRet = new MyEditBox();
    
    if (pRet != NULL && pRet->initWithSizeAndBackgroundSprite(size, pNormal9SpriteBg))
    {
        if (pPressed9SpriteBg != NULL)
        {
            pRet->setBackgroundSpriteForState(pPressed9SpriteBg, CCControlStateHighlighted);
        }
        
        if (pDisabled9SpriteBg != NULL)
        {
            pRet->setBackgroundSpriteForState(pDisabled9SpriteBg, CCControlStateDisabled);
        }
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
    
    return pRet;
}

bool MyEditBox::initWithSizeAndBackgroundSprite(const CCSize& size, CCScale9Sprite* pPressed9SpriteBg)
{
    if (CCControlButton::initWithBackgroundSprite(pPressed9SpriteBg))
    {
        m_pEditBoxImpl = __createSystemEditBox(this);
        m_pEditBoxImpl->initWithSize(size);
        
        this->setZoomOnTouchDown(false);
        this->setPreferredSize(size);
        this->setPosition(ccp(0, 0));
        this->addTargetWithActionForControlEvent(this, cccontrol_selector(MyEditBox::touchDownAction), CCControlEventTouchUpInside);
        
        return true;
    }
    return false;
}

void MyEditBox::setDelegate(MyEditBoxDelegate* pDelegate)
{
    m_pDelegate = pDelegate;
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->setDelegate(pDelegate);
    }
}

MyEditBoxDelegate* MyEditBox::getDelegate()
{
    return m_pDelegate;
}

void MyEditBox::setText(const char* pText)
{
    if (pText != NULL)
    {
        m_strText = pText;
        if (m_pEditBoxImpl != NULL)
        {
            m_pEditBoxImpl->setText(pText);
        }
    }
}

const char* MyEditBox::getText(void)
{
    if (m_pEditBoxImpl != NULL)
    {
		const char* pText = m_pEditBoxImpl->getText();
		if(pText != NULL)
			return pText;
    }
    
    return "";
}

void MyEditBox::setFont(const char* pFontName, int fontSize)
{
    m_strFontName = pFontName;
    m_nFontSize = fontSize;
    if (pFontName != NULL)
    {
        if (m_pEditBoxImpl != NULL)
        {
            m_pEditBoxImpl->setFont(pFontName, fontSize);
        }
    }
}

void MyEditBox::setFontName(const char* pFontName)
{
    m_strFontName = pFontName;
    if (m_pEditBoxImpl != NULL && m_nFontSize != -1)
    {
        m_pEditBoxImpl->setFont(pFontName, m_nFontSize);
    }
}

void MyEditBox::setFontSize(int fontSize)
{
    m_nFontSize = fontSize;
    if (m_pEditBoxImpl != NULL && m_strFontName.length() > 0)
    {
        m_pEditBoxImpl->setFont(m_strFontName.c_str(), m_nFontSize);
    }
}

void MyEditBox::setFontColor(const ccColor3B& color)
{
    m_colText = color;
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->setFontColor(color);
    }
}

void MyEditBox::setPlaceholderFont(const char* pFontName, int fontSize)
{
    m_strPlaceholderFontName = pFontName;
    m_nPlaceholderFontSize = fontSize;
    if (pFontName != NULL)
    {
        if (m_pEditBoxImpl != NULL)
        {
            m_pEditBoxImpl->setPlaceholderFont(pFontName, fontSize);
        }
    }
}

void MyEditBox::setPlaceholderFontName(const char* pFontName)
{
    m_strPlaceholderFontName = pFontName;
    if (m_pEditBoxImpl != NULL && m_nPlaceholderFontSize != -1)
    {
        m_pEditBoxImpl->setPlaceholderFont(pFontName, m_nFontSize);
    }
}

void MyEditBox::setPlaceholderFontSize(int fontSize)
{
    m_nPlaceholderFontSize = fontSize;
    if (m_pEditBoxImpl != NULL && m_strPlaceholderFontName.length() > 0)
    {
        m_pEditBoxImpl->setPlaceholderFont(m_strPlaceholderFontName.c_str(), m_nFontSize);
    }
}

void MyEditBox::setPlaceholderFontColor(const ccColor3B& color)
{
    m_colText = color;
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->setPlaceholderFontColor(color);
    }
}

void MyEditBox::setPlaceHolder(const char* pText)
{
    if (pText != NULL)
    {
        m_strPlaceHolder = pText;
        if (m_pEditBoxImpl != NULL)
        {
            m_pEditBoxImpl->setPlaceHolder(pText);
        }
    }
}

const char* MyEditBox::getPlaceHolder(void)
{
    return m_strPlaceHolder.c_str();
}

void MyEditBox::setInputMode(EditBoxInputMode inputMode)
{
    m_eEditBoxInputMode = inputMode;
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->setInputMode(inputMode);
    }
}

void MyEditBox::setMaxLength(int maxLength)
{
    m_nMaxLength = maxLength;
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->setMaxLength(maxLength);
    }
}


int MyEditBox::getMaxLength()
{
    return m_nMaxLength;
}

void MyEditBox::setInputFlag(EditBoxInputFlag inputFlag)
{
    m_eEditBoxInputFlag = inputFlag;
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->setInputFlag(inputFlag);
    }
}

void MyEditBox::setReturnType(KeyboardReturnType returnType)
{
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->setReturnType(returnType);
    }
}

/* override function */
void MyEditBox::setPosition(const CCPoint& pos)
{
    CCControlButton::setPosition(pos);
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->setPosition(pos);
    }
}

void MyEditBox::setVisible(bool visible)
{
    CCControlButton::setVisible(visible);
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->setVisible(visible);
    }
}

void MyEditBox::setContentSize(const CCSize& size)
{
    CCControlButton::setContentSize(size);
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->setContentSize(size);
    }
}

void MyEditBox::setAnchorPoint(const CCPoint& anchorPoint)
{
    CCControlButton::setAnchorPoint(anchorPoint);
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->setAnchorPoint(anchorPoint);
    }
}

void MyEditBox::visit(void)
{
    CCControlButton::visit();
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->visit();
    }
}

void MyEditBox::onEnter(void)
{
    CCControlButton::onEnter();
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->onEnter();
    }
}

void MyEditBox::onExit(void)
{
    CCControlButton::onExit();
    if (m_pEditBoxImpl != NULL)
    {
        // remove system edit control
        m_pEditBoxImpl->closeKeyboard();
    }
}

static CCRect getRect(CCNode * pNode)
{
	CCSize contentSize = pNode->getContentSize();
	CCRect rect = CCRectMake(0, 0, contentSize.width, contentSize.height);
	return CCRectApplyAffineTransform(rect, pNode->nodeToWorldTransform());
}

void MyEditBox::keyboardWillShow(CCIMEKeyboardNotificationInfo& info)
{
    // CCLOG("MyEditBox::keyboardWillShow");
    CCRect rectTracked = getRect(this);
	// some adjustment for margin between the keyboard and the edit box.
	rectTracked.origin.y -= 4;

    // if the keyboard area doesn't intersect with the tracking node area, nothing needs to be done.
    if (!rectTracked.intersectsRect(info.end))
    {
        CCLOG("needn't to adjust view layout.");
        return;
    }
    
    // assume keyboard at the bottom of screen, calculate the vertical adjustment.
    m_fAdjustHeight = info.end.getMaxY() - rectTracked.getMinY();
    // CCLOG("MyEditBox:needAdjustVerticalPosition(%f)", m_fAdjustHeight);
    
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->doAnimationWhenKeyboardMove(info.duration, m_fAdjustHeight);
    }
}

void MyEditBox::keyboardDidShow(CCIMEKeyboardNotificationInfo& info)
{
	
}

void MyEditBox::keyboardWillHide(CCIMEKeyboardNotificationInfo& info)
{
    // CCLOG("MyEditBox::keyboardWillHide");
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->doAnimationWhenKeyboardMove(info.duration, -m_fAdjustHeight);
    }
}

void MyEditBox::keyboardDidHide(CCIMEKeyboardNotificationInfo& info)
{
	
}

void MyEditBox::registerScriptEditBoxHandler(int handler)
{
    unregisterScriptEditBoxHandler();
    m_nScriptEditBoxHandler = handler;
}

void MyEditBox::unregisterScriptEditBoxHandler(void)
{
    if (0 != m_nScriptEditBoxHandler)
    {
        CCScriptEngineManager::sharedManager()->getScriptEngine()->removeScriptHandler(m_nScriptEditBoxHandler);
        m_nScriptEditBoxHandler = 0;
    }
}
