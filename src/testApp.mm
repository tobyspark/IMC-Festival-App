#include "testApp.h"

//--------------------------------------------------------------
void imcFestivalApp::setup(){
    
    ofSetLogLevel(OF_LOG_WARNING);
    
    #include "tbzPlatformDefineTests.h"
    
    bool success = false;
    
    //// TASK: Setup Retina detection
    tbzScreenScale::detectScale();
    
    //// TASK: Setup Fonts
    // The iOS font example has 72 dpi set, but thats fugly in this app.
    ofTrueTypeFont::setGlobalDpi(96);
    
    venueFontTitle.loadFont("Arial Narrow.ttf", tbzScreenScale::retinaScale * 18, true, true);
    venueFontBody.loadFont("Arial Narrow.ttf", tbzScreenScale::retinaScale * 12, true, true);
    socialMessageFont.loadFont("Arial Narrow.ttf", tbzScreenScale::retinaScale * 12, true, true);
    
    //// TASK: Set base directory
    // iOS - this is done for us.
    // OSX - we are bundling the data dir within the app as an extra build phase
    #ifdef TARGET_OSX
        ofSetDataPathRoot("../Resources/");
    #endif
    
    //// TASK: Set temporary directory
    
    tempDirAbsolutePath = "";
    #ifdef TARGET_OSX
    NSString* tempPath = NSTemporaryDirectory();
    if (tempPath) tempDirAbsolutePath = ofxNSStringToString(tempPath);
    #endif
    
    #if TARGET_OS_IPHONE
    NSString* tempPath = NSTemporaryDirectory();
    if (tempPath) tempDirAbsolutePath = ofxNSStringToString(tempPath);
    #endif
    
    //// TASK: Set persistent directory
    
    tempDirAbsolutePath = "";
    #ifdef TARGET_OSX
    // Can just be app bundle data path for now.
    persistentDirAbsolutePath = dataPathRoot();
    #endif
    
    #if TARGET_OS_IPHONE
    // iOS is sandboxed and app store review is picky. We need to put non-user facing persistent files in app's Library folder.
    NSString* persistentPath = [NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    if (persistentPath) persistentDirAbsolutePath = ofxNSStringToString(tempPath);
    #endif
    
    //// TASK: Load and act on our eventSite event details from the app bundle
    success = eventSiteSettings.loadFile("eventSiteSettings.xml");
    
    if (!success) ofLog(OF_LOG_WARNING, "Failed to load eventSiteSettings.xml");
    
    // Use model / site data
    
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
    
    // Use venue / programme data
    
    int venueCount = eventSiteSettings.getNumTags("venue");
        
    for (int i = venueCount-1; i >= 0; i--)
    {
        tbzVenue venue;
        
        bool xmlChanged = false;
        venue.setupFromXML(eventSiteSettings, xmlChanged, i);
        venue.tag.fontTitle = &venueFontTitle;
        venue.tag.fontBody  = &venueFontBody;
        eventSite.addVenue(venue);
        
        if (xmlChanged)
        {
            bool ok = eventSiteSettings.saveFile();
            
            if (ok) ofLog(OF_LOG_NOTICE, "Added parsed KML data to eventSiteSettings.xml");
            else    ofLog(OF_LOG_WARNING, "Failed to save eventSiteSettings.xml with parsed KML data");
        }
    }
    
    // User person / social media data
    
    int peopleCount = eventSiteSettings.getNumTags("person");
    
    for (int i = peopleCount-1; i >= 0; i--)
    {
        Poco::SharedPtr<tbzPerson> person = new tbzPerson;
        
        ofxXmlSettings newXML;
        bool xmlChanged;
        
        person->setupFromXML(eventSiteSettings, newXML, xmlChanged, i);
        person->fontTitle = &socialMessageFont;
        
        eventSite.addPromoter(person);
    }
    
    // Load our event site 3D model in.
    eventSite.setup(modelName, geoTopLeft, geoTopRight, geoBottomLeft, geoBottomRight);
    
    // Set origin
    eventSiteOriginPermanent = ofPoint(ofGetWidth()/2.0f, ofGetHeight()/2.0f);
    eventSiteOriginDesired = eventSiteOriginPermanent;
    eventSite.origin = eventSiteOriginPermanent;
    
    //// TASK: Configure rendering
    
    // currently have some frame-by-frame rather than time based decays etc., so lets keep framerate consistent on OSX
    ofSetVerticalSync(true);
	ofSetFrameRate(60);
    
    //  we need GL_TEXTURE_2D for our models coords.
    ofDisableArbTex();
    
    // we need alpha blending as we have images with alpha in 3D space
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    glAlphaFunc ( GL_GREATER, 0.1);
    glEnable ( GL_ALPHA_TEST );
    
    
    
    
    //// TASK: Load in previously stored social messages, ie tweets and possibly facebook status updates
    if (IMCFestivalApp_TwitterUseCached)
    {
        string readFilePath = "socialMessageStore.xml";
        string readWriteFilePath = ofFilePath::addTrailingSlash(persistentDirAbsolutePath) + "socialMessageStore.xml";
        
        if (socialMessageStore.loadFile(readWriteFilePath))
        {
            success = true;
        }
        else
        {
            success = socialMessageStore.loadFile(readFilePath);
        }
        
        if (!success)
        {
            ofLog(OF_LOG_WARNING, "No existing social message store found");
            //loadAndParseTwitterTestData();
        }
        
        int count = socialMessageStore.getNumTags("message");
        ofLog(OF_LOG_VERBOSE, "On startup, socialMessageStore has " + ofToString(count) + " entries");
    }

    
    
    //// TASK: Startup Twitter (Geolocated search)
    
    if (IMCFestivalApp_TwitterSearchGeo)
    {
        /* API 1.0 - https://dev.twitter.com/docs/using-search
         *
         * Location data is only included if the query includes the geocode parameter, and the user Tweeted with Geo information. When conduction Geo searches, the Search API will:
         *
         * Attempt to find Tweets which have a place or lat/long within the queried geocode.
         * Attempt to find Tweets created by users whose profile location can be reverse geocoded into a lat/long within the queried geocode.
         */
        
        stringstream geoString;
        ofPoint center = eventSite.groundBounds.getCenter();
        
        // Approximate calculation of search radius
        // Get approx radius from centre to corner. This should easily encompass our event site, which is likely an irregular shape within that rect bounds.
        // Latitude: 1 deg = 110.54 km
        // Longitude: 1 deg = 111.320*cos(latitude) km
        float heightKM = eventSite.groundBounds.height * 110.54f;
        float widthKM  = eventSite.groundBounds.width * 111.320f * cos(ofDegToRad(eventSite.groundBounds.height));
        float radius   = sqrt(heightKM*heightKM + widthKM*widthKM) / 2.0f;
        
        // API 1.0: The parameter value is specified by "latitude,longitude,radius", where radius units must be specified as either "mi" (miles) or "km" (kilometers)
        geoString << "geocode=\"" << center.y << "," << center.x << "," << radius << "km\"";
        
        list<string> searchParameters;
        searchParameters.push_front(geoString.str());
        
        twitterGeo.connect("", searchParameters, IMCFestivalApp_TwitterSearchGeoPeriod);
    }
    
    
    //// TASK: Setup 'mothership' server
    
    // Load settings from XML
    {
        string readFilePath = "serverSettings.xml";
        string readWriteFilePath = ofFilePath::addTrailingSlash(persistentDirAbsolutePath) + "serverSettings.xml";
        
        serverSettings.loadFile(readFilePath);
        
        // Set our server endpoint from app supplied data (read only on iOS)
        if (serverSettings.tagExists("serverEndPoint"))
        {
            server.setEndPointURL(serverSettings.getValue("serverEndPoint", ""));
        }
        
        // Setup serverSettings to be stored in persistent read-write location.
        if (persistentDirAbsolutePath.length() > 0)
        {
            if (!serverSettings.loadFile(readWriteFilePath))
            {
                // Copy the settings file there and re-read from new location
                ofFile::copyFromTo(readFilePath, readWriteFilePath);
                serverSettings.loadFile(readWriteFilePath);
            }
        }
        
        // Load in settings updateable by our app
        if (serverSettings.tagExists("lastSessionID"))
        {
            server.setSessionID(serverSettings.getValue("lastSessionID", ""));
        }
    }
    
    // Go...
    ofAddListener(server.onNewSessionID, this, &imcFestivalApp::onNewSessionID);
    server.startSession();
    
    
    //// TASK: Setup Data Logging
    dataLogger.setup();
    dataLogger.setLogsFolder(persistentDirAbsolutePath);
    ofAddListener(dataLogger.onLogFileWritten, this, &imcFestivalApp::onDataLogFileWritten);
    
}

void imcFestivalApp::onDataLogFileWritten(string &filename)
{
    server.addFileForUpload(filename);
    
    server.startFileUploads(); // TODO: Do this after initial network activity dies down
}

void imcFestivalApp::onNewSessionID(string &sessionID)
{
    string sessionLogFolder = ofFilePath::addTrailingSlash(tempDirAbsolutePath) + sessionID;
    
    // Create it
    ofDirectory folder(sessionLogFolder);
    folder.create();
//    if (!folder.canWrite())
//    {
//        ofLog(OF_LOG_ERROR, "Cannot write logs; aborting logging");
//        return;
//    }
    
    // Use it
    server.scanFolderForUpload(sessionLogFolder);
    dataLogger.setLogsFolder(sessionLogFolder);
    
    server.startFileUploads(); // TODO: Do this after initial network activity dies down
    
    // Store it
    serverSettings.setValue("lastSessionID", sessionID);
    serverSettings.saveFile();
}

//--------------------------------------------------------------
void imcFestivalApp::update()
{
    /* TASK: Act on app State
     *
     * Idling; displaying event site
     *  - Display venue name tags
     *  - Display people with name tags
     *  - Display social messages
     *
     * Idling; displaying focussed venue
     *  - Display venue tag with full programme
     *  - Event site repositioned if neccessary so tag fits screen
     *
     * Idling; displaying focussed person
     *  - Display person + friends with name tags
     *  - Display social messages from friends
     *
     * User is navigating event site
     *  - Venue tag pops up on being centered on screen, it is focussed
     *  - Person tag pops up being centered, it is focussed
     *  - Can't have both, one is always nearer
     */
    
    tbzEventSite::ViewState state;
    bool stateChanged;
    stateChanged = eventSite.updateViewState(state);
    
    if (state == tbzEventSite::planView || state == tbzEventSite::transitioningToPlanView)
    {
        if (stateChanged)
        {
            eventSiteOriginDesired = eventSiteOriginPermanent;
        }
        
        // Test to see if a venue is over eventSite origin
        float searchRadius = ofGetWidth()*0.2f;
        float radius = searchRadius;
        tbzVenue* nearestVenue = eventSite.nearestVenue(radius);
        
        // If we have a venue within range, "focus" on it
        if (nearestVenue)
        {
            /* Vestigal 'rollover' animation position calculation
            // If we're within hitRadius, animPos is 1, if we're at searchRadius animPos is 0
            float hitRadius = searchRadius * 0.4f;
            
            float animPos;
            if (radius > searchRadius) animPos = 0;
            if (radius > hitRadius) animPos = 1 - ((radius - hitRadius) / (searchRadius - hitRadius));
            else animPos = 1;
            */
            
            // We have a new venue
            if (nearestVenue != venueFocussed)
            {
                // Deselect old venue
                if (venueFocussed != NULL) venueFocussed->setTagTextType(tbzVenue::nothing);
                
                // Select new
                venueFocussed = nearestVenue;
            }
            
            // Ensure selected venue is showing now and next
            venueFocussed->setTagTextType(tbzVenue::nowAndNext);
        }
        // If we don't, but one is still focussed, deselect
        else if (venueFocussed != NULL)
        {
            venueFocussed->setTagTextType(tbzVenue::nothing);
            venueFocussed = NULL;
        }
    }

    if (state == tbzEventSite::sideElevationView || state == tbzEventSite::transitioningToElevationView)
    {
        if (stateChanged)
        {
            if (venueFocussed)
            {
                venueFocussed->setTagTextType(tbzVenue::programme);
                eventSite.puntersDraw = false;
                
                // If displayed programme goes off top of screen, move the whole thing down
                float tagHeight = venueFocussed->tag.getBounds().height;
                if (tagHeight > eventSite.origin.y)
                {
                    eventSiteOriginDesired.y = tagHeight; // TODO: need to work out offset to make this fit just so
                }
            }
            else
            {
                eventSite.puntersDraw = true;
            }
        }

    }
    
    
    // Animate site origin in same manner as eventSite elevation tweening
    if (eventSiteOriginDesired != eventSite.origin)
    {
        eventSite.origin += (eventSiteOriginDesired - eventSite.origin) * kTBZES_Damping;
    }
    
    /* TASK: Update data logger
     *
     */
    
    // TODO: If Opted-in
    dataLogger.update();
    
    
    /* TASK: Ingest any new social messages
     * 
     *
     *
     */
    
    if (IMCFestivalApp_TwitterSearchGeo)
    {
        while(twitterGeo.hasNewTweets())
        {
            ofxTweet tweet = twitterGeo.getNextTweet();
            tbzTweet* tTweet = static_cast<tbzTweet*>(&tweet);
            
            // TASK: Extract location from tweet, skip to next tweet if missing
            
            bool    hasGeo;
            ofPoint geoPoint;
            
            hasGeo = tTweet->getGeoLocation(geoPoint);
            
            // Ignore this tweet if no coordinates attached. A Twitter search with geocoords will return tweets from users whose profile location is within the area specified; we don't want these.
            if (!hasGeo) continue;
            
            // Ignore this tweet if coordinates are not within site bounds (unable to search that precisely on twitter)
            if (!eventSite.groundBounds.inside(geoPoint)) continue;
            
    //        cout << endl;
    //        cout << "---" << endl;
    //        cout << "GeoTweet text:" << tweet.getText() << endl;
    //        //cout << "Name: " << tweet.getScreenName() << endl;
    //        //cout << "UserID: " << tweet.getUserID() << endl;
    //        cout << "Geo: " << geoPoint.y << "N, " << geoPoint.x << "E" << endl;
    //        //cout << "Source:" << tweet.getSourceJSON() << endl;
            
            // TASK: Create message and add to eventSite.
            Poco::SharedPtr<tbzSocialMessage> message = new tbzSocialMessage(tweet.getText(), tweet.getScreenName(), "Twitter", "TODO:Time");
            message->geoLocation = new ofPoint(geoPoint);
            message->tag.fontTitle = &socialMessageFont;
            message->tag.fontBody = &socialMessageFont;
            message->attributeTo = tweet.getUserID();
            
            eventSite.addMessage(message);
            
            // TASK: Cache message
            
    //        socialMessageStore.addTag("message");
    //        socialMessageStore.pushTag("message", socialMessageStore.getNumTags("message") - 1);
    //        {
    //            // Note: Twitter IDs can exceed max int size, so keep IDs as strings.
    //            socialMessageStore.setValue("text", tweet.getText());
    //            socialMessageStore.setValue("author", tweet.getScreenName());
    //            socialMessageStore.setValue("latitude", geoPoint.x);
    //            socialMessageStore.setValue("longitude", geoPoint.y);
    //            socialMessageStore.setValue("twitter:id", tweet.getID());
    //            socialMessageStore.setValue("twitter:userid", tweet.getUserID());
    //        }
    //        socialMessageStore.popTag();
        }
    }
    
    if (IMCFestivalApp_TwitterUseCached)
    {
        int framePeriod = 120;
        if (ofGetFrameNum() % framePeriod == 0)
        {
            if (socialMessageStore.getNumTags("message") == 0) return;
            
            int indexToDisplay = (ofGetFrameNum() / framePeriod) % socialMessageStore.getNumTags("message");
            
            socialMessageStore.pushTag("message", indexToDisplay);
            {
                Poco::SharedPtr<tbzSocialMessage> message = new tbzSocialMessage(
                                                                                socialMessageStore.getValue("text", ""),
                                                                                socialMessageStore.getValue("twitter:userid", ""),
                                                                                "Twitter",
                                                                                "TODO: Time"
                                                                                );
                message->geoLocation = new ofPoint(socialMessageStore.getValue("longitude", 0.0f), socialMessageStore.getValue("latitude", 0.0f));
                message->tag.fontTitle = &socialMessageFont;
                message->tag.fontBody = &socialMessageFont;
                message->attributeTo = socialMessageStore.getValue("twitter:userid", "");
                eventSite.addMessage(message);
            }
            socialMessageStore.popTag();
        }
    }
    
}

//--------------------------------------------------------------
void imcFestivalApp::draw()
{
    ofBackground(50, 50, 50, 0);

    bool debug3D = false;
    
    // Draw 3D axes in centre of screen
    if (debug3D)
    {
        ofPushMatrix();
        ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
        ofColor(255,0,0);
        ofLine(0, 0, 0, 100, 0, 0);
        ofColor(0,255,0);
        ofLine(0, 0, 0, 0, 100, 0);
        ofColor(0,0,255);
        ofLine(0, 0, 0, 0, 0, 100);
        ofPopMatrix();
    }
    
	eventSite.render();

    // Draw FPS
    ofSetColor(255, 255, 255, 255);
    //ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate(), 2), 10, 15);
    socialMessageFont.drawString("fps: " + ofToString(ofGetFrameRate(), 2), 10, 15);

}

//--------------------------------------------------------------
void imcFestivalApp::exit(){
    
}

#if TARGET_OS_IPHONE

//--------------------------------------------------------------
void imcFestivalApp::touchDown(ofTouchEventArgs & touch)
{
	eventSite.actionTouchDown(touch.x, touch.y, touch.id);
}

//--------------------------------------------------------------
void imcFestivalApp::touchMoved(ofTouchEventArgs & touch)
{
    eventSite.actionTouchMoved(touch.x, touch.y, touch.id);
}

//--------------------------------------------------------------
void imcFestivalApp::touchUp(ofTouchEventArgs & touch)
{
    eventSite.actionTouchUp(touch.x, touch.y, touch.id);
}

//--------------------------------------------------------------
void imcFestivalApp::touchDoubleTap(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void imcFestivalApp::touchCancelled(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void imcFestivalApp::lostFocus(){
    
}

//--------------------------------------------------------------
void imcFestivalApp::gotFocus(){
    
}

//--------------------------------------------------------------
void imcFestivalApp::gotMemoryWarning(){
    
}

//--------------------------------------------------------------
void imcFestivalApp::deviceOrientationChanged(int newOrientation){
    
}

#endif

#ifdef TARGET_OSX

//--------------------------------------------------------------
void imcFestivalApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void imcFestivalApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void imcFestivalApp::mouseMoved(int x, int y){
    
}

//--------------------------------------------------------------
void imcFestivalApp::mouseDragged(int x, int y, int button)
{
    eventSite.actionTouchMoved(x, y, 0);
}

//--------------------------------------------------------------
void imcFestivalApp::mousePressed(int x, int y, int button)
{
    eventSite.actionTouchDown(x, y, 0);
}

//--------------------------------------------------------------
void imcFestivalApp::mouseReleased(int x, int y, int button)
{
    eventSite.actionTouchUp(x, y, 0);
}

//--------------------------------------------------------------
void imcFestivalApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void imcFestivalApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void imcFestivalApp::dragEvent(ofDragInfo dragInfo){
    
}

#endif