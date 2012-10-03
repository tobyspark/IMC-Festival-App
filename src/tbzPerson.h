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
#include "ofxAssimpModelLoader.h"
#include "tbzSocialMessage.h"
#include <list>

#define kTBZPerson_MaxMessagesDisplayed 3
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
    
    void            setup(string name, string modelName, ofPoint geoLocation);
    
protected:
    void            startNewMessageAnimation(Poco::SharedPtr<tbzSocialMessage> message);
    void            onNewMessageAnimationEnd(int &tweenID);
    
    ofMutex         messagesDisplayMutex;
    list< Poco::SharedPtr<tbzSocialMessage> >  messagesDisplay;
    queue< Poco::SharedPtr<tbzSocialMessage> > messagesQueue;
    
    ofxAssimpModelLoader    personModel;
    
    ofxTween        newMessageAnimPos;
    float           newMessageHeight;
    ofxEasingCubic  easing;
    
    ofxTween        locationTween;
    
};


#endif /* defined(__IMCFestivalApp__tbzPerson__) */
