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
    
    
    // get test data
    //FILE* pFile;
    //pFile = fopen("/Users/johan/Desktop/file.h264", "w");
    
    //avcodec_register_all();
    //video_decode_example("test%02d.pgm", "http://59.124.145.72:57282");
    
    // initialize decoder
    
    // decode
    
    // draw
    
    cout<<"start"<<endl;
    
    SwsContext *img_convert_ctx;
    AVFormatContext* context = avformat_alloc_context();
    AVCodecContext* ccontext = avcodec_alloc_context3(NULL);
    int video_stream_index;
    
    av_register_all();
    avformat_network_init();
    //av_log_set_callback(&log_callback);
    
    cout<<"try to open stream"<<endl;
    
    // where to use this ?
    AVDictionary * dict;
    av_dict_set( &dict, "video_size", "320x240",0);
    
    string url = "udp://@:1234";
    string format = "h264"; // mpeg2video, x264, mpegts, h264 ? 
    
    AVInputFormat *fmt = NULL;
    
    int res;
    
    fmt = av_find_input_format(format.c_str());
    if(avformat_open_input(&context, url.c_str(),fmt,NULL) != 0){
        return EXIT_FAILURE;
    }
    
    if(avformat_find_stream_info(context,NULL) < 0){
        return EXIT_FAILURE;
    }
    
    //search video stream
    for(int i =0;i<context->nb_streams;i++){
        if(context->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
            video_stream_index = i;
    }
    
    AVPacket packet;
    av_init_packet(&packet);
    
    //open output file
   // AVOutputFormat* fmto = av_guess_format(NULL,"test2.mp4",NULL);
    AVFormatContext* oc = avformat_alloc_context();
    //oc->oformat = fmto;
    //avio_open2(&oc->pb, "test.mp4", AVIO_FLAG_WRITE,NULL,NULL);
    
    AVStream* stream=NULL;
    int cnt = 0;
    //start reading packets from stream and write them to file
    av_read_play(context);//play stream
    
    AVCodec *codec = NULL;
    codec = avcodec_find_decoder(CODEC_ID_H264);
    if (!codec) exit(1);
    
    avcodec_get_context_defaults3(ccontext, codec);
    avcodec_copy_context(ccontext,context->streams[video_stream_index]->codec);
    ofstream myfile;
    
    if (avcodec_open2(ccontext, codec, NULL) < 0) exit(1);
    
    img_convert_ctx = sws_getContext(ccontext->width, ccontext->height, ccontext->pix_fmt, ccontext->width, ccontext->height,
                                     PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
    
    int size = avpicture_get_size(PIX_FMT_YUV420P, ccontext->width, ccontext->height);
    uint8_t* picture_buf = (uint8_t*)(av_malloc(size));
    AVFrame* pic = avcodec_alloc_frame();
    AVFrame* picrgb = avcodec_alloc_frame();
    int size2 = avpicture_get_size(PIX_FMT_RGB24, ccontext->width, ccontext->height);
    uint8_t* picture_buf2 = (uint8_t*)(av_malloc(size2));
    avpicture_fill((AVPicture *) pic, picture_buf, PIX_FMT_YUV420P, ccontext->width, ccontext->height);
    avpicture_fill((AVPicture *) picrgb, picture_buf2, PIX_FMT_RGB24, ccontext->width, ccontext->height);
    
    cout<<"will read frames"<<endl;
    while(av_read_frame(context,&packet)>=0 && cnt <1000)
    {
        
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
            if(cnt > 100)//cnt < 0)
            {
                sws_scale(img_convert_ctx, pic->data, pic->linesize, 0, ccontext->height, picrgb->data, picrgb->linesize);
                stringstream name;
                name << "test" << cnt << ".ppm";
                
                myfile.open(name.str().c_str());
                myfile << "P3 " << ccontext->width << " " << ccontext->height << " 255\n";
                
                for(int y = 0; y < ccontext->height; y++)
                {
                    for(int x = 0; x < ccontext->width * 3; x++)
                        myfile << (int)(picrgb->data[0] + y * picrgb->linesize[0])[x] << " ";
                }
                
                myfile.close();
                
            }
            cnt++;
        }
        av_free_packet(&packet);
        av_init_packet(&packet);
    }
    av_free(pic);
    av_free(picrgb);
    av_free(picture_buf);
    av_free(picture_buf2);
    
    av_read_pause(context);
    avio_close(oc->pb);
    avformat_free_context(oc);
    
    
    cout<<"exit"<<endl;
    return (EXIT_SUCCESS);
    
    
    
}

/*void ofxX264Decoder::pgm_save(unsigned char *buf, int wrap, int xsize, int ysize,
                     char *filename)
{
    FILE *f;
    int i;
    f=fopen(filename,"w");
    fprintf(f,"P5\n%d %d\n%d\n",xsize,ysize,255);
    for(i=0;i<ysize;i++)
        fwrite(buf + i * wrap,1,xsize,f);
    fclose(f);
}*/



void ofxX264Decoder::log_callback(void *ptr, int level, const char *fmt, va_list vargs)
{
    static char message[8192];
    const char *module = NULL;
    
    if (ptr)
    {
        AVClass *avc = *(AVClass**) ptr;
        module = avc->item_name(ptr);
    }
    vsnprintf(message, sizeof(message), fmt, vargs);
    
    cout << "LOG: " << message << endl;
}




/*int ofxX264Decoder::decode_write_frame(const char *outfilename, AVCodecContext *avctx,
                              AVFrame *frame, int *frame_count, AVPacket *pkt, int last)
{
    int len, got_frame;
    char buf[1024];
    len = avcodec_decode_video2(avctx, frame, &got_frame, pkt);
    if (len < 0) {
        fprintf(stderr, "Error while decoding frame %d\n", *frame_count);
        return len;
    }
    if (got_frame) {
        printf("Saving %sframe %3d\n", last ? "last " : "", *frame_count);
        fflush(stdout);
        // the picture is allocated by the decoder, no need to free it 
        snprintf(buf, sizeof(buf), outfilename, *frame_count);
        
        pgm_save(frame->data[0], frame->linesize[0],
                 avctx->width, avctx->height, buf);
        
        (*frame_count)++;
    }
    if (pkt->data) {
        pkt->size -= len;
        pkt->data += len;
    }
    return 0;
}*/


/*void ofxX264Decoder::video_decode_example(const char *outfilename, const char *filename)
{
    AVCodec *codec;
    AVCodecContext *c= NULL;
    int frame_count;
    FILE *f;
    AVFrame *frame;
    uint8_t inbuf[INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
    AVPacket avpkt;
    av_init_packet(&avpkt);
    // set end of buffer to 0 (this ensures that no overreading happens for damaged mpeg streams)
    //memset(inbuf + INBUF_SIZE, 0, FF_INPUT_BUFFER_PADDING_SIZE);
    //printf("Decode video file %s to %s\n", filename, outfilename);
    // find the mpeg1 video decoder 
    //codec = avcodec_find_decoder(AV_CODEC_ID_MPEG1VIDEO);

    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }
    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }
    if(codec->capabilities&CODEC_CAP_TRUNCATED)
        c->flags|= CODEC_FLAG_TRUNCATED;*/ /* we do not send complete frames */
    // For some codecs, such as msmpeg4 and mpeg4, width and height
    // MUST be initialized there because this information is not
    // available in the bitstream.
    /* open it *//*
    if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }
    f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }
    frame = avcodec_alloc_frame();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    frame_count = 0;
    for(;;) {
        avpkt.size = fread(inbuf, 1, INBUF_SIZE, f);
        if (avpkt.size == 0)
            break;*/
        /* NOTE1: some codecs are stream based (mpegvideo, mpegaudio)
         and this is the only method to use them because you cannot
         know the compressed data size before analysing it.
         BUT some other codecs (msmpeg4, mpeg4) are inherently frame
         based, so you must call them with all the data for one
         frame exactly. You must also initialize 'width' and
         'height' before initializing them. */
        /* NOTE2: some codecs allow the raw parameters (frame size,
         sample rate) to be changed at any frame. We handle this, so
         you should also take care of it */
        /* here, we use a stream based decoder (mpeg1video), so we
         feed decoder and see if it could decode a frame */
        /*avpkt.data = inbuf;
        while (avpkt.size > 0)
            if (decode_write_frame(outfilename, c, frame, &frame_count, &avpkt, 0) < 0)
                exit(1);
    }*/
    /* some codecs, such as MPEG, transmit the I and P frame with a
     latency of one frame. You must do the following to have a
     chance to get the last frame of the video */
    /*avpkt.data = NULL;
    avpkt.size = 0;
    decode_write_frame(outfilename, c, frame, &frame_count, &avpkt, 1);
    fclose(f);
    avcodec_close(c);
    av_free(c);
    avcodec_free_frame(&frame);
    printf("\n");
}*/

