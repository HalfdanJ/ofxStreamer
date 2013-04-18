//
//  ofxX264Decoder.h
//  x264Example
//
//  Created by Johan Bichel Lindegaard on 4/18/13.
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

#define WIDTH       640
#define HEIGHT      480
#define FPS         30
#define BITRATE     400000
#define RTP_ADDRESS "127.0.0.1"
#define RTP_PORT    49990



class ofxX264Decoder {
    
    
public:
    ofxX264Decoder();
    
    bool decodeData(const char * data, int data_length);
    
    struct AVFormatContext* avctx;
    struct x264_t* encoder;
    struct SwsContext* imgctx;
    
};






