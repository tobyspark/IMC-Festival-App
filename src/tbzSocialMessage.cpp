//
//  tbzSocialMessage.cpp
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 23/08/2012.
//
//

#include "tbzSocialMessage.h"

#define kTBZSocialMessage_Radius 5
#define kTBZSocialMessage_ArrowSize 10 // this is side length of arrow and height bubble is raised

tbzSocialMessage::tbzSocialMessage(string _text, float _latitude, float _longitude, ofTrueTypeFont *_font)
{
    text = _text;
    geoLocation.set(_longitude, _latitude);
    font = _font;
    fbo = NULL;
}

void tbzSocialMessage::setupFBO()
{
    if (font)
    {
        fbo = new ofFbo;
        
        // Just a reminder: origin is top left, and this is an arrow pointing down.
        
        // Get bounds of text. This will have an offset origin to place the baseline of the font at desired point
        ofRectangle textBounds = font->getStringBoundingBox(text, 0, 0);
        textBounds.x += kTBZSocialMessage_Radius;
        textBounds.y += kTBZSocialMessage_Radius + textBounds.height + 10; // 10 is a magic number. I guess the bounds aren't reported correctly.
        
        // Outline the text bounds with our round rect corner radius
        ofRectangle backBounds = textBounds;
        backBounds.width    += kTBZSocialMessage_Radius*2.0f;
        backBounds.height   += kTBZSocialMessage_Radius*2.0f;
        backBounds.x        = 0;
        backBounds.y        = 0;
        
        bounds.width        = backBounds.width;
        bounds.height       = backBounds.height + kTBZSocialMessage_ArrowSize;
        
        float centreX = bounds.width / 2.0f;
        ofPoint arrowTL(centreX - kTBZSocialMessage_ArrowSize/2.0f, backBounds.height);
        ofPoint arrowTR(centreX + kTBZSocialMessage_ArrowSize/2.0f, backBounds.height);
        ofPoint arrowB(centreX, backBounds.height + kTBZSocialMessage_ArrowSize * sin(ofDegToRad(60.0f)));
        
        // Create the empty image to draw into
        ofFbo::Settings s;
        s.width             = bounds.width;
        s.height            = bounds.height;
        s.internalformat    = GL_RGBA;
        s.useDepth          = false;
        
        fbo->allocate(s);

        // Draw into image
        ofPushStyle();
        fbo->begin();
            // Clear the image
            ofClear(255,255,255, 0);
        
            // Draw container shape
            ofSetColor(255);
            ofRectRounded(backBounds, kTBZSocialMessage_Radius);
            ofTriangle(arrowTL, arrowTR, arrowB);
            
            // Draw text
            ofSetColor(255,0,0);
            font->drawString(text, textBounds.x, textBounds.y);
        fbo->end();
        ofPopStyle();
         
        // We want to draw centered horizontally and aligned to bottom
        bounds.x            = -centreX;
        bounds.y            = -bounds.height;
    }
    else
    {
     ofLog(OF_LOG_WARNING, "TBZSocialMessage: Setup: Font not allocated");
    }
    
}
void tbzSocialMessage::draw()
{
    ofPushMatrix();
    
    ofRotate(-90, 1, 0, 0);
        
    if (font)
    {
        if (!fbo) setupFBO();
        fbo->draw(bounds.x, bounds.y);
    }
    else
    {
        ofDrawBitmapString(text, 0, 0);
    }
    
    ofPopMatrix();
}

tbzSocialMessage::~tbzSocialMessage()
{
    if (fbo) delete fbo;
}