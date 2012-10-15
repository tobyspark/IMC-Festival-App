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
    // TODO: initialise tbzEventSiteFeature with font and style info
    
    string          name;
    ofPoint         geoLocation;
    ofPoint         siteModelLocation;
    float           selected;
    float           transition;
    ofxAssimpModelLoader model;
    
    enum            State {uninitialised, nothing, preview, full};
    virtual void    setState(State inState) = 0;
    virtual State   getState() = 0;
    
    virtual float   getTagHeight() = 0;
    
    virtual void    update() = 0;
    virtual void    drawFeature() = 0;
    virtual void    drawTag() = 0;
    
    virtual         ~tbzEventSiteFeature() {};
    
protected:
    State           state;
};

#endif
