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

#include "tbzSocialMessage.h"

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
    
    ofRectangle groundBounds;
    
    list<tbzSocialMessage> socialMessages;
        
protected:

    bool loadModel(string modelName, float initialSize, ofxLatLon geoTopLeft, ofxLatLon geoTopRight, ofxLatLon geoBottomLeft, ofxLatLon geoBottomRight);

    ofxAssimpModelLoader siteModel;
    
    ofPoint     groundToModel(const ofPoint &modelPoint);
    
    ofPoint     modelTopLeft;
    ofPoint     modelTopRight;
    ofPoint     modelBottomLeft;
    ofPoint     modelBottomRight;
    ofPoint     groundTopLeft;
    ofPoint     groundTopRight;
    ofPoint     groundBottomLeft;
    ofPoint     groundBottomRight;
};

#endif
