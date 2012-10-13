//
//  tbzEventSiteThing.h
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 13/10/2012.
//
//

#ifndef IMCFestivalApp_tbzEventSiteThing_h
#define IMCFestivalApp_tbzEventSiteThing_h

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"

class tbzEventSiteFeature {
    
public:
    string          name;
    ofPoint         geoLocation;
    float           selected;
    float           transition;
    ofxAssimpModelLoader model;
    
    virtual void    update() {};
    virtual void    drawFeature() {};
    virtual void    drawTag() {};
};

#endif
