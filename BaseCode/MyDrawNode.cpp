//
//  MyDrawNode.cpp
//
//  Created by Ruoqian, Chen on 2014/04/23
//
#include "StdAfx.h"

#include "MyDrawNode.h"

MyDrawNode::MyDrawNode( const CCPoint& rPtFrom, const CCPoint &rPtTo, DWORD dwColor )
: _displayedOpacity(255)
, _realOpacity (255)
, _displayedColor(ccWHITE)
, _realColor(ccWHITE)
, _cascadeOpacityEnabled(false)
, _cascadeColorEnabled(false)
{
	m_nDrawStyle = DRAW_STYLE_DEFAULT;
	m_ptFrom = rPtFrom;
	m_ptTo = rPtTo;
	ccColor4B color4b = GetColor4B(dwColor);
	m_color4f = ccc4FFromccc4B(color4b);
}

MyDrawNode::MyDrawNode(const CCPoint& rPtFrom, const CCPoint &rPtTo, const ccColor3B& rColor)
: _displayedOpacity(255)
, _realOpacity (255)
, _displayedColor(ccWHITE)
, _realColor(ccWHITE)
, _cascadeOpacityEnabled(false)
, _cascadeColorEnabled(false)
{
	m_nDrawStyle = DRAW_STYLE_DEFAULT;
	m_ptFrom = rPtFrom;
	m_ptTo = rPtTo;
	m_color4f = ccc4FFromccc3B(rColor);
}

MyDrawNode* MyDrawNode::create(const CCPoint& rPtFrom, const CCPoint &rPtTo, DWORD dwColor)
{
	MyDrawNode* pNode = new MyDrawNode(rPtFrom, rPtTo, dwColor);
	pNode->autorelease();
	return pNode;
}

void MyDrawNode::draw()
{
	ccColor4F colorDraw = m_color4f;
	colorDraw.a = colorDraw.a * this->getOpacity() / 255; 

	switch (m_nDrawStyle) {
	case DRAW_STYLE_SOLIDRECT:
		ccDrawSolidRect(m_ptFrom, m_ptTo, colorDraw);
		break;
	case DRAW_STYLE_RECT:
		ccDrawColor4F(colorDraw.r, colorDraw.g, colorDraw.b, colorDraw.a);
		ccDrawRect(m_ptFrom, m_ptTo);
		break;
	default:
		break;
	}
}

GLubyte MyDrawNode::getOpacity()
{
	return _realOpacity;
}

GLubyte MyDrawNode::getDisplayedOpacity()
{
	return _displayedOpacity;
}

/** Override synthesized setOpacity to recurse items */
void MyDrawNode::setOpacity(GLubyte opacity)
{
	_displayedOpacity = _realOpacity = opacity;
    
	if( _cascadeOpacityEnabled )
    {
		GLubyte parentOpacity = 255;
        CCRGBAProtocol *parent = dynamic_cast<CCRGBAProtocol*>(m_pParent);
        if (parent && parent->isCascadeOpacityEnabled())
        {
            parentOpacity = parent->getDisplayedOpacity();
        }
        updateDisplayedOpacity(parentOpacity);
	}
}

const ccColor3B& MyDrawNode::getColor()
{
	return _realColor;
}

const ccColor3B& MyDrawNode::getDisplayedColor()
{
	return _displayedColor;
}

void MyDrawNode::setColor(const ccColor3B& color)
{
	_displayedColor = _realColor = color;
	
	if (_cascadeColorEnabled)
    {
		ccColor3B parentColor = ccWHITE;
        CCRGBAProtocol* parent = dynamic_cast<CCRGBAProtocol*>(m_pParent);
		if (parent && parent->isCascadeColorEnabled())
        {
            parentColor = parent->getDisplayedColor();
        }

        updateDisplayedColor(parentColor);
	}
}

void MyDrawNode::updateDisplayedOpacity(GLubyte parentOpacity)
{
	_displayedOpacity = _realOpacity * parentOpacity/255.0;
    
    if (_cascadeOpacityEnabled)
    {
        CCObject *obj = NULL;
        CCARRAY_FOREACH(m_pChildren, obj)
        {
            CCRGBAProtocol *item = dynamic_cast<CCRGBAProtocol*>(obj);
            if (item)
            {
                item->updateDisplayedOpacity(_displayedOpacity);
            }
        }
    }
}

void MyDrawNode::updateDisplayedColor(const ccColor3B& parentColor)
{
	_displayedColor.r = _realColor.r * parentColor.r/255.0;
	_displayedColor.g = _realColor.g * parentColor.g/255.0;
	_displayedColor.b = _realColor.b * parentColor.b/255.0;
    
    if (_cascadeColorEnabled)
    {
        CCObject *obj = NULL;
        CCARRAY_FOREACH(m_pChildren, obj)
        {
            CCRGBAProtocol *item = dynamic_cast<CCRGBAProtocol*>(obj);
            if (item)
            {
                item->updateDisplayedColor(_displayedColor);
            }
        }
    }
}

bool MyDrawNode::isCascadeOpacityEnabled()
{
    return _cascadeOpacityEnabled;
}

void MyDrawNode::setCascadeOpacityEnabled(bool cascadeOpacityEnabled)
{
    _cascadeOpacityEnabled = cascadeOpacityEnabled;
}

bool MyDrawNode::isCascadeColorEnabled()
{
    return _cascadeColorEnabled;
}

void MyDrawNode::setCascadeColorEnabled(bool cascadeColorEnabled)
{
    _cascadeColorEnabled = cascadeColorEnabled;
}

void MyDrawNode::SetDrawPos( const CCPoint& rPtFrom, const CCPoint& rPtTo )
{
	m_ptFrom = rPtFrom;
	m_ptTo = rPtTo;
}

void MyDrawNode::SetDrawStyle( int nStyle )
{
	m_nDrawStyle = nStyle;
}
