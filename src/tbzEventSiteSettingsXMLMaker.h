//
//  tbzEventSiteSettingsXMLMaker.h
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 28/08/2012.
//
//

#ifndef IMCFestivalApp_tbzEventSiteSettingsXMLMaker_h
#define IMCFestivalApp_tbzEventSiteSettingsXMLMaker_h

/*
    // CODE TO MINT A CANONICAL EVENT SITE XML SETTINGS FILE
    // ...while we want these to be human editable in a text editor, you've gotta start somewhere
 
    ofxXmlSettings eventSiteXML;

    eventSiteXML.addValue("modelName", "FieldDay-2012-3D.dae");

    eventSiteXML.addTag("model");
    eventSiteXML.pushTag("model");
    
        eventSiteXML.addTag("topLeftCorner");
        eventSiteXML.pushTag("topLeftCorner");
            eventSiteXML.addValue("latitude", "51°32'23.03\"N");
            eventSiteXML.addValue("longitude", "0° 2'27.37\"W");
        eventSiteXML.popTag();
        
        eventSiteXML.addTag("topRightCorner");
        eventSiteXML.pushTag("topRightCorner");
            eventSiteXML.addValue("latitude", "51°32'23.03\"N");
            eventSiteXML.addValue("longitude", "0° 1'41.31\"W");
        eventSiteXML.popTag();
        
        eventSiteXML.addTag("bottomLeftCorner");
        eventSiteXML.pushTag("bottomLeftCorner");
            eventSiteXML.addValue("latitude", "51°32'1.96\"N");
            eventSiteXML.addValue("longitude", "0° 2'27.37\"W");
        eventSiteXML.popTag();
        
        eventSiteXML.addTag("BottomRightCorner");
        eventSiteXML.pushTag("BottomRightCorner");
            eventSiteXML.addValue("latitude", "51°32'1.96\"N");
            eventSiteXML.addValue("longitude", "0° 1'41.31\"W");
        eventSiteXML.popTag();
    
    eventSiteXML.popTag();
    
    eventSiteXML.saveFile("eventSiteSettings.xml");
*/

/*
    ofxXmlSettings tempXML;

    // make sure to have a blank new line at end of text file.
    // and hypen is not an mdash or whatnot

    std::ifstream t("/Users/tbzphd/Dropbox/3 - Festival App/Field Day 2012 Source Data/field day 2012 programme for parsing.txt");
    std::stringstream ss;
    ss << t.rdbuf();

    string line;
    int counter = 0;

    do {
        getline(ss, line);
        
        stringstream ssline(line);
        string time;
        string act;
        
        getline(ssline, time, '-');
        getline(ssline, act);
        
        tempXML.addTag("slot");
        tempXML.pushTag("slot", counter++);
        tempXML.addValue("time", time);
        tempXML.addValue("name", act);
        tempXML.popTag();
        
        ofLog(OF_LOG_ERROR, line);
        
    } while (line.length() > 0);

    tempXML.saveFile("temp.xml");
*/

/*
void imcFestivalApp::loadAndParseTwitterTestData()
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

 */

#endif
