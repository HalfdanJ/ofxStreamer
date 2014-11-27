#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetWindowTitle("Receiver");
    ofSetWindowPosition(0, 540);

    ofLogLevel(OF_LOG_WARNING);
    receiver.setup(1234);
}

//--------------------------------------------------------------
void testApp::update(){
    receiver.update();
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackgroundGradient(ofColor(28,78,28), ofColor(14,25,14),OF_GRADIENT_LINEAR);
    
   receiver.draw(0, 0, 640, 480);

    int y = 15;
    int x = 650;
    
    ofDrawBitmapString("Streamer Receiver Example", 650, y);
    ofDrawBitmapString("Frame Num: \t\t"+ofToString(receiver.frameNum), 650, y+=20);
    ofDrawBitmapString("Frame Rate: "+ofToString(receiver.frameRate,1)+" fps", 650, y+=15);
    ofDrawBitmapString("bitrate: "+ofToString(receiver.bitrate)+" kbits/s", 650, y+=15);
    ofDrawBitmapString("URL: "+receiver.url, 650, y+=35);
    ofDrawBitmapString("Resolution: "+ofToString(receiver.getWidth())+"x"+ofToString(receiver.getHeight()), 650, y+=35);
    
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