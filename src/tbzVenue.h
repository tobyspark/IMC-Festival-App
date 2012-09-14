//
//  tbzVenue.h
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 03/09/2012.
//
//

#ifndef __IMCFestivalApp__tbzVenue__
#define __IMCFestivalApp__tbzVenue__

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include <list>
#include <vector>

class tbzVenueSlot
{
    public:
        string  name;
        tm      starts;
        tm      ends;
    
        static int minutesFromDayStart(tm time);
    
        bool operator < (tbzVenueSlot);

};

class tbzVenue
{
    public:
        tbzVenue();
    
        string          name;
        list<tbzVenueSlot>    slots;
        ofPoint         stageGeoLocation;
        ofPolyline      audienceGeoArea;
        ofTrueTypeFont *fontTitle;
        ofTrueTypeFont *fontBody;
    
        list<tbzVenueSlot>::iterator    slotAtTime(tm time);
        list<tbzVenueSlot>::iterator    slotAfterTime(tm time);
    
        void            update();
    
        void            drawTag(float animPos = 1.0f);
        enum            TagTextType {nowAndNext, programme, nothing};
        void            setTagTextType(TagTextType type = nothing);
        TagTextType     getTagTextType();
    
        void            drawProgramme(float animPos = 1.0f);
    
        void            setupFromXML(ofxXmlSettings &xml, int which = 0);
        void            setupFromXML(ofxXmlSettings &xml, bool &xmlChanged, int which = 0);
    
    protected:
        void            updateTagFBO(bool resize = false);
        ofFbo           tagFBO;
        vector<string>  tagTextLines;
        TagTextType     tagTextType;
        float           tagTextHeight;
        float           tagTextHeightTarget;
        float           tagTextWidth;
        float           tagTextWidthTarget;
    
        void            updateProgrammeFBO();
        ofFbo           programmeFBO;
    
        bool            stageGeoLocationFromKMZ(string filename);
        bool            audienceGeoAreaFromKMZ(string filename);
    
        string          slotTextForSlot(tbzVenueSlot &slot);
};


#endif /* defined(__IMCFestivalApp__tbzVenue__) */
