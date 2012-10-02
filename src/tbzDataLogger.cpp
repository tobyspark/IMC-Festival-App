//
//  tbzDataLogger.cpp
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 28/09/2012.
//
//

#include "tbzDataLogger.h"

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
    
    logsFolder = folderPath;
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
        dataFrame.forceX = force.x;
        dataFrame.forceY = force.y;
        dataFrame.forceZ = force.z;
        dataFrame.pitch = orientation.x;
        dataFrame.roll  = orientation.y;
        
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
            json_object_set_new(dataFrameJSON, "forceX", json_real(dataFrame->forceX));
            json_object_set_new(dataFrameJSON, "forceY", json_real(dataFrame->forceY));
            json_object_set_new(dataFrameJSON, "forceZ", json_real(dataFrame->forceZ));
            json_object_set_new(dataFrameJSON, "pitch", json_real(dataFrame->pitch));
            json_object_set_new(dataFrameJSON, "roll", json_real(dataFrame->roll));
            
            json_array_append_new(json, dataFrameJSON);
        }
        
        string filename = ofFilePath::addTrailingSlash(logsFolder) + ofGetTimestampString(); 
        
        json_dump_file(json, filename.c_str(), JSON_COMPACT);
        
        // TASK: Clear sensorData for new samples
        
        sensorData.clear();
        
        // TASK: Notify host app that there is new file for upload etc.
        
        ofNotifyEvent(onLogFileWritten, filename);
    }
}
