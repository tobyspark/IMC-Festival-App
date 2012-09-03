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

#endif
