//
//  ofxStreamerReceiver.h
//  x264Example
//
//  Created by Johan Bichel Lindegaard on 4/18/13.
//
//

#pragma once

#define 	FF_INPUT_BUFFER_PADDING_SIZE   16
#define     INBUF_SIZE 4096

#include "ofMain.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libswscale/swscale.h>
}

class ofxStreamerReceiver {
    
public:
    ofxStreamerReceiver();
    
    bool connected;
    
    bool allocated;
    bool bHavePixelsChanged;
    
    int width;
    int height;
    
    int port;
    std::string host;
        
    bool setup(int port, std::string host="udp://@");
    
    void update();
    bool isFrameNew();
    
    void draw(float x, float y);
    void draw(float x, float y, float w, float h);
    void draw(const ofPoint &p);
    void draw(const ofRectangle &r);
    
    void close();
    
    ofTexture getTextureReference();
    unsigned char * getPixels();
    
    float getHeight();
    float getWidth();
    
    bool isPlaying();
    
    
    
private:
    
    struct AVFormatContext* avctx;
    struct x264_t* encoder;
    struct SwsContext* imgctx;
    
    AVFormatContext* context;
    AVCodecContext* ccontext;
    SwsContext* img_convert_ctx;
    
    int video_stream_index;
    
    AVPacket packet;
    
    AVStream* stream;
    int cnt;
    AVFormatContext* oc;
    AVFrame* pic;
    AVFrame* picrgb;
    uint8_t* picture_buf2;
    uint8_t* picture_buf;
    
    ofImage * lastFrame;
    
};







