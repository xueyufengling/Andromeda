#include <andromeda/media/muxer.h>

#include <iostream>
#include <string.h>

#include <andromeda/media/codec.h>
#include <andromeda/media/pcm_audio.h>

#include <andromeda/media/video_codec.h>
#include <andromeda/media/audio_codec.h>
#include <andromeda/media/raster_image.h>
#include <andromeda/io/files.h>

using namespace andromeda::media;
using namespace andromeda::io;

bool muxer::construct(video_codec* vc, audio_codec* ac)
{
	current_video_codec = vc;
	current_audio_codec = ac;
	set_video_stream_num(1);
	set_audio_stream_num(1);
	set_subtile_stream_num(0);
	select_video_stream(0);
	select_audio_stream(0);
	format_context = avformat_alloc_context();
	if(!format_context)
	{
		std::cout << "Allocate format context failed." << std::endl;
		return false;
	}
	if(vc)
	{
		format_context->video_codec_id = vc->avcodec->id;
		format_context->video_codec = vc->avcodec;
	}
	if(ac)
	{
		format_context->audio_codec_id = ac->avcodec->id;
		format_context->audio_codec = ac->avcodec;
	}
	return true;
}

void muxer::deconstruct()
{
	if(format_context)
	{
		avformat_free_context(format_context);
	}
}

void muxer::new_stream(void)
{
	if(current_video_codec)
	{
		append_stream(format_context, current_video_codec->avcodec, current_video_codec->video_properties->nb_video_stream);
		init_video_stream(format_context, current_video_codec->codec_context, current_video_codec->avcodec, current_video_stream);
	}
	if(current_audio_codec)
	{
		//addStream(format_context,video_codec->codec,audio_codec->properties->maxnb_stream);
		//initAudioStream(format_context,video_codec->codec_context,audio_codec->codec,thisAudioStreamIdx);
	}
}

void muxer::open_output_file(const char* file)
{
	int ret = 0;
	strcpy(format_context->filename, file);
	format_context->oformat = av_guess_format(nullptr, file, nullptr);
	clear(file);
	ret = avio_open(&format_context->pb, file, AVIO_FLAG_WRITE);
	if(ret < 0)
		LogError(ret, "Open the file failed.");
	ret = avformat_write_header(format_context, nullptr);
	if(ret < 0)
		LogError(ret, "Write file header failed.");
	current_video_codec->encoded_frame_count = 0;
}

void muxer::close_output_file(void)
{
	int ret = 0;
	startEncodeFlush(current_video_codec->codec_context, *(format_context->streams) + current_video_stream, packet)
	av_interleaved_write_frame(format_context, packet);
	std::cout << "flu" << std::endl;
	logEncodingStatus(current_video_codec->frame, current_video_codec->packet);
	endEncodeFlush(current_video_codec->codec_context, *(format_context->streams) + current_video_stream, packet)
	ret = av_write_trailer(format_context);
	if(ret < 0)
		LogError(ret, "Write file trailer failed.");
	avio_close(format_context->pb);
	current_video_codec->encoded_frame_count = 0;
}

void muxer::append_video_frame(raster_image& img, int last_frame_count)
{
	if(last_frame_count < 1)
		return;
	video_codec::loadYUV444PDataFromRasterImageToFrame(img, current_video_codec->frame);
	for(; last_frame_count; --last_frame_count)
	{
		startEncode(current_video_codec->codec_context, current_video_codec->frame, *(format_context->streams) + current_video_stream, current_video_codec->packet, &(current_video_codec->encoded_frame_count))
		av_interleaved_write_frame(format_context, current_video_codec->packet);
		std::cout << "enc" << std::endl;
		logEncodingStatus(current_video_codec->frame, current_video_codec->packet);
		endEncode(current_video_codec->codec_context, current_video_codec->frame, *(format_context->streams) + current_video_stream, current_video_codec->packet, &(current_video_codec->encoded_frame_count))
	}
}

void muxer::append_video_frame(video_codec::frame_info* infos, size_t length)
{
	for(int pos = 0; pos < length; ++pos)
		append_video_frame(*((*(infos + pos)).img), (*(infos + pos)).last_frame_count);
}
