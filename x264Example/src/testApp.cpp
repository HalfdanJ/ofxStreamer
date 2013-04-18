#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    x264Encoder.setup(640, 480);
    
    // fake image generation
    // disregard how wrong this is; just writing a quick test
    int strides = 640 * 3;
    unsigned char * data = (unsigned char*) malloc(sizeof(char)* 640 * 480 * 3*10);
    

    int i=0;
    for(int y=0;y<480;y++){
        for(int x=0;x<640;x++){
            data[i*3] = 255*x/640.0;
            data[i*3+1] = 0;
            data[i*3+2] = 255*y/480.0;
            i++;
        }
    }
    
    x264Encoder.encodeData(data, 640 * 480 * 3, strides, 480);
    
    ofBuffer buffer;
    buffer.set((char*)data, 640 * 480 * 3);
    
    inputImage.setFromPixels(data, 640, 480, OF_IMAGE_COLOR);
    // scale the image
    
    
}

//--------------------------------------------------------------
void testApp::update(){
    
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(0,0,0);
    inputImage.draw(0,0,640,480);
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