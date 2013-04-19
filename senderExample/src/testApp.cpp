#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofLogLevel(OF_LOG_WARNING);
    
    //    sender.setup(640, 480, "jive.local", 1234);
    sender.setup(640, 480);
    
    data = (unsigned char*) malloc(sizeof(char)* 640 * 480 * 3*10);
    
    grabber.initGrabber(640, 480);
    
    
    ofSetFrameRate(30);
}

//--------------------------------------------------------------
void testApp::update(){
    grabber.update();
    
    
    if(grabber.isFrameNew()){

        ofLogLevel(OF_LOG_WARNING);
        
        int i=0;
        for(int y=0;y<480;y++){
            for(int x=0;x<640;x++){
                data[i*3] = 255*x/640.0;
                data[i*3+1] = (sin((x+ofGetFrameNum()*5)/60.0)+1)*100;
                data[i*3+2] = 255*y/480.0;
                i++;
            }
        }
        
        //    cout<<ofGetFrameRate()<<endl;
        
        
        
        ofBuffer buffer;
        buffer.set((char*)data, 640 * 480 * 3);
        
        inputImage.setFromPixels(data, 640, 480, OF_IMAGE_COLOR);
        
        sender.encodeFrame(grabber.getPixels(),  640 * 480 * 3);
        //    x264Encoder.encodeFrame(data, 640 * 480 * 3);
        
        sender.sendFrame();
    }

}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackgroundGradient(ofColor(50,50,50), ofColor(0,0,0));
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