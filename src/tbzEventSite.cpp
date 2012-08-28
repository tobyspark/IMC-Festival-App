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
    
    // We can drag horizontally and vertically to navigate around site
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
    
    // We can tap on venues?
    setIsTappable(true);    
}

void tbzEventSite::updateContent()
{
    float scale = width;
    
    // TASK: Bound Y such that the model cannot leave the centreline of screen
    // This ensures our elevation view should stay centered on a part of the model
    y = min(y, ofGetHeight()/2 - modelTopLeft.y*scale);
    y = max(y, ofGetHeight()/2 - modelBottomLeft.y*scale);
    
//    // TASK: Bound x to within screen given scale factor
//    // if scale = 1, bounds are screenwidth/2,screenwidth/2
//    // if scale = 2, bounds are 0, screenwidth
//    x = max(x, ofGetWidth() - scale*ofGetWidth()/2);
//    x = min(x, scale*ofGetWidth()/2);
    
    // TASK: If we're dragging, elevate to plan view.
    if (state == FIXE)
    {
        elevationFactorTarget = 1.0f;
    }
    else
    {
        elevationFactorTarget = 0.0f;
    }
    
    float damping = 0.1;
    elevationFactor += (elevationFactorTarget - elevationFactor) * damping;
}

void tbzEventSite::drawContent()
{
    bool debug3D = true;
    
    // Get scale for our eventSite
    float scale = width;
    
    ofPushMatrix();
    {
        // TASK: Rotate model for viewing elevation
        // We want to maintain the current viewing point of the model and change elevation from plan to a view looking across model from above head height.
        // elevationFactor of 0 is no elevation, ie. stay in plan view
        // elevationFactor of 1 is max elevation
        
        // Current viewing point of model is what part of the model is lying at the screen's centre.
        // Change elevation is a rotation of the model around x axis at that point of model.
        
        // First translate so that point of model lies on x axis
        ofTranslate(0, elevationFactor*(-y + ofGetHeight()/2));
        
        // Now rotate
        float elevationAngle = elevationFactor * kTBZES_ElevationAngle;
        ofRotate(elevationAngle, 1, 0, 0);
        
        // BUT THIS DOESN'T QUITE WORK, I'M DAMNED IF I CAN WORK IT OUT, AND I'VE TRIED OH SO MANY ALTERNATIVE STRATEGIES ON THE WAY
        
        // TASK: Draw model, coord space will be scaled
        ofPushMatrix();
        {
            // Perform scale from centre of screen
            ofScale(scale, scale, scale);
            
            //siteModel.drawFaces();
            ofSetColor(100, 100, 100, 255);
            ofFill();
            ofRect(modelTopLeft.x, modelTopLeft.y, modelTopRight.x - modelTopLeft.x, modelBottomLeft.y - modelTopLeft.y);
            
            if (debug3D)
            {
                ofSetColor(0, 0, 255, 255);
                ofDrawBitmapString("eventSite modelLoc", 0 ,0);
                ofDrawBitmapString("TL: " + ofToString(groundTopLeft.x,2) + ", " + ofToString(groundTopLeft.y,2), modelTopLeft.x, modelTopLeft.y);
                ofDrawBitmapString("TR: " + ofToString(groundTopRight.x,2) + ", " + ofToString(groundTopRight.y,2), modelTopRight.x, modelTopRight.y);
                ofDrawBitmapString("BL: " + ofToString(groundBottomLeft.x,2) + ", " + ofToString(groundBottomLeft.y,2), modelBottomLeft.x, modelBottomLeft.y);
                ofDrawBitmapString("BR: " + ofToString(groundBottomRight.x,2) + ", " + ofToString(groundBottomRight.y,2), modelBottomRight.x, modelBottomRight.y);
            }
        }
        ofPopMatrix();
        
        // TASK: Draw what isn't model, coord space will not be scaled
        ofPushMatrix();
        {
            // CAN I PUSH THE MATRIX AND APPLY TRANS_PERSPECTIVE DERIVED MATRIX SO THAT GEOCOORDS WORK?
            
            list<tbzSocialMessage>::iterator message;
            for (message = socialMessages.begin(); message != socialMessages.end(); message++)
            {
                ofPushMatrix();
                {
                    ofPoint modelLocation = groundToModel(message->geoLocation); // TODO: This should be cached somehow, no point in recaculating every frame
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
        ofSetColor(0, 0, 255, 255);
        ofDrawBitmapString("eventSite 0,0,r" + ofToString(ofRadToDeg(rotation),0) + " at " + ofToString(x,0) + "," + ofToString(y,0), 0 ,0);
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