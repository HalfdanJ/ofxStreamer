//
//  ofxStreamer
//  by Johan Bichel && Jonas Jongejan
//  https://github.com/HalfdanJ/ofxStreamer
//
//
//

#pragma once


#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <vector.h>

extern "C" {
#include <x264.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include "ofMain.h"



class ofxStreamerSender {
    
    
public:
    ofxStreamerSender();
    
    void setup(int width, int height, string destination_ip = "127.0.0.1", int destination_port= 1234, string preset="ultrafast");
    
    //Supports only RGB formatted image data (so data_length should be width*height*3)
    bool encodeFrame(unsigned char *data, int data_length);
    
    bool encodeFrame(ofImage image);
    
    //Sends the encoded frame
    bool sendFrame();
    
    int width;
    int height;
    
    int frameNum;
    int bitrate;
    float frameRate;

    string url;
    
    string preset;

    
    x264_picture_t * getPictureRef();
    
    //Raw encoded h264 frame data
    unsigned char * encodedFrameData;
    int encodedFrameSize;
    
private:
    struct AVFormatContext* avctx;
    struct x264_t* encoder;
    struct SwsContext* imgctx;

    x264_picture_t picture_in;
    x264_picture_t * picture_out;
    AVStream * stream;
    
    long long lastSendTime;


};



