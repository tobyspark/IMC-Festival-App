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
        tbzSocialMessage(string text, string author, string service, string time);
        
        string attributeTo;
    
        tbzCaptionGraphic tag;
    
        Poco::SharedPtr<ofPoint> geoLocation;
    
        void update();
        void draw(float animPos = 1.0f);
};

#endif /* defined(__IMCFestivalApp__tbzSocialMessage__) */
