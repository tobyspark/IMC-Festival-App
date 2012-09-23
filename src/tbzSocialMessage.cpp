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

tbzSocialMessage::tbzSocialMessage(string _text, string author, string service, string time)
{
    list<string> body;
    string bodyLine = author + " on " + service + " at " + time;
    body.push_back(bodyLine);
    tag.setContent(_text, body);
    tag.setStyle(kTBZSocialMessage_Radius, kTBZSocialMessage_ArrowSize, ofColor(255,0,0), ofColor(255,0,0), ofColor(255));
}

void tbzSocialMessage::update()
{
    tag.update();
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
