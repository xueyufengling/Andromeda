#include <andromeda/media/muxer.h>

#include <iostream>
#include <string.h>

#include <andromeda/media/codec.h>
#include <andromeda/media/pcm_audio.h>

#include <andromeda/media/video_codec.h>
#include <andromeda/media/audio_codec.h>
#include <andromeda/media/raster_image.h>
#include <andromeda/io/files.h>
#include <andromeda/media/video_factory.h>

using namespace andromeda::media;
using namespace andromeda::io;

Muxer::Muxer(VideoCodec* vc,AudioCodec* ac)
{
	construct(vc,ac);
}

Muxer::~Muxer()
{
	deconstruct();
}

bool Muxer::construct(VideoCodec* vc,AudioCodec* ac)
{
	video_codec=vc;
	audio_codec=ac;
	setVideoStreamNumber(1);
	setAudioStreamNumber(1);
	setSubtileStreamNumber(0);
	selectVideoStreamIndex(0);
	selectAudioStreamIndex(0);
	format_context=avformat_alloc_context();
	if(!format_context)
	{
		std::cout<<"Allocate format context failed."<<std::endl;
		return false;
	}
	if(vc)
	{
		format_context->video_codec_id=vc->codec->id;
		format_context->video_codec=vc->codec;
	}
	if(ac)
	{
		format_context->audio_codec_id=ac->codec->id;
		format_context->audio_codec=ac->codec;
	}
	return true;
}

void Muxer::deconstruct()
{
	if(format_context)
	{
		avformat_free_context(format_context);
	}
}

void Muxer::applySettings(void)
{
	if(video_codec)
	{
		addStream(format_context,video_codec->codec,video_codec->properties->nb_video_stream);
		initVideoStream(format_context,video_codec->codec_context,video_codec->codec,thisVideoStreamIdx);
	}
	if(audio_codec)
	{
		//addStream(format_context,video_codec->codec,audio_codec->properties->maxnb_stream);
		//initAudioStream(format_context,video_codec->codec_context,audio_codec->codec,thisAudioStreamIdx);
	}
}

void Muxer::openOutputVideoFile(const char* file)
{
	int ret=0;
	strcpy(format_context->filename,file);
	format_context->oformat=av_guess_format(nullptr,file,nullptr);
	clearFile(file);
	ret=avio_open(&format_context->pb,file,AVIO_FLAG_WRITE);
	if(ret<0)
		logError(ret,"Open the file failed.");
	ret=avformat_write_header(format_context,nullptr);
	if(ret<0)
		logError(ret,"Write file header failed.");
	video_codec->encoded_frame_count=0;
}

void Muxer::closeOutputVideoFile(void)
{
	int ret=0;
	startEncodeFlush(video_codec->codec_context,*(format_context->streams)+thisVideoStreamIdx,packet)
av_interleaved_write_frame		(format_context,packet);std::cout<<"flu"<<std::endl;logEncodingStatus(video_codec->frame,video_codec->packet);
		endEncodeFlush(video_codec->codec_context,*(format_context->streams)+thisVideoStreamIdx,packet)
	ret=av_write_trailer(format_context);
	if(ret<0)
		logError(ret,"Write file trailer failed.");
	avio_close(format_context->pb);
	video_codec->encoded_frame_count=0;
}

void Muxer::inputVideoFrame(RasterImage& img,int last_frame_count)
{
	if(last_frame_count<1)
		return;
	VideoCodec::loadYUV444PDataFromRasterImageToFrame(img,video_codec->frame);
	for(;last_frame_count;--last_frame_count)
	{
		startEncode(video_codec->codec_context,video_codec->frame,*(format_context->streams)+thisVideoStreamIdx,video_codec->packet,&(video_codec->encoded_frame_count))
av_interleaved_write_frame			(format_context,video_codec->packet);std::cout<<"enc"<<std::endl;logEncodingStatus(video_codec->frame,video_codec->packet);
			endEncode(video_codec->codec_context,video_codec->frame,*(format_context->streams)+thisVideoStreamIdx,video_codec->packet,&(video_codec->encoded_frame_count))
	}
}

void Muxer::inputVideoFrames(VideoFrameInfo* infos,int length)
{
	for(int pos=0;pos<length;++pos)
		inputVideoFrame(*((*(infos+pos)).img),(*(infos+pos)).last_frame_count);
}
