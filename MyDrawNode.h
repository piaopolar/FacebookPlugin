//
//  MyDrawNode.h
//
//  Created by Ruoqian, Chen on 2014/04/23
//

#ifndef __MY_DRAW_NODE_H__
#define __MY_DRAW_NODE_H__

enum
{
	DRAW_STYLE_SOLIDRECT,
	DRAW_STYLE_DEFAULT = DRAW_STYLE_SOLIDRECT,
	DRAW_STYLE_RECT,
};

class MyDrawNode : public CCNode, public CCRGBAProtocol
{
public:
	MyDrawNode(const CCPoint& rPtFrom, const CCPoint &rPtTo, DWORD dwColor);
	MyDrawNode(const CCPoint& rPtFrom, const CCPoint &rPtTo, const ccColor3B& rColor);

	void SetDrawPos(const CCPoint& rPtFrom, const CCPoint& rPtTo);
	void SetDrawStyle(int nStyle);

	virtual void draw();

	static MyDrawNode* create(const CCPoint& rPtFrom, const CCPoint &rPtTo, DWORD dwColor);

    virtual GLubyte getOpacity();
    virtual GLubyte getDisplayedOpacity();
    virtual void setOpacity(GLubyte opacity);
    virtual void updateDisplayedOpacity(GLubyte parentOpacity);
    virtual bool isCascadeOpacityEnabled();
    virtual void setCascadeOpacityEnabled(bool cascadeOpacityEnabled);
    
    virtual const ccColor3B& getColor();
    virtual const ccColor3B& getDisplayedColor();
    virtual void setColor(const ccColor3B& color);
    virtual void updateDisplayedColor(const ccColor3B& parentColor);
    virtual bool isCascadeColorEnabled();
    virtual void setCascadeColorEnabled(bool cascadeColorEnabled);
    
    virtual void setOpacityModifyRGB(bool bValue) {}
    virtual bool isOpacityModifyRGB() { return false; }
protected:
	GLubyte		_displayedOpacity, _realOpacity;
	ccColor3B	_displayedColor, _realColor;
	bool		_cascadeOpacityEnabled, _cascadeColorEnabled;

private:
	int m_nDrawStyle;
	CCPoint m_ptFrom;
	CCPoint m_ptTo;
	ccColor4F m_color4f;
};

#endif
