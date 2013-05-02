#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetWindowTitle("Sender");
    ofSetWindowPosition(0, 0);
    ofSetFrameRate(70);

    ofLogLevel(OF_LOG_WARNING);
    
    //sender.setup(640, 480, "jive.local", 1234);
    sender.setup(640, 480);
    
    data = (unsigned char*) malloc(sizeof(char)* 640 * 480 * 3*10);
    
    grabber.initGrabber(640, 480);
    
    
    //oscReceiver.setup(9999);
    
    
    latency = 0;
    
}

//--------------------------------------------------------------
void testApp::update(){
    grabber.update();
    
    if(sendPing){
        sendPing = false;
        sendPingTime = ofGetElapsedTimeMillis();
        
        
        unsigned char bytes[640*480*3];
        for(int i=0;i<640*480*3;i++){
            bytes[i] = 255;
        }
        
        
        sender.encodeFrame(bytes, 640*480*3);
        sender.sendFrame();
        
    }
    else if(grabber.isFrameNew()){
        
        ofBuffer buffer;
        buffer.set((char*)data, 640 * 480 * 3);
        
        inputImage.setFromPixels(data, 640, 480, OF_IMAGE_COLOR);
        
        sender.encodeFrame(grabber.getPixels(),  640 * 480 * 3);
        //    x264Encoder.encodeFrame(data, 640 * 480 * 3);
        
        sender.sendFrame();
   }
    
    
    
   /* if(oscReceiver.hasWaitingMessages()){
        ofxOscMessage msg;
        oscReceiver.getNextMessage(&msg);
        
        cout<<"Got message "<<msg.getAddress()<<endl;
        
        latency = ofGetElapsedTimeMillis() -  sendPingTime;
        
        cout<<"Diff "<<latency<<endl;
    }*/

}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackgroundGradient(ofColor(78,28,28), ofColor(25,14,14),OF_GRADIENT_LINEAR);
   // inputImage.draw(0,0,640,480);
    grabber.draw(0, 0, 640, 480);
    
    int y = 15;
    int x = 650;
    
    ofDrawBitmapString("Streamer Sender Example", 650, y);
    ofDrawBitmapString("Frame Num: \t\t"+ofToString(sender.frameNum), 650, y+=20);
    ofDrawBitmapString("Frame Rate: "+ofToString(sender.frameRate,1)+" fps", 650, y+=15);
    ofDrawBitmapString("bitrate: "+ofToString(sender.bitrate)+" kbits/s", 650, y+=15);
    ofDrawBitmapString("URL: "+sender.url, 650, y+=35);
    ofDrawBitmapString("Preset: "+sender.preset, 650, y+=15);
    
    if(latency){
        ofDrawBitmapString("Latency: "+ofToString(latency)+" ms", 650, y+=15);

    } else {
        ofDrawBitmapString("Latency: Press any key", 650, y+=15);
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    sendPing = true;
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