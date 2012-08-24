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

class tbzSocialMessage
{
    public:
        tbzSocialMessage(string text, float latitude, float longitude);
        
        void draw();
    
        ofPoint geoLocation;
    
        ofTrueTypeFont *font;
    
    protected:
        string text;
        ofPoint arrowTL, arrowTR, arrowB;
};

#endif /* defined(__IMCFestivalApp__tbzSocialMessage__) */
