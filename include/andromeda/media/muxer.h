#ifndef ANDROMEDA_MEDIA_MUXER
#define ANDROMEDA_MEDIA_MUXER

#include <ffmpeg/libavcodec/avcodec.h>
#include <ffmpeg/libavformat/avformat.h>

#include "raster_image.h"
#include "video_codec.h"
#include "audio_codec.h"

namespace andromeda
{
namespace media
{
class muxer
{
public:
	bool construct(video_codec* vc, audio_codec* ac);

	muxer(video_codec* vc, audio_codec* ac)
	{
		construct(vc, ac);
	}

	void deconstruct();

	~muxer()
	{
		deconstruct();
	}

	void new_stream(void);

	void open_output_file(const char* file);
	void close_output_file(void);

	/**
	 * @brief 为视频流添加图像帧
	 * @param img 图像
	 * @param last_frame_count 该图像持续时间
	 */
	void append_video_frame(andromeda::media::raster_image& img, int last_frame_count = 1);

	void append_video_frame(video_codec::frame_info* infos, size_t length);

	inline int get_streams_max_num()
	{
		return current_video_codec->video_properties->nb_video_stream + current_video_codec->video_properties->nb_audio_stream + current_video_codec->video_properties->nb_subtitle_stream;
	}

	inline void set_video_stream_num(int nb)
	{
		current_video_codec->video_properties->nb_video_stream = nb;
	}

	inline void set_audio_stream_num(int nb)
	{
		current_video_codec->video_properties->nb_audio_stream = nb;
	}

	inline void set_subtile_stream_num(int nb)
	{
		current_video_codec->video_properties->nb_subtitle_stream = nb;
	}

	inline void select_video_stream(int new_idx)
	{
		current_video_stream = new_idx;
	}

	inline void select_audio_stream(int new_idx)
	{
		current_audio_stream = current_video_codec->video_properties->nb_video_stream + new_idx;
	}

	inline void select_subtile_stream(int new_idx)
	{
		current_subtitle_stream = current_video_codec->video_properties->nb_video_stream + current_video_codec->video_properties->nb_audio_stream + new_idx;
	}

protected:
	int current_video_stream, current_audio_stream, current_subtitle_stream;
	video_codec* current_video_codec;
	audio_codec* current_audio_codec;
	AVPacket* packet;
	AVFormatContext* format_context;
};
}
}

#endif // ANDROMEDA_MEDIA_MUXER
