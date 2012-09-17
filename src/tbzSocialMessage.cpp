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
#define kTBZSocialMessage_AnimInDistance 500

tbzSocialMessage::tbzSocialMessage(string _text, float _latitude, float _longitude)
{
    list<string> nothingForNow;
    tag.setContent(_text, nothingForNow);
    tag.setStyle(kTBZSocialMessage_Radius, kTBZSocialMessage_ArrowSize, ofColor(255,0,0), ofColor(255,0,0), ofColor(255));
    geoLocation.set(_longitude, _latitude);
}

void tbzSocialMessage::draw(float animPos)
{
    if (animPos > 0.01f)
    {
        ofPushStyle();
        ofPushMatrix();
        {
            ofColor fadeInAlpha(255, animPos*255.0f);
            float   animInPos = (1.0f-animPos)*kTBZSocialMessage_AnimInDistance;
        
            ofSetColor(fadeInAlpha);
            ofTranslate(0, -animInPos);
            tag.draw();
        }
        ofPopMatrix();
        ofPopStyle();
    }
}
