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
        tbzSocialMessage(string text, float latitude, float longitude, ofTrueTypeFont *font = NULL);
        ~tbzSocialMessage();
        
        void setupFBO();
        void draw();
    
        ofPoint geoLocation;
    
        ofTrueTypeFont *font;
    
    protected:
        string text;
        ofRectangle bounds;
        
        // GL resources don't copy on c++ object copy.
        // See draw() for why this is a pointer.
        ofFbo *fbo;
    
        // perhaps should have material.
};

#endif /* defined(__IMCFestivalApp__tbzSocialMessage__) */
