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
#include "ofxPlaylist.h"

#include "agg_trans_bilinear.h"

#include "tbzSocialMessage.h"
#include "tbzVenue.h"
#include "tbzPerson.h"

#define kTBZES_ViewElevationAngle 70
#define kTBZES_MessageElevationHeight 20
#define kTBZES_Damping 0.1
#define kTBZES_MaxPunters 20

class tbzEventSite : public ofxMtActionsObject
{

public:
    // Class methods
    
    void setup(string modelName, ofxLatLon modelTopLeft, ofxLatLon modelTopRight, ofxLatLon modelBottomLeft, ofxLatLon modelBottomRight);
    
    void addVenue(tbzVenue venue);
    void addPromoter(Poco::SharedPtr<tbzPerson> person);
    void addPunter(Poco::SharedPtr<tbzPerson> person);
    void addMessage(Poco::SharedPtr<tbzSocialMessage> message);
    
    tbzVenue* nearestVenue(float &distance);
    
    enum ViewState { planView, transitioningToPlanView, sideElevationView, transitioningToElevationView };
    bool updateViewState(ViewState &viewState);
    
    // ofxMtActionsObject overrides
    
    bool actionTouchHitTest(float _x, float _y);
    
    void updateContent();
    
    void drawContent();
    
    // Class properties
    
    ofPoint     origin;
    ofRectangle groundBounds;
    
    list<tbzVenue> venues;
    list< Poco::SharedPtr<tbzPerson> > punters;
    list< Poco::SharedPtr<tbzPerson> > promoters;
    bool puntersDraw;
    
    float       elevationFactor;
    
    ofxPlaylist siteAnimation;
    
protected:

    bool loadModel(string modelName, float initialSize, ofxLatLon geoTopLeft, ofxLatLon geoTopRight, ofxLatLon geoBottomLeft, ofxLatLon geoBottomRight);

    ViewState   viewState;
    ViewState   lastViewState;
    
    ofxAssimpModelLoader siteModel;
    //float       elevationFactor;
    float       elevationFactorTarget;
    
    ofPoint     groundToModel(const ofPoint &groundPoint);
    
    ofPoint     modelTopLeft;
    ofPoint     modelTopRight;
    ofPoint     modelBottomLeft;
    ofPoint     modelBottomRight;
    ofPoint     groundTopLeft;
    ofPoint     groundTopRight;
    ofPoint     groundBottomLeft;
    ofPoint     groundBottomRight;
    
    ofLight     light;
        
private:
    agg::trans_bilinear groundToModelTransform;
};

#endif
