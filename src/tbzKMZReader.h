//
//  tbzKMZReader.h
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 20/09/2012.
//
//

#ifndef __IMCFestivalApp__tbzKMZReader__
#define __IMCFestivalApp__tbzKMZReader__

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxUnzip.h"

class tbzKMZReader
{
    public:
        static bool pointFromKMZ(string filename, ofPoint &point);
        static bool polylineFromKMZ(string filename, ofPolyline &polyline);
};

#endif /* defined(__IMCFestivalApp__tbzKMZReader__) */
