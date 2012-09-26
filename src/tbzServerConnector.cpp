//
//  tbzServerConnector.cpp
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 26/09/2012.
//
//

#include "tbzServerConnector.h"

tbzServerConnection::tbzServerConnection()
{
    setEndPointURL(TBZServerConnector_EndPointURL);
    setSessionID("");
    sessionActive = false;
    
    ofAddListener(httpUtils.newResponseEvent,this,&tbzServerConnection::responseHandler);
	httpUtils.start();
}

void tbzServerConnection::startSession()
{
    ofxHttpForm form;
    form.action = URLWithPath("registerID");
    form.method = OFX_HTTP_POST;
    
    form.addFormField("sessionID", sessionID);
    form.addFormField("time", ofGetTimestampString());
    
    httpUtils.addForm(form);
}

void tbzServerConnection::uploadData()
{
    if (!sessionActive)
    {
        cout << "Session not active, aborting uploadData" << endl;
        return;
    }
    ofxHttpForm form;
    form.action = URLWithPath("uploadData");
    form.method = OFX_HTTP_POST;
    
    form.addFormField("sessionID", sessionID);
    form.addFormField("time", ofGetTimestampString());
    form.addFile("sensorData", "socialMessageStore.xml");
    
    httpUtils.addForm(form);
}

void tbzServerConnection::responseHandler(ofxHttpResponse & response)
{
    if (response.status != 200)
    {
        cout << "Unexpected status of response: " + response.status << endl;
        return;
    }
    
	if (response.url.compare(URLWithPath("registerID")))
    {
        // For now, response body is plain text assigned sessionID
        setSessionID(response.responseBody);
    }
    else if (response.url.compare(URLWithPath("uploadData")))
    {
        // TASK: Confirm upload with queue
        cout << "TODO: CONFIRM FILE UPLOAD WITH QUEUE" << endl;
    }
    else
    {
        cout << "Can't handle response: " << response.status << ": " << response.responseBody << endl;
    }
}

void tbzServerConnection::setSessionID(string inSessionID)
{
    // TASK: Assign new sessionID if tests valid.
    
    // For now...
    if (inSessionID.length() > 0)
    {
        sessionID = inSessionID;
        sessionActive = true;
    }
}

void tbzServerConnection::setEndPointURL(string inURL)
{
    endPointURL = ofFilePath::addTrailingSlash(inURL);
}

string tbzServerConnection::URLWithPath(string inPath)
{
    return endPointURL + inPath;
}