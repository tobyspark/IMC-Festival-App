//
//  tbzTweet.h
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 20/09/2012.
//
//

#ifndef __IMCFestivalApp__tbzTweet__
#define __IMCFestivalApp__tbzTweet__

#include "ofMain.h"
#include "ofxTweet.h"
#include "jansson.h"

class tbzTweet
{
    public:
        // Oh! to have Obj-C and categories on classes.
        // In the meantime...
        static bool getGeoFromTweet(ofxTweet &tweet, ofPoint &point);
};

#endif /* defined(__IMCFestivalApp__tbzTweet__) */
