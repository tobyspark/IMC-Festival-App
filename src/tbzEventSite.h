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
#include "tbzScreenScale.h"

#define kTBZES_ViewElevationAngle 70
#define kTBZES_VenueTagElevationHeight (15 * tbzScreenScale::retinaScale)
#define kTBZES_PersonTagElevationHeight (10 * tbzScreenScale::retinaScale)
#define kTBZES_Damping 0.1
#define kTBZES_MaxPunters 20

class tbzFeatureAndDist
{
public:
    Poco::SharedPtr<tbzEventSiteFeature> feature;
    float                                distance;
    
    bool operator < (tbzFeatureAndDist);
};

class tbzEventSite : public ofxMtActionsObject
{

public:
    // Class methods
    
    void setup(string modelName, ofxLatLon modelTopLeft, ofxLatLon modelTopRight, ofxLatLon modelBottomLeft, ofxLatLon modelBottomRight);
    
    void addVenue(Poco::SharedPtr<tbzVenue> venue);
    void addPromoter(Poco::SharedPtr<tbzPerson> person);
    void addPunter(Poco::SharedPtr<tbzPerson> person);
    void addMessageToPunters(Poco::SharedPtr<tbzSocialMessage> message);
    void addMessageToPromoters(Poco::SharedPtr<tbzSocialMessage> message);
    
    ofTrueTypeFont* venueTitleFont;
    ofTrueTypeFont* venueBodyFont;
    ofColor venueForeColour;
    ofColor venueBackColour;
    
    ofTrueTypeFont* promoterTitleFont;
    ofTrueTypeFont* promoterBodyFont;
    ofColor promoterForeColour;
    ofColor promoterBackColour;
    
    ofTrueTypeFont* personTitleFont;
    ofTrueTypeFont* personBodyFont;
    ofColor personForeColour;
    ofColor personBackColour;
    
    void setOrigin(ofPoint origin);
        
    enum ViewState { planView, transitioningToPlanView, sideElevationView, transitioningToElevationView };
    
    // ofxMtActionsObject overrides
    
    bool actionTouchHitTest(float _x, float _y);
    
    void updateContent();
    
    void drawContent();
    
    // Class properties
    
    ofRectangle groundBounds;
    
    ofxPlaylist siteAnimation;
    
protected:

    list< Poco::SharedPtr<tbzVenue> > venues;
    list< Poco::SharedPtr<tbzPerson> > punters;
    list< Poco::SharedPtr<tbzPerson> > promoters;
    
    void featuresDistanceFromOriginSort();
    void featuresYPosSort();
    list< tbzFeatureAndDist > featuresDepthSorted;
    
    // This should be redone now we have tbzEventSiteFeature with selected property.
    tbzEventSiteFeature* nearestFeatureTest(float &distance);
    tbzEventSiteFeature* featureFocussed;
    
    ofPoint     originCurrent;
    ofPoint     originDesired;
    ofPoint     originAsSet;
    
    bool loadModel(string modelName, float initialSize, ofxLatLon geoTopLeft, ofxLatLon geoTopRight, ofxLatLon geoBottomLeft, ofxLatLon geoBottomRight);

    bool        viewStateChanged;
    ViewState   viewState;
    ViewState   lastViewState;
    
    ofxAssimpModelLoader siteModel;
    float       scale;
    float       elevationFactor;
    float       planFactor;
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
