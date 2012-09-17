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
#define kTBZVenue_AnimInDistance 150

int tbzVenueSlot::minutesFromDayStart(tm time)
{
    // TASK: generate some form of absolute time that we can compare against
    // We're assuming that say a 5am slot is late-night and say an 8am slot is beginning of the day early.
    
    int dayStartHour = 7;
    int hours = time.tm_hour - dayStartHour;
    if (hours < 0) hours += 24;
    
    return (hours * 60) + time.tm_min;
}

bool tbzVenueSlot::operator < (tbzVenueSlot comp)
{
    // TASK: used to sort the list of venue slots, does this by start time
    return minutesFromDayStart(starts) < minutesFromDayStart(comp.starts) ? true : false;
}



tbzVenue::tbzVenue()
{
    tag.setStyle(kTBZVenue_Radius, kTBZVenue_ArrowSize, ofColor(255), ofColor(255), ofColor(255,0,0));
    tag.fontTitle = NULL;
    tag.fontBody = NULL;
    
    tagTextType = uninitialised;
}

void tbzVenue::update()
{
    // TASK: Animate tag as appropriate, set by content
    
    tag.update();
}

list<tbzVenueSlot>::iterator tbzVenue::slotAtTime(tm time)
{
    list<tbzVenueSlot>::iterator returnSlot = slots.end();
    
    tm now;
    now.tm_hour = ofGetHours();
    now.tm_min = ofGetMinutes();
    int searchTime = tbzVenueSlot::minutesFromDayStart(now);
    
    list<tbzVenueSlot>::iterator slot;
    for (slot = slots.begin(); slot != slots.end(); slot++)
    {
        int startTime = tbzVenueSlot::minutesFromDayStart(slot->starts);
        
        // We've found it: we've just passed it, it's what we got last time
        if (returnSlot != slots.end() && startTime > searchTime) break;
        
        if (startTime < searchTime)
        {
            returnSlot = slot;
            
            // If there's an end time...
            if (slot->ends.tm_hour != -1)
            {
                // ...and we're within in, we've found it.
                if (searchTime < tbzVenueSlot::minutesFromDayStart(slot->ends)) break;
            }
        }
    }
    
    if (returnSlot == slots.end())
    {
        // We haven't found a time
        ofLog(OF_LOG_WARNING, "slotAtTime not found for venue " + name + " at time " + ofToString(time.tm_hour) + ":" + ofToString(time.tm_min));
    }
    
    return returnSlot;
}

list<tbzVenueSlot>::iterator tbzVenue::slotAfterTime(tm time)
{
    list<tbzVenueSlot>::iterator returnSlot = slots.end();
    
    tm now;
    now.tm_hour = ofGetHours();
    now.tm_min = ofGetMinutes();
    int searchTime = tbzVenueSlot::minutesFromDayStart(now);
    
    list<tbzVenueSlot>::iterator slot;
    for (slot = slots.begin(); slot != slots.end(); slot++)
    {
        int startTime = tbzVenueSlot::minutesFromDayStart(slot->starts);
        
        if (startTime > searchTime)
        {
            returnSlot = slot;
            break;
        }
    }
    
    if (returnSlot == slots.end())
    {
        // We haven't found a time
        ofLog(OF_LOG_WARNING, "slotAfterTime not found for venue " + name + " at time " + ofToString(time.tm_hour) + ":" + ofToString(time.tm_min));
    }
    
    return returnSlot;
}

void tbzVenue::drawTag(float animPos)
{
    
//        ofPushStyle();
//        
//            ofColor fadeInAlpha(255, animPos*255.0f);
//            float   animInPos = (1.0f-animPos)*kTBZVenue_AnimInDistance;
//            
//            ofSetColor(fadeInAlpha);
//            tagFBO.draw(0, 0 - animInPos);
//        
//        ofPopStyle();
        
    tag.draw();
}

void tbzVenue::setTagTextType(TagTextType type)
{
    if (tagTextType != type)
    {
        tagTextType = type;
        
        list<string> tagTextLines;
        
        if (type == nowAndNext)
        {
            time_t rawtime;
            tm* now;
            
            time (&rawtime);
            now = localtime(&rawtime);
            
            list<tbzVenueSlot>::iterator nowSlot = slotAtTime(*now);
            if (nowSlot != slots.end())
            {
                tagTextLines.push_back("Now: " + nowSlot->name);
            }
            
            list<tbzVenueSlot>::iterator nextSlot = slotAfterTime(*now);
            if (nextSlot != slots.end())
            {
                tagTextLines.push_back("Next: " + nextSlot->name);
            }
        }
        else if (type == programme)
        {
            list<tbzVenueSlot>::iterator slot;
            for (slot = slots.begin(); slot != slots.end(); slot++)
            {
                tagTextLines.push_back(slotTextForSlot(*slot));
            }
        }
        else
        {
            tagTextType = nothing;
        }
        
        tag.setContent(name, tagTextLines);
    }
}

tbzVenue::TagTextType tbzVenue::tbzVenue::getTagTextType()
{
    return tagTextType;
}

string tbzVenue::slotTextForSlot(tbzVenueSlot &slot)
{
    stringstream slotText;
    
    slotText << setw(2) << setfill('0') << slot.starts.tm_hour << ":" << setw(2) << setfill('0') << slot.starts.tm_min << " - " << slot.name;
    
    return slotText.str();
}

void tbzVenue::setupFromXML(ofxXmlSettings &xml, int which)
{
    bool ignore;
    setupFromXML(xml, ignore, which);
}

void tbzVenue::setupFromXML(ofxXmlSettings &xml, bool &xmlChanged, int which)
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
    
    xmlChanged = false;
    
    if (xml.pushTag("venue", which))
    {
        name = xml.getValue("name", "A Venue");
        
        // TASK: Setup stage geolocation
        
        if (xml.tagExists("stageCenter"))
        {
            stageGeoLocation.x = xml.getValue("stageCenter:longitude", 0.0f);
            stageGeoLocation.y = xml.getValue("stageCenter:latitude", 0.0f);
        }
        // We're only going to process KML on OSX, on iOS we need fast startup and so will use the parsed info put back into the XML
        #ifdef TARGET_OSX
        else if (xml.tagExists("stageKML"))
        {
            bool ok = false;
            ok = stageGeoLocationFromKMZ(xml.getValue("stageKML", "no filename could be read from XML"));
            
            if (ok)
            {
                xml.addTag("stageCenter");
                xml.pushTag("stageCenter");
                    xml.addValue("longitude", stageGeoLocation.x);
                    xml.addValue("latitude", stageGeoLocation.y);
                xml.popTag();
                
                xmlChanged = true;
            }
        }
        #endif
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
        // We're only going to process KML on OSX, on iOS we need fast startup and so will use the parsed info put back into the XML
        #ifdef TARGET_OSX
        else if (xml.tagExists("audienceKML"))
        {
            bool ok = false;
            ok = audienceGeoAreaFromKMZ(xml.getValue("audienceKML", "no filename could be read from XML"));

            if (ok)
            {
                xml.addTag("audienceArea");
                xml.pushTag("audienceArea");
                    vector<ofPoint> vertices = audienceGeoArea.getVertices();
                    for (int i = 0; i < vertices.size(); i++)
                    {
                        xml.addTag("vertex");
                        xml.pushTag("vertex", i);
                            xml.addValue("x", vertices[i].x);
                            xml.addValue("y", vertices[i].y);
                        xml.popTag();
                    }
                xml.popTag();
                
                xmlChanged = true;
            }
        }
        #endif
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
    
    slots.sort();
    
    setTagTextType(nothing);
    
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
            }
        }
        ofFile::removeFile(docFileLocation);
    }
    
    return success;
}

