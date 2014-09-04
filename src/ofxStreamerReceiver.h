//
//  ofxStreamerReceiver.h
//  x264Example
//
//

#pragma once

#define 	FF_INPUT_BUFFER_PADDING_SIZE   16
#define     INBUF_SIZE 1024

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

class ofxStreamerReceiver : public ofBaseVideoPlayer, ofThread {
    
public:
    ofxStreamerReceiver();
    bool            connected;
    bool            allocated;
    bool            bHavePixelsChanged;
    int             width;
    int             height;
    int             port;
    string          host;
    string          url;
    
    bool            setup(int port, std::string host="udp://@");
    void            update();
    bool            isFrameNew();
    void            draw(float x, float y);
    void            draw(float x, float y, float w, float h);
    void            draw(const ofPoint &p);
    void            draw(const ofRectangle &r);
    void            close();
    ofTexture &     getTextureReference();
    ofTexture *     getTexture(){return NULL;}
    unsigned char * getPixels();
    ofPixelsRef     getPixelsRef();
    float           getHeight();
    float           getWidth();
    bool            isConnected();
    long            frameNum;
    int             bitrate;
    float           frameRate;
    
    bool            loadMovie(string name){return false;}
    void            play(){}
    void            pause(){}
    void            stop(){}
    
    bool isPaused(){return false;}
    bool isLoaded(){return connected;}
    bool isPlaying(){return false;}
    bool setPixelFormat(ofPixelFormat pixelFormat){return false;}
    ofPixelFormat getPixelFormat(){return OF_PIXELS_RGB;}
    
    void setConnected(bool d);
    
    ofEvent<void> onConnect;
    ofEvent<void> onDisconnect;

    
private:
    struct SwsContext* imgctx;
    
    AVFormatContext*    mFormatContext;
    SwsContext*         img_convert_ctx;
    AVFrame*            mFrame;
    AVFrame*            picrgb;
    uint8_t*            picture_buf2;
    uint8_t*            picture_buf;
    AVStream*           mVideoStream;
    AVCodecContext*     mVideoDecodeContext;
    int                 mVideoStreamIdx;    
    
    long long           lastReceiveTime;
    
    ofImage *           lastFrame;
    int                 encodedFrameSize;
    
    void                threadedFunction();
    
    bool                newFrame;
    bool                dead;
    
    unsigned char *     pixelData;
    
    ofMutex             mutex;
    bool                open;
    

};







