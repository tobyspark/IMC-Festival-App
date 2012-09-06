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
    fontTitle = NULL;
}

tbzVenueSlot    tbzVenue::slotAtTime(tm time)
{
    tbzVenueSlot returnSlot;
    return returnSlot;
}

void tbzVenue::updateTagFBO()
{
    if (fontTitle)
    {
        // Just a reminder: origin is top left, and this is an arrow pointing down.
        
        // Get bounds of text. This will have an offset origin to place the baseline of the font at desired point
        ofRectangle textBounds = fontTitle->getStringBoundingBox(name, 0, 0);
        textBounds.x += kTBZVenue_Radius;
        textBounds.y += kTBZVenue_Radius + textBounds.height + 20; // This is a magic number. I guess the bounds aren't reported correctly.
        
        // Outline the text bounds with our round rect corner radius
        ofRectangle backBounds = textBounds;
        backBounds.width    += kTBZVenue_Radius*2.0f;
        backBounds.height   += kTBZVenue_Radius*2.0f;
        backBounds.x        = 0;
        backBounds.y        = 0;
        
        tagBounds.width        = backBounds.width;
        tagBounds.height       = backBounds.height + kTBZVenue_ArrowSize;
        
        float centreX = tagBounds.width / 2.0f;
        ofPoint arrowTL(centreX - kTBZVenue_ArrowSize/2.0f, backBounds.height);
        ofPoint arrowTR(centreX + kTBZVenue_ArrowSize/2.0f, backBounds.height);
        ofPoint arrowB(centreX, backBounds.height + kTBZVenue_ArrowSize * sin(ofDegToRad(60.0f)));
        
        // Create the empty image to draw into
        ofFbo::Settings s;
        s.width             = tagBounds.width;
        s.height            = tagBounds.height;
        s.internalformat    = GL_RGBA;
        s.useDepth          = false;
        
        // Create the correctly sized FBO if neccessary
        if (!tagFBO.isAllocated() || tagFBO.getWidth() != s.width || tagFBO.getHeight() != s.height)
        {
            tagFBO.setDefaultTextureIndex(0); // oF bug needs this as fix( http://forum.openframeworks.cc/index.php?topic=10536.0 )
            tagFBO.allocate(s);
        }
        
        // Draw into image
        ofPushStyle();
        tagFBO.begin();
        // Clear the image
        ofClear(255,255,255, 0);
        
        // Draw container shape
        ofSetColor(255,0,0);
        ofRectRounded(backBounds, kTBZVenue_Radius);
        ofTriangle(arrowTL, arrowTR, arrowB);
        
        // Draw text
        ofSetColor(255);
        fontTitle->drawString(name, textBounds.x, textBounds.y);
        tagFBO.end();
        ofPopStyle();
        
        // We want to draw centered horizontally and aligned to bottom
        tagFBO.setAnchorPoint(centreX, tagBounds.height);
    }
    else
    {
        ofLog(OF_LOG_WARNING, "TBZSocialMessage: Setup: fontTitle not allocated");
    }
    
}

void tbzVenue::drawTag()
{
    if (fontTitle)
    {
        if (!tagFBO.isAllocated()) updateTagFBO();
        tagFBO.draw(0, 0);
    }
    else
    {
        ofDrawBitmapString(name, 0, 0);
    }
}

void tbzVenue::updateProgrammeFBO()
{
    if (fontTitle && fontBody)
    {
        // Create the empty image to draw into
        ofFbo::Settings s;
        s.width             = ofGetWidth();
        s.height            = ofGetHeight();
        s.internalformat    = GL_RGBA;
        s.useDepth          = false;
        
        // Create the correctly sized FBO if neccessary
        if (!programmeFBO.isAllocated())
        {
            programmeFBO.setDefaultTextureIndex(0); // oF bug needs this as fix( http://forum.openframeworks.cc/index.php?topic=10536.0 )
            programmeFBO.allocate(s);
        }
        
        // Draw into image
        ofPushStyle();
        programmeFBO.begin();
        {
            // Clear the image
            ofClear(255, 255, 255, 0); // Clearing with white to antialias onto background colour
        
            // TASK: Draw background fading in over model
            // Perhaps loading a PNG would be more flexible, but this is fast if we're just doing a simple gradient and solid
            ofColor whiteOpaque(255, 255);
            ofColor whiteClear(whiteOpaque, 0);
            float maxX = ofGetWidth();
            float maxY = ofGetHeight();
            float midX = maxX * 0.2f;
            
            ofMesh backgroundFillMesh;
            
            backgroundFillMesh.addVertex(ofVec3f(0,0));
            backgroundFillMesh.addColor(whiteClear);
            
            backgroundFillMesh.addVertex(ofVec3f(0,maxY));
            backgroundFillMesh.addColor(whiteClear);
            
            backgroundFillMesh.addVertex(ofVec3f(midX, 0));
            backgroundFillMesh.addColor(whiteOpaque);
            
            backgroundFillMesh.addVertex(ofVec3f(midX, maxY));
            backgroundFillMesh.addColor(whiteOpaque);
            
            backgroundFillMesh.addVertex(ofVec3f(maxX,0));
            backgroundFillMesh.addColor(whiteOpaque);
            
            backgroundFillMesh.addVertex(ofVec3f(maxX, maxY));
            backgroundFillMesh.addColor(whiteOpaque);
            
            ofVbo backgroundFillVBO;
            backgroundFillVBO.setMesh(backgroundFillMesh, GL_STATIC_DRAW);
            
            backgroundFillVBO.draw(GL_TRIANGLE_STRIP, 0, 6);
            
            // TASK: Draw Text
            
            int lineSpace = 10;
            int yPos = 0;
            int xPos = midX + lineSpace;
            
            // Title
            ofSetColor(0, 0, 0, 255);
            yPos += 10 + fontTitle->getSize();
            fontTitle->drawString(name, xPos, yPos);
            
            // Slots
            list<tbzVenueSlot>::iterator slot;
            for (slot = slots.begin(); slot != slots.end(); slot++)
            {
                //char slotTime[5];
                //strftime(slotTime, 5, "%H:%M", &slot->starts);
                
                stringstream slotText;
                slotText << setw(2) << setfill('0') << slot->starts.tm_hour << ":" << setw(2) << setfill('0') << slot->starts.tm_min << " - " << slot->name;
                
                yPos += lineSpace + fontBody->getSize();
                
                fontBody->drawString(slotText.str(), xPos, yPos);
            }
        }
        programmeFBO.end();
        ofPopStyle();
    }
    else
    {
        ofLog(OF_LOG_WARNING, "TBZVenue: updateProgrammeFBO: fontTitle not allocated");
    }
    
    
    

    
}

void tbzVenue::drawProgramme(float animPos)
{
    if (fontTitle && fontBody)
    {
        if (!programmeFBO.isAllocated()) updateProgrammeFBO();
        ofSetColor(255, 255*animPos);
        programmeFBO.draw(0,0);
    }
    else
    {
        ofDrawBitmapString(name, 0, 0);
    }
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
                stringstream hourString(tempString);
                int hours;
                hourString >> hours;

                getline(timeString, tempString);
                stringstream minuteString(tempString);
                int minutes;
                minuteString >> minutes;
            
                newSlot.starts.tm_hour = hours;
                newSlot.starts.tm_min = minutes;
                
                // Slot end time
                // TODO: check for remaining time line, ie 12:00 - 12:45
                // Else, give non-time
                newSlot.ends.tm_hour = -1;
                newSlot.ends.tm_min = -1;
            
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

