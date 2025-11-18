#ifndef ANDROMEDA_MEDIA_VIDEOCODEC
#define ANDROMEDA_MEDIA_VIDEOCODEC

#include <fstream>

#include <ffmpeg/libavcodec/avcodec.h>
#include <ffmpeg/libavformat/avformat.h>

#include "raster_image.h"
#include "codec.h"

namespace andromeda
{
namespace media
{
class video_codec
{
	friend class muxer;

public:
	typedef struct properties
	{
		int width, height;
		int frame_rate_num, frame_rate_den;
		int timebase_num = 0, timebase_den = 0;
		int bit_rate;
		int gop_size;
		int max_b_frames;
		int nb_video_stream, nb_audio_stream, nb_subtitle_stream;
	} properties;

	typedef struct frame_info
	{
		andromeda::media::raster_image* img;
		int last_frame_count;

		frame_info(raster_image& image) :
				img(&image), last_frame_count(1)
		{
		}

		frame_info(raster_image& image, int lfc) :
				img(&image), last_frame_count(lfc)
		{
		}
	} frame_info;

	enum output_mode
	{
		OUTPUT_NONE = -1, OUTPUT_VIDEO_BUFFER, OUTPUT_VIDEO_FILE, OUTPUT_BITSTREAM_BUFFER, OUTPUT_BITSTREAM_FILE
	};

protected:
	AVCodec* codec;
	AVCodecContext* codec_context;
	AVFrame* frame;
	AVPacket* packet;
	std::ofstream* output_file;
	properties* video_properties;
	bool is_buf_alloc;
	long int encoded_frame_count, packet_count;

	bool init_codec(AVCodecID id);
	bool construct(AVCodecID id);
	void deconstruct();

public:
	video_codec(AVCodecID id);
	~video_codec();

	/**
	 * @brief 为视频流添加图像帧
	 * @param img 图像
	 * @param last_frame_count 该图像持续时间
	 */
	void append_frame(andromeda::media::raster_image& img, int last_frame_count);
	void append_frame(frame_info* infos, int length);

	void applySettings(void);

	void openOutputBitstreamFile(const char* file);
	void closeOutputBitstreamFile(void);

	inline void setFrameSize(int width, int height)
	{
		video_properties->width = width;
		video_properties->height = height;
	}

	inline void setFrameRate(int numerator, int denominator)
	{
		video_properties->frame_rate_num = numerator;
		video_properties->frame_rate_den = denominator;
	}

	inline void setTimeBase(int numerator, int denominator)
	{
		video_properties->timebase_num = numerator;
		video_properties->timebase_den = denominator;
	}

	inline void setBitRate(int rate)
	{
		video_properties->bit_rate = rate;
	}

	inline void setGopSize(int gopsize)
	{
		video_properties->gop_size = gopsize;
	}

	inline void setMaxBFrames(int max)
	{
		video_properties->max_b_frames = max;
	}

	inline void setProperties(properties* pro)
	{
		video_properties = pro;
	}

	static void loadYUV444PDataFromRasterImageToFrame(andromeda::media::raster_image& img, AVFrame* frame_);
};

void encode(AVCodecContext* cc, AVFrame* frm, AVFormatContext* fc, AVStream* st, AVPacket* pkt, long int* count, andromeda::media::packet_proc p);
void encode_flush(AVCodecContext* cc, AVFormatContext* fc, AVStream* st, AVPacket* pkt, andromeda::media::packet_proc p);
void decode(AVCodecContext* cc, AVPacket* pkt, AVFormatContext* fc, AVFrame* frm, andromeda::media::frame_proc p);
void decode_flush(AVCodecContext* cc, AVFormatContext* fc, AVFrame* frm, andromeda::media::frame_proc p);

inline char* get_err_msg(int err_code, char* msg_buf = new char[AV_ERROR_MAX_STRING_SIZE])
{
	av_make_error_string(msg_buf, AV_ERROR_MAX_STRING_SIZE, err_code);
	return msg_buf;
}

void append_stream(AVFormatContext* fc, AVCodec* c, int num);
int append_stream_to(AVFormatContext* fc, AVCodec* c, int lim);
void init_video_stream(AVFormatContext* fc, AVCodecContext* cc, AVCodec* c, int pos);

}
}

#endif // ANDROMEDA_MEDIA_VIDEOCODEC
