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
    
    if (kTBZPerson_MessageTagGrowAnimation)
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
        messagesDisplay.pop_back();
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
    // FIXME: This is polling hack as event listener is not working?
    int dummy = 0;
    if (newMessageAnimPos.isCompleted()) onNewMessageAnimationEnd(dummy);
    
    // TASK: Animate to new position
    if (locationTween.isRunning())
    {
        geoLocation.x = locationTween.update();
        geoLocation.y = locationTween.getTarget(1);
    }
    
    // TASK: Grab new tweets
    while(twitter.hasNewTweets())
    {
        ofxTweet    tweet = twitter.getNextTweet();
        tbzTweet*   tTweet = static_cast<tbzTweet*>(&tweet);
        
        cout << "Person (" << name << ") search tweet" << endl;
        cout << "text:" << tweet.getText() << endl;
        cout << "author:" << tweet.getScreenName() << endl;
        cout << "---" << endl;
        
        Poco::SharedPtr<tbzSocialMessage> message = new tbzSocialMessage(tweet.getText(), tweet.getScreenName(), "twitter", "TODO: Time");
        message->tag.fontTitle = fontTitle;
        message->tag.fontBody = fontBody;
        
        bool hasGeo;
        ofPoint geoPoint;
        
        hasGeo = tTweet->getGeoLocation(geoPoint);
        
        if (hasGeo) message->geoLocation = new ofPoint(geoPoint);
        
        addMessage(message);
    }
    
    // TASK: Update displayed messages
    list< Poco::SharedPtr<tbzSocialMessage> >::iterator message;
    for (message = messagesDisplay.begin(); message != messagesDisplay.end(); ++message)
    {
        // for now...
        (*message)->tag.update();
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
            float newMessageAnimHeight = newMessageHeight * (1.0f - newMessageAnimPos.update());
            ofTranslate(0, -animInPos + newMessageAnimHeight);
            
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
                            ofSetColor(255, 255*(1.0f - newMessageAnimPos.update()));
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
        }
    }
    ofPopMatrix();
    ofPopStyle();
}

bool tbzPerson::loadModel(string modelName)
{
    bool success = false;
    
    if(personModel.loadModel(modelName, true))
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
        
        success = true;
    }
    
    return success;
}

void tbzPerson::setupFromXML(ofxXmlSettings &xml, ofxXmlSettings &newXMLFromParsing, bool &xmlChanged, int which)
{
    /*
     
     XML we're expecting is in the form
     
     <person>
     <name>Eat Your Own Ears</name>
     <modelName>EOYE-3D.dae</modelName>
     <positionKML>EYOE-location.kmz</positionKML>
     <position>
     <longitude>-0.0317333</longitude>
     <latitude>51.5393</latitude>
     <position>
     <twitterAccount>eatyourownears</twitterAccount>
     <twitterHashtag>#FieldDay</twitterHashtag>
     </person>
     */
    
    xmlChanged = false;
    
    if (xml.pushTag("person", which))
    {
        name = xml.getValue("name", "A Person");
        
        // TASK: Setup stage geolocation
        
        if (xml.tagExists("position"))
        {
            geoLocation.x = xml.getValue("position:longitude", 0.0f);
            geoLocation.y = xml.getValue("position:latitude", 0.0f);
        }
        // We're only going to process KML on OSX, on iOS we need fast startup and so will use the parsed info put back into the XML
#ifdef TARGET_OSX
        else if (xml.tagExists("positionKML"))
        {
            bool ok = false;
            string filenameKMZ = xml.getValue("positionKML", "no filename could be read from XML");
            
            ok = pointFromKMZ(filenameKMZ, geoLocation);
            
            if (ok)
            {
                newXMLFromParsing.addTag("position");
                newXMLFromParsing.pushTag("position");
                newXMLFromParsing.addValue("longitude", geoLocation.x);
                newXMLFromParsing.addValue("latitude", geoLocation.y);
                newXMLFromParsing.popTag();
                
                xmlChanged = true;
            }
        }
#endif
        else
        {
            ofLog(OF_LOG_WARNING, "Failed to locate person: " + name);
        }

        // TASK: Setup 3D model
        
        string modelName = xml.getValue("modelName", kTBZPerson_DefaultModelName);
        if (modelName.compare(""))
        {
            loadModel(modelName);
        }
        
        
        // TASK: Setup twitter
        
        string searchTerms;
        
        if (xml.tagExists("twitterAccount"))
        {
            searchTerms += "from:" + xml.getValue("twitterAccount", "");
        }
        
        if (xml.tagExists("twitterHashtag"))
        {
            if (searchTerms.length()>0)
            {
                searchTerms += " OR ";
            }
            
            searchTerms += xml.getValue("twitterHashtag", "");
        }
        
        // If multiple people, this will somewhat avoid simultaneous fires of search
        int pollPeriod = ofRandom(kTBZPerson_TwitterPollPeriod - 4, kTBZPerson_TwitterPollPeriod + 4);
        
        list<string> searchParameters;
        if (searchTerms.length() > 0)
        {
            twitter.connect(searchTerms, searchParameters, pollPeriod);
        }
        
        // TASK: Pop back out of the person settings data before returning
        xml.popTag();
    }
    
}
