//
//  tbzSocialMessage.cpp
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 23/08/2012.
//
//

#include "tbzSocialMessage.h"

#define kTBZSocialMessage_Radius 5
#define kTBZSocialMessage_BaseHeight 20
#define kTBZSocialMessage_ArrowSize 10

tbzSocialMessage::tbzSocialMessage(string _text, float _latitude, float _longitude)
{
    text = _text;
    geoLocation.set(_longitude, _latitude);
    
    font = NULL;
    
    arrowTL.set(-kTBZSocialMessage_ArrowSize/2, -kTBZSocialMessage_BaseHeight);
    arrowTR.set( kTBZSocialMessage_ArrowSize/2, -kTBZSocialMessage_BaseHeight);
    arrowB.set(0, -kTBZSocialMessage_BaseHeight + kTBZSocialMessage_ArrowSize * sin(ofDegToRad(60.0f)));
}

void tbzSocialMessage::draw()
{
    //ofDrawBitmapString(text, geoLocation);
    
    ofPushMatrix();
    
    ofTranslate(modelLocation.x, modelLocation.y);
    ofRotate(-90, 1, 0, 0);
    
    // Where we want the text, centred on x and baseline on y
    float textOriginX = 0;
    float textOriginY = -kTBZSocialMessage_BaseHeight -kTBZSocialMessage_Radius;
    
    if (font)
    {
        // Get bounds of text then centre the bounds horizontally.
        ofRectangle textBounds = font->getStringBoundingBox(text, textOriginX,  textOriginY); // bounds doesn't give origin of 0,0; rather ie. 1, -9.
        textBounds.x        += -textBounds.width/2;
        
        // Outline the text bounds with our round rect corner radius
        ofRectangle backBounds = textBounds;
        backBounds.width    += kTBZSocialMessage_Radius*2.0f;
        backBounds.height   += kTBZSocialMessage_Radius*2.0f;

        // Place backBounds where container should be (don't do this relative to textBounds)
        backBounds.x        = -textBounds.width/2;
        backBounds.y        = -kTBZSocialMessage_BaseHeight - backBounds.height;
        
        // Draw container shape
        ofSetColor(255);
        ofRectRounded(backBounds, kTBZSocialMessage_Radius);
        ofTriangle(arrowTL, arrowTR, arrowB);
        
        // Draw text
        ofSetColor(255,0,0);
        ofTranslate(0, 0, 0.1); // Rise off plane of back container so no geometery intersect
        font->drawString(text, textOriginX - textBounds.width/2, textOriginY);
    }
    else
    {
        ofDrawBitmapString(text, textOriginX, textOriginY);
    }
    
    ofPopMatrix();
}