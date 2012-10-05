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

#if TARGET_OS_IPHONE
#include "ofxiPhoneCoreLocation.h"
#endif

struct tbzSensorData
{
    string timestamp;
    float accForceX;
    float accForceY;
    float accForceZ;
    float accPitch;
    float accRoll;
    float locLatitude;
    float locLongitude;
    float locAccuracy;
    float comHeading;
    float comAccuracy;
    float comX;
    float comY;
    float comZ;
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
    
        
        ofxiPhoneCoreLocation coreLocation;
    
    
};


#endif /* defined(__IMCFestivalApp__tbzDataLogger__) */

