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
    ofEnableSeparateSpecularLight();
    light.setSpecularColor(ofColor(255.f, 255.f, 255.f)); // White
    light.setDiffuseColor( ofColor(255.f, 255.f, 200.f)); // A slight golden tone, sun!
    light.setPosition(0, 0, 0); // FIXME: This needs to be positioned properly. assumed 0,0, +- a large number 
    light.enable();
    
    // Set opening animation
    elevationFactor = 0.0f;
    width = 5.0f;
    siteAnimation.addKeyFrame( Playlist::Action::tween(5000.0f, &width, 1, Playlist::TWEEN_CUBIC, TWEEN_EASE_OUT));
}

void tbzEventSite::addVenue(Poco::SharedPtr<tbzVenue> venue)
{
    venue->tag.fontTitle = venueTitleFont;
    venue->tag.fontBody = venueBodyFont;
    venue->tag.setStyle(tbzScreenScale::retinaScale * 8, tbzScreenScale::retinaScale * 10, venueForeColour, venueForeColour*0.8, venueBackColour);
    venues.push_back(venue);
    
    tbzVenueAndDist venueAndDist;
    venueAndDist.venue = venue;
    venueAndDist.distance = 0.0f;
    venuesDistanceFromOriginSorted.push_back(venueAndDist);
    venuesDistanceSort();
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
        cout << "popping punter" << endl;
        punters.pop_back();
    }
}

void tbzEventSite::addMessageToPunters(Poco::SharedPtr<tbzSocialMessage> message)
{
    message->tag.fontTitle = personTitleFont;
    message->tag.fontBody = personBodyFont;
    message->tag.setStyle(tbzScreenScale::retinaScale * 5, tbzScreenScale::retinaScale * 10, personForeColour, personForeColour, personBackColour);
    
    // TASK: Assign message to person. 
    
    // Attempt to find person message is attributed to
    list< Poco::SharedPtr<tbzPerson> >::iterator person;
    for (person = punters.begin(); person != punters.end(); ++person)
    {
        if ((*person)->name.compare(message->attributeTo) == 0)
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
        newPerson->geoLocation = *(message->geoLocation);
        newPerson->addMessage(message);
        
        addPunter(newPerson);
    }
}

void tbzEventSite::addMessageToPromoters(Poco::SharedPtr<tbzSocialMessage> message)
{
    message->tag.fontTitle = promoterTitleFont;
    message->tag.fontBody = promoterBodyFont;
    message->tag.setStyle(tbzScreenScale::retinaScale * 5, tbzScreenScale::retinaScale * 10, promoterForeColour, promoterForeColour, promoterBackColour);
    
    // TODO: Assign to correct promoter
    promoters.front()->addMessage(message);
}

tbzVenue* tbzEventSite::nearestVenueTest(float &distance)
{
    tbzVenue* venuePointer = NULL;
    
    tbzVenueAndDist venueAndDist;
    venueAndDist = venuesDistanceFromOriginSorted.front();

    if (venueAndDist.distance < distance)
    {
        venuePointer = venueAndDist.venue.get();
        distance = venueAndDist.distance;
    }
    
    return venuePointer;
}

bool tbzEventSite::actionTouchHitTest(float _x, float _y)
{
    // We're ignoring the 2D rect inheritance, and spinning a 3D model using all the screen as gesture space.
    return true;
}

void tbzEventSite::setOrigin(ofPoint origin)
{
    originAsSet = origin;
    originDesired = origin;
}

void tbzEventSite::updateContent()
{
    float scaleDelta = width - scale;
    scale = width;
    
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
    
    // Adjust centre so scaling happens about centre of screen not centre of eventSite
    x *= 1 + scaleDelta/width;
    y *= 1 + scaleDelta/width;
    
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
    planFactor = 1.0f - elevationFactor;
    
    // Determine state from elevation factor
    if (elevationFactor < 0.001f)           viewState = planView;
    else if (elevationFactor < 0.999f)
    {
        if (elevationDifference > 0)        viewState = transitioningToElevationView;
        if (elevationDifference < 0)        viewState = transitioningToPlanView;
    }
    else                                    viewState = sideElevationView;
    
    // Determine if state has changed
    viewStateChanged = (lastViewState != viewState);
    lastViewState = viewState;
    
    // If we're dragging, then our venue distances from origin need to be recalculated
    if (state == DRAGGING)
    {
        venuesDistanceSort();
    }

    //// TASK: Update venues, this tasks their animation etc.
    list< Poco::SharedPtr<tbzVenue> >::iterator venue;
    for (venue = venues.begin(); venue != venues.end(); venue++)
    {
        (*venue)->update();
    }

    //// TASK: Update people, this tasks their animation etc.
    list< Poco::SharedPtr<tbzPerson> >::iterator person;
    for (person = punters.begin(); person != punters.end(); person++)
    {
        (*person)->update();
    }
    for (person = promoters.begin(); person != promoters.end(); person++)
    {
        (*person)->update();
    }
    
    //// TASK: Translate between graphical rendering for plan view and lit rendering for elevated
    ofColor whiteIfPlanView((1.0f - elevationFactor) * 255);
    light.setAmbientColor(whiteIfPlanView);
    
    
    //// TASK: Update site animation
    siteAnimation.update();
    
    
    /* TASK: Act on app State
     *
     * Idling; displaying event site
     *  - Display venue name tags
     *  - Display people with name tags
     *  - Display social messages
     *
     * Idling; displaying focussed venue
     *  - Display venue tag with full programme
     *  - Event site repositioned if neccessary so tag fits screen
     *
     * Idling; displaying focussed person
     *  - Display person + friends with name tags
     *  - Display social messages from friends
     *
     * User is navigating event site
     *  - Venue tag pops up on being centered on screen, it is focussed
     *  - Person tag pops up being centered, it is focussed
     *  - Can't have both, one is always nearer
     */
    
    if (viewState == tbzEventSite::planView || viewState == tbzEventSite::transitioningToPlanView)
    {
        if (viewStateChanged)
        {
            originDesired = originAsSet;
        }
        
        // Test to see if a venue is over eventSite origin
        float searchRadius = ofGetWidth()*0.2f;
        float radius = searchRadius;
        tbzVenue* nearestVenue = nearestVenueTest(radius);
        
        // If we have a venue within range, "focus" on it
        if (nearestVenue)
        {
            /* Vestigal 'rollover' animation position calculation
             // If we're within hitRadius, animPos is 1, if we're at searchRadius animPos is 0
             float hitRadius = searchRadius * 0.4f;
             
             float animPos;
             if (radius > searchRadius) animPos = 0;
             if (radius > hitRadius) animPos = 1 - ((radius - hitRadius) / (searchRadius - hitRadius));
             else animPos = 1;
             */
            
            // We have a new venue
            if (nearestVenue != venueFocussed)
            {
                // Deselect old venue
                if (venueFocussed != NULL) venueFocussed->setTagTextType(tbzVenue::nothing);
                
                // Select new
                venueFocussed = nearestVenue;
            }
            
            // Ensure selected venue is showing now and next
            venueFocussed->setTagTextType(tbzVenue::nowAndNext);
        }
        // If we don't, but one is still focussed, deselect
        else if (venueFocussed != NULL)
        {
            venueFocussed->setTagTextType(tbzVenue::nothing);
            venueFocussed = NULL;
        }
    }
    
    if (viewState == tbzEventSite::sideElevationView || viewState == tbzEventSite::transitioningToElevationView)
    {
        if (viewStateChanged)
        {
            if (venueFocussed)
            {
                venueFocussed->setTagTextType(tbzVenue::programme);
                
                // If displayed programme goes off top of screen, move the whole thing down
                float tagHeight = venueFocussed->tag.getBounds().height;
                if (tagHeight > originCurrent.y)
                {
                    originDesired.y = tagHeight - tbzScreenScale::retinaScale * 20; // TODO: need to work out offset to make this fit just so. Arrow height?
                }
            }
        }
    }
    
    
    // Animate site origin in same manner as eventSite elevation tweening
    if (originDesired != originCurrent)
    {
        originCurrent += (originDesired - originCurrent) * kTBZES_Damping;
    }
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
        ofTranslate(originCurrent);
        
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
            
            //// TASK: Draw VENUES
            
            // If we're in plan view, we are selecting the venue that is closest to origin
            // Render strategy is to ignore 3D depth and render a stack ordered by distance so the nearest is on top.
            // Interaction works best when we fade in nearest venues - declutters and reinforces "pan around, highlight what at centre"
            if (viewState == tbzEventSite::planView || viewState == tbzEventSite::transitioningToPlanView)
            {
                glDisable(GL_DEPTH_TEST);
                
                list< tbzVenueAndDist >::reverse_iterator venueAndDist;
                for (venueAndDist = venuesDistanceFromOriginSorted.rbegin(); venueAndDist != venuesDistanceFromOriginSorted.rend(); ++venueAndDist)
                {
                    ofPushMatrix();
                    ofPushStyle();
                    {
                        ofPoint modelLocation = groundToModel(venueAndDist->venue->stageGeoLocation); // TODO: This should be cached somehow, no point in recaculating every frame
                        ofTranslate(modelLocation.x * scale, modelLocation.y * scale, kTBZES_VenueTagElevationHeight * scale);
                        ofRotate(-elevationAngle, 1, 0, 0);
                        if (venueAndDist->venue->getTagTextType() == tbzVenue::nothing)
                        {
                            float nearFadeIn = ofMap(venueAndDist->distance, ofGetWidth()*0.3f, ofGetWidth()*0.2f, 0, 255);
                            ofSetColor(255, nearFadeIn * (planFactor));
                        }
                        venueAndDist->venue->drawTag();
                    }
                    ofPopStyle();
                    ofPopMatrix();
                }
                
                glEnable(GL_DEPTH_TEST);
            }
            // If we're in elevated view, we want to render in proper 3D space alongside people etc.
            // We're going to animate up and fade in
            // Caveat - need to animate between our venue tag displayed as 2D planar graphic on screen and true 3D angle
            // Caveat - need to render the venue displaying its programme on top of any others, as continuation of plan view programme display
            else
            {
                if (venueFocussed)
                {
                    ofPushMatrix();
                    {
                        ofPoint modelLocation = groundToModel(venueFocussed->stageGeoLocation);
                        ofTranslate(modelLocation.x * scale, modelLocation.y * scale, kTBZES_VenueTagElevationHeight * scale * elevationFactor);
                        ofRotate(-elevationAngle, 1, 0, 0);
                        
                        venueFocussed->drawTag();
                    }
                    ofPopMatrix();
                }
                else
                {
                    list< Poco::SharedPtr<tbzVenue> >::iterator venue;
                    ofPushStyle();
                    {
                        ofSetColor(255, elevationFactor*255);
                        for (venue = venues.begin(); venue != venues.end(); ++venue)
                        {
                            ofPushMatrix();
                            {
                                ofPoint modelLocation = groundToModel((*venue)->stageGeoLocation); // TODO: This should be cached somehow, no point in recaculating every frame
                                ofTranslate(modelLocation.x * scale, modelLocation.y * scale, kTBZES_VenueTagElevationHeight * scale * elevationFactor);
                                
                                ofRotate((planFactor * -elevationAngle) + (elevationFactor * -90.0f), 1, 0, 0);
                                
                                (*venue)->drawTag();
                            }
                            ofPopMatrix();
                        }
                    }
                    ofPopStyle();
                }
            }
            
            //// TASK: Draw People
            
            if (viewState != planView && !venueFocussed)
            {
                float personScale = scale / maxSize; // maxSize is max scale, part of co-opting MSAIntObj.width for scale.
                
                list< Poco::SharedPtr<tbzPerson> >::iterator person;
            
                for (person = promoters.begin(); person != promoters.end(); ++person)
                {
                    ofPushMatrix();
                    {
                        ofPoint modelLocation = groundToModel((*person)->geoLocation);
                        ofTranslate(modelLocation.x * scale, modelLocation.y * scale, kTBZES_PersonTagElevationHeight * scale * elevationFactor);
                        
                        ofRotate(-90, 1, 0, 0);
                        
                        ofScale(personScale, personScale, 1.0f);
                        
                        (*person)->draw(elevationFactor);
                    }
                    ofPopMatrix();
                }
            
                for (person = punters.begin(); person != punters.end(); ++person)
                {
                    ofPoint modelLocation = groundToModel((*person)->geoLocation);
                    if (modelLocation.y * scale > -y) continue; // don't draw if inbetween origin area and viewpoint
                    
                    ofPushMatrix();
                    {
                        ofTranslate(modelLocation.x * scale, modelLocation.y * scale, kTBZES_PersonTagElevationHeight * scale);
                        
                        ofRotate(-90, 1, 0, 0);
                        
                        ofScale(personScale, personScale, 1.0f);
                        
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

bool tbzVenueAndDist::operator < (tbzVenueAndDist comp)
{
    return distance < comp.distance;
}

void tbzEventSite::venuesDistanceSort()
{
    list< tbzVenueAndDist >::iterator venueAndDist;
    for (venueAndDist = venuesDistanceFromOriginSorted.begin(); venueAndDist != venuesDistanceFromOriginSorted.end(); ++venueAndDist)
    {
        ofPoint venueModelPoint = groundToModel(venueAndDist->venue->stageGeoLocation);
        venueModelPoint *= scale;
        
        venueAndDist->distance = venueModelPoint.distance(ofPoint(-x, -y));
    }
    
    venuesDistanceFromOriginSorted.sort();
}

bool tbzEventSite::loadModel(string modelName, float initialSize, ofxLatLon geoTopLeft, ofxLatLon geoTopRight, ofxLatLon geoBottomLeft, ofxLatLon geoBottomRight)
{
    // We're using SketchUp, but...
    // A post on C4D to OF model making: http://lab.arn.com/2011/10/cinema-4d-to-openframeworks-workflow/
    
    bool success = false;
    
    if(siteModel.loadModel(modelName, true))
    {
        siteModel.setAnimation(0);
//        siteModel.enableTextures();
//        siteModel.enableNormals();
//        siteModel.enableColors();
//        siteModel.enableMaterials();
        
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