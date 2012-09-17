//
//  tbzCaptionGraphic.h
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 17/09/2012.
//
//

#ifndef __IMCFestivalApp__tbzCaptionGraphic__
#define __IMCFestivalApp__tbzCaptionGraphic__

#include "ofMain.h"
#include <list>

class tbzCaptionGraphic
{
public:
    tbzCaptionGraphic();
    
    void setStyle(int cornerRadius, int arrowSize, ofColor titleTextColour, ofColor bodyTextColour, ofColor backColour);
    void setContent(string title, list<string>bodyLines);
    void snapToTargetSize();
    
    ofTrueTypeFont *fontTitle;
    ofTrueTypeFont *fontBody;
    
    void            update();
    void            draw(float animPos = 1.0f);
    
protected:
    string          titleText;
    list<string>  tagTextLines;
    int             cornerRadius;
    int             arrowSize;
    ofColor         titleTextColour;
    ofColor         bodyTextColour;
    ofColor         backColour;
    void            updateTagFBO(bool resize = false);
    ofFbo           tagFBO;
    float           tagTextHeight;
    float           tagTextHeightTarget;
    float           tagTextWidth;
    float           tagTextWidthTarget;
};

#endif /* defined(__IMCFestivalApp__tbzCaptionGraphic__) */
