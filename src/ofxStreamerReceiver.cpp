//
//  ofxStreamerReceiver.c
//  x264Example
//
//  Created by Johan Bichel Lindegaard on 4/18/13.
//
//


#include "ofxStreamerReceiver.h"

using namespace std;

ofxStreamerReceiver::ofxStreamerReceiver(){
    bHavePixelsChanged = false;
    allocated = false;
    connected = false;
    newFrame = false;
}

bool ofxStreamerReceiver::setup(int _port, string _host) {
    
    port = _port; host = _host;
    url = host + ":" + ofToString(port);
    ofLog(OF_LOG_NOTICE, "Opening stream at " + url);
    
    startThread(false,false);

    lastFrame = new ofImage();
    lastFrame->allocate(1, 1, OF_IMAGE_COLOR);

    return connected = true;
    
    
}


void ofxStreamerReceiver::threadedFunction(){


    av_register_all();
    avformat_network_init();
    mFormatContext = avformat_alloc_context();

    int err = avformat_open_input(&mFormatContext, url.c_str(), NULL, NULL);
    if(err<0){
        cout<<"avformat_open_input error "<<err<<endl;
    }
    
   // mFormatContext->max_analyze_duration = 5000000*5;
    

    // These flags work! But this negates the use of av_read_frame() as it now
    // does not guarantee to return one frame.
    // mFormatContext->flags |= AVFMT_FLAG_NOPARSE | AVFMT_FLAG_NOFILLIN;
    
    // These flags seem to have no effect.
    // mFormatContext->flags |= AVFMT_FLAG_NOBUFFER;
    // mFormatContext->flags |= AVFMT_FLAG_FLUSH_PACKETS;
    // mFormatContext->avio_flags |= AVIO_FLAG_DIRECT;
    
    
    
    if(avformat_find_stream_info(mFormatContext,NULL) < 0){
        ofLog(OF_LOG_ERROR, "Stream information not found.");
        connected = false;
        return;
    }
    av_dump_format(mFormatContext, 0, url.c_str(), 0);

    
    // Video stream.
    mVideoStreamIdx = av_find_best_stream(mFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);

    AVStream* st = mFormatContext->streams[mVideoStreamIdx];
    // This flag works! But this negates the use of av_read_frame() as it now
    // does not guarantee to return one frame.
    // st->need_parsing = AVSTREAM_PARSE_NONE;
    
    
    // find decoder for the stream
    AVCodecContext* dec_ctx = st->codec;
    AVCodec* dec = avcodec_find_decoder(dec_ctx->codec_id);
    if (dec->capabilities & CODEC_CAP_TRUNCATED)
        dec_ctx->flags |= CODEC_FLAG_TRUNCATED;
    dec_ctx->thread_type  = FF_THREAD_SLICE;
    dec_ctx->thread_count = 4;
    
   
    avcodec_open2(dec_ctx, dec, NULL);
    mVideoStream        = mFormatContext->streams[mVideoStreamIdx];
    mVideoDecodeContext = mVideoStream->codec;
    
    width = mVideoDecodeContext->width;
    height = mVideoDecodeContext->height;
    
    img_convert_ctx = sws_getContext(width, height, mVideoDecodeContext->pix_fmt, width, height,
                                     PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
    


    
    picrgb = avcodec_alloc_frame();
    int size2 = avpicture_get_size(PIX_FMT_RGB24, width, height);
    picture_buf2 = (uint8_t*)(av_malloc(size2));
    avpicture_fill((AVPicture *) picrgb, picture_buf2, PIX_FMT_RGB24, width, height);

    
    
    while(isThreadRunning()){
        AVPacket pkt;
        av_init_packet(&pkt);
        pkt.data = NULL;
        pkt.size = 0;
        
        // wait for data.
        if (av_read_frame(mFormatContext, &pkt) < 0){
            cout<<"No frame"<<endl;
        } else {
            // decode video frame
            int got_frame = 0;
            mFrame = avcodec_alloc_frame();
            int decodeResult = avcodec_decode_video2(mVideoDecodeContext, mFrame, &got_frame, &pkt);
            if(decodeResult > 0 && got_frame == 1) {
                mutex.lock();
                
                sws_scale(img_convert_ctx, mFrame->data, mFrame->linesize, 0, height, picrgb->data, picrgb->linesize);
                encodedFrameSize = pkt.size;
                
                newFrame = true;
                
                mutex.unlock();
                
            } else {
                cout<<"No frame decoded result is:"<<ofToString(decodeResult)<<endl;
            }

            
            av_free_packet(&pkt);
            avcodec_free_frame(&mFrame);
        }
        
    }
}

void ofxStreamerReceiver::update() {
    if(mutex.tryLock()){
        if(newFrame){
            if(!allocated){
                lastFrame = new ofImage();
                lastFrame->allocate(width, height, OF_IMAGE_COLOR);
                allocated = true;
            }
            
            
            // save frame to image
            lastFrame->setFromPixels(picrgb->data[0], width, height, OF_IMAGE_COLOR);
            
            float timeDiff = ofGetElapsedTimeMillis() - lastReceiveTime;
            frameRate += ((1.0/(timeDiff/1000.0)) - frameRate)*0.8;
            bitrate = 8 * encodedFrameSize * frameRate / 1000.0;
            lastReceiveTime = ofGetElapsedTimeMillis();
            frameNum++;
            
            bHavePixelsChanged = true;
            
            newFrame=false;
            
            
        }
        mutex.unlock();
    } else {
      //  cout<<"Could not lock"<<endl;
    }
}

void ofxStreamerReceiver::draw(const ofPoint &p) {
    draw(p.x, p.y);
}

void ofxStreamerReceiver::draw(float x, float y) {
    draw(x, y, getWidth(), getHeight());
}

void ofxStreamerReceiver::draw(const ofRectangle &r) {
    draw(r.x, r.y, r.width, r.height);
}

void ofxStreamerReceiver::draw(float x, float y, float w, float h) {
    bHavePixelsChanged = false;
    if(allocated){
        lastFrame->draw(x,y,w,h);
    } else {
    }
}

unsigned char * ofxStreamerReceiver::getPixels() {
    if(allocated){
        return lastFrame->getPixels();
    }
    return nil;
}

ofPixelsRef ofxStreamerReceiver::getPixelsRef() {
    return lastFrame->getPixelsRef();
}

ofTexture & ofxStreamerReceiver::getTextureReference() {

    return lastFrame->getTextureReference();
}

bool ofxStreamerReceiver::isFrameNew() {
    return (bHavePixelsChanged && allocated);
}

bool ofxStreamerReceiver::isConnected() {
    return connected;
}

void ofxStreamerReceiver::close() {
    delete lastFrame;
    allocated = false;
}

float ofxStreamerReceiver::getWidth() {
    return width;
}

float ofxStreamerReceiver::getHeight() {
    return height;
}
