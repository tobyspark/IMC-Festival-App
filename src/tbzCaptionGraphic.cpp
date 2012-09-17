//
//  tbzCaptionGraphic.cpp
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 17/09/2012.
//
//

#include "tbzCaptionGraphic.h"

#define kTBZVenue_Damping 0.1f

tbzCaptionGraphic::tbzCaptionGraphic()
{
    cornerRadius = 5;
    arrowSize = 10;
    titleTextColour = ofColor(255);
    bodyTextColour = ofColor(200);
    backColour = ofColor(50);
    fontTitle = NULL;
    fontBody = NULL;
}


void tbzCaptionGraphic::setStyle(int radius, int size, ofColor title, ofColor body, ofColor back)
{
    cornerRadius = radius;
    arrowSize = size;
    titleTextColour = title;
    bodyTextColour = body;
    backColour = back;
    
    // Resize and redraw FBO
    updateTagFBO(true);
}

void tbzCaptionGraphic::setContent(string _title, list<string>_bodyLines)
{
    titleText = _title;
    tagTextLines = _bodyLines;
    
    // Resize and redraw FBO
    updateTagFBO(true);
}

void tbzCaptionGraphic::snapToTargetSize()
{
    tagTextHeight = tagTextHeightTarget;
    tagTextWidth = tagTextWidthTarget;
    updateTagFBO();
}

void tbzCaptionGraphic::update()
{
    // TASK: Animate tag as appropriate, set by content
    
    float difference;
    bool update = false;
    
    difference = tagTextHeightTarget-tagTextHeight;
    if (fabs(difference) > 0.001)
    {
        tagTextHeight += (tagTextHeightTarget - tagTextHeight) * kTBZVenue_Damping;
        
        difference = tagTextHeightTarget-tagTextHeight;
        if (fabs(difference) < 0.001)
        {
            tagTextHeight = tagTextHeightTarget;
        }
        
        update = true;
    }
    
    difference = tagTextWidthTarget-tagTextWidth;
    if (fabs(difference) > 0.001)
    {
        tagTextWidth += (tagTextWidthTarget - tagTextWidth) * kTBZVenue_Damping;
        
        difference = tagTextWidthTarget-tagTextWidth;
        if (fabs(difference) < 0.001)
        {
            tagTextWidth = tagTextWidthTarget;
        }
        
        update = true;
    }
    
    if (update)
    {
        updateTagFBO();
    }
}

void tbzCaptionGraphic::updateTagFBO(bool resize)
{
    if (fontTitle)
    {
        // Just a reminder: origin is top left, and this is an arrow pointing down.
        
        if (!tagFBO.isAllocated() || resize)
        {
            // TASK: Determine tagTextTarget sizes given however many subtitle / info lines
            if (tagTextLines.size() > 0)
            {
                // Vertically
                tagTextHeightTarget = tagTextLines.size() * fontBody->getLineHeight();
                
                // Horiztonally
                tagTextWidthTarget = fontTitle->stringWidth(titleText);
                list<string>::iterator line;
                for (line = tagTextLines.begin(); line != tagTextLines.end(); line++)
                {
                    tagTextWidthTarget = max(tagTextWidthTarget, fontBody->stringWidth(*line));
                }
            }
            else
            {
                tagTextHeightTarget = 0;
                tagTextWidthTarget = fontTitle->stringWidth(titleText);
            }
        }
        
        // backBounds is the rect of the bubble only, ie. text with border
        ofRectangle backBounds;
        backBounds.width    = tagTextWidth + cornerRadius*2.0f;
        backBounds.height   = fontTitle->getLineHeight() + tagTextHeight + cornerRadius*2.0f;
        
        // tagBounds is the rect of the whole tag, ie. backBounds plus the caption arrow
        ofRectangle tagBounds;
        tagBounds           = backBounds;
        tagBounds.height   += arrowSize;
        
        ofRectangle tagTargetBounds;
        tagTargetBounds.width   = tagTextWidthTarget + cornerRadius*2.0f;
        tagTargetBounds.height  = fontTitle->getLineHeight() + tagTextHeightTarget + cornerRadius*2.0f + arrowSize;
        
        // Create the empty image to draw into if neccessary
        if (!tagFBO.isAllocated() ||
            (resize && tagTargetBounds.height > tagFBO.getHeight()) || // We're getting bigger
            (tagBounds.height != tagFBO.getHeight() && tagTextHeight == tagTextHeightTarget)) // Having got smaller, we've finished animating down.
        {
            ofFbo::Settings s;
            s.width             = min((int)tagTargetBounds.width, ofGetWidth());
            s.height            = min((int)tagTargetBounds.height, ofGetHeight());
            s.internalformat    = GL_RGBA;
            s.useDepth          = false;
            
            tagFBO.setDefaultTextureIndex(0); // oF bug needs this as fix( http://forum.openframeworks.cc/index.php?topic=10536.0 )
            tagFBO.allocate(s);
            
            // We want to draw centered horizontally and aligned to bottom
            tagFBO.setAnchorPoint(s.width/2, s.height);
        }
        
        tagBounds.x     = (tagFBO.getWidth() - tagBounds.width) / 2.0f; // Center horizontally
        tagBounds.y     = tagFBO.getHeight() - tagBounds.height;        // Place at bottom
        
        backBounds.x        = tagBounds.x;
        backBounds.y        = tagBounds.y;
        
        ofPoint textOrigin;
        textOrigin.x        = backBounds.x + cornerRadius;
        //textOrigin.y        = backBounds.y + cornerRadius + fontTitle->getLineHeight();
        textOrigin.y        = backBounds.y + cornerRadius + fontTitle->stringHeight(titleText);
        
        
        float centreX = tagFBO.getWidth() / 2.0f;
        float backBottom = backBounds.y + backBounds.height;
        ofPoint arrowTL(centreX - arrowSize/2.0f, backBottom);
        ofPoint arrowTR(centreX + arrowSize/2.0f, backBottom);
        ofPoint arrowB(centreX, backBottom + arrowSize * sin(ofDegToRad(60.0f)));
        
        // Draw into image
        ofPushStyle();
        tagFBO.begin();
        {
            // Clear the image
            ofClear(255,255,255, 0);
            
            // Draw container shape
            ofSetColor(backColour);
            ofRectRounded(backBounds, cornerRadius);
            ofTriangle(arrowTL, arrowTR, arrowB);
            
            // Draw text
            ofSetColor(titleTextColour);
            fontTitle->drawString(titleText, textOrigin.x, textOrigin.y);
            
            ofSetColor(bodyTextColour);
            list<string>::iterator line;
            for (line = tagTextLines.begin(); line != tagTextLines.end(); line++)
            {
                if (textOrigin.y > backBottom - fontBody->getLineHeight()) break;
                
                textOrigin.y += fontBody->getLineHeight();
                
                float lineWidth = fontBody->stringWidth(*line);
                float availableWidth = tagTextWidth + cornerRadius;
                if (lineWidth > availableWidth)
                {
                    int charsToDraw = line->length() * (tagTextWidth / lineWidth);
                    fontBody->drawString(line->substr(0,charsToDraw), textOrigin.x, textOrigin.y);
                }
                else
                {
                    fontBody->drawString(*line, textOrigin.x, textOrigin.y);
                }
            }
        }
        tagFBO.end();
        ofPopStyle();
    }
    else
    {
        ofLog(OF_LOG_WARNING, "TBZSocialMessage: Setup: fontTitle not allocated");
    }
    
}

void tbzCaptionGraphic::draw(float animPos)
{
    if (fontTitle && fontBody)
    {
        if (!tagFBO.isAllocated()) updateTagFBO();
        
        tagFBO.draw(0,0);
    }
    else
    {
        ofDrawBitmapString(titleText, 0, 0);
    }
}
