#ifndef ANDROMEDA_MEDIA_PCMAUDIO
#define ANDROMEDA_MEDIA_PCMAUDIO

#include <algorithm>

extern "C"
{
#include <ffmpeg/libavcodec/avcodec.h>
}

#include "../util/log.h"
#include "../util/linked_buffer.h"
#include "audio_codec.h"

namespace andromeda
{
namespace media
{

enum audio_channel
{
	AUDIO_CHANNEL_LEFT = 0, AUDIO_CHANNEL_RIGHT = 1
};

enum audio_operation
{
	AUDIO_OPERATION_NONE = -1, AUDIO_ORDER_RESERVE, AUDIO_ORDER_REGULAR
};
//= signed short int
template<typename T>
class pcm_audio
{
public:
	class sample
	{
		T l, r;

		static constexpr size_t size = sizeof(sample);

		sample(T lv = 0, T rv = 0) :
				l(lv), r(rv)
		{
		}

		template<typename O>
		inline operator typename pcm_audio<O>::sample()
		{
			return typename pcm_audio<O>::sample(l, r);
		}

		template<typename O>
		inline sample operator+(const typename pcm_audio<O>::sample& other)
		{
			return sample(l + other.l, r + other.r);
		}

		template<typename O>
		inline bool operator==(const typename pcm_audio<O>::sample& other)
		{
			return (l == other.l) && (r == other.r);
		}

		template<typename O>
		inline bool operator!=(const typename pcm_audio<O>::sample& other)
		{
			return (l != other.l) || (r != other.r);
		}
	};

protected:
	sample* sample_data;

public:
	int bit_rate;
	int sample_rate;
	size_t nb_channels;
	size_t sample_count;
	size_t data_size;

	pcm_audio(int br, int sr, size_t nbc, pcm_audio::sample* data, size_t data_size) :
			bit_rate(br), sample_rate(sr), nb_channels(nbc), sample_data(data), data_size(data_size), sample_count(data_size / sample::size)
	{
	}

	pcm_audio(int br, int sr, size_t nbc, size_t sample_count) :
			bit_rate(br), sample_rate(sr), nb_channels(nbc), data_size(sample_count * sample::size), sample_count(sample_count)
	{
		sample_data = (sample*)malloc(data_size);
	}

	inline sample* get_samples()
	{
		return sample_data;
	}

	inline void set_samples(sample* data)
	{
		sample_data = data;
	}

	inline sample& get_sample(int sp_idx)
	{
		return *(sample_data + sp_idx);
	}

	void mul_volume(float factor)
	{
		sample* samples = (sample*)(sample_data);
		for(int pos = 0; pos < sample_count; ++pos)
		{
			sample& current_sample = *(samples + pos);
			current_sample.l = getS16SampleInt(current_sample.l * factor);
			current_sample.r = getS16SampleInt(current_sample.r * factor);
		}
	}

	template<typename O>
	static pcm_audio<T>* mix(pcm_audio<T>& aud1, pcm_audio<O>& aud2, long int pos_off)
	{
		pcm_audio<T>* mixed_audio = new pcm_audio<T>(aud1.bit_rate, aud1.sample_rate, std::max(aud1.nb_channels, aud2.nb_channels), std::max(aud1.sample_count, aud2.sample_count + pos_off));
		memcpy(mixed_audio->sample_data, aud1.sample_data, getSmallerNumber(sizeof(sample) * pos_off, aud1.data_size));
		if(pos_off > aud1.sample_count)
		{
			memset(mixed_audio->sample_data + aud1.data_size, 0, sizeof(sample) * pos_off - aud1.data_size);
			memcpy(mixed_audio->sample_data + sizeof(sample) * pos_off, aud2.sample_data, aud2.data_size);
			return *mixed_audio;
		}
		sample* samples = (sample*)(mixed_audio->sample_data), * samples1 = (sample*)(aud1.sample_data), * samples2 = (sample*)(aud2.sample_data);
		if(pos_off + aud2.sample_count > aud1.sample_count)
		{
			for(int pos = pos_off, pos2 = 0; pos < aud1.sample_count; ++pos, ++pos2)
				*(samples + pos) = *(samples1 + pos) + (*(samples2 + pos2));
			memcpy(mixed_audio->sample_data + aud1.data_size, aud2.sample_data + aud1.data_size - sizeof(sample) * pos_off, aud2.data_size - aud1.data_size + sizeof(sample) * pos_off);
			return *mixed_audio;
		}
		for(int pos = pos_off, pos2 = 0; pos2 < aud2.sample_count; ++pos, ++pos2)
			*(samples + pos) = *(samples1 + pos) + (*(samples2 + pos2));
		memcpy(mixed_audio->sample_data + sizeof(sample) * pos_off + aud2.data_size, aud1.sample_data + sizeof(sample) * pos_off + aud2.data_size, aud1.data_size - sizeof(sample) * pos_off - aud2.data_size);
		return mixed_audio;
	}

	static pcm_audio& mul_speed(pcm_audio& audio, float factor);

	static pcm_audio* read(const char* file)
	{
		int ret = 0;
		AVPacket* pkt = nullptr;
		AVFrame* frm = nullptr;
		AVCodec* c = nullptr;
		AVCodecContext* cc = nullptr;
		AVFormatContext* fc = nullptr;
		if((ret = avformat_open_input(&fc, file, nullptr, nullptr)) < 0)
		{
			LogError(ret, "Open input file failed.");
			return nullptr;
		}
		if((ret = avformat_find_stream_info(fc, nullptr)) < 0)
		{
			LogError(ret, "Find stream info failed.");
			return nullptr;
		}
		andromeda::util::linked_buffer<pcm_audio> audios(fc->nb_streams);
		frm = av_frame_alloc();
		if(!frm)
		{
			LogError(ret, "Allocate frame failed.");
			return nullptr;
		}
		pkt = av_packet_alloc();
		if(!pkt)
		{
			LogError(ret, "Allocate packet failed.");
			return nullptr;
		}
		for(int stream_index = 0; stream_index < fc->nb_streams; ++stream_index)
		{
			if(fc->streams[stream_index]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
			{
				cc = fc->streams[stream_index]->codec;
				c = avcodec_find_decoder(cc->codec_id);
				if(!c)
				{
					LogError(ret, "Find decoder failed.");
					return nullptr;
				}
				pcm_audio current_audio(cc->bit_rate, cc->sample_rate, sizeof_sample(cc->sample_fmt), cc->channels);
				linked_buffer<unsigned char> audio_data(2304);
				if((ret = avcodec_open2(cc, c, nullptr)) < 0)
				{
					LogError(ret, "Open codec failed.");
					return nullptr;
				}
				ret = av_read_frame(fc, pkt);
				while(ret >= 0)
				{
					StartDecode(cc, pkt, frm)
					for(int cp = 0; cp < cc->channels; ++cp)
					{
						audio_data.put(frm->extended_data[cp], frm->linesize[cp]);
						current_audio.data_size += frm->linesize[cp];
					}
					EndDecode(cc, pkt, frm)
					ret = av_read_frame(fc, pkt);
				}
				StartDecodeFlush(cc, frm)
				for(int cp = 0; cp < cc->channels; ++cp)
				{
					audio_data.put(frm->extended_data[cp], frm->linesize[cp]);
					current_audio.data_size += frm->linesize[cp];
				}
				EndDecodeFlush(cc, frm)
				current_audio.sample_count = current_audio.data_size / sizeof(sample);
				current_audio.sample_data = (sample*)(audio_data.to_array());
				audios.put(current_audio);
			}
		}
		avformat_close_input(&fc);
		av_free_packet(pkt);
		av_frame_free(&frm);
		avformat_free_context(fc);
		return audios.to_array_copy();
	}

	void write(const char* dest_file)
	{
		int ret = 0;
		AVPacket* pkt = nullptr;
		AVFrame* frm = nullptr;
		AVCodec* c = nullptr;
		AVCodecContext* cc = nullptr;
		AVFormatContext* format_context = avformat_alloc_context();
		if(!format_context)
		{
			LogError(ret, "Allocate format context failed.");
			return;
		}
		//ret=avcodec_parameters_from_context((*(format_context->streams))->codecpar,vc->codec_context);
		if(ret < 0)
			LogError(ret, "Copy parameters from video encoder context failed.");
		strcpy(format_context->filename, dest_file);
		format_context->oformat = av_guess_format(nullptr, dest_file, nullptr);
		//format_context->audio_codec_id=ac->codec->id;
		//addStream(format_context,vc->codec_context,vc->codec);
		ret = avio_open(&format_context->pb, dest_file, AVIO_FLAG_WRITE);
		if(ret < 0)
		{
			LogError(ret, "Open the file failed.");
			return;
		}
		ret = avformat_write_header(format_context, nullptr);
		if(ret < 0)
		{
			LogError(ret, "Write file header failed.");
			return;
		}
		frm = av_frame_alloc();
		if(!frm)
		{
			LogError(ret, "Allocate frame failed.");
			return;
		}
		pkt = av_packet_alloc();
		if(!pkt)
		{
			LogError(ret, "Allocate packet failed.");
			return;
		}
	}

	T** depart_channels()
	{
		short int* left = new short int[data_size / 2];
		short int* right = new short int[data_size / 2];
		short int** channels = new short int*[2]{left, right};
		for(int pos = 0; pos < sample_count; ++pos)
		{
			*(left + pos) = get_sample(pos).l;
			*(right + pos) = get_sample(pos).r;
		}
		return channels;
	}

	static void resample(void* src, void* dest, int start_pos, int end_pos, int stride)
	{
		sample* current_src_sample = (sample*)src + start_pos;
		sample* current_dest_sample = (sample*)dest;
		for(int pos = start_pos; pos < end_pos; ++pos += stride, ++current_dest_sample += stride, ++current_src_sample)
			*current_dest_sample = *current_src_sample;
	}

	static sample* resample(void* src, int start_pos, int end_pos, int stride)
	{
		sample* sampled_data = (sample*)malloc(sizeof(sample) * ceil((double)(1 + start_pos - end_pos) / (1 + stride)));
		resample(src, sampled_data, start_pos, end_pos, stride);
		return sampled_data;
	}

	static void resample(void* src, void* dest, int length, int stride)
	{
		resample(src, dest, 0, length, stride);
	}

	static sample* resample(void* src, int length, int stride)
	{
		return resample(src, 0, length, stride);
	}
};

}
}

#endif // ANDROMEDA_MEDIA_PCMAUDIO
