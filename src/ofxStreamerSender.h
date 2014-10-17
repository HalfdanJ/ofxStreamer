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
#include  <libavutil/opt.h>
}

#include "ofMain.h"

class ofxStreamerSender {
    
public:
    ofxStreamerSender();
    ~ofxStreamerSender();
    
    void setup(int width, int height, string destination_ip = "127.0.0.1", int destination_port= 1234, string preset="ultrafast", string tune="zerolatency");
    
    void close();
    
    //Sends the encoded frame
    //Supports only RGB formatted image data (so data_length should be width*height*3)
    bool sendFrame(unsigned char *data, int data_length);
    bool sendFrame(ofPixels image);
    
    int width;
    int height;
    
    int frameNum;
    int bitrate;
    float frameRate;

    string url;
    
    string preset;
    string tune;
    
    //Raw encoded h264 frame data
    unsigned char * encodedFrameData;
    int encodedFrameSize;
    
    bool streaming;
    
    
    // todo: getters and setters for tune and preset
    // setter getter for framerate
    // setter for bitrate

    
private:
    struct AVFormatContext* mFormatContext;
    struct x264_t* encoder;
    struct SwsContext* imgctx;
    
    AVCodec *mCodec;
    AVCodecContext *mCodecContext;

    x264_picture_t picture_in;
    x264_picture_t * picture_out;
    AVStream * stream;
    
    long long lastSendTime;
    AVFrame * frame;
    uint8_t *frameBuf;

    
};



