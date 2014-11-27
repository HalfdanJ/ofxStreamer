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

class ofxStreamerReceiver : public ofBaseVideoDraws, ofThread {
    
public:
    ofxStreamerReceiver();
    bool            connected;
    bool            allocated;
    int             width;
    int             height;
    int             port;
    string          host;
    string          url;
    float           frameRate;
    int             frameNum;
    int             bitrate;

    
    bool            setup(int port, std::string host="udp://@");
    void            close();

    
    void            update();
    
    bool isConnected() const;
    void setConnected(bool d);
    
    ofEvent<void> onConnect;
    ofEvent<void> onDisconnect;
    
    
    //ofBaseVideoDraws
    ofPixels& 			getPixels();
    const ofPixels&		getPixels() const;
    
    bool isFrameNew() const;

    bool isInitialized() const {};
    
    bool setPixelFormat(ofPixelFormat pixelFormat) {}
    
    ofPixelFormat getPixelFormat() const {}
    
    
    void draw(float x, float y) const;
    void draw(float x,  float y, float w, float h) const;
    
    float getHeight() const;
    float getWidth() const;
    
    
    ofTexture & getTexture();
    
    const ofTexture & getTexture() const;
    
    void setUseTexture(bool bUseTex) {};
    
    bool isUsingTexture() const {};
    
    vector<ofTexture> & getTexturePlanes(){};
    const vector<ofTexture> & getTexturePlanes() const{};



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
    
    bool                bHavePixelsChanged;
    

};







