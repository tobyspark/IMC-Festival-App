//
//  tbzPerson.cpp
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 18/09/2012.
//
//

#include "tbzPerson.h"

tbzPerson::tbzPerson()
{
    ofAddListener(newMessageAnimPos.end_E, this, &tbzPerson::onNewMessageAnimationEnd);
    state = uninitialised;
}

tbzPerson::~tbzPerson()
{
    ofRemoveListener(newMessageAnimPos.end_E, this, &tbzPerson::onNewMessageAnimationEnd);
}

void tbzPerson::addMessage(Poco::SharedPtr<tbzSocialMessage> message)
{
    if (newMessageAnimPos.isCompleted())
    {
        startNewMessageAnimation(message);
    }
    else
    {
        messagesQueue.push(message);
    }
}

void tbzPerson::startNewMessageAnimation(Poco::SharedPtr<tbzSocialMessage> message)
{
    // Set animation parameters now our message isn't being copied any more
    newMessageAnimPos.setParameters(easing, ofxTween::easeOut, 0, 1, kTBZPerson_MessageAnimationPeriodMillis, 0);
    newMessageHeight = message->tag.getBounds().height;
    
    if (!kTBZPerson_MessageTagGrowAnimation)
    {
        message->tag.snapToTargetSize();
    }
    
    if (message->geoLocation)
    {
        locationTween.setParameters(easing, ofxTween::easeOut, geoLocation.x, message->geoLocation->x, kTBZPerson_MessageAnimationPeriodMillis, 0);
        locationTween.addValue(geoLocation.y, message->geoLocation->y);
        locationTween.start();
    }
    else
    {
        // If there isn't a location, then make sure we're back at the default location
        if (geoLocation != geoLocationDefault)
        {
            locationTween.setParameters(easing, ofxTween::easeOut, geoLocation.x, geoLocationDefault.x, kTBZPerson_MessageAnimationPeriodMillis, 0);
            locationTween.addValue(geoLocation.y, geoLocationDefault.y);
            locationTween.start();
        }
    }
    // Start tween animating
    newMessageAnimPos.start();

    // Copy message into display list
    messagesDisplay.push_front(message);
}

void tbzPerson::onNewMessageAnimationEnd(int &tweenID)
{
    // TASK: Finish the animation cycle
    // If we've gone over our display size, remove the oldest from the list
    while (messagesDisplay.size() > kTBZPerson_MaxMessagesDisplayed)
    {
        if (messagesDisplayMutex.tryLock())
        {
            messagesDisplay.pop_back();
            messagesDisplayMutex.unlock();
        }
        else
        {
            ofLog(OF_LOG_WARNING, "tbzPerson: Couldn't pop messagesDisplay. List should be updated outside of draw.");
            break;
        }
    }
    
    // TASK: Start animation cycle if needed
    // If there are messages waiting, start a new animation cycle animating in the next
    if (!messagesQueue.empty())
    {
        Poco::SharedPtr<tbzSocialMessage> message = messagesQueue.front();
        messagesQueue.pop();
        
        startNewMessageAnimation(message);
    }
}

void tbzPerson::setState(State inState)
{
    if (state != inState)
    {
        state = inState;
        
        if (inState == preview)
        {
            list<string> lines;
            stringstream summaryLine;
            
            summaryLine << messagesDisplay.size() + messagesQueue.size() << " messages";
            
            lines.push_back(summaryLine.str());
            tag.setContent(name, lines);
        }
        else if (inState == full)
        {
            // switch to showing full message stack?
        }
        else
        {
            state = nothing;
            list<string> noLines;
            tag.setContent(name, noLines);
        }
    }
}

tbzEventSiteFeature::State tbzPerson::getState()
{
    return state;
}

float tbzPerson::getTagHeight()
{
    return 0; // TODO
}

void tbzPerson::update()
{
    // TASK: Update tweens
    // Do this here so end events happen within update stage not draw
    newMessageAnimPos.update();
    
    // TASK: Animate to new position
    if (locationTween.isRunning())
    {
        geoLocation.x = locationTween.update();
        geoLocation.y = locationTween.getTarget(1);
    }
    
    // TASK: Update displayed messages
    
    tag.update();
    
    // messages only currently need to update if tag graphic itself is changing
    if (kTBZPerson_MessageTagGrowAnimation)
    {
        list< Poco::SharedPtr<tbzSocialMessage> >::iterator message;
        for (message = messagesDisplay.begin(); message != messagesDisplay.end(); ++message)
        {
            (*message)->update();
        }
    }
}

void tbzPerson::drawFeature()
{
    //model.drawFaces();
}

void tbzPerson::drawTag()
{
    ofPushMatrix();
    ofPushStyle();
    {
        switch (state) {
            case uninitialised:
                break;
            case nothing:
            case preview:
                tag.draw();
                break;
            case full:
                if (!messagesDisplay.empty())
                {
                    ofFloatColor fadeInAlpha(1.0f, transition);
                    float   animInPos = (1.0f-transition)*kTBZPerson_AnimInDistance;
                    
                    ofSetColor(fadeInAlpha);
                    float newMessageAnimHeight = newMessageHeight * (1.0f - newMessageAnimPos.getTarget(0));
                    ofTranslate(0, -animInPos + newMessageAnimHeight);
                    
                    messagesDisplayMutex.lock();
                    list< Poco::SharedPtr<tbzSocialMessage> >::iterator message;
                    for (message = messagesDisplay.begin(); message != messagesDisplay.end(); ++message)
                    {
                        if (newMessageAnimPos.isRunning()) {
                            // Is the newest message?
                            if (message == messagesDisplay.begin())
                            {
                                // Do something special to the message animating in?
                                (*message)->tag.draw();
                            }
                            // Is this the oldest message which will be removed at animation end?
                            else if (message == --messagesDisplay.end() && messagesDisplay.size() > kTBZPerson_MaxMessagesDisplayed)
                            {
                                // Fade it out
                                ofPushStyle();
                                ofSetColor(255, 255*(1.0f - newMessageAnimPos.getTarget(0)));
                                (*message)->tag.draw();
                                ofPopStyle();
                            }
                            else
                            {
                                (*message)->tag.draw();
                            }
                        }
                        else
                        {
                            (*message)->tag.draw();
                        }
                        
                        ofTranslate(0, -(*message)->tag.getBounds().height);
                    }
                    messagesDisplayMutex.unlock();
                }
                
                tag.draw();
                
                break;
        }
    }
    ofPopMatrix();
    ofPopStyle();
}


void tbzPerson::setup(string inName, ofPoint inGeoLocation, string inModelName)
{
    name = inName;
    geoLocation = inGeoLocation;
    geoLocationDefault = inGeoLocation;
    
//    if (inModelName == "")
//    {
//        // TODO: use shared default model
//    }
//    else
    {
        model.setScaleNomalization(false);
        if(model.loadModel(inModelName, true))
        {
            model.setAnimation(0);
            
            // TODO: Set scale to overall model scale
            
            // TODO: Position to 0,0 so the person can be placed according to geocoords
    //        model.setPosition(0 - (model.getSceneCenter().x * siteModelScale),
    //                              0 + (model.getSceneCenter().y * siteModelScale),
    //                              0
    //                              );
            model.setScale(1,1,1);
            
            //model.setScale(16, 16, 16);
            //model.setPosition(0 - (model.getSceneCenter().x * 16),
    //                          0 + (model.getSceneCenter().y * 16),
    //                          0
    //                          );
            
            
             ofLog(OF_LOG_VERBOSE,"siteModel.getSceneMin() " + ofToString(model.getSceneMin().x) + ", " + ofToString(model.getSceneMin().y) + ", " + ofToString(model.getSceneMin().z));
             ofLog(OF_LOG_VERBOSE,"siteModel.getSceneMax() " + ofToString(model.getSceneMax().x) + ", " + ofToString(model.getSceneMax().y) + ", " + ofToString(model.getSceneMax().z));
             ofLog(OF_LOG_VERBOSE,"siteModel.getSceneCenter() " + ofToString(model.getSceneCenter().x) + ", " + ofToString(model.getSceneCenter().y) + ", " + ofToString(model.getSceneCenter().z));
             ofLog(OF_LOG_VERBOSE,"siteModel.getNormalisedScale()" + ofToString(model.getNormalizedScale()));
             
            cout << "MODEL NAME: " << inModelName << endl;
            cout << "PERSON SCALE: " << model.getNormalizedScale() << endl;
        }
    }
    setState(nothing);
}