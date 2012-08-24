//
//  tbzEventSite.cpp
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 16/08/2012.
//
//

#include "tbzEventSite.h"



tbzEventSite::tbzEventSite() {};

bool tbzEventSite::loadModel(string modelName, float initialSize, ofxLatLon geoTopLeft, ofxLatLon geoTopRight, ofxLatLon geoBottomLeft, ofxLatLon geoBottomRight)
{
    // We're using SketchUp, but...
    // A post on C4D to OF model making: http://lab.arn.com/2011/10/cinema-4d-to-openframeworks-workflow/
    
    bool success = false;
    
    if(siteModel.loadModel(modelName, true))
    {
        siteModel.setAnimation(0);
        
        // TASK: Set model position and scale in bounding rect
        
        // siteModel.scale is normalised to GL units, so to fill screen width (initialsize of 1) we need a scale of 2
        float scale = initialSize * 2.0f;
        siteModel.setScale(scale, scale, scale);
        
        // Centre in x + y, leave z at ground level.
        siteModel.setPosition(0 - (siteModel.getSceneCenter().x * siteModel.getNormalizedScale() * scale),
                              0 + (siteModel.getSceneCenter().y * siteModel.getNormalizedScale() * scale),
                              0
                              );
        
        /*
         ofLog(OF_LOG_VERBOSE,"siteModel.getSceneMin() " + ofToString(siteModel.getSceneMin().x) + ", " + ofToString(siteModel.getSceneMin().y) + ", " + ofToString(siteModel.getSceneMin().z));
         ofLog(OF_LOG_VERBOSE,"siteModel.getSceneMax() " + ofToString(siteModel.getSceneMax().x) + ", " + ofToString(siteModel.getSceneMax().y) + ", " + ofToString(siteModel.getSceneMax().z));
         ofLog(OF_LOG_VERBOSE,"siteModel.getSceneCenter() " + ofToString(siteModel.getSceneCenter().x) + ", " + ofToString(siteModel.getSceneCenter().y) + ", " + ofToString(siteModel.getSceneCenter().z));
         ofLog(OF_LOG_VERBOSE,"siteModel.getNormalisedScale()" + ofToString(siteModel.getNormalizedScale()));
        */
        
        // TASK: Register model's earthbound geometry to our eventSite's geometry
        
        float width     = (siteModel.getSceneMax().x - siteModel.getSceneMin().x) * siteModel.getNormalizedScale() * scale;
        float height    = (siteModel.getSceneMax().y - siteModel.getSceneMin().y) * siteModel.getNormalizedScale() * scale;

        modelTopLeft.set(-width/2, -height/2);
        modelTopRight.set(width/2, -height/2);
        modelBottomLeft.set(-width/2, height/2);
        modelBottomRight.set(width/2, height/2);
        
        groundTopLeft.set(geoTopLeft.lon, geoTopLeft.lat);
        groundTopRight.set(geoTopRight.lon, geoTopRight.lat);
        groundBottomLeft.set(geoBottomLeft.lon, geoBottomLeft.lat);
        groundBottomRight.set(geoBottomRight.lon, geoBottomRight.lat);
        
        // TASK: Calculate earthbound bounds of our eventSite
        
        // our map isn't necessarily oriented north, so we can't assume topLeft value is smaller than topRight
        float boundsX, boundsY, boundsW, boundsH;
        
        if (groundTopLeft.x < groundTopRight.x)
        {
            boundsX = min(groundTopLeft.x, groundBottomLeft.x);
            boundsW = max(groundTopRight.x, groundBottomRight.x) - boundsX;
        }
        else
        {
            boundsX = min(groundTopRight.x, groundBottomRight.x);
            boundsW = max(groundTopLeft.x, groundBottomLeft.x) - boundsX;
        }
        
        if (groundTopLeft.y < groundBottomLeft.y)
        {
            boundsY = min(groundTopLeft.y, groundTopRight.y);
            boundsH = max(groundBottomLeft.y, groundBottomRight.y) - boundsY;
        }
        else
        {
            boundsY = min(groundBottomLeft.y, groundBottomRight.y);
            boundsH = max(groundTopLeft.y, groundTopRight.y) - boundsY;
        }
        
        groundBounds.set(boundsX, boundsY, boundsW, boundsH);
        
        // TASK: Calculate ground to model transformation matrix.
        
        double groundQuad[] = {groundTopLeft.x, groundTopLeft.y, groundTopRight.x, groundTopRight.y, groundBottomRight.x, groundBottomRight.y, groundBottomLeft.x, groundBottomLeft.y};
        double modelQuad[] = {modelTopLeft.x, modelTopLeft.y, modelTopRight.x, modelTopRight.y, modelBottomRight.x, modelBottomRight.y, modelBottomLeft.x, modelBottomLeft.y};
        
        groundToModelTransform.quad_to_quad(groundQuad, modelQuad);
        if (!groundToModelTransform.is_valid())
        {
            ofLog(OF_LOG_WARNING, "Failed to create mapping from Ground coords to Model coords. Check your numbers.");
        }
        
        success = true;
    }
    
    return success;
}


bool tbzEventSite::actionTouchHitTest(float _x, float _y)
{
    // We're ignoring the 2D rect inheritance, and spinning a 3D model using all the screen as gesture space.
    return true;
}

void tbzEventSite::setup(string modelName, ofxLatLon geoTopLeft, ofxLatLon geoTopRight, ofxLatLon geoBottomLeft, ofxLatLon geoBottomRight)
{
    // Load model
    loadModel(modelName, 1, geoTopLeft, geoTopRight, geoBottomLeft, geoBottomRight);
    
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
}

void tbzEventSite::updateContent()
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

void tbzEventSite::drawContent()
{
    bool debug3D = true;
    
    // Rotate site around the x axis to change elevation angle from plan to just overhead
    float minElevation = 0;
    float maxElevation = 90;
    float elevationRot = minElevation + (maxElevation - minElevation) * (y / ofGetHeight());

    // Get scale for our eventSite
    float scale = width;
    
    ofPushMatrix();
    {
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
        ofPushMatrix();
        {
            // Perform scale from centre of screen
            ofScale(scale, scale, scale);
            
            ofRotate(elevationRot, 1, 0, 0);
            
            // Rotate site around its z axis to spin with horizontal swipe
            ofRotate(ofRadToDeg(rotation), 0, 0, 1);
            
            siteModel.drawFaces();
            
            if (debug3D)
            {
                ofDrawBitmapString("TL: " + ofToString(groundTopLeft.x,2) + ", " + ofToString(groundTopLeft.y,2), modelTopLeft.x, modelTopLeft.y);
                ofDrawBitmapString("TR: " + ofToString(groundTopRight.x,2) + ", " + ofToString(groundTopRight.y,2), modelTopRight.x, modelTopRight.y);
                ofDrawBitmapString("BL: " + ofToString(groundBottomLeft.x,2) + ", " + ofToString(groundBottomLeft.y,2), modelBottomLeft.x, modelBottomLeft.y);
                ofDrawBitmapString("BR: " + ofToString(groundBottomRight.x,2) + ", " + ofToString(groundBottomRight.y,2), modelBottomRight.x, modelBottomRight.y);
            }
        }
        ofPopMatrix();
        
        ofPushMatrix();
        {
            // CAN I PUSH THE MATRIX AND APPLY TRANS_PERSPECTIVE MATRIX SO THAT GEOCOORDS WORK?
            
            ofRotate(elevationRot, 1, 0, 0);
            ofRotate(ofRadToDeg(rotation), 0, 0, 1);
            
            list<tbzSocialMessage>::iterator message;
            for (message = socialMessages.begin(); message != socialMessages.end(); message++)
            {
                ofPushMatrix();
                {
                    ofPoint modelLocation = groundToModel(message->geoLocation);
                    ofTranslate(modelLocation.x * scale, modelLocation.y * scale, 20 * scale); // 20 is a magic number standing in for desired height of messages within model
                    message->draw();
                };
            }
        }
        ofPopMatrix();
    }
    ofPopMatrix();
    

    
    if (debug3D)
    {
        ofSetColor(255, 255, 255, 255);
        ofDrawBitmapString("eventSite 0,0,r" + ofToString(ofRadToDeg(rotation)), 0 ,0);
    }
}

ofPoint tbzEventSite::groundToModel(const ofPoint &groundPoint)
{
    // TASK: Convert a ground coordinate to its equivalent coordinate in the model
    
    double xToTranslate = groundPoint.x;
    double yToTranslate = groundPoint.y;
    
    groundToModelTransform.transform(&xToTranslate, &yToTranslate);
    
    return ofPoint(xToTranslate, yToTranslate);
}