//
//  tbzServerConnector.h
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 26/09/2012.
//
//

#pragma once

#include "ofxHttpUtils.h"

#define TBZServerConnector_EndPointURL "http://localhost:8888/" 

class tbzServerConnection
{
    public:
        tbzServerConnection();
        void startSession();
        void uploadData();
    
        void setSessionID(string inSessionID);
        void setEndPointURL(string inURL);
        string URLWithPath(string inPath);
    
        void responseHandler(ofxHttpResponse & response);
    
    private:
        ofxHttpUtils httpUtils;
        string endPointURL;
        string sessionID;
        bool sessionActive;
};