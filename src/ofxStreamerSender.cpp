//
//  ofxX264.cpp
//  x264Example
//
//  Created by Jonas Jongejan on 17/04/13.
//
//

#include "ofxStreamerSender.h"





ofxStreamerSender::ofxStreamerSender(){
    
}



void ofxStreamerSender::setup(int _width, int _height, string destination_ip, int destination_port ,const char * preset){
    width = _width,
    height = _height;
    
    av_register_all();
    avformat_network_init();
    
    
    //Create scale context for converting from rgb to yuv
    imgctx = sws_getContext(width, height, AV_PIX_FMT_RGB24,
                            width, height, PIX_FMT_YUV420P,
                            SWS_FAST_BILINEAR, NULL, NULL, NULL);
    
    
    //Allocate the input picture
    x264_picture_alloc(&picture_in, X264_CSP_I420, width, height);
    //Allocate the output picture
    picture_out = (x264_picture_t*) malloc(sizeof(x264_picture_t));

    
    
    //Initialize the h264 encoder
    x264_param_t param;
    x264_param_default_preset(&param, preset, "zerolatency");
    param.i_threads = 5;
    param.i_width = width;
    param.i_height = height;
    param.i_fps_num = 30;
    param.i_fps_den = 1;
    // Intra refres:
    param.i_keyint_max = 30;
    param.b_intra_refresh = 1;
    //Rate control:
    param.rc.i_rc_method = X264_RC_CRF;
    param.rc.f_rf_constant = 25;
    param.rc.f_rf_constant_max = 35;
    //For streaming:
    param.b_repeat_headers = 1;
    param.b_annexb = 1;
    x264_param_apply_profile(&param, "baseline");
    
    encoder = x264_encoder_open(&param);
    
    
    
    
    // initalize the AV context
    avctx = avformat_alloc_context();
    if (!avctx)
    {
        ofLog(OF_LOG_FATAL_ERROR, "Couldn't initalize AVFormat output context");
        exit(0);
    }
    
    // get the output format container
    AVOutputFormat * fmt = av_guess_format("mpegts", NULL, NULL);
    if (!fmt)
    {
        ofLog(OF_LOG_FATAL_ERROR, "Unsuitable output format");
        exit(0);
    }
    avctx->oformat = fmt;
    
    
    
    // try to open the UDP stream
    char filename[100];
    snprintf(filename, sizeof(filename), "udp://%s:%d", destination_ip.c_str(), destination_port);
    if (avio_open(&avctx->pb, filename, AVIO_FLAG_WRITE) < 0)
    {
        ofLog(OF_LOG_FATAL_ERROR, "Couldn't open UDP output stream %s",filename);
        exit(0);
    }
    
    // add an H.264 stream
    stream = avformat_new_stream(avctx, NULL);
    if (!stream)
    {
        ofLog(OF_LOG_FATAL_ERROR, "Couldn't allocate H.264 stream");
        exit(0);
    }
    
    
    
    // initalize codec
    AVCodecContext* c = stream->codec;
    c->codec_id = CODEC_ID_H264;
    c->codec_type = AVMEDIA_TYPE_VIDEO;
    c->bit_rate = 400000;
    c->width = width;
    c->height = height;
    c->time_base.den = 30;
    c->time_base.num = 1;
    
    // write the header
    avformat_write_header(avctx, nil);
}


bool ofxStreamerSender::encodeFrame(ofImage image){
    if(image.type != OF_IMAGE_COLOR){
        cout<<"Only implemented OF_IMAGE_COLOR type image in encodeFrame"<<endl;
        return false;
    }
    
    unsigned char * data = image.getPixelsRef().getPixels();
    int length = image.getWidth() * image.getHeight() * 3;
    
    return encodeFrame(data, length);
}
	
bool ofxStreamerSender::encodeFrame(unsigned char *data, int data_length){
    encodedFrameSize = 0;
    

    int stride = width * 3;
    
    //Convert to YUV format
    sws_scale(imgctx, (const uint8_t* const*) &data, &stride, 0, height, picture_in.img.plane, picture_in.img.i_stride);
    
    
    //Encode h264 frame
    x264_nal_t* nals;
    int num_nals;

    int frame_size = x264_encoder_encode(encoder, &nals, &num_nals, &picture_in, picture_out);
    if (frame_size > 0)
    {
        encodedFrameData = (unsigned char*)nals[0].p_payload ;
        encodedFrameSize = frame_size ;
        return true;
    }

    return false;
}


bool ofxStreamerSender::sendFrame(){
    if(encodedFrameSize == 0){
        ofLog(OF_LOG_WARNING, "No encoded frame to send, make sure to call encodeFrame");
        return false;
    }
       

    AVCodecContext *codecContext = stream->codec;
    
    // initalize a packet
    AVPacket p;
    av_init_packet(&p);
    p.data = encodedFrameData;
    p.size = encodedFrameSize;
    p.stream_index = codecContext->frame_number;
    
    
    //   p.pts = int64_t(0x8000000000000000);
    //  p.dts = int64_t(0x8000000000000000);
    
    /*    if(codecContext->coded_frame->key_frame)
     p.flags |= AV_PKT_FLAG_KEY;
     */
    
    
    // send it out
    return av_write_frame(avctx, &p);
}


x264_picture_t* ofxStreamerSender::getPictureRef(){
    return picture_out;
}

