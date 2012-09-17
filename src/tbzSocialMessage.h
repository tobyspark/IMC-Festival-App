//
//  tbzSocialMessage.h
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 23/08/2012.
//
//

#ifndef __IMCFestivalApp__tbzSocialMessage__
#define __IMCFestivalApp__tbzSocialMessage__

#include "ofMain.h"
#include "tbzCaptionGraphic.h"
#include <list>

class tbzSocialMessage
{
    public:
        tbzSocialMessage(string text, float latitude, float longitude);
        
        tbzCaptionGraphic tag;
    
        ofPoint geoLocation;
    
        void draw(float animPos = 1.0f);
};

#endif /* defined(__IMCFestivalApp__tbzSocialMessage__) */
