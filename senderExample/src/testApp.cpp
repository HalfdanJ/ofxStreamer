#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetWindowTitle("Sender");
    ofSetWindowPosition(0, 0);
    ofSetFrameRate(30);

    ofLogLevel(OF_LOG_WARNING);
    
    grabber.initGrabber(640, 480);
    
    sender.setup(grabber.getWidth(), grabber.getHeight(), "127.0.0.1");
}

//--------------------------------------------------------------
void testApp::update(){
    grabber.update();
    
    if(grabber.isFrameNew()){
        sender.sendFrame(grabber.getPixelsRef());
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackgroundGradient(ofColor(78,28,28), ofColor(25,14,14),OF_GRADIENT_LINEAR);

    grabber.draw(0, 0, 640, 480);
    
    int y = 15;
    int x = 650;
    
    ofDrawBitmapString("Streamer Sender Example", 650, y);
    ofDrawBitmapString("Frame Num: \t\t"+ofToString(sender.frameNum), 650, y+=20);
    ofDrawBitmapString("Frame Rate: "+ofToString(sender.frameRate,1)+" fps", 650, y+=15);
    ofDrawBitmapString("App Frame Rate: "+ofToString(ofGetFrameRate(),1)+" fps", 650, y+=15);
    ofDrawBitmapString("bitrate: "+ofToString(sender.bitrate)+" kbits/s", 650, y+=15);
    ofDrawBitmapString("URL: "+sender.url, 650, y+=35);
    ofDrawBitmapString("Preset: "+sender.preset, 650, y+=15);
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == 'o'){
        sender.setup(grabber.getWidth(), grabber.getHeight());
    }
    if(key == 'c'){
        sender.close();
    }
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