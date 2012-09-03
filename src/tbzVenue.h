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
        ofTrueTypeFont *font;
    
        tbzVenueSlot    slotAtTime(tm time);
        
        void            setupFBO();
        void            draw();
        
        void            setupFromXML(ofxXmlSettings &xml, int which = 0);
    
    protected:
        ofRectangle     bounds;
        ofFbo          *fbo;
    
        bool            stageGeoLocationFromKMZ(string filename);
        bool            audienceGeoAreaFromKMZ(string filename);
};


#endif /* defined(__IMCFestivalApp__tbzVenue__) */
