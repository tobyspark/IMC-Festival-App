//
//  tbzVenue.cpp
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 03/09/2012.
//
//

#include "tbzVenue.h"

#include "ofxUnZip.h"

#define kTBZVenue_Radius 5
#define kTBZVenue_ArrowSize 10 // this is side length of arrow and height bubble is raised

tbzVenue::tbzVenue()
{
    font = NULL;
    fbo = NULL;
}

tbzVenueSlot    tbzVenue::slotAtTime(tm time)
{
    tbzVenueSlot returnSlot;
    return returnSlot;
}

void tbzVenue::setupFBO()
{
    if (font)
    {
        fbo = new ofFbo;
        
        // Just a reminder: origin is top left, and this is an arrow pointing down.
        
        // Get bounds of text. This will have an offset origin to place the baseline of the font at desired point
        ofRectangle textBounds = font->getStringBoundingBox(name, 0, 0);
        textBounds.x += kTBZVenue_Radius;
        textBounds.y += kTBZVenue_Radius + textBounds.height + 20; // 10 is a magic number. I guess the bounds aren't reported correctly.
        
        // Outline the text bounds with our round rect corner radius
        ofRectangle backBounds = textBounds;
        backBounds.width    += kTBZVenue_Radius*2.0f;
        backBounds.height   += kTBZVenue_Radius*2.0f;
        backBounds.x        = 0;
        backBounds.y        = 0;
        
        bounds.width        = backBounds.width;
        bounds.height       = backBounds.height + kTBZVenue_ArrowSize;
        
        float centreX = bounds.width / 2.0f;
        ofPoint arrowTL(centreX - kTBZVenue_ArrowSize/2.0f, backBounds.height);
        ofPoint arrowTR(centreX + kTBZVenue_ArrowSize/2.0f, backBounds.height);
        ofPoint arrowB(centreX, backBounds.height + kTBZVenue_ArrowSize * sin(ofDegToRad(60.0f)));
        
        // Create the empty image to draw into
        ofFbo::Settings s;
        s.width             = bounds.width;
        s.height            = bounds.height;
        s.internalformat    = GL_RGBA;
        s.useDepth          = false;
        
        fbo->allocate(s);
        
        // Draw into image
        ofPushStyle();
        fbo->begin();
        // Clear the image
        ofClear(255,255,255, 0);
        
        // Draw container shape
        ofSetColor(255,0,0);
        ofRectRounded(backBounds, kTBZVenue_Radius);
        ofTriangle(arrowTL, arrowTR, arrowB);
        
        // Draw text
        ofSetColor(255);
        font->drawString(name, textBounds.x, textBounds.y);
        fbo->end();
        ofPopStyle();
        
        // We want to draw centered horizontally and aligned to bottom
        bounds.x            = -centreX;
        bounds.y            = -bounds.height;
    }
    else
    {
        ofLog(OF_LOG_WARNING, "TBZSocialMessage: Setup: Font not allocated");
    }
    
}

void tbzVenue::draw()
{
    ofPushMatrix();
    
    ofRotate(-90, 1, 0, 0);
    
    if (font)
    {
        if (!fbo) setupFBO();
        fbo->draw(bounds.x, bounds.y);
    }
    else
    {
        ofDrawBitmapString(name, 0, 0);
    }
    
    ofPopMatrix();
}

void tbzVenue::setupFromXML(ofxXmlSettings &xml, int which)
{
    /*
     
     XML we're expecting is in the form
     
     <venue>
         <name>Eat Your Own Ears</name>
         <slot>
             <time>12:00 </time>
             <name> Eat Your Own Ears DJ</name>
         </slot>
         <slot>
             <time>12:40 </time>
             <name> Last Dinosaurs</name>
         </slot>
         <slot>
             <time>13:10 </time>
             <name> Eat Your Own Ears DJ</name>
         </slot>
     </venue>
     */
    
    if (xml.pushTag("venue", which))
    {
        name = xml.getValue("name", "A Venue");
        
        // TASK: Setup stage geolocation
        
        if (xml.tagExists("stageCenter"))
        {
            stageGeoLocation.x = xml.getValue("stageCenter:longitude", 0.0f);
            stageGeoLocation.y = xml.getValue("stageCenter:latitude", 0.0f);
        }
        else if (xml.tagExists("stageKML"))
        {
            stageGeoLocationFromKMZ(xml.getValue("stageKML", "no filename could be read from XML"));
        }
        else
        {
            ofLog(OF_LOG_WARNING, "Failed to locate venue: " + name);
        }
        
        // TASK: Setup audience geo area
        
        if (xml.tagExists("audienceArea"))
        {
            xml.pushTag("audienceArea");
                audienceGeoArea.clear();
                
                int areaVertexCount = xml.getNumTags("vertex");
                for (int i = areaVertexCount-1; i >= 0; i--)
                {
                    xml.pushTag("vertex", i);
                        int x = xml.getValue("x", 0.0f);
                        int y = xml.getValue("y", 0.0f);
                        audienceGeoArea.addVertex(x, y);
                    xml.popTag();
                }

                audienceGeoArea.close();
            xml.popTag();
        }
        else if (xml.tagExists("audienceKML"))
        {
            audienceGeoAreaFromKMZ(xml.getValue("audienceKML", "no filename could be read from XML"));
        }
        else
        {
            ofLog(OF_LOG_WARNING, "Failed to plot venue audience area: " + name);
        }
        
        // TASK: Setup programme slots
        
        int slotCount = xml.getNumTags("slot");
        tbzVenueSlot newSlot;
        
        for (int i = slotCount-1; i >= 0; i--)
        {
            xml.pushTag("slot", i);
                // Slot name
                newSlot.name = xml.getValue("name", "An Act");
                
                // Slot start time
                stringstream timeString(xml.getValue("time", "12:00"));
                string tempString;
                
                getline(timeString, tempString, ':');
                stringstream minuteString(tempString);
                int minutes;
                minuteString >> minutes;
                
                getline(timeString, tempString);
                stringstream hourString(tempString);
                int hours;
                hourString >> hours;
                
                newSlot.starts.tm_hour = hours;
                newSlot.starts.tm_min = minutes;
                
                // Slot end time
                // TODO: check for remaining time line, ie 12:00 - 12:45
                // Else, give non-time
                newSlot.starts.tm_hour = -1;
                newSlot.starts.tm_min = -1;
            
                slots.push_front(newSlot);
            xml.popTag();
        }
        
        xml.popTag();
    }
    
    ofLog(OF_LOG_VERBOSE, "Venue setup: " + name + " with " + ofToString(slots.size()) + " slots");
    ofLog(OF_LOG_VERBOSE, "Location: " + ofToString(stageGeoLocation.x) + ", " + ofToString(stageGeoLocation.y));
}

bool tbzVenue::stageGeoLocationFromKMZ(string filename)
{
    // In an ideal world, we'd use this library. In an ideal world, I'd also be able to compile it.
    // http://code.google.com/p/libkml

    // Note this will only work in iOS emulator, not on actual iOS devices!
    // We want this already parsed and in eventSiteSettings.xml for actual iOS startup.
    
    bool debugMethod = false;
    
    ofLog(OF_LOG_VERBOSE, "Parsing KML: " + filename);
    
    bool success = false;
    
    string destFileLocation = "unzipTemp";
    
    success = ofxUnZip(filename, destFileLocation);
    if (success)
    {
        string docFileLocation = destFileLocation + "/doc.kml";
        {
            ofxXmlSettings kml;
            success = kml.loadFile(docFileLocation);
            if (success)
            {
                string temp;
                kml.copyXmlToString(temp);
                
                kml.pushTag("kml");
                kml.pushTag("Document");
                kml.pushTag("Placemark");
                kml.pushTag("Point");
                
                stringstream coordinates(kml.getValue("coordinates", ""));
                
                if (debugMethod) ofLog(OF_LOG_VERBOSE, coordinates.str());
                
                string tempString;
                
                getline(coordinates, tempString, ',');
                stringstream xString(tempString);
                
                getline(coordinates, tempString, ',');
                stringstream yString(tempString);
                
                getline(coordinates, tempString, ' ');
                stringstream zString(tempString);
                
                xString >> stageGeoLocation.x;
                yString >> stageGeoLocation.y;
                
                // TODO: Write parsed data back to eventSettings.xml
            }
        }
        ofFile::removeFile(docFileLocation);
    }

    return success;
}

bool tbzVenue::audienceGeoAreaFromKMZ(string filename)
{
    // Note this will only work in iOS emulator, not on actual iOS devices!
    // We want this already parsed and in eventSiteSettings.xml for actual iOS startup.
    
    bool debugMethod = false;
    
    ofLog(OF_LOG_VERBOSE, "Parsing KML: " + filename);
    
    bool success = false;
    
    string destFileLocation = "unzipTemp";
    
    success = ofxUnZip(filename, destFileLocation);
    if (success)
    {
        string docFileLocation = destFileLocation + "/doc.kml";
        {
            ofxXmlSettings kml;
            success = kml.loadFile(docFileLocation);
            if (success)
            {
                kml.pushTag("kml");
                kml.pushTag("Document");
                kml.pushTag("Placemark");
                kml.pushTag("Polygon");
                kml.pushTag("outerBoundaryIs");
                kml.pushTag("LinearRing");
                
                stringstream coordinates(kml.getValue("coordinates", ""));
                
                if (debugMethod) ofLog(OF_LOG_VERBOSE, coordinates.str());
                
                audienceGeoArea.clear();
                
                while (true)
                {
                    string tempString;
                    
                    getline(coordinates, tempString, ',');
                    if (tempString.length() == 0) break;
                    stringstream xString(tempString);
                    
                    getline(coordinates, tempString, ',');
                    if (tempString.length() == 0) break;
                    stringstream yString(tempString);

                    getline(coordinates, tempString, ' ');
                    if (tempString.length() == 0) break;
                    stringstream zString(tempString);
                    
                    float x,y;
                    xString >> x;
                    yString >> y;
                    
                    if (debugMethod) ofLog(OF_LOG_VERBOSE, ofToString(x) + ", " + ofToString(y));
                    
                    audienceGeoArea.addVertex(x, y);
                }
                
                audienceGeoArea.close();
                
                // TODO: Write parsed data back to eventSettings.xml
            }
        }
        ofFile::removeFile(docFileLocation);
    }
    
    return success;
}

