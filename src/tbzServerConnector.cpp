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
    
    sessionID = "";
    sessionActive = false;
    
    shouldUpload = false;
    
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

void tbzServerConnection::responseHandler(ofxHttpResponse & response)
{
    if (response.status != 200)
    {
        cout << "Unexpected status of response: " + response.status << endl;
        return;
    }
    
	if (response.url.compare("/registerID") == 0)
    {
        // For now, response body is plain text assigned sessionID
        setSessionID(response.responseBody);
    }
    else if (response.url.compare("/uploadData") == 0)
    {
        // TASK: Confirm upload with queue
    
        json_error_t error;
        json_t* responseJSON = json_loads(response.responseBody.getBinaryBuffer(), 0, &error);
        
        if(!responseJSON) {
            cout <<  "error: on line:" << error.line << ", " << error.text << endl;
            return;
        }
        
        string logSessionID = json_string_value(json_object_get(responseJSON, "logSessionID"));
        string fileName = json_string_value(json_object_get(responseJSON, "fileName"));
        bool success = json_is_true(json_object_get(responseJSON, "success"));
        
        cout << "Received upload response from sessionID: " << logSessionID << " : " << fileName << " with success " << success << endl;
        
        list<tbzFileToUpload>::iterator uploadFile;
        for (uploadFile = uploadQueue.begin(); uploadFile != uploadQueue.end(); ++uploadFile)
        {
            if (uploadFile->fileName.compare(fileName) == 0 && uploadFile->folderName.compare(logSessionID) == 0)
            {
                break;
            }
        }

        if (uploadFile != uploadQueue.end())
        {
            string uploadedFilePath = uploadFile->path;
            
            // Remove from upload queue
            uploadQueue.erase(uploadFile);
            
            // Delete file
            bool success = ofFile::removeFile(uploadedFilePath);
            if (!success) ofLog(OF_LOG_WARNING, "Failed to remove file: " + uploadedFilePath);
        }
        else
        {
            ofLog(OF_LOG_WARNING, "Upload data returned an upload file confirmation not in queue");
        }
        
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
        ofLog(OF_LOG_VERBOSE, "New sessionID: " + inSessionID);
        
        sessionID = inSessionID;
        sessionActive = true;
        
        ofNotifyEvent(onNewSessionID, inSessionID);
    }
    else
    {
        ofLog(OF_LOG_WARNING, "new session ID not valid, ignoring");
    }
}

void tbzServerConnection::addFileForUpload(string filePath)
{
    Path pocoPath(filePath);
    
    tbzFileToUpload newFileToUpload;
    newFileToUpload.path = filePath;
    newFileToUpload.folderName = pocoPath.directory(pocoPath.depth()-1);;
    newFileToUpload.fileName = pocoPath.getFileName();;
    newFileToUpload.uploadStartCount = 0;
    newFileToUpload.uploadStartTime = 0;
    newFileToUpload.uploadInProgress = false;
    
    uploadQueue.push_back(newFileToUpload);
    
    if (shouldUpload) startFileUploads();
}

void tbzServerConnection::scanFolderForUpload(string path)
{
    ofDirectory directory(path);
    directory.listDir();
    
    list<tbzFileToUpload>::iterator fileToUpload;
    
    // TASK: Add any new files in folder to upload queue
    for(int i = 0; i < directory.numFiles(); i++)
    {
        string searchPath = directory.getPath(i);
        
        // Is this one already in the queue?
        for (fileToUpload = uploadQueue.begin(); fileToUpload != uploadQueue.end(); ++fileToUpload)
        {
            if (fileToUpload->path.compare(searchPath) == 0) break;
        }
        
        // If we did not find the file in our queue, add it to the back
        if (fileToUpload == uploadQueue.end())
        {
            addFileForUpload(fileToUpload->path);
        }
    }
}

void tbzServerConnection::startFileUploads()
{
    // TASK: Start upload from front of queue. Upload of next happens on successful upload via responseHandler()
    
    shouldUpload = true;
    
    if (!uploadQueue.empty())
    {
        // Should put some heuristics here to bypass files that are blocking upload
        // Have startCount and startTime to consider
        list<tbzFileToUpload>::iterator fileToUpload = uploadQueue.begin();
        if (!fileToUpload->uploadInProgress)
        {
            uploadFile(fileToUpload);
        }
    }
}

void tbzServerConnection::uploadFile(list<tbzFileToUpload>::iterator fileToUpload)
{
    if (!sessionActive)
    {
        cout << "Session not active, aborting uploadData" << endl;
        return;
    }
    
    ofxHttpForm form;
    form.action = URLWithPath("uploadData");
    form.method = OFX_HTTP_POST;
    
    form.addFormField("uploadSessionID", sessionID);
    form.addFormField("time", ofGetTimestampString());
    form.addFormField("folder", fileToUpload->folderName);
    form.addFile("file", fileToUpload->path);
    
    fileToUpload->uploadStartCount++;
    fileToUpload->uploadStartTime = ofGetUnixTime();
    fileToUpload->uploadInProgress = true;
    
    httpUtils.addForm(form);
    
}

void tbzServerConnection::setEndPointURL(string inURL)
{
    endPointURL = ofFilePath::addTrailingSlash(inURL);
}

string tbzServerConnection::URLWithPath(string inPath)
{
    return endPointURL + inPath;
}