//
//  tbzEventSite.h
//  assimpExample
//
//  Created by TBZ.PhD on 08/08/2012.
//
//

#ifndef assimpExample_tbzEventSite_h
#define assimpExample_tbzEventSite_h

#include "ofMain.h"

#include <list> // Needed for ofxMtActionsObject.h and friends to compile
#include <map>  // Needed for ofxMtActionsObject.h and friends to compile
#include "ofxMtActionsObject.h"
#include "ofxAssimpModelLoader.h"

class tbzEventSite : public ofxMtActionsObject
{

public:
    tbzEventSite() {};

    
    ofxAssimpModelLoader siteModel;
    
    void loadModel(string modelName)
    {
        // We're using SketchUp, but...
        // A post on C4D to OF model making: http://lab.arn.com/2011/10/cinema-4d-to-openframeworks-workflow/
        
        if(siteModel.loadModel(modelName, true))
        {
            siteModel.setAnimation(0);
            
            setModel();
        }
    }
    
    void setModel()
    {
        // Model does not import with SketchUp axis correct. Pain, in 3D.
        
        // Model draws with x -> -x, y = y, z = z
        // siteModel.scale is normalised to GL units, so to fill screen width
        float normToDesiredSize = 2.0;
        siteModel.setScale(-normToDesiredSize, normToDesiredSize, normToDesiredSize);
        
        // Sketchup min is 0,0,0 max is 900,800,20
        // AssImp min is 0, 0, -31.4961 max is 35.4331, 1.1811, 0
        
        // Deduce scale of 35.4331 / 900 = 0.03937, note scale is irrelevant here as it draws normalised.
        // Deduce x -> x, y -> z, z -> -y, which we need to know to fiddle the following
        
        siteModel.setPosition(0 - (siteModel.getSceneCenter().x * siteModel.getNormalizedScale() * normToDesiredSize),
                              0 - (siteModel.getSceneCenter().z * siteModel.getNormalizedScale() * normToDesiredSize),
                              0 - (siteModel.getSceneCenter().y * siteModel.getNormalizedScale() * normToDesiredSize)
                             );
        
        /*
        ofLog(OF_LOG_VERBOSE,"siteModel.getSceneMin() " + ofToString(siteModel.getSceneMin().x) + ", " + ofToString(siteModel.getSceneMin().y) + ", " + ofToString(siteModel.getSceneMin().z));
        ofLog(OF_LOG_VERBOSE,"siteModel.getSceneMax() " + ofToString(siteModel.getSceneMax().x) + ", " + ofToString(siteModel.getSceneMax().y) + ", " + ofToString(siteModel.getSceneMax().z));
        ofLog(OF_LOG_VERBOSE,"siteModel.getSceneCenter() " + ofToString(siteModel.getSceneCenter().x) + ", " + ofToString(siteModel.getSceneCenter().y) + ", " + ofToString(siteModel.getSceneCenter().z));
        ofLog(OF_LOG_VERBOSE,"siteModel.getNormalisedScale()" + ofToString(siteModel.getNormalizedScale()));
        */
    }
    
    bool actionTouchHitTest(float _x, float _y)
    {
        // We're ignoring the 2D rect inheritance, and spinning a 3D model using all the screen as gesture space.
        return true;
    }
    
    void setup()
    {
        // Set position
        setPos(ofGetWidth()/2, ofGetHeight()/2);
        
        // Set size - given workings of MSAInteractiveObject, we aren't drawing and scaling a rect, but are instead use width for a scale variable 
        setSize(1, 1);
        
        // Bound multitouch's ability to change size
        minSize = 1;
        maxSize = 5;
        
        // We can drag horizontally to navigate around site
        // We can drag vertically to change elevation onto site
        int *tRange = new int[2];
        tRange[0] = 1;
        tRange[1] = -4;;
        setIsDraggable(true, tRange, 2);
        delete [] tRange;
        
        // We can scale to zoom in and out
        tRange = new int[1];
        tRange[0] = 2;
		setIsScalable(true, tRange, 1 );
        delete [] tRange;
        
        // We can rotate to navigate around site
		tRange = new int[2];
        tRange[0] = 2;
        tRange[1] = 3;
        setIsRotatable(true, tRange, 2 );
        delete [] tRange;
        
        // We can tap on venues?
        setIsTappable(true);        
        
        // Set model position and scale in bounding rect
        setModel();

    }

    void updateContent()
    {
        // TASK: Bound Y to height of screen, thus clamping elevation angle
        y = max(y, 0.0f);
        y = min(y, (float)ofGetHeight());
        
        // TASK: Bound x to within screen given scale factor
        // if scale = 1, bounds are screenwidth/2,screenwidth/2
        // if scale = 2, bounds are 0, screenwidth
        x = max(x, ofGetWidth() - width*ofGetWidth()/2);
        x = min(x, width*ofGetWidth()/2);
    }
    
    void drawContent()
    {
        bool debug3D = false;
        
        ofPushMatrix();
        
            // TASK: Compensate for being a MSAInteractiveObject, we don't want to draw where we are but in the centre of the screen.
        
            // Keep model non-rotated until we want to apply it ourselves
            ofRotate(ofRadToDeg(-rotation), 0, 0, 1);
            // Keep model vertically aligned as y coord goes up and down (we're co-opting for elevation angle)
            ofTranslate(0, -y + ofGetHeight()/2);
            
            if (debug3D)
            {
                ofSetColor(255, 255, 255, 255);
                ofDrawBitmapString("eventSite modelLoc", 0 ,0);
            }

            // TASK: Pan, twirl and zoom around
        
            // Perform scale from centre of screen
            float scale = width;
            ofScale(scale, scale, scale);
        
            // Rotate site around the x axis to change elevation angle from plan to just overhead
            float minElevation = 0;
            float maxElevation = 90;
            float elevationRot = minElevation + (maxElevation - minElevation) * (y / ofGetHeight());
            ofRotate(elevationRot, 1, 0, 0);
            
            // Rotate site around its z axis to spin with horizontal swipe
            ofRotate(ofRadToDeg(rotation), 0, 0, 1);
        
            siteModel.drawFaces();
        
        ofPopMatrix();
        
        if (debug3D)
        {
            ofSetColor(255, 255, 255, 255);
            ofDrawBitmapString("eventSite 0,0,r" + ofToString(ofRadToDeg(rotation)), 0 ,0);
        }
    }
    
protected:


};

#endif
