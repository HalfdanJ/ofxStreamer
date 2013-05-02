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
}

bool ofxStreamerReceiver::setup(int _port, string _host) {
    
    port = _port; host = _host;
    url = host + ":" + ofToString(port);
    ofLog(OF_LOG_NOTICE, "Opening stream at " + url);
    
    
    startThread(true,false);


    
    return connected = true;
}


void ofxStreamerReceiver::threadedFunction(){
  
    context = avformat_alloc_context();
    ccontext = avcodec_alloc_context3(NULL);
    
    av_register_all();
    avformat_network_init();
    
    if(avformat_open_input(&context, url.c_str(),NULL,NULL) != 0){
        ofLog(OF_LOG_ERROR, "Could not open input.");
        return connected = false;
    }
    
    if(avformat_find_stream_info(context,NULL) < 0){
        ofLog(OF_LOG_ERROR, "Stream information not found.");
        return connected = false;
    }
    
    //search video stream
    for(int i =0;i<context->nb_streams;i++){
        if(context->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
            video_stream_index = i;
    }
    
    av_init_packet(&packet);
    
    oc = avformat_alloc_context();
    
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
    
    
    
    while(1){
        lock();
        cout<<"Thread Lock"<<endl;

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
                
                // decode
                int frameFinished = 0;
                int result = avcodec_decode_video2(ccontext, pic, &frameFinished, &packet);
                
                if(result > 0 && frameFinished == 1) {
                    
                    
                    sws_scale(img_convert_ctx, pic->data, pic->linesize, 0, ccontext->height, picrgb->data, picrgb->linesize);
                    
                    
                    cout<<"Memcpu"<<endl;
                    memcpy(pixelData,  picrgb->data[0], width*height*3);
                    newFrame = true;
                    
                } else {
                    cout<<"No frame decoded result is:"<<ofToString(result)<<endl;
                }
            }
            
        } else {
            cout<<"EOF or error statuscode is: "<<ofToString(readStatus)<<endl;
        }
        
        cout<<"Thread unlock"<<endl;

        unlock();


    }
}

void ofxStreamerReceiver::update() {
    if(newFrame){
        
        if(!allocated){
            lastFrame = new ofImage();
            lastFrame->allocate(width, height, OF_IMAGE_COLOR);
            allocated = true;
            
        }
        
        
        lock(); {
            if(newFrame){
                
                
                // save frame to image
                lastFrame->setFromPixels(pixelData, width, height, OF_IMAGE_COLOR);
                cout<<int(pixelData[0])<<endl;
                float timeDiff = ofGetElapsedTimeMillis() - lastReceiveTime;
                frameRate += ((1.0/(timeDiff/1000.0)) - frameRate)*0.8;
                bitrate = 8 * encodedFrameSize * frameRate / 1000.0;
                lastReceiveTime = ofGetElapsedTimeMillis();
                frameNum++;
                
                bHavePixelsChanged = true;
                
            }
        } unlock();
        
        newFrame=false;
        
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

ofTexture ofxStreamerReceiver::getTextureReference() {
    return lastFrame->getTextureReference();
}

bool ofxStreamerReceiver::isFrameNew() {
    return (bHavePixelsChanged && allocated);
}

void ofxStreamerReceiver::close() {
    delete lastFrame;
    av_free(pic);
    av_free(picrgb);
    av_free(picture_buf);
    av_free(picture_buf2);
    av_read_pause(context);
    avformat_free_context(oc);
    allocated = false;
}

float ofxStreamerReceiver::getWidth() {
    return width;
}

float ofxStreamerReceiver::getHeight() {
    return height;
}
