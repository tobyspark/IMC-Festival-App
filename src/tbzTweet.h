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

class tbzTweet : public ofxTweet
{
    public:
        bool getGeoLocation(ofPoint &point);
};

#endif /* defined(__IMCFestivalApp__tbzTweet__) */
