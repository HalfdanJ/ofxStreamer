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
    dec_ctx->thread_count = 1;
    
   
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
                
                newFrame = true;
                
                mutex.unlock();
                
            } else {
                cout<<"No frame decoded result is:"<<ofToString(decodeResult)<<endl;
            }

            
            av_free_packet(&pkt);
            avcodec_free_frame(&mFrame);
        }
        
    }
    
    
    
    
  /*  context = avformat_alloc_context();
   
    ccontext = avcodec_alloc_context3(NULL);
   
    av_register_all();
    avformat_network_init();
    
    if(avformat_open_input(&context, url.c_str(),NULL,NULL) != 0){
        ofLog(OF_LOG_ERROR, "Could not open input.");
        connected = false;
        return;
    }
    
    if(avformat_find_stream_info(context,NULL) < 0){
        ofLog(OF_LOG_ERROR, "Stream information not found.");
         connected = false;
        return;
    }
    
    //search video stream
    for(int i =0;i<context->nb_streams;i++){
        if(context->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
            video_stream_index = i;
    }
    
    av_init_packet(&packet);
    
    oc = avformat_alloc_context();
    oc->ctx_flags = AVFMT_FLAG_NOBUFFER;
    stream = NULL;
    frameNum = 0;
    
    //start reading packets from stream and write them to file
    av_read_play(context);
    
    AVCodec *codec = NULL;
    codec = avcodec_find_decoder(CODEC_ID_H264);
    if (!codec) {
        ofLog(OF_LOG_FATAL_ERROR, "Codec not found.");
        exit(1);
    }
    
    avcodec_get_context_defaults3(ccontext, codec);
    avcodec_copy_context(ccontext,context->streams[video_stream_index]->codec);
    
    if (avcodec_open2(ccontext, codec, NULL) < 0) {
        ofLog(OF_LOG_FATAL_ERROR, "Could not open codec.");
        exit(1);
    }
    
    width = ccontext->width;
    height = ccontext->height;
    
    
    pixelData = (unsigned char*)malloc(sizeof(unsigned char)*width*height*3);
    
    for(int i=0;i<width*height*3;i++){
        pixelData[i] = 0;
    }
    
    img_convert_ctx = sws_getContext(width, height, ccontext->pix_fmt, width, height,
                                     PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
    
    int size = avpicture_get_size(PIX_FMT_YUV420P, width, height);
    picture_buf = (uint8_t*)(av_malloc(size));
    pic = avcodec_alloc_frame();
    picrgb = avcodec_alloc_frame();
    int size2 = avpicture_get_size(PIX_FMT_RGB24, width, height);
    picture_buf2 = (uint8_t*)(av_malloc(size2));
    avpicture_fill((AVPicture *
                    ) pic, picture_buf, PIX_FMT_YUV420P, width, height);
    avpicture_fill((AVPicture *) picrgb, picture_buf2, PIX_FMT_RGB24, width, height);
    
    
    
    
    
    while(isThreadRunning()){
        
        if(&packet){
            av_free_packet(&packet);
            av_init_packet(&packet);
        }
        
        int readStatus = av_read_frame(context,&packet);
        
        if (readStatus == 0) {
                        
            if(packet.stream_index == video_stream_index){ //packet is video
                
                if(stream == NULL) {
                    // create stream
                    stream = avformat_new_stream(oc,context->streams[video_stream_index]->codec->codec);
                    avcodec_copy_context(stream->codec,context->streams[video_stream_index]->codec);
                    stream->sample_aspect_ratio = context->streams[video_stream_index]->codec->sample_aspect_ratio;
                }
                packet.stream_index = stream->id;
                encodedFrameSize = packet.size;
                
                packet.flags = AV_PKT_FLAG_KEY;
                
                // decode
                int frameFinished = 0;
                int result = avcodec_decode_video2(ccontext, pic, &frameFinished, &packet);
                
                if(result > 0 && frameFinished == 1) {
                    mutex.lock();

                    sws_scale(img_convert_ctx, pic->data, pic->linesize, 0, ccontext->height, picrgb->data, picrgb->linesize);
                    
                    newFrame = true;
                    
                    mutex.unlock();

                } else {
                    cout<<"No frame decoded result is:"<<ofToString(result)<<endl;
                }
                
            }
        } else {
            cout<<"EOF or error statuscode is: "<<ofToString(readStatus)<<endl;
        }
        
    }
    */
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
