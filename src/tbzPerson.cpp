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

void tbzPerson::draw(float animPos)
{
    ofPushMatrix();
    ofPushStyle();
    {
        personModel.drawFaces();
        
        if (!messagesDisplay.empty())
        {
            ofColor fadeInAlpha(255, animPos*255.0f);
            float   animInPos = (1.0f-animPos)*kTBZPerson_AnimInDistance;

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
    }
    ofPopMatrix();
    ofPopStyle();
}


void tbzPerson::setup(string inName, string inModelName, ofPoint inGeoLocation)
{
    name = inName;
    geoLocation = inGeoLocation;
    
    if(personModel.loadModel(inModelName, true))
    {
        personModel.setAnimation(0);
        
        // TASK: Set model position and scale in bounding rect
        
        // siteModel.scale is normalised to GL units, so to fill screen width (initialsize of 1) we need a scale of 2
        float scale = 0.5f;
        personModel.setScale(scale, scale, scale);
        
        // Centre in x + y, leave z at ground level.
        //        personModel.setPosition(0 - (personModel.getSceneCenter().x * personModel.getNormalizedScale() * scale),
        //                              0 + (personModel.getSceneCenter().y * personModel.getNormalizedScale() * scale),
        //                              0
        //                              );
        
        /*
         ofLog(OF_LOG_VERBOSE,"siteModel.getSceneMin() " + ofToString(siteModel.getSceneMin().x) + ", " + ofToString(siteModel.getSceneMin().y) + ", " + ofToString(siteModel.getSceneMin().z));
         ofLog(OF_LOG_VERBOSE,"siteModel.getSceneMax() " + ofToString(siteModel.getSceneMax().x) + ", " + ofToString(siteModel.getSceneMax().y) + ", " + ofToString(siteModel.getSceneMax().z));
         ofLog(OF_LOG_VERBOSE,"siteModel.getSceneCenter() " + ofToString(siteModel.getSceneCenter().x) + ", " + ofToString(siteModel.getSceneCenter().y) + ", " + ofToString(siteModel.getSceneCenter().z));
         ofLog(OF_LOG_VERBOSE,"siteModel.getNormalisedScale()" + ofToString(siteModel.getNormalizedScale()));
         */
    }
}