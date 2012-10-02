#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxTwitter.h"

#if TARGET_OS_IPHONE
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
#endif

#include "tbzEventSite.h"
#include "tbzTweet.h"
#include "tbzScreenScale.h"
#include "tbzServerConnector.h"
#include "tbzDataLogger.h"

#define IMCFestivalApp_TwitterSearchGeo NO
#define IMCFestivalApp_TwitterSearchGeoPeriod 60
#define IMCFestivalApp_TwitterUseCached YES

#if TARGET_OS_IPHONE
class imcFestivalApp : public ofxiPhoneApp
#endif
#ifdef TARGET_OSX
class imcFestivalApp : public ofBaseApp
#endif
{
public:
       
    tbzEventSite eventSite;
    ofPoint      eventSiteOriginPermanent;
    ofPoint      eventSiteOriginDesired;
    ofxXmlSettings eventSiteSettings;
    
    ofxXmlSettings socialMessageStore;
    ofTrueTypeFont socialMessageFont;
    
    ofTrueTypeFont venueFontTitle;
    ofTrueTypeFont venueFontBody;
    
    tbzVenue*      venueFocussed;
        
    ofxTwitterSearch twitterGeo;
    
    ofxXmlSettings serverSettings;
    tbzServerConnection server;
    void            onNewSessionID(string &sessionID);
    
    tbzDataLogger   dataLogger;
    void            onDataLogFileWritten(string &filename);
    
    string          tempDirAbsolutePath;
    string          persistentDirAbsolutePath;
    
    // openFrameworks app methods
    void setup();
    void update();
    void draw();
    
    void exit();
    
    // openFrameworks multitouch app methods
#if TARGET_OS_IPHONE
    void touchDown(ofTouchEventArgs & touch);
    void touchMoved(ofTouchEventArgs & touch);
    void touchUp(ofTouchEventArgs & touch);
    void touchDoubleTap(ofTouchEventArgs & touch);
    void touchCancelled(ofTouchEventArgs & touch);
	
    void lostFocus();
    void gotFocus();
    void gotMemoryWarning();
    void deviceOrientationChanged(int newOrientation);
#endif
    
    // openFrameworks desktop app methods
#ifdef TARGET_OSX
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
#endif
    
};