//
//  tbzPlatformDefineTests.h
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 30/08/2012.
//
//

#ifndef IMCFestivalApp_tbzPlatformDefineTests_h
#define IMCFestivalApp_tbzPlatformDefineTests_h

#ifdef TARGET_OSX
ofLog(OF_LOG_VERBOSE, "TARGET_OSX");
#endif

#if TARGET_OS_IPHONE
ofLog(OF_LOG_VERBOSE, "TARGET_OS_IPHONE");
#endif

#ifdef OF_TARGET_IPHONE
ofLog(OF_LOG_VERBOSE, "OF_TARGET_IPHONE");
#endif

#ifdef OF_TARGET_OSX
ofLog(OF_LOG_VERBOSE, "OF_TARGET_OSX");
#endif

#if OF_TARGET_IPHONE
ofLog(OF_LOG_VERBOSE, "OF_TARGET_IPHONE");
#endif

#if OF_TARGET_OSX
ofLog(OF_LOG_VERBOSE, "OF_TARGET_OSX");
#endif

#endif
