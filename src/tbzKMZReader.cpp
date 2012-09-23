//
//  tbzKMZReader.cpp
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 20/09/2012.
//
//

#include "tbzKMZReader.h"

bool tbzKMZReader::pointFromKMZ(string filename, ofPoint &point)
{
    // In an ideal world, we'd use this library. In an ideal world, I'd also be able to compile it.
    // http://code.google.com/p/libkml
    
    // Note this will only work in iOS emulator, not on actual iOS devices!
    // We want this already parsed and in eventSiteSettings.xml for actual iOS startup.
    
    bool debugMethod = false;
    
    ofLog(OF_LOG_VERBOSE, "Parsing KML: " + filename);
    
    bool success = false;
    
    string destFileLocation = "unzipTemp";
    
    success = ofxUnZip(filename, destFileLocation);
    if (success)
    {
        string docFileLocation = destFileLocation + "/doc.kml";
        {
            ofxXmlSettings kml;
            success = kml.loadFile(docFileLocation);
            if (success)
            {
                string temp;
                kml.copyXmlToString(temp);
                
                kml.pushTag("kml");
                kml.pushTag("Document");
                kml.pushTag("Placemark");
                kml.pushTag("Point");
                
                stringstream coordinates(kml.getValue("coordinates", ""));
                
                if (debugMethod) ofLog(OF_LOG_VERBOSE, coordinates.str());
                
                string tempString;
                
                getline(coordinates, tempString, ',');
                stringstream xString(tempString);
                
                getline(coordinates, tempString, ',');
                stringstream yString(tempString);
                
                getline(coordinates, tempString, ' ');
                stringstream zString(tempString);
                
                xString >> point.x;
                yString >> point.y;
            }
        }
        ofFile::removeFile(docFileLocation);
    }
    
    return success;
}

bool tbzKMZReader::polylineFromKMZ(string filename, ofPolyline &polyline)
{
    // Note this will only work in iOS emulator, not on actual iOS devices!
    // We want this already parsed and in eventSiteSettings.xml for actual iOS startup.
    
    bool debugMethod = false;
    
    ofLog(OF_LOG_VERBOSE, "Parsing KML: " + filename);
    
    bool success = false;
    
    string destFileLocation = "unzipTemp";
    
    success = ofxUnZip(filename, destFileLocation);
    if (success)
    {
        string docFileLocation = destFileLocation + "/doc.kml";
        {
            ofxXmlSettings kml;
            success = kml.loadFile(docFileLocation);
            if (success)
            {
                kml.pushTag("kml");
                kml.pushTag("Document");
                kml.pushTag("Placemark");
                kml.pushTag("Polygon");
                kml.pushTag("outerBoundaryIs");
                kml.pushTag("LinearRing");
                
                stringstream coordinates(kml.getValue("coordinates", ""));
                
                if (debugMethod) ofLog(OF_LOG_VERBOSE, coordinates.str());
                
                polyline.clear();
                
                while (true)
                {
                    string tempString;
                    
                    getline(coordinates, tempString, ',');
                    if (tempString.length() == 0) break;
                    stringstream xString(tempString);
                    
                    getline(coordinates, tempString, ',');
                    if (tempString.length() == 0) break;
                    stringstream yString(tempString);
                    
                    getline(coordinates, tempString, ' ');
                    if (tempString.length() == 0) break;
                    stringstream zString(tempString);
                    
                    float x,y;
                    xString >> x;
                    yString >> y;
                    
                    if (debugMethod) ofLog(OF_LOG_VERBOSE, ofToString(x) + ", " + ofToString(y));
                    
                    polyline.addVertex(x, y);
                }
                
                polyline.close();
            }
        }
        ofFile::removeFile(docFileLocation);
    }
    
    return success;
}

