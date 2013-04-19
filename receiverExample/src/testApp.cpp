#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetWindowTitle("Receiver");
    ofLogLevel(OF_LOG_WARNING);
    receiver.setup(1234);
}

//--------------------------------------------------------------
void testApp::update(){
    receiver.update();
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(0,0,0);
    
    if(receiver.isFrameNew()) {
        receiver.draw(0, 0);
    }
}

void testApp::exit(){
    receiver.close();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}