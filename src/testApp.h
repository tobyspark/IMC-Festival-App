#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
#include "ofxXmlSettings.h"
#include "ofMain.h"

#include "tbzEventSite.h"

class testApp : public ofxiPhoneApp
{
	public:

		void setup();
		void update();
		void draw();

        void exit();
    
        void touchDown(ofTouchEventArgs & touch);
        void touchMoved(ofTouchEventArgs & touch);
        void touchUp(ofTouchEventArgs & touch);
        void touchDoubleTap(ofTouchEventArgs & touch);
        void touchCancelled(ofTouchEventArgs & touch);
	
        void lostFocus();
        void gotFocus();
        void gotMemoryWarning();
        void deviceOrientationChanged(int newOrientation);

        tbzEventSite eventSite;
        ofxXmlSettings eventSiteSettings;
        
        ofxXmlSettings socialMessageStore;
        ofTrueTypeFont socialMessageFont;
        string         socialMessageStoreFileLoc;
    
        ofLight light;
        
        bool flipFlop;
        
    private:
        void loadAndParseTwitterTestData();
        
};

