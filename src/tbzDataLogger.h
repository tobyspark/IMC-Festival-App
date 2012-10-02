//
//  tbzDataLogger.h
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 28/09/2012.
//
//

#ifndef __IMCFestivalApp__tbzDataLogger__
#define __IMCFestivalApp__tbzDataLogger__

#include "ofMain.h"
#include "ofxAccelerometer.h"
#include "jansson.h"
#include <list>

struct tbzSensorData
{
    string timestamp;
    float forceX;
    float forceY;
    float forceZ;
    float pitch;
    float roll;
};

class tbzDataLogger
{
    public:
        tbzDataLogger();
        void setup();
        void update();
        void exit();
    
        void setLogsFolder(string folderPath);
    
        ofEvent<string> onLogFileWritten;
    
    protected:
        int pollingIntervalFrames;
        int newFileIntervalFrames;
        list<tbzSensorData>sensorData;
        string logsFolder;
};


#endif /* defined(__IMCFestivalApp__tbzDataLogger__) */

