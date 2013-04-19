#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    x264Decoder.setup(1234);
    x264Encoder.setup(640, 480, "127.0.0.1", 1234);
    data = (unsigned char*) malloc(sizeof(char)* 640 * 480 * 3*10);
}

//--------------------------------------------------------------
void testApp::update(){
    x264Decoder.update();
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
    
    x264Encoder.encodeFrame(inputImage);
//    x264Encoder.encodeFrame(data, 640 * 480 * 3);

    x264Encoder.sendFrame();

}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(0);
    x264Decoder.draw(0, 0);
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