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
#include "tbzEventSiteFeature.h"
#include "ofxXmlSettings.h"
#include "tbzCaptionGraphic.h"
#include "tbzKMZReader.h"
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

class tbzVenue : public tbzEventSiteFeature
{
    public:
        tbzVenue();
    
        list<tbzVenueSlot>    slots;
        ofPolyline      audienceGeoArea;
    
        list<tbzVenueSlot>::iterator    slotAtTime(tm time);
        list<tbzVenueSlot>::iterator    slotAfterTime(tm time);
    
        void            setState(State inState);
        State           getState();
    
        void            update();
        void            drawTag();
        void            drawFeature();
        
        tbzCaptionGraphic tag;
        float           getTagHeight();
    
        void            setupFromXML(ofxXmlSettings &xml, int which = 0);
        void            setupFromXML(ofxXmlSettings &xml, bool &xmlChanged, int which = 0);
    
    protected:    
        string          slotTextForSlot(tbzVenueSlot &slot);
};


#endif /* defined(__IMCFestivalApp__tbzVenue__) */
