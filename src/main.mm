#include "ofMain.h"
#include "testApp.h"

/*
 TODO:
    - Opening animation zooming out to whole site
    - Change rendering of venues and people tags to a depth sorted list
    - KMZ code to write out parsed results to separate XML file in writable location
    - XML code to check for this separate file and merge into in-memory xml before passing to venue / person setup
    - tweets and twitter max_id cached
    - TestFlight integration (again obj-c in of setup)
    
 
 BUGS:
    - twitter not working as part of person instance?
    - mangling of (non)unicode eventSiteXML when saving back parsed XML values. Fixes in to-do list.
    - venue tags when in plan view - depth fighting. could turn off gldepthtest, sort once per modelpos, and draw venues ordered back to front.
 
 
 
 NOTES:
    "ofxFBOTexture with Fonts showing as a white box" - http://forum.openframeworks.cc/index.php?topic=3805.0
 
 
 
 
 
 
 */



#if TARGET_OS_IPHONE

//========================================================================
int main( ){
    
	ofAppiPhoneWindow * iOSWindow = new ofAppiPhoneWindow();
	
    iOSWindow->enableAntiAliasing(4);
	
	iOSWindow->enableRetinaSupport();
    
    iOSWindow->enableDepthBuffer();
    
    ofSetupOpenGL(iOSWindow, 480, 320, OF_FULLSCREEN);
    
	ofRunApp( new imcFestivalApp());
}

#endif


#ifdef TARGET_OSX

#include "ofAppGlutWindow.h"

//--------------------------------------------------------------
int main(){
	ofAppGlutWindow window; // create a window
	// set width, height, mode (OF_WINDOW or OF_FULLSCREEN)
	ofSetupOpenGL(&window, 1024, 768, OF_WINDOW);
	ofRunApp(new imcFestivalApp()); // start the app
}

#endif


