//
//  ofxX264.cpp
//  x264Example
//
//  Created by Jonas Jongejan on 17/04/13.
//
//

#include "ofxStreamerSender.h"





ofxStreamerSender::ofxStreamerSender(){
    streaming = false;
}



void ofxStreamerSender::setup(int _width, int _height, string destination_ip, int destination_port ,string _preset){
    width = _width,
    height = _height;
    preset = _preset;
    
    frameNum = 0;
    
    av_register_all();
    avformat_network_init();
    
    
    //Create scale context for converting from rgb to yuv
    imgctx = sws_getContext(width, height, AV_PIX_FMT_RGB24,
                            width, height, PIX_FMT_YUV420P,
                            SWS_FAST_BILINEAR, NULL, NULL, NULL);
    
    
    // try to open the UDP stream
    url = "udp://"+destination_ip+":"+ofToString(destination_port);

    int err = avformat_alloc_output_context2(&mFormatContext, NULL, "mpegts", url.c_str());
    if(err < 0){
        cout<<"Error "<<err<<endl;
    }
    
    if (avio_open(&mFormatContext->pb, url.c_str(), AVIO_FLAG_WRITE) < 0)
    {
        ofLog(OF_LOG_FATAL_ERROR, "Couldn't open UDP output stream %s",url.c_str());
        exit(0);
    }

    
    
    // add an H.264 stream
    mCodec = avcodec_find_encoder_by_name("libx264");

    stream = avformat_new_stream(mFormatContext, mCodec);
    if (!stream)
    {
        ofLog(OF_LOG_FATAL_ERROR, "Couldn't allocate H.264 stream");
        exit(0);
    }
    stream->id = mFormatContext->nb_streams-1;
    
    
    //Higher value creates better quality (doh)
    int mBitrate = 800*1000;
    
    //Lower value creates lower delay, and lower quality
    int mGopSize = 60;
    
    // initalize codec
    mCodecContext = stream->codec;
    mCodecContext->codec_id = mCodec->id;
    mCodecContext->bit_rate = mBitrate;
    mCodecContext->width    = width;
    mCodecContext->height   = height;
    mCodecContext->gop_size = mGopSize;
    mCodecContext->pix_fmt  = AV_PIX_FMT_YUV420P;
    mCodecContext->time_base.den = 30;
    mCodecContext->time_base.num = 1;
    mCodecContext->max_b_frames  = 0;
    mCodecContext->thread_count  = 4;
    mCodecContext->thread_type   = FF_THREAD_SLICE;

    if (mFormatContext->oformat->flags & AVFMT_GLOBALHEADER)
    {
        mCodecContext->flags     |= CODEC_FLAG_GLOBAL_HEADER;
    }
    
    
    av_opt_set(mCodecContext->priv_data, "preset", "fast", 0);
    av_opt_set(mCodecContext->priv_data, "tune", "zerolatency", 0);
    
    
    avcodec_open2(mCodecContext, mCodec, NULL);

    // write the header
    int error = avformat_write_header(mFormatContext, nil);
    if(error < 0){
        cout<<"Error avformat_write_header "<<error<<endl;
    }
    
    streaming = true;
    
    frame = avcodec_alloc_frame();
    frame->pts = 0;
    
    int num_bytes = avpicture_get_size(PIX_FMT_YUV420P, width, height);
    frameBuf = (uint8_t*)malloc(num_bytes);

    avpicture_fill((AVPicture*)frame, frameBuf,PIX_FMT_YUV420P, width, height);
}


bool ofxStreamerSender::sendFrame(ofImage image){
    if(image.type != OF_IMAGE_COLOR){
        cout<<"Only implemented OF_IMAGE_COLOR type image in encodeFrame"<<endl;
        return false;
    }
    
    unsigned char * data = image.getPixelsRef().getPixels();
    int length = image.getWidth() * image.getHeight() * 3;
    
    return sendFrame(data, length);
}



bool ofxStreamerSender::sendFrame(unsigned char *data, int data_length){
    AVPacket p;

    if(streaming){
        encodedFrameSize = 0;
        
        
        int stride = width * 3;
        
        //Convert to YUV format
        sws_scale(imgctx, (const uint8_t* const*) &data, &stride, 0, height, frame->data, frame->linesize);
        
        // initalize a packet
        av_init_packet(&p);
        p.data = NULL;
        p.size = 0;
        
        int got_packet;
        
        int err = avcodec_encode_video2(stream->codec, &p, frame, &got_packet);
        if (err < 0) {
            cout<<"Could not encode frame"<<endl;
            return false;
        };
        
        encodedFrameSize = p.size;
        
        
        if(encodedFrameSize == 0){
            ofLog(OF_LOG_WARNING, "No encoded frame to send, make sure to call encodeFrame");
            return false;
        }
        
        
        AVCodecContext *codecContext = stream->codec;
        p.stream_index = stream->index;
        p.duration = 0;
        
        frameNum++;
        
        float timeDiff = ofGetElapsedTimeMillis() - lastSendTime;
        
        frameRate += ((1.0/(timeDiff/1000.0)) - frameRate)*0.8;
        
        bitrate = 8 * encodedFrameSize * frameRate / 1000.0;
        
        
        lastSendTime = ofGetElapsedTimeMillis();
        
        // send it out
        av_write_frame(mFormatContext, &p);

        frame->pts += av_rescale_q(1, stream->codec->time_base, stream->time_base);
        


        return true;
    }
    return false;
}


x264_picture_t* ofxStreamerSender::getPictureRef(){
    return picture_out;
}

