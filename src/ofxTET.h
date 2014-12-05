//
//  ofxTET.h
//  TET003
//
//  Created by MORIOKAToyoshi on 2014/10/18.
//
//

#ifndef __TET003__ofxTET__
#define __TET003__ofxTET__

#include "ofMain.h"
#include "ofxNetwork.h"
#include "jsonxx.h"

#include "gazeapi_types.h"

static int portNum = 6555;
static string address = "127.0.0.1";

class ofxTET:public ofThread{
public:
    ofxTET();
    ~ofxTET();
    void setup();
    void close();
    
    void requestServerSatus(); // get server status
    void requestCalibResult(); // get calibresult
    void requestStreamingData(); // streaming start
    void requestStreamingDataStop(); // streaming stop
    void sendHeatBeat(); // send heartbeat
    //void sendCalibResult(); // send calibresult
    
    //void loadCalibResult();
    //void saveCalibResult();
    
    gtl::GazeData getGazeData(){
        return gazedata;
    }
    
    bool foundGaze(){
        if (gazedata.state != gtl::GazeData::GD_STATE_TRACKING_FAIL &&
            gazedata.state != gtl::GazeData::GD_STATE_TRACKING_LOST ) {
            return true;
        }else{
            return false;
        }
    }
    
    
private:
    void update();
    void updateGazeData(jsonxx::Object &src);
    void updateStatus(jsonxx::Object &src);
    void updateCalibResult(jsonxx::Object &src);
    
    // function for thread
    void start();
    void stop();
    void threadedFunction();
    
    ofxTCPClient client;
    bool setStatus;
    bool setConnect;
    bool setPullRequest;
    int heartbeattimer;
    string tcpMsg;
    
    gtl::ServerState serverstate;
    gtl::Screen screen;
    gtl::CalibResult calibresult;
    gtl::GazeData gazedata;
};



#endif /* defined(__TET003__ofxTET__) */
