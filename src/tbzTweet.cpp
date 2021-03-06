//
//  tbzTweet.cpp
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 20/09/2012.
//
//

#include "tbzTweet.h"


bool tbzTweet::getGeoLocation(ofPoint &point)
{
    json_error_t error;
    json_t* tweetJSON = json_loads(sourceJSON.c_str(), 0, &error);
    if(!tweetJSON) {
        cout <<  "error: on line:" << error.line << ", " << error.text << endl;
        return false;
    }
    
    json_t* geo = json_object_get(tweetJSON, "geo");
    if(!json_is_object(geo)) {
        // No geo key or unexpected format
        return false;
    }
    
    json_t* coords = json_object_get(geo, "coordinates");
    if (!json_is_array(coords)) {
        cout << "error: cannot get coords from tweet geo" << endl;
        return false;
    }
    
    json_t* jsonLat = json_array_get(coords, 0);
    json_t* jsonLong = json_array_get(coords, 1);
    
    point.set(json_real_value(jsonLong), json_real_value(jsonLat));
    
    return true;
}