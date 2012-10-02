//
//  tbzServerConnector.h
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 26/09/2012.
//
//

#pragma once

#include "ofxHttpUtils.h"
#include "jansson.h"
#include <list>

#define TBZServerConnector_EndPointURL "http://localhost:8888/"

struct tbzFileToUpload
{
    string          path;
    string          folderName;
    string          fileName;
    int             uploadStartCount;
    unsigned int    uploadStartTime;
    bool            uploadInProgress;
};

class tbzServerConnection
{
    public:
        tbzServerConnection();
   
        void setEndPointURL(string inURL);
        void setSessionID(string inSessionID);
        
        void startSession();
        ofEvent<string> onNewSessionID;
    
        void addFileForUpload(string filepath);
        void scanFolderForUpload(string path);
    
        void startFileUploads();
    
    private:
        string URLWithPath(string inPath);
        void uploadFile(list<tbzFileToUpload>::iterator fileToUpload);
        void responseHandler(ofxHttpResponse & response);
    
        ofxHttpUtils httpUtils;
        string endPointURL;
        string sessionID;
        bool sessionActive;
        list<tbzFileToUpload> uploadQueue;
        bool shouldUpload;
};