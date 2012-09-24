//
//  tbzPerson.h
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 18/09/2012.
//
//

#ifndef __IMCFestivalApp__tbzPerson__
#define __IMCFestivalApp__tbzPerson__

#include "ofMain.h"
#include "ofxTween.h"
#include "ofxXmlSettings.h"
#include "ofxAssimpModelLoader.h"
#include "ofxTwitter.h"
#include "tbzTweet.h"
#include "tbzKMZReader.h"
#include "tbzSocialMessage.h"
#include <list>

#define kTBZPerson_MaxMessagesDisplayed 5
#define kTBZPerson_MessageAnimationPeriodMillis 2000
#define kTBZPerson_TwitterPollPeriod 45
#define kTBZPerson_DefaultModelName ""
#define kTBZPerson_MessageTagGrowAnimation false
#define kTBZPerson_AnimInDistance 1000

class tbzPerson
{
public:
    tbzPerson();
    ~tbzPerson();
    
    string          name;
    ofPoint         geoLocation;
    
    void            addMessage(Poco::SharedPtr<tbzSocialMessage> message);
    
    void            update();
    void            draw(float animPos = 1.0f);
    
    void            setupFromXML(ofxXmlSettings &xml, ofxXmlSettings &newXMLFromParsing, bool &xmlChanged, int which = 0);
    bool            loadModel(string modelName);
    
    ofxTwitterSearch twitter;
    
    ofTrueTypeFont* fontTitle;
    ofTrueTypeFont* fontBody;
    
protected:
    void            startNewMessageAnimation(Poco::SharedPtr<tbzSocialMessage> message);
    void            onNewMessageAnimationEnd(int &tweenID);
    
    list< Poco::SharedPtr<tbzSocialMessage> >  messagesDisplay;
    queue< Poco::SharedPtr<tbzSocialMessage> > messagesQueue;
    
    ofxAssimpModelLoader    personModel;
    
    ofxTween        newMessageAnimPos;
    float           newMessageHeight;
    ofxEasingCubic  easing;
    
    ofxTween        locationTween;
    
};


#endif /* defined(__IMCFestivalApp__tbzPerson__) */
