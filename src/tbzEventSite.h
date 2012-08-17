//
//  tbzEventSite.h
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 08/08/2012.
//
//

#ifndef tbzEventSite_h
#define tbzEventSite_h

#include "ofMain.h"
#include <list> // Needed for ofxMtActionsObject.h and friends to compile
#include <map>  // Needed for ofxMtActionsObject.h and friends to compile
#include "ofxMtActionsObject.h"
#include "ofxAssimpModelLoader.h"
#include "ofxGeometry.h"

class tbzEventSite : public ofxMtActionsObject
{

public:
    // Class methods
    
    tbzEventSite();
    
    void setup(string modelName, ofxLatLon modelTopLeft, ofxLatLon modelTopRight, ofxLatLon modelBottomLeft, ofxLatLon modelBottomRight);
    
    // ofxMtActionsObject overrides
    
    bool actionTouchHitTest(float _x, float _y);
    
    void updateContent();
    
    void drawContent();
    
    // Class properties
    
    ofxAssimpModelLoader siteModel;
        
protected:

    bool loadModel(string modelName, float initialSize, ofxLatLon geoTopLeft, ofxLatLon geoTopRight, ofxLatLon geoBottomLeft, ofxLatLon geoBottomRight);
    
    ofVec2f     modelTopLeft;
    ofVec2f     modelTopRight;
    ofVec2f     modelBottomLeft;
    ofVec2f     modelBottomRight;
    ofVec2f     groundTopLeft;
    ofVec2f     groundTopRight;
    ofVec2f     groundBottomLeft;
    ofVec2f     groundBottomRight;
};

#endif
