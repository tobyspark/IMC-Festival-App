#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    #include "tbzPlatformDefineTests.h"
        
    bool success = false;
    
    // TASK: Load our eventSite event details from the app bundle
    success = eventSiteSettings.loadFile("eventSiteSettings.xml");
    
    if (!success) ofLog(OF_LOG_WARNING, "Failed to load eventSiteSettings.xml");
    
    string modelName = eventSiteSettings.getValue("modelName", "eventSiteModelDefault.dae");
    
    eventSiteSettings.pushTag("model");
        ofxLatLon geoTopLeft(       eventSiteSettings.getValue("topLeftCorner:latitude", "51°32'23.03\"N"),
                                    eventSiteSettings.getValue("topLeftCorner:longitude", "0° 2'27.37\"W"));
        ofxLatLon geoTopRight(      eventSiteSettings.getValue("topRightCorner:latitude", "51°32'23.03\"N"),
                                    eventSiteSettings.getValue("topRightCorner:longitude", "0° 1'41.31\"W"));
        ofxLatLon geoBottomLeft(    eventSiteSettings.getValue("bottomLeftCorner:latitude", "51°32'1.96\"N"),
                                    eventSiteSettings.getValue("bottomLeftCorner:longitude", "0° 2'27.37\"W"));
        ofxLatLon geoBottomRight(   eventSiteSettings.getValue("bottomRightCorner:latitude", "51°32'1.96\"N"),
                                    eventSiteSettings.getValue("bottomRightCorner:longitude", "0° 1'41.31\"W"));
    eventSiteSettings.popTag();
    
    // Load our event site 3D model in.
    eventSite.setup(modelName, geoTopLeft, geoTopRight, geoBottomLeft, geoBottomRight);
    
    // TASK: Configure rendering
    
    // we're rendering a true 3D scene, depth is by position not rendering order
    glEnable(GL_DEPTH_TEST);
    
    //  we need GL_TEXTURE_2D for our models coords.
    ofDisableArbTex();
    
//    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
//    glAlphaFunc ( GL_GREATER, 0.5) ;
//    glEnable ( GL_ALPHA_TEST ) ;
    
    // TASK: Load in previously stored social messages, ie tweets and possibly facebook status updates
    socialMessageStoreFileLoc = "socialMessageStore.xml";
    
    #if TARGET_OS_IPHONE
    // While we can read from oF app's data dir, we can only read/write to documents dir in iOS
    socialMessageStoreFileLoc = ofxiPhoneGetDocumentsDirectory() + "socialMessageStore.xml";
    #endif
    
    success = socialMessageStore.loadFile(socialMessageStoreFileLoc);
    
    if (!success)
    {
        ofLog(OF_LOG_WARNING, "No existing social message store found");
        loadAndParseTwitterTestData();
    }
    
    int count = socialMessageStore.getNumTags("message");
    ofLog(OF_LOG_VERBOSE, "On startup, socialMessageStore has " + ofToString(count) + " entries");
    
    socialMessageFont.setGlobalDpi(72);
    socialMessageFont.loadFont("Arial Narrow.ttf", 16, true, true);
}

//--------------------------------------------------------------
void testApp::update()
{
    // TASK: Cycle through social messages
    int indexToDisplay = ofGetFrameNum() % socialMessageStore.getNumTags("message");

    socialMessageStore.pushTag("message", indexToDisplay);
    {
        tbzSocialMessage socialMessage(socialMessageStore.getValue("text", ""), socialMessageStore.getValue("latitude", 0.0f), socialMessageStore.getValue("longitude", 0.0f));
        socialMessage.font = &socialMessageFont;
        eventSite.socialMessages.push_front(socialMessage);
    }
    socialMessageStore.popTag();
    
    if (eventSite.socialMessages.size() > 10)
    {
        eventSite.socialMessages.pop_back();
    }
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofBackground(50, 50, 50, 0);

    // Draw 3D axes in centre of screen
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    ofColor(255,0,0);
    ofLine(0, 0, 0, 100, 0, 0);
    ofColor(0,255,0);
    ofLine(0, 0, 0, 0, 100, 0);
    ofColor(0,0,255);
    ofLine(0, 0, 0, 0, 0, 100);
    ofPopMatrix();
    
	eventSite.render();

    ofSetColor(255, 255, 255, 255);
    //ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate(), 2), 10, 15);
    socialMessageFont.drawString("fps: " + ofToString(ofGetFrameRate(), 2), 10, 15);

}

//--------------------------------------------------------------
void testApp::exit(){
    
}

#if TARGET_OS_IPHONE

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs & touch)
{
	eventSite.actionTouchDown(touch.x, touch.y, touch.id);
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs & touch)
{
    eventSite.actionTouchMoved(touch.x, touch.y, touch.id);
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs & touch)
{
    eventSite.actionTouchUp(touch.x, touch.y, touch.id);
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs & touch){

    if (flipFlop)
    {
        glShadeModel(GL_SMOOTH);
        light.enable();
        ofEnableSeparateSpecularLight();
    }
    else
    {
        glShadeModel(GL_SMOOTH);
        light.disable();
        ofDisableSeparateSpecularLight();
    }
    
    flipFlop = !flipFlop;
}

//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void testApp::lostFocus(){
    
}

//--------------------------------------------------------------
void testApp::gotFocus(){
    
}

//--------------------------------------------------------------
void testApp::gotMemoryWarning(){
    
}

//--------------------------------------------------------------
void testApp::deviceOrientationChanged(int newOrientation){
    
}

#endif

#ifdef TARGET_OSX

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    if (flipFlop)
    {
        glShadeModel(GL_SMOOTH);
        light.enable();
        ofEnableSeparateSpecularLight();
    }
    else
    {
        glShadeModel(GL_SMOOTH);
        light.disable();
        ofDisableSeparateSpecularLight();
    }
    
    flipFlop = !flipFlop;
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{
    eventSite.actionTouchMoved(x, y, 0);
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
    eventSite.actionTouchDown(x, y, 0);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{
    eventSite.actionTouchUp(x, y, 0);
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){
    
}

#endif

void testApp::loadAndParseTwitterTestData()
{
    // TASK: Read in Twitter Test Data from XML and if geolocated within eventSite add it to our message store
    ofLog(OF_LOG_VERBOSE, "Loading in twitter test data from XML dump");
    
    // The XML data dump looks like
    //    <?xml version="1.0"?>
    //    
    //    <!--
    //    -
    //    - Sequel Pro XML dump
    //    - Version 3408
    //    -
    //    - http://www.sequelpro.com/
    //    - http://code.google.com/p/sequel-pro/
    //    -
    //    - Host: quark (MySQL 5.0.95)
    //    - Database: DCS318
    //    - Generation Time Time: 2012-08-22 16:30:40 +0000
    //    -
    //    -->
    //    
    //    <DCS318>
    //    
    //        <custom>
    //        <row>
    //        <id>99630789989371904</id>
    //        <text>Wind 0.6 mph N. Barometer 1008.7 mb, Falling. Temperature 12.4 Â°C. Rain today 0.0 mm. Humidity 74%</text>
    //        <favorited>False</favorited>
    //        <truncated>False</truncated>
    //        <source>&lt;a href=&quot;http://sandaysoft.com/&quot; rel=&quot;nofollow&quot;&gt;Sandaysoft Cumulus&lt;/a&gt;</source>
    //        <in_reply_to_screen_name>None</in_reply_to_screen_name>
    //        <geo>{'type': 'Point', 'coordinates': [53.055277779999997, -2.3011111099999999]}</geo>
    //        <in_reply_to_status_id>0</in_reply_to_status_id>
    //        <in_reply_to_user_id>0</in_reply_to_user_id>
    //        <userid>165919629</userid>
    //        <created_at>2011-08-06 00:00:06</created_at>
    //        </row>
    
    bool success;
    
    ofxXmlSettings xml;
    xml.setVerbose(true);
    
    success = xml.loadFile("twitter-testData.xml");
    if (success)
    {
        bool addedMessage = false;
        
        xml.pushTag("DCS318");
        xml.pushTag("custom");
        
        int maxIndex = xml.getNumTags("row") - 1;
        ofLog(OF_LOG_VERBOSE, "Parsing " + ofToString(maxIndex+1) + " tweets");
        
        for (int i = maxIndex; i >= 0; i--)
        {
            xml.pushTag("row", i);
            {
                // TASK: Our test for inclusion is whether in geographic bounds
                
                // This is a hack to read the JSON that forms this value in this database.
                // The format to parse is {'type': 'Point', 'coordinates': [54.071950999999999, -2.8640210000000002]}
                // Not using a JSON library as XML can be used to pull from Twitter live, and we've already got XML parsing incorporated.
                stringstream geo(xml.getValue("geo", ""));
                
                string tempString;
                getline(geo, tempString, '[');
                
                getline(geo, tempString, ',');
                stringstream latitudeString(tempString);
                
                getline(geo, tempString, ']');
                stringstream longitudeString(tempString);
                
                float latitude, longitude;
                longitudeString >> longitude;
                latitudeString >> latitude;
                
                if (eventSite.groundBounds.inside(longitude, latitude))
                {
                    // Create our message data structure
                    socialMessageStore.addTag("message");
                    socialMessageStore.pushTag("message", socialMessageStore.getNumTags("message") - 1);
                    {
                        // Note: Twitter IDs can exceed max int size, so keep IDs as strings.
                        socialMessageStore.setValue("text", xml.getValue("text", ""));
                        socialMessageStore.setValue("latitude", latitude);
                        socialMessageStore.setValue("longitude", longitude);
                        socialMessageStore.setValue("twitter:id", xml.getValue("id", ""));
                        socialMessageStore.setValue("twitter:userid", xml.getValue("userid", ""));
                        socialMessageStore.setValue("twitter:in_reply_to_status_id", xml.getValue("in_reply_to_status_id", ""));
                        socialMessageStore.setValue("twitter:in_reply_to_user_id", xml.getValue("in_reply_to_user_id", ""));
                    }
                    socialMessageStore.popTag();
                    
                    addedMessage = true;
                    ofLog(OF_LOG_VERBOSE, "Added test tweet " + ofToString(i) + ", " + xml.getValue("text", ""));
                }
            }
            xml.popTag();
            
            // Now we've used it, lets clear it to reclaim memory. We can do this without indexes being affected as we're counting down not up.
            //xml.clearTagContents("row", i); //actually, lets not, it mysteriously crashes after some 10k entries
            
        }
        
        if (addedMessage) socialMessageStore.saveFile(socialMessageStoreFileLoc);
        
    }
    else
    {
        ofLog(OF_LOG_WARNING, "Failed to load twitter test data from XML");
    }
}