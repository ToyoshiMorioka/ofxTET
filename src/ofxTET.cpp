//
//  ofxTET.cpp
//  TET003
//
//  Created by MORIOKAToyoshi on 2014/10/18.
//
//

#include "ofxTET.h"

ofxTET::ofxTET(){
    
    
    serverstate.framerate = 30;
    serverstate.heartbeatinterval = 3000;
    serverstate.iscalibrated = true;
    serverstate.iscalibrating = false;
    serverstate.push = false;
    serverstate.trackerstate = gtl::ServerState::TRACKER_NOT_CONNECTED;
    serverstate.version = 1;
    
    gazedata.avg.x = 0.0;
    gazedata.avg.y = 0.0;
    gazedata.fix = false;
    gazedata.raw.x = 0.0;
    gazedata.raw.y = 0.0;
    gazedata.state = gtl::GASC_UNKNOWN;
    gazedata.time = 0;
    gazedata.timestamp = "";
    gazedata.lefteye.avg.x = 0.0;
    gazedata.lefteye.avg.y = 0.0;
    gazedata.lefteye.pcenter.x = 0.0;
    gazedata.lefteye.pcenter.y = 0.0;
    gazedata.lefteye.psize = 0.0;
    gazedata.lefteye.raw.x = 0.0;
    gazedata.lefteye.raw.y = 0.0;
    gazedata.righteye.avg.x = 0.0;
    gazedata.righteye.avg.y = 0.0;
    gazedata.righteye.pcenter.x = 0.0;
    gazedata.righteye.pcenter.y = 0.0;
    gazedata.righteye.psize = 0.0;
    gazedata.righteye.raw.x = 0.0;
    gazedata.righteye.raw.y = 0.0;
    
    screen.set(0, 0, 0, 0, 0);
    calibresult.clear();
    
    setStatus = false;
    setConnect = false;
    setPullRequest = false;
    tcpMsg.clear();
    heartbeattimer = serverstate.heartbeatinterval;
    
}

ofxTET::~ofxTET(){
    
}

void ofxTET::setup(){
    setConnect = client.setup(address, portNum);
    if (setConnect) {
        requestServerSatus();
        start();
    }
}

void ofxTET::updateGazeData(jsonxx::Object &src){
    if (src.has<string>("timestamp")) {
        gazedata.timestamp = src.get<string>("timestamp");
    }
    
    if (src.has<jsonxx::Number>("time")) {
        gazedata.time = src.get<jsonxx::Number>("time");
        
    }
    
    if (src.has<jsonxx::Boolean>("fix")) {
        gazedata.fix = src.get<jsonxx::Boolean>("fix");
    }
    
    if (src.has<jsonxx::Number>("state")) {
        gazedata.state = src.get<jsonxx::Number>("state");
    }
    
    if (src.has<jsonxx::Object>("avg")) {
        gazedata.avg.x = (float)(src.get<jsonxx::Object>("avg").get<jsonxx::Number>("x"));
        gazedata.avg.y = (float)(src.get<jsonxx::Object>("avg").get<jsonxx::Number>("y"));
    }
    
    if (src.has<jsonxx::Object>("raw")) {
        gazedata.raw.x = (float)(src.get<jsonxx::Object>("raw").get<jsonxx::Number>("x"));
        gazedata.raw.y = (float)(src.get<jsonxx::Object>("raw").get<jsonxx::Number>("y"));
    }
    
    if (src.has<jsonxx::Object>("lefteye")) {
        if (src.get<jsonxx::Object>("lefteye").has<jsonxx::Number>("psize")) {
            gazedata.lefteye.psize = src.get<jsonxx::Object>("lefteye").get<jsonxx::Number>("psize");
        }
        
        if (src.get<jsonxx::Object>("lefteye").has<jsonxx::Object>("avg")) {
            gazedata.lefteye.avg.x = (float)(src.get<jsonxx::Object>("lefteye").get<jsonxx::Object>("avg").get<jsonxx::Number>("x"));
            gazedata.lefteye.avg.y = (float)(src.get<jsonxx::Object>("lefteye").get<jsonxx::Object>("avg").get<jsonxx::Number>("y"));
        }
        
        if (src.get<jsonxx::Object>("lefteye").has<jsonxx::Object>("raw")) {
            gazedata.lefteye.raw.x = (float)(src.get<jsonxx::Object>("lefteye").get<jsonxx::Object>("raw").get<jsonxx::Number>("x"));
            gazedata.lefteye.raw.y = (float)(src.get<jsonxx::Object>("lefteye").get<jsonxx::Object>("raw").get<jsonxx::Number>("y"));
        }
    }
    
    if (src.has<jsonxx::Object>("righteye")) {
        if (src.get<jsonxx::Object>("righteye").has<jsonxx::Number>("psize")) {
            gazedata.righteye.psize = src.get<jsonxx::Object>("righteye").get<jsonxx::Number>("psize");
        }
        
        if (src.get<jsonxx::Object>("righteye").has<jsonxx::Object>("avg")) {
            gazedata.righteye.avg.x = (float)(src.get<jsonxx::Object>("righteye").get<jsonxx::Object>("avg").get<jsonxx::Number>("x"));
            gazedata.righteye.avg.y = (float)(src.get<jsonxx::Object>("righteye").get<jsonxx::Object>("avg").get<jsonxx::Number>("y"));
        }
        
        if (src.get<jsonxx::Object>("righteye").has<jsonxx::Object>("raw")) {
            gazedata.righteye.raw.x = (float)(src.get<jsonxx::Object>("righteye").get<jsonxx::Object>("raw").get<jsonxx::Number>("x"));
            gazedata.righteye.raw.y = (float)(src.get<jsonxx::Object>("righteye").get<jsonxx::Object>("raw").get<jsonxx::Number>("y"));
        }
    }
    
    
}

void ofxTET::update(){
    if (setConnect) {
        string tempMsg = client.receiveRaw();
        
        // if
        if (tempMsg.length() < TCP_MAX_MSG_SIZE && tempMsg.length() > 0) {
            tcpMsg += tempMsg;
            
            std::cout << "recieve message:" << tcpMsg << endl;
            
            if (jsonxx::validate(tcpMsg)) {
                jsonxx::Object msg;
                msg.parse(tcpMsg);
                if(msg.has<jsonxx::Object>("values")){
                    jsonxx::Object valuesObj = msg.get<jsonxx::Object>("values");
                    // values
                    if (valuesObj.has<jsonxx::Object>("frame")) { // get frame
                        jsonxx::Object frameObj = valuesObj.get<jsonxx::Object>("frame");
                        updateGazeData(frameObj);
                    }if (valuesObj.has<jsonxx::Object>("calibresult")) { // get calibresult
                        jsonxx::Object calibObj = valuesObj.get<jsonxx::Object>("calibresult");
                        updateCalibResult(calibObj);
                    }else{ // other heatbeat....etc
                        updateStatus(valuesObj);
                    }
                }
            }
            tcpMsg.clear();
        }else if (tempMsg.length() == TCP_MAX_MSG_SIZE){
            tcpMsg += tempMsg;
        }
    }
}

void ofxTET::close(){
    if (isThreadRunning()) {
        stop();
    }
    if (setConnect) {
        client.close();
    }
}

void ofxTET::threadedFunction(){
    while(isThreadRunning())
    {
        // Attempt to lock the mutex.  If blocking is turned on,
        if(lock())
        {
            update();
            if (setStatus) {
                heartbeattimer -= 1000 / serverstate.framerate;
                if (heartbeattimer <= 0) {
                    sendHeatBeat();
                }
            }
            unlock();
            sleep(1000 / serverstate.framerate);
        }
        else
        {
            ofLogWarning("threadedFunction()") << "Unable to lock mutex.";
        }
    }
}

void ofxTET::stop()
{
    stopThread();
}

void ofxTET::start(){
    startThread();
}

void ofxTET::sendHeatBeat(){
    if (setConnect && setPullRequest) {
        jsonxx::Object params;
        params << "category" << "heartbeat";
        
        const string src = params.write(jsonxx::JSON);
        client.sendRaw(src);
        std::cout << "send message:" << src << endl;
        heartbeattimer = serverstate.heartbeatinterval;
    }
}

void ofxTET::requestServerSatus(){
    if (setPullRequest == false &&setConnect) {
        jsonxx::Object params;
        params << "category" << "tracker";
        params << "request" << "get";
        
        jsonxx::Array value;
        value << "push"
        << "iscalibrated"
        << "heartbeatinterval"
        << "version"
        << "trackerstate"
        << "framerate"
        << "iscalibrating"
        << "screenindex"
        << "screenresw"
        << "screenresh"
        << "screenpsyw"
        << "screenpsyh";
        params << "values" << value;
        
        // << "calibresult"
        
        const string src = params.write(jsonxx::JSON);
        client.sendRaw(src);
        std::cout << "send message:" << src << endl;
    }
}

void ofxTET::requestCalibResult(){
    if (setPullRequest == false && setConnect) {
        jsonxx::Object params;
        params << "category" << "tracker";
        params << "request" << "get";
        
        jsonxx::Array value;
        value << "calibresult";
        params << "values" << value;
        
        const string src = params.write(jsonxx::JSON);
        client.sendRaw(src);
        std::cout << "send message:" << src << endl;
    }
}

void ofxTET::requestStreamingData(){
    if (setConnect && setStatus) {
        jsonxx::Object params;
        params << "category" << "tracker";
        params << "request" << "set";
        
        jsonxx::Object value;
        value << "push" << true;
        value << "version" << 1;
        params << "values" << value;
        
        const string src = params.write(jsonxx::JSON);
        client.sendRaw(src);
        setPullRequest = true;
    }
}

void ofxTET::updateStatus(jsonxx::Object &src){
    if (src.has<jsonxx::Number>("framerate")) {
        serverstate.framerate = src.get<jsonxx::Number>("framerate");
    }else{
        return;
    }
    
    if (src.has<jsonxx::Number>("heartbeatinterval")) {
        serverstate.heartbeatinterval = src.get<jsonxx::Number>("heartbeatinterval");
        
        if (heartbeattimer > serverstate.heartbeatinterval) {
            heartbeattimer = serverstate.heartbeatinterval;
        }
        
    }else{
        return;
    }
    
    if (src.has<jsonxx::Number>("trackerstate")) {
        serverstate.trackerstate = src.get<jsonxx::Number>("trackerstate");
    }else{
        return;
    }
    
    if (src.has<jsonxx::Number>("version")) {
        serverstate.version = src.get<jsonxx::Number>("version");
    }else{
        return;
    }
    
    if (src.has<jsonxx::Boolean>("iscalibrated")) {
        serverstate.iscalibrated = src.get<jsonxx::Boolean>("iscalibrated");
    }else{
        return;
    }
    
    if (src.has<jsonxx::Boolean>("iscalibrating")) {
        serverstate.iscalibrating = src.get<jsonxx::Boolean>("iscalibrating");
    }else{
        return;
    }
    
    if (src.has<jsonxx::Boolean>("push")) {
        serverstate.push = src.get<jsonxx::Boolean>("push");
    }else{
        return;
    }
    
    if (src.has<jsonxx::Number>("screenindex")) {
        screen.screenindex = src.get<jsonxx::Number>("screenindex");
    }else{
        return;
    }
    
    if (src.has<jsonxx::Number>("screenpsyh")) {
        screen.screenpsyh = src.get<jsonxx::Number>("screenpsyh");
    }else{
        return;
    }
    
    if (src.has<jsonxx::Number>("screenpsyw")) {
        screen.screenpsyw = src.get<jsonxx::Number>("screenpsyw");
    }else{
        return;
    }
    
    if (src.has<jsonxx::Number>("screenresh")) {
        screen.screenresh = src.get<jsonxx::Number>("screenresh");
    }else{
        return;
    }
    
    if (src.has<jsonxx::Number>("screenresw")) {
        screen.screenresw = src.get<jsonxx::Number>("screenresw");
    }else{
        return;
    }
    
    setStatus = true;
}

void ofxTET::updateCalibResult(jsonxx::Object &src){
    if (src.has<jsonxx::Boolean>("result")) {
        calibresult.result = src.get<jsonxx::Boolean>("result");
    }
    
    if (src.has<jsonxx::Number>("deg")) {
        calibresult.deg = src.get<jsonxx::Number>("deg");
    }
    
    if (src.has<jsonxx::Number>("degl")) {
        calibresult.degl = src.get<jsonxx::Number>("degl");
    }
    
    if (src.has<jsonxx::Number>("degr")) {
        calibresult.degr = src.get<jsonxx::Number>("degr");
    }
    
    calibresult.calibpoints.resize(0);
    if (src.has<jsonxx::Array>("calibpoints")) {
        jsonxx::Array calibpoints = src.get<jsonxx::Array>("calibpoints");
        for (int i = 0; i < calibpoints.size(); i++) {
            if (calibpoints.has<jsonxx::Object>(i)) {
                jsonxx::Object tempObj = calibpoints.get<jsonxx::Object>(i);
                gtl::CalibPoint temp;
                
                if (tempObj.has<jsonxx::Object>("acd")) {
                    temp.acd.ad = tempObj.get<jsonxx::Object>("acd").get<jsonxx::Number>("ad");
                    temp.acd.adl = tempObj.get<jsonxx::Object>("acd").get<jsonxx::Number>("adl");
                    temp.acd.adr = tempObj.get<jsonxx::Object>("acd").get<jsonxx::Number>("adr");
                }
                
                if (tempObj.has<jsonxx::Object>("asdp")) {
                    temp.asdp.asd = tempObj.get<jsonxx::Object>("asdp").get<jsonxx::Number>("asd");
                    temp.asdp.asdl = tempObj.get<jsonxx::Object>("asdp").get<jsonxx::Number>("asdl");
                    temp.asdp.asdr = tempObj.get<jsonxx::Object>("asdp").get<jsonxx::Number>("asdr");
                }
                
                if (tempObj.has<jsonxx::Object>("mepix")) {
                    temp.mepix.mep = tempObj.get<jsonxx::Object>("mepix").get<jsonxx::Number>("mep");
                    temp.mepix.mepl = tempObj.get<jsonxx::Object>("mepix").get<jsonxx::Number>("mepl");
                    temp.mepix.mepr = tempObj.get<jsonxx::Object>("mepix").get<jsonxx::Number>("mepr");
                }
                
                if (tempObj.has<jsonxx::Object>("cp")) {
                    temp.cp.x = tempObj.get<jsonxx::Object>("cp").get<jsonxx::Number>("x");
                    temp.cp.y = tempObj.get<jsonxx::Object>("cp").get<jsonxx::Number>("y");
                }
                
                if (tempObj.has<jsonxx::Object>("mecp")) {
                    temp.mecp.x = tempObj.get<jsonxx::Object>("mecp").get<jsonxx::Number>("x");
                    temp.mecp.y = tempObj.get<jsonxx::Object>("mecp").get<jsonxx::Number>("y");
                }
                
                calibresult.calibpoints.push_back(temp);
            }
        }
    }
}

void ofxTET::requestStreamingDataStop(){
    if (setPullRequest) {
        jsonxx::Object params;
        params << "category" << "tracker";
        params << "request" << "set";
        
        jsonxx::Object value;
        value << "push" << false;
        value << "version" << 1;
        params << "values" << value;
        
        const string src = params.write(jsonxx::JSON);
        client.sendRaw(src);
        setPullRequest = false;
    }
}

//void ofxTET::loadCalibResult(){
//    
//}
//
//void ofxTET::saveCalibResult(){
//    jsonxx::Object resultData;
//    
////    resultData << YOGAMOJIJSON_ACCESS_KEY << personalData.access_key;
////    
////    jsonxx::Object data;
////    data << YOGAMOJIJSON_EVENT << data_event;
////    data << YOGAMOJIJSON_PARAMS << params;
//
//    
//}
//
//void ofxTET::sendCalibResult(){
//    
//}
