//
//  tbzEventSite.cpp
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 16/08/2012.
//
//

#include "tbzEventSite.h"



void tbzEventSite::setup(string modelName, ofxLatLon geoTopLeft, ofxLatLon geoTopRight, ofxLatLon geoBottomLeft, ofxLatLon geoBottomRight)
{
    // Load model
    loadModel(modelName, 1, geoTopLeft, geoTopRight, geoBottomLeft, geoBottomRight);
    
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
    
    // Create our lights
    ofSetSmoothLighting(true);
    light.setSpecularColor(ofColor(255.f, 255.f, 255.f)); // White
    light.setDiffuseColor( ofColor(255.f, 255.f, 200.f)); // A slight golden tone, sun!
    light.setPosition(0, 0, 0); // FIXME: This needs to be positioned properly. assumed 0,0, +- a large number 
    light.enable();
    
    // Set opening animation
    elevationFactor = 0;
    width = 5.0f;
    siteAnimation.addKeyFrame( Playlist::Action::tween(5000.0f, &width, 1, Playlist::TWEEN_CUBIC, TWEEN_EASE_OUT));
    
}

void tbzEventSite::addVenue(tbzVenue venue)
{
    venues.push_back(venue);
}

void tbzEventSite::addPromoter(Poco::SharedPtr<tbzPerson> person)
{
    promoters.push_front(person);
}

void tbzEventSite::addPunter(Poco::SharedPtr<tbzPerson> person)
{
    punters.push_front(person);
    
    // TASK: Limit number of punters displayed, can't visualise everything for ever, both aesthetically and in resources.
    while (punters.size() > kTBZES_MaxPunters)
    {
        punters.pop_back();
    }
}

void tbzEventSite::addMessage(Poco::SharedPtr<tbzSocialMessage> message)
{
    // TASK: Assign message to person. 
    
    // Attempt to find person message is attributed to
    list< Poco::SharedPtr<tbzPerson> >::iterator person;
    for (person = punters.begin(); person != punters.end(); ++person)
    {
        if (!(*person)->name.compare(message->attributeTo))
        {
            break;
        }
    }
    
    // If we've found that person, give them the message
    if (person != punters.end())
    {
        (*person)->addMessage(message);
    }
    // If not, handle the addition of a new person with a new message
    else
    {
        Poco::SharedPtr<tbzPerson> newPerson = new tbzPerson;
        newPerson->name = message->attributeTo;
        newPerson->addMessage(message);
        
        addPunter(newPerson);
    }
}

tbzVenue* tbzEventSite::nearestVenue(float &distance)
{
    tbzVenue*   venuePointer = NULL;
    
    list<tbzVenue>::iterator venue;
    for (venue = venues.begin(); venue != venues.end(); venue++)
    {
        ofPoint venueModelPoint = groundToModel(venue->stageGeoLocation);
        venueModelPoint *= width; // width is co-opted for our model scale var
        
        float venueDistance = venueModelPoint.distance(ofVec3f(-x,-y));
        
        if (venueDistance < distance)
        {
            venuePointer = &(*venue);
            distance = venueDistance;
        }
    }
    
    return venuePointer;
}

bool tbzEventSite::updateViewState(ViewState &vs)
{
    bool changed = (lastViewState != viewState);

    vs = viewState;
    lastViewState = viewState;
    
    return changed;
}

bool tbzEventSite::actionTouchHitTest(float _x, float _y)
{
    // We're ignoring the 2D rect inheritance, and spinning a 3D model using all the screen as gesture space.
    return true;
}

void tbzEventSite::updateContent()
{
    float scale = width;
    
    //// TASK: Respond to interaction: clamp position of model so it can't be dragged offscreen etc.
    
    // Bound Y such that the model cannot leave the centreline of screen
    // This ensures our elevation view should stay centered on a part of the model
    y = max(y, modelTopLeft.y*scale);
    y = min(y, modelBottomLeft.y*scale);
    
//    // Bound x to within screen given scale factor
//    // if scale = 1, bounds are screenwidth/2,screenwidth/2
//    // if scale = 2, bounds are 0, screenwidth
//    x = max(x, ofGetWidth() - scale*ofGetWidth()/2);
//    x = min(x, scale*ofGetWidth()/2);
    
    //// TASK: Respond to interaction: if we're dragging, elevate to plan view.
    
    // Determine actual (damped) and desired elevation factor, 0 = plan view, 1 = side elevation
    if (state == FIXE)
    {
        elevationFactorTarget = 1.0f;
    }
    else
    {
        elevationFactorTarget = 0.0f;
    }
    
    float elevationDifference = elevationFactorTarget - elevationFactor;
    elevationFactor += elevationDifference * kTBZES_Damping;
    
    // Determine state from elevation factor
    if (elevationFactor < 0.001f)           viewState = planView;
    else if (elevationFactor < 0.999f)
    {
        if (elevationDifference > 0)        viewState = transitioningToElevationView;
        if (elevationDifference < 0)        viewState = transitioningToPlanView;
    }
    else                                    viewState = sideElevationView;

    //// TASK: Update venues, this tasks their animation etc.
    list<tbzVenue>::iterator venue;
    for (venue = venues.begin(); venue != venues.end(); venue++)
    {
        venue->update();
    }

    //// TASK: Update messages, this tasks their animation etc.
    list< Poco::SharedPtr<tbzPerson> >::iterator person;
    for (person = punters.begin(); person != punters.end(); person++)
    {
        (*person)->update();
    }
    
    //// TASK: Translate between graphical rendering for plan view and lit rendering for elevated
    ofColor whiteIfPlanView((1.0f - elevationFactor) * 255);
    light.setAmbientColor(whiteIfPlanView);
    
    
    //// TASK: Update site animation
    siteAnimation.update();
}

void tbzEventSite::drawContent()
{
    bool debug3D = false;
    
    // Get scale for our eventSite
    float scale = width;
    
    // we're rendering a true 3D scene, depth is by position not rendering order!
    glEnable(GL_DEPTH_TEST);
    
    ofPushMatrix();
    {
        // TASK: Translate to origin
        ofTranslate(origin);
        
        // TASK: Rotate model for viewing elevation
        // We want to maintain the current viewing point of the model and change elevation from plan to a view looking across model from above head height.
        // elevationFactor of 0 is no elevation, ie. stay in plan view
        // elevationFactor of 1 is max elevation
        
        // Current viewing point of model (focus) is what part of the model is lying at the screen's centre.
        // Change elevation is a rotation of the model around x axis at that point of model.
        
        // First translate vertically to where we want model focus to sit on the screen
        // ie. middle but with a bit more room for 'sky'
        ofTranslate(0, elevationFactor*(-y + ofGetHeight()*0.1));
        
        // Now rotate coord system
        float elevationAngle = elevationFactor * kTBZES_ViewElevationAngle;
        ofRotate(elevationAngle, 1, 0, 0);
        
        // Now we've rotated, shift back to yPos, which is actually in/out of screen since rotation
        ofTranslate(0, elevationFactor * y);
        
        // TASK: Draw model, coord space will be scaled
        ofPushMatrix();
        ofPushStyle();
        ofEnableLighting();
        {
            // Perform scale from centre of screen
            ofScale(scale, scale, scale);
            
            siteModel.drawFaces();
            
            if (debug3D)
            {
                ofSetColor(100, 100, 100, 255);
                ofFill();
                ofRect(modelTopLeft.x, modelTopLeft.y, modelTopRight.x - modelTopLeft.x, modelBottomLeft.y - modelTopLeft.y);
                
                ofSetColor(0, 0, 255, 255);
                ofDrawBitmapString("eventSite modelLoc", 0 ,0);
                ofDrawBitmapString("TL: " + ofToString(groundTopLeft.x,2) + ", " + ofToString(groundTopLeft.y,2), modelTopLeft.x, modelTopLeft.y);
                ofDrawBitmapString("TR: " + ofToString(groundTopRight.x,2) + ", " + ofToString(groundTopRight.y,2), modelTopRight.x, modelTopRight.y);
                ofDrawBitmapString("BL: " + ofToString(groundBottomLeft.x,2) + ", " + ofToString(groundBottomLeft.y,2), modelBottomLeft.x, modelBottomLeft.y);
                ofDrawBitmapString("BR: " + ofToString(groundBottomRight.x,2) + ", " + ofToString(groundBottomRight.y,2), modelBottomRight.x, modelBottomRight.y);
            }
        }
        ofDisableLighting();
        ofPopStyle();
        ofPopMatrix();
        
        // TASK: Draw what isn't model, coord space will not be scaled
        ofPushMatrix();
        {
            // CAN I PUSH THE MATRIX AND APPLY TRANS_PERSPECTIVE DERIVED MATRIX SO THAT GEOCOORDS WORK?
            
            list<tbzVenue>::iterator venue;
            for (venue = venues.begin(); venue != venues.end(); ++venue)
            {
                ofPushMatrix();
                {
                    ofPoint modelLocation = groundToModel(venue->stageGeoLocation); // TODO: This should be cached somehow, no point in recaculating every frame
                    ofTranslate(modelLocation.x * scale, modelLocation.y * scale, kTBZES_MessageElevationHeight * scale);
                    //ofRotate(-90, 1, 0, 0);
                    ofRotate(-elevationAngle, 1, 0, 0);
                    
                    venue->drawTag();
                }
                ofPopMatrix();
            }
            
            list< Poco::SharedPtr<tbzPerson> >::iterator person;
            for (person = promoters.begin(); person != promoters.end(); ++person)
            {
                ofPushMatrix();
                {
                    ofPoint modelLocation = groundToModel((*person)->geoLocation); // Don't cache - people move!
                    ofTranslate(modelLocation.x * scale, modelLocation.y * scale, kTBZES_MessageElevationHeight * scale);
                    
                    ofRotate(-90, 1, 0, 0);
                    
                    (*person)->draw(elevationFactor);
                }
                ofPopMatrix();
            }
            
            if (puntersDraw)
            {
                list< Poco::SharedPtr<tbzPerson> >::iterator person;
                for (person = punters.begin(); person != punters.end(); ++person)
                {
                    ofPushMatrix();
                    {
                        ofPoint modelLocation = groundToModel((*person)->geoLocation); // Don't cache - people move!
                        ofTranslate(modelLocation.x * scale, modelLocation.y * scale, kTBZES_MessageElevationHeight * scale);
                        
                        ofRotate(-90, 1, 0, 0);
                        
                        (*person)->draw(elevationFactor);
                    }
                    ofPopMatrix();
                }
            }
        }
        ofPopMatrix();
    }
    ofPopMatrix();
    
    glDisable(GL_DEPTH_TEST);
    
    if (debug3D)
    {
        ofSetColor(0, 0, 255, 255);
        ofDrawBitmapString("eventSite 0,0,r" + ofToString(ofRadToDeg(rotation),0) + " at " + ofToString(x,0) + "," + ofToString(y,0), 0 ,0);
    }
}

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

ofPoint tbzEventSite::groundToModel(const ofPoint &groundPoint)
{
    // TASK: Convert a ground coordinate to its equivalent coordinate in the model
    
    double xToTranslate = groundPoint.x;
    double yToTranslate = groundPoint.y;
    
    groundToModelTransform.transform(&xToTranslate, &yToTranslate);
    
    return ofPoint(xToTranslate, yToTranslate);
}