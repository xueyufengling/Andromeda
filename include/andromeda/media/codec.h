#ifndef ANDROMEDA_MEDIA_CODEC
#define ANDROMEDA_MEDIA_CODEC

#define set_property(obj,key,val) obj->properties->key=val

//cc=codec_context*,frm=avframe*,st=avstream*,pkt=avpacket*,frmc=frame pts counter(int*)
#define startEncode(cc,frm,st,pkt,frmc)\
        {\
            ((AVFrame*)(frm))->pts=*((int*)(frmc));\
            if(avcodec_send_frame(((AVCodecContext*)(cc)),((AVFrame*)(frm)))<0)\
            {\
                std::cout<<"Send the frame failed."<<std::endl;\
            }\
            int ret_enc=0;\
            while(ret_enc>=0)\
            {\
                ret_enc=avcodec_receive_packet(((AVCodecContext*)(cc)),((AVPacket*)(pkt)));\
                if(ret_enc==AVERROR(EAGAIN)||ret_enc==AVERROR_EOF)\
                    break;\
                else if(ret_enc<0)\
                {\
                    std::cout<<"Receive the packet failed."<<std::endl;\
                    break;\
                }\
                ((AVPacket*)(pkt))->pts=((AVFrame*)(frm))->pts;\
                if(((AVStream*)(st))){\
                    av_packet_rescale_ts(((AVPacket*)(pkt)),((AVCodecContext*)(cc))->time_base,((AVStream*)(st))->time_base);\
                    ((AVPacket*)(pkt))->stream_index=((AVStream*)(st))->index;\
                }\
                {

#define endEncode(cc,frm,st,pkt,frmc)\
                }\
                av_packet_unref(((AVPacket*)(pkt)));\
            }\
            ++*((int*)(frmc));\
        }

#define startEncodeFlush(cc,st,pkt)\
        {\
            avcodec_send_frame(((AVCodecContext*)(cc)),nullptr);\
            int ret_enc_flu=0;\
            while(ret_enc_flu>=0)\
            {\
                ret_enc_flu=avcodec_receive_packet(((AVCodecContext*)(cc)),((AVPacket*)(pkt)));std::cout<<"cl"<<std::endl;\
                if(ret_enc_flu==AVERROR(EAGAIN)||ret_enc_flu==AVERROR_EOF)\
                    break;\
                else if(ret_enc_flu<0)\
                {\
                    std::cout<<"Receive the packet failed."<<std::endl;\
                    break;\
                }\
                if(((AVStream*)(st)))\
                {\
                    av_packet_rescale_ts(((AVPacket*)(pkt)),((AVCodecContext*)(cc))->time_base,((AVStream*)(st))->time_base);\
                    ((AVPacket*)(pkt))->stream_index=((AVStream*)(st))->index;\
                }\
                {

#define endEncodeFlush(cc,st,pkt)\
                }\
                av_packet_unref(((AVPacket*)(pkt)));\
            }\
        }

#define startDecode(cc,pkt,frm)\
        {\
            if(avcodec_send_packet(cc,pkt)<0)\
            {\
                std::cout<<"Send the packet failed."<<std::endl;\
            }\
            int ret_dec=0;\
            while(ret_dec>=0)\
            {\
                ret_dec=avcodec_receive_frame(cc,frm);\
                if(ret_dec==AVERROR(EAGAIN)||ret_dec==AVERROR_EOF)\
                    break;\
                else if(ret_dec<0)\
                {\
                    std::cout<<"Receive the frame failed."<<std::endl;\
                    break;\
                }\
                {

#define endDecode(cc,pkt,frm)\
                }\
                av_packet_unref(((AVPacket*)(pkt)));\
            }\
        }

#define startDecodeFlush(cc,frm)\
        {\
            avcodec_send_packet(cc,nullptr);\
            int ret_dec_flu=0;\
            while(ret_dec_flu>=0)\
            {\
                ret_dec_flu=avcodec_receive_frame(cc,frm);\
                if(ret_dec_flu==AVERROR(EAGAIN)||ret_dec_flu==AVERROR_EOF)\
                    break;\
                else if(ret_dec_flu<0)\
                {\
                    std::cout<<"Receive the frame failed."<<std::endl;\
                    break;\
                }\
                {

#define endDecodeFlush(cc,frm)\
                }\
                av_packet_unref(((AVPacket*)(pkt)));\
            }\
        }

#endif //ANDROMEDA_MEDIA_CODEC
