#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxTwitter.h"
#include "tbzTweet.h"
#include "tbzScreenScale.h"

#if TARGET_OS_IPHONE
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
#endif

#include "tbzEventSite.h"

#define IMCFestivalApp_TwitterSearchGeo NO
#define IMCFestivalApp_TwitterUseCached YES

float gRetinaScale = 1;

#if TARGET_OS_IPHONE
class imcFestivalApp : public ofxiPhoneApp
#endif
#ifdef TARGET_OSX
class imcFestivalApp : public ofBaseApp
#endif
{
public:
       
    tbzEventSite eventSite;
    ofxXmlSettings eventSiteSettings;
    
    ofxXmlSettings socialMessageStore;
    ofTrueTypeFont socialMessageFont;
    string         socialMessageStoreFileLoc;
    
    ofTrueTypeFont venueFontTitle;
    ofTrueTypeFont venueFontBody;
    
    tbzVenue*      venueFocussed;
        
    ofxTwitterSearch twitterGeo;
    
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