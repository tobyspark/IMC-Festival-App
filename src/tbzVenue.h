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

struct tbzVenueSlot
{
    string  name;
    tm      starts;
    tm      ends;
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
        
        void            updateTagFBO();
        void            drawTag(float animPos = 1.0f);
        void            setTagTextToNowAndNext();
        void            setTagTextToProgramme();
        void            setTagTextToNothing();
        void            setTextLinesAnimPos(float animPos);
    
        void            updateProgrammeFBO();
        void            drawProgramme(float animPos = 1.0f);
    
        void            setupFromXML(ofxXmlSettings &xml, int which = 0);
        void            setupFromXML(ofxXmlSettings &xml, bool &xmlChanged, int which = 0);
    
    protected:
        ofFbo           tagFBO;
        float           textLinesAnimPos;
        vector<string>  tagTextLines;
    
        ofFbo           programmeFBO;
    
        bool            stageGeoLocationFromKMZ(string filename);
        bool            audienceGeoAreaFromKMZ(string filename);
    
        string          slotTextForSlot(tbzVenueSlot &slot);
};


#endif /* defined(__IMCFestivalApp__tbzVenue__) */
