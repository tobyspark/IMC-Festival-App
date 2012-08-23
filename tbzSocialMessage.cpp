//
//  tbzSocialMessage.cpp
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 23/08/2012.
//
//

#include "tbzSocialMessage.h"

tbzSocialMessage::tbzSocialMessage(string _text, float _latitude, float _longitude)
{
    text = _text;
    geoLocation.set(_longitude, _latitude);
}

void tbzSocialMessage::draw()
{
    ofDrawBitmapString(text, geoLocation);
}