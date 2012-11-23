#include "ofMain.h"
#include "testApp.h"

/*
 TODO:
    - 3D RENDERING WOES GO AWAY WHEN YOU DON'T DRAW ANY ASSIMP MODELS. ASSIMP MUST FUCK GL STATE. GAH AND WOAH.
    - Make people selectable as well as venues, to show tweets clearly.
    - Change rendering of venues and people to a depth sorted list. Only way to get alpha blending.
    - Animate around site as tweets come in
    - KMZ code to write out parsed results to separate XML file in writable location
    - XML code to check for this separate file and merge into in-memory xml before passing to venue / person setup
    - tweets and twitter max_id cached
    - TestFlight integration (again obj-c in of setup)
    - Only update people when they're drawn. Need to change tweens to playlists for this to work?
 
 BUGS:
    - mangling of (non)unicode eventSiteXML when saving back parsed XML values. Fixes in to-do list.
    - display of non-unicode text in tweets etc.
 
 
 NOTES:
    "ofxFBOTexture with Fonts showing as a white box" - http://forum.openframeworks.cc/index.php?topic=3805.0
    "Displaying material colors correctly without textures using Assimp" - http://forum.openframeworks.cc/index.php?topic=8708.0
 
 
 
 
 
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


