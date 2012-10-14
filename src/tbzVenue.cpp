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
    
    state = uninitialised;
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

void tbzVenue::drawFeature()
{
    ofPushStyle();
    {
        ofFloatColor selectedColour(255, 1.0f-selected, 1.0f-selected); // Red if selected
        ofSetColor(selectedColour);
        
        model.drawFaces();
    }
    ofPopStyle();
}

void tbzVenue::drawTag()
{
    if (transition > 0.0f)
    {
        tag.draw();
    }
}

void tbzVenue::setState(State inState)
{
    if (state != inState)
    {
        state = inState;
        
        list<string> tagTextLines;
        
        if (inState == preview)
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
        else if (inState == full)
        {
            list<tbzVenueSlot>::iterator slot;
            for (slot = slots.begin(); slot != slots.end(); slot++)
            {
                tagTextLines.push_back(slotTextForSlot(*slot));
            }
        }
        else
        {
            state = nothing;
        }
        
        tag.setContent(name, tagTextLines);
    }
}

tbzEventSiteFeature::State tbzVenue::getState()
{
    return state;
}

float tbzVenue::getTagHeight()
{
    return tag.getBounds().height;
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
            geoLocation.x = xml.getValue("stageCenter:longitude", 0.0f);
            geoLocation.y = xml.getValue("stageCenter:latitude", 0.0f);
        }
        // We're only going to process KML on OSX, on iOS we need fast startup and so will use the parsed info put back into the XML
        #ifdef TARGET_OSX
        else if (xml.tagExists("stageKML"))
        {
            bool ok = false;
            string filenameKMZ = xml.getValue("stageKML", "no filename could be read from XML");
            ok = pointFromKMZ(filenameKMZ, geoLocation);
            
            if (ok)
            {
                xml.addTag("stageCenter");
                xml.pushTag("stageCenter");
                    xml.addValue("longitude", geoLocation.x);
                    xml.addValue("latitude", geoLocation.y);
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
            string filenameKMZ = xml.getValue("audienceKML", "no filename could be read from XML")
            ok = audienceGeoAreaFromKMZ(filenameKMZ, audienceGeoArea);

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
    
    setState(nothing);
    
    ofLog(OF_LOG_VERBOSE, "Venue setup: " + name + " with " + ofToString(slots.size()) + " slots");
    ofLog(OF_LOG_VERBOSE, "Location: " + ofToString(geoLocation.x) + ", " + ofToString(geoLocation.y));
}

