#include "ofMain.h"
#include "testApp.h"

/*
 TODO:
 
 
 
 
 BUGS:
    
 
 
 
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


