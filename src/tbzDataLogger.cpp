//
//  tbzDataLogger.cpp
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 28/09/2012.
//
//

#include "tbzDataLogger.h"

// Note, for iOS ensure file type is set to Objective-C++ in Xcode. Not using .mm extension to keep cross platform.

tbzDataLogger::tbzDataLogger()
{
    pollingIntervalFrames = 10; // 10 Hz at 60fps
    newFileIntervalFrames = 60*60; // One minute at 60 fps
}

void tbzDataLogger::setup()
{
    // initialize the accelerometer
	ofxAccelerometer.setup();
}

void tbzDataLogger::exit()
{
    // stop using the accelerometer
	ofxAccelerometer.exit();
}

void tbzDataLogger::setLogsFolder(string folderPath)
{
    // Only set if folder is writeable
    ofFile dir(folderPath);
    if (dir.isDirectory() && dir.canWrite())
    {
        logsFolder = folderPath;
    }
    else
    {
        ofLog(OF_LOG_WARNING, "tbzDataLogger: Aborting set of logs folder. <" + folderPath + "> cannot be written to.");
    }
}

void tbzDataLogger::update()
{
    int frameNum = ofGetFrameNum();

    // TASK: Capture sensor data every [x] frames
    if (frameNum % pollingIntervalFrames == 0)
    {
        // returns current smoothed accelerometer data (value in number of g's (1g = gravity, 9.8m/s^2)
        ofPoint force = ofxAccelerometer.getForce();
        
        // returns current orientation in degrees (x: pitch, y: roll, z: not used)
        ofPoint orientation = ofxAccelerometer.getOrientation();
        
        tbzSensorData dataFrame;
        dataFrame.timestamp = ofGetTimestampString();
        dataFrame.accForceX = force.x;
        dataFrame.accForceY = force.y;
        dataFrame.accForceZ = force.z;
        dataFrame.accPitch = orientation.x;
        dataFrame.accRoll  = orientation.y;
        dataFrame.locLatitude = coreLocation.getLatitude();
        dataFrame.locLongitude = coreLocation.getLongitude();
        dataFrame.locAccuracy = coreLocation.getLocationAccuracy();
        dataFrame.comHeading = coreLocation.getTrueHeading();
        dataFrame.comAccuracy = coreLocation.getHeadingAccuracy();
        dataFrame.comX = coreLocation.getCompassX();
        dataFrame.comY = coreLocation.getCompassY();
        dataFrame.comZ = coreLocation.getCompassZ();
        
        // TODO: Get when corelocation last updated, and only add if data is new.
        // TODO: This should be a circular buffer not building and clearing a list, dumping out when cycles round.
        
        sensorData.push_back(dataFrame);
    }
    
    // TASK: Write logged sensor data to file
    if (frameNum > 0 && frameNum % newFileIntervalFrames == 0 && !sensorData.empty() && logsFolder.length() > 0)
    {
        // Needs to be Async? Or should tbzDataLogger be its own thread that sleeps between sensor frames?
        
        // TASK: Write sensorData out to file in JSON format
        
        json_t* json = json_array();

        list<tbzSensorData>::iterator dataFrame;
        for (dataFrame = sensorData.begin(); dataFrame != sensorData.end(); dataFrame++)
        {
            json_t* dataFrameJSON = json_object();
            json_object_set_new(dataFrameJSON, "timestamp", json_string(dataFrame->timestamp.c_str()));
            json_object_set_new(dataFrameJSON, "accForceX", json_real(dataFrame->accForceX));
            json_object_set_new(dataFrameJSON, "accForceY", json_real(dataFrame->accForceY));
            json_object_set_new(dataFrameJSON, "accForceZ", json_real(dataFrame->accForceZ));
            json_object_set_new(dataFrameJSON, "accPitch", json_real(dataFrame->accPitch));
            json_object_set_new(dataFrameJSON, "accRoll", json_real(dataFrame->accRoll));
            json_object_set_new(dataFrameJSON, "locLatitude", json_real(dataFrame->locLatitude));
            json_object_set_new(dataFrameJSON, "locLongitude", json_real(dataFrame->locLongitude));
            json_object_set_new(dataFrameJSON, "locAccuracy", json_real(dataFrame->locAccuracy));
            json_object_set_new(dataFrameJSON, "comHeading", json_real(dataFrame->comHeading));
            json_object_set_new(dataFrameJSON, "comAccuracy", json_real(dataFrame->comAccuracy));
            json_object_set_new(dataFrameJSON, "comX", json_real(dataFrame->comX));
            json_object_set_new(dataFrameJSON, "comY", json_real(dataFrame->comY));
            json_object_set_new(dataFrameJSON, "comZ", json_real(dataFrame->comZ));
            
            // TODO: More compact form ie. { time: xxx, accXYZPR: [x,y,z,p,r], locLtLnA: [lt, ln, a], comHXYZA
            
            json_array_append_new(json, dataFrameJSON);
        }
        
        string filename = ofFilePath::addTrailingSlash(logsFolder) + ofGetTimestampString(); 
        
        int success = json_dump_file(json, filename.c_str(), JSON_COMPACT);
        
        // TASK: Notify host app that there is new file for upload etc.
        if (success >= 0)
        {
            ofNotifyEvent(onLogFileWritten, filename);
        }
        else
        {
            ofLog(OF_LOG_WARNING, "tbzDataLogger: Failed to write log");
        }
            
        // TASK: Clear sensorData for new samples
        
        sensorData.clear();
        
        
    }
}
