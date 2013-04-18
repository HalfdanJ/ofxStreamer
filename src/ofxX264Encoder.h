//
//  ofxX264.h
//  x264Example
//
//  Created by Jonas Jongejan on 17/04/13.
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

#define FPS         30
#define BITRATE     400000
#define RTP_ADDRESS "127.0.0.1"
#define RTP_PORT    49990



class ofxX264Encoder {
    
    
public:
    ofxX264Encoder();
    
    void setup(int width, int height);
    
    bool encodeData(unsigned char *data, int data_length, int stride, int height );
    
    int width;
    int height;
    
    
    x264_picture_t * getPicture();
    
    
    unsigned char * encodedFrameData;
    int encodedFrameSize;
    
private:
    struct AVFormatContext* avctx;
    struct x264_t* encoder;
    struct SwsContext* imgctx;

    x264_picture_t picture_in;
    x264_picture_t * picture_out;


};



