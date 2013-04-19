//
//  ofxX264.cpp
//  x264Example
//
//  Created by Jonas Jongejan on 17/04/13.
//
//

#include "ofxX264Encoder.h"





ofxX264Encoder::ofxX264Encoder(){
    
}

	
bool ofxX264Encoder::encodeData(unsigned char *data, int data_length, int stride, int height ){
    

    sws_scale(imgctx, (const uint8_t* const*) &data, &stride, 0, height, picture_in.img.plane, picture_in.img.i_stride);
    
    
    
    x264_nal_t* nals;
    int num_nals;

    vector<char*> m_vNALs;
    vector<int> m_vSizes;
    
    int frame_size = x264_encoder_encode(encoder, &nals, &num_nals, &picture_in, picture_out);
    if (frame_size > 0)
    {
        printf("frame size: %i\n", frame_size);
        
        encodedFrameData = (unsigned char*)nals[0].p_payload ;
        encodedFrameSize = frame_size ;
    }
    
    
    
}


x264_picture_t* ofxX264Encoder::getPicture(){
    return picture_out;
}

void ofxX264Encoder::setup(int _width, int _height){
    width = _width,
    height = _height;
    
    //Create scale context
    imgctx = sws_getContext(width, height, AV_PIX_FMT_RGB24,
                            width, height, PIX_FMT_YUV420P,
                            SWS_FAST_BILINEAR, NULL, NULL, NULL);
    

    //Allocate the imput picture 
    x264_picture_alloc(&picture_in, X264_CSP_I420, width, height);

    
    
    x264_param_t param;
    x264_param_default_preset(&param, "veryfast", "zerolatency");
    param.i_threads = 1;
    param.i_width = width;
    param.i_height = height;
    param.i_fps_num = FPS;
    param.i_fps_den = 1;
    // Intra refres:
    param.i_keyint_max = FPS;
    param.b_intra_refresh = 1;
    //Rate control:
    param.rc.i_rc_method = X264_RC_CRF;
    param.rc.f_rf_constant = 25;
    param.rc.f_rf_constant_max = 35;
    //For streaming:
    param.b_repeat_headers = 1;
    param.b_annexb = 1;
    x264_param_apply_profile(&param, "baseline");
    

    FILE* pFile;
    pFile = fopen("/Users/jonas/Desktop/file.h264", "w");

    
    
    
    
    x264_param_apply_profile(&param, "high");
    encoder = x264_encoder_open(&param);
    
    
    picture_out = (x264_picture_t*) malloc(sizeof(x264_picture_t));
    
    
    
    av_register_all();
    
    
    // initalize the AV context
    avctx = avformat_alloc_context();
    if (!avctx)
    {
        printf("Couldn't initalize AVFormat output context");
        exit(0);
    }
    
    // get the output format
    AVOutputFormat * fmt = av_guess_format("rtp", NULL, NULL);
    if (!fmt)
    {
        printf("Unsuitable output format");
        exit(0);
    }
    avctx->oformat = fmt;

//  struct AVStream* stream = av_new_stream(avctx, 1);
    
    // initalize codec
    /*AVCodecContext* c = stream->codec;
    c->codec_id = CODEC_ID_H264;
    c->codec_type = AVMEDIA_TYPE_VIDEO;
    c->flags = CODEC_FLAG_GLOBAL_HEADER;
    c->width = width;
    c->height = height;
    c->time_base.den = FPS;
    c->time_base.num = 1;
    c->gop_size = FPS;
    c->bit_rate = BITRATE;
//    avctx->flags = AVFMT_FLAG_RTP_HINT;
    
    // write the header
//    av_write_header(avctx);

/*

    for(int i=0;i<10;i++){
        
        
        vector<char*> m_vNALs;
        vector<int> m_vSizes;
        
        int frame_size = x264_encoder_encode(encoder, &nals, &num_nals, picture_in, picture_out);
        if (frame_size > 0)
        {
            printf("%i\n", frame_size);
            
            m_vNALs.push_back( (char*)nals[0].p_payload );
            m_vSizes.push_back( frame_size );
        }
        
        for( int nIndex = 0; nIndex < m_vNALs.size(); nIndex++ )
        {
            fwrite( m_vNALs[ nIndex ], m_vSizes[ nIndex ], 1, pFile );
        }
    }*/

}