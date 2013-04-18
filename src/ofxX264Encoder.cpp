//
//  ofxX264.cpp
//  x264Example
//
//  Created by Jonas Jongejan on 17/04/13.
//
//

#include "ofxX264Encoder.h"


uint8_t test = 0x80;




void create_sample_picture(x264_picture_t* picture)
{
    // create a frame to store in
    x264_picture_alloc(picture, X264_CSP_I420, WIDTH, HEIGHT);
    
    // fake image generation
    // disregard how wrong this is; just writing a quick test
    int strides = WIDTH / 8;
    uint8_t* data = (uint8_t*) malloc(WIDTH * HEIGHT * 3);
    memset(data, test, WIDTH * HEIGHT * 3);
   // test = (test << 1) | (test >> (8 - 1));
    
    // scale the image
}

/*
bool ofxX264Encoder::encodeData(const char *data, int data_length){
    sws_scale(imgctx, (const uint8_t* const*) &data, &strides, 0, HEIGHT,
              picture->img.plane, picture->img.i_stride);

}*/


ofxX264Encoder::ofxX264Encoder(){
    
    imgctx = sws_getContext(WIDTH, HEIGHT, PIX_FMT_MONOWHITE,
                            WIDTH, HEIGHT, PIX_FMT_YUV420P,
                            SWS_FAST_BILINEAR, NULL, NULL, NULL);
    
    x264_param_t param;
    x264_param_default_preset(&param, "veryfast", "zerolatency");
    param.i_threads = 1;
    param.i_width = WIDTH;
    param.i_height = HEIGHT;
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
    
    
    
    
    x264_nal_t* nals;
    int num_nals;
    x264_picture_t pic_out;
    
    
    x264_param_apply_profile(&param, "high");
    encoder = x264_encoder_open(&param);
    
    FILE* pFile;
    pFile = fopen("/Users/jonas/Desktop/file.h264", "w");

    for(int i=0;i<10;i++){
        x264_picture_t* pic = (x264_picture_t*) malloc(sizeof(x264_picture_t));
        create_sample_picture(pic);
        
        
        vector<char*> m_vNALs;
        vector<int> m_vSizes;
        
        int frame_size = x264_encoder_encode(encoder, &nals, &num_nals, pic, &pic_out);
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
    }

}