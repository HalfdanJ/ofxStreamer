//
//  ofxX264Decoder.c
//  x264Example
//
//  Created by Johan Bichel Lindegaard on 4/18/13.
//
//


// An HBO RTSP stream
//"http://59.124.145.72:17282";

// Original from example
//"rtsp://134.169.178.187:8554/h264.3gp"



#include "ofxX264Decoder.h"

using namespace std;



ofxX264Decoder::ofxX264Decoder(){
    
    bHavePixelsChanged = false;
    allocated = false;
    connected = false;
        
}

bool ofxX264Decoder::setup(int _port, string _host) {
    port = _port; host = _host;
    string url = host + ":" + ofToString(port);
    ofLog(OF_LOG_NOTICE, "Opening stream at " + url);
    
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
    
    stream=NULL;
    cnt = 0;
    
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
    
    img_convert_ctx = sws_getContext(width, height, ccontext->pix_fmt, width, height,
                                     PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
    
    int size = avpicture_get_size(PIX_FMT_YUV420P, width, height);
    uint8_t* picture_buf = (uint8_t*)(av_malloc(size));
    pic = avcodec_alloc_frame();
    picrgb = avcodec_alloc_frame();
    int size2 = avpicture_get_size(PIX_FMT_RGB24, width, height);
    uint8_t* picture_buf2 = (uint8_t*)(av_malloc(size2));
    avpicture_fill((AVPicture *) pic, picture_buf, PIX_FMT_YUV420P, width, height);
    avpicture_fill((AVPicture *) picrgb, picture_buf2, PIX_FMT_RGB24, width, height);
    
    return connected = true;
}

void ofxX264Decoder::update() {
    
    if (av_read_frame(context,&packet)) {

        cout << "1 Frame: " << cnt << endl;
        if(packet.stream_index == video_stream_index){//packet is video
            cout << "2 Is Video" << endl;
            if(stream == NULL)
            {//create stream in file
                cout << "3 create stream" << endl;
                stream = avformat_new_stream(oc,context->streams[video_stream_index]->codec->codec);
                avcodec_copy_context(stream->codec,context->streams[video_stream_index]->codec);
                stream->sample_aspect_ratio = context->streams[video_stream_index]->codec->sample_aspect_ratio;
            }
            int check = 0;
            packet.stream_index = stream->id;
            cout << "4 decoding" << endl;
            int result = avcodec_decode_video2(ccontext, pic, &check, &packet);
            cout << "Bytes decoded " << result << " check " << check << endl;

            sws_scale(img_convert_ctx, pic->data, pic->linesize, 0, ccontext->height, picrgb->data, picrgb->linesize);
            
            for(int y = 0; y < ccontext->height; y++)
            {
                for(int x = 0; x < width * 3; x++) {
                    //myfile << (int)(picrgb->data[0] + y * picrgb->linesize[0])[x] << " ";
                }
            }
            
            cnt++;
        }
        av_free_packet(&packet);
        av_init_packet(&packet);
    }
    
}

void ofxX264Decoder::draw(const ofPoint &p) {
    draw(p.x, p.y);
}

void ofxX264Decoder::draw(float x, float y) {
    draw(x, y, getWidth(), getHeight());
}

void ofxX264Decoder::draw(const ofRectangle &r) {
    draw(r.x, r.y, r.width, r.height);
}

void ofxX264Decoder::draw(float x, float y, float w, float h) {
    
    
    
}

bool ofxX264Decoder::isFrameNew() {
    return (bHavePixelsChanged);
}

void ofxX264Decoder::close() {
    /*av_free(pic);
    av_free(picrgb);
    av_free(picture_buf);
    av_free(picture_buf2);
    
    av_read_pause(context);
    avio_close(oc->pb);
    avformat_free_context(oc);*/
}

float ofxX264Decoder::getWidth() {
    return width;
}

float ofxX264Decoder::getHeight() {
    return height;
}
