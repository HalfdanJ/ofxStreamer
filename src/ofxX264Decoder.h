//
//  ofxX264Decoder.h
//  x264Example
//
//  Created by Johan Bichel Lindegaard on 4/18/13.
//
//

#pragma once


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


#define 	FF_INPUT_BUFFER_PADDING_SIZE   16
#define     INBUF_SIZE 4096


class ofxX264Decoder {
    
    
public:
    ofxX264Decoder();
    
    
    static int decode_write_frame(const char *outfilename, AVCodecContext *avctx,
                                  AVFrame *frame, int *frame_count, AVPacket *pkt, int last);
    
    static void video_decode_example(const char *outfilename, const char *filename);
    
    static void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize,
                         char *filename);
    
    bool decodeData(const char * data, int data_length);
    
    struct AVFormatContext* avctx;
    struct x264_t* encoder;
    struct SwsContext* imgctx;
    
    
    // new example
    void log_callback(void *ptr, int level, const char *fmt, va_list vargs);

    
    
    
    
};







