#include "ofApp.h"

void ofApp::exit(){
    tet.close();
}

//--------------------------------------------------------------
void ofApp::setup(){
    tet.setup();
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackground(0, 0, 0);
    
    // eye data draw
    if (tet.foundGaze()) {
        ofSetColor(0, 255, 0);
        ofRect(tet.getGazeData().lefteye.raw.x, tet.getGazeData().lefteye.raw.y, 10, 10);
        ofRect(tet.getGazeData().righteye.raw.x, tet.getGazeData().righteye.raw.y, 10, 10);
    }
    
    // detail data
    ofSetColor(255, 255, 255);
    
    // key assign
    ofSetColor(255, 2552, 255);
    ofDrawBitmapString(" key s: start streaming data \n key e: stop streaming data \n key d: download calibresult \n key u: upload calibresult \n key t: save calibresult \n key c: load calibresult from text \n", 10,10);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key == 's') {
        tet.requestStreamingData();
    }else if (key == 'e'){
         tet.requestStreamingDataStop();
    }else if (key == 'd'){
        tet.requestCalibResult();
    }else if (key == 't'){
        // API not supported !
        // save calibdata to text
        // tet.saveCalibResult();
    }else if (key == 'u'){
        // API not supported !
        //tet.sendCalibResult();
    }else if (key == 'c'){
        // API not supported !
        // load text to calibdata
        // tet.loadCalibResult();
    }else if (key == 'f'){
        ofToggleFullscreen();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
