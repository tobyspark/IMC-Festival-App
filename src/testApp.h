#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

#if TARGET_OS_IPHONE
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
#endif

#include "tbzEventSite.h"

#if TARGET_OS_IPHONE
class testApp : public ofxiPhoneApp
#endif
#ifdef TARGET_OSX
class testApp : public ofBaseApp
#endif
{
public:
    
    tbzEventSite eventSite;
    ofxXmlSettings eventSiteSettings;
    
    ofxXmlSettings socialMessageStore;
    ofTrueTypeFont socialMessageFont;
    string         socialMessageStoreFileLoc;
    
    ofTrueTypeFont venueFont;
    
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
    
private:
    void loadAndParseTwitterTestData();
    
};