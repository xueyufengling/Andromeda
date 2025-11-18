#include <andromeda/media/pcm_audio.h>

#include <malloc.h>
#include <string.h>
#include <math.h>
#include <andromeda/media/codec.h>
#include <andromeda/math/operations.h>
#include <andromeda/util/linked_chunk.h>

using namespace andromeda::media;
using namespace andromeda::util;

pcm_audio* pcm_audio::read(const char* file)
{
	int ret = 0;
	AVPacket* pkt = nullptr;
	AVFrame* frm = nullptr;
	AVCodec* c = nullptr;
	AVCodecContext* cc = nullptr;
	AVFormatContext* fc = nullptr;
	if((ret = avformat_open_input(&fc, file, nullptr, nullptr)) < 0)
	{
		logError(ret, "Open input file failed.");
		return nullptr;
	}
	if((ret = avformat_find_stream_info(fc, nullptr)) < 0)
	{
		logError(ret, "Find stream info failed.");
		return nullptr;
	}
	linked_buffer<pcm_audio> audios(fc->nb_streams);
	frm = av_frame_alloc();
	if(!frm)
	{
		logError(ret, "Allocate frame failed.");
		return nullptr;
	}
	pkt = av_packet_alloc();
	if(!pkt)
	{
		logError(ret, "Allocate packet failed.");
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
				logError(ret, "Find decoder failed.");
				return nullptr;
			}
			pcm_audio this_audio(cc->bit_rate, cc->sample_rate, getSampleSize(cc->sample_fmt), cc->channels);
			linked_buffer<unsigned char> audio_data(2304);
			if((ret = avcodec_open2(cc, c, nullptr)) < 0)
			{
				logError(ret, "Open codec failed.");
				return nullptr;
			}
			ret = av_read_frame(fc, pkt);
			while(ret >= 0)
			{
				startDecode(cc,pkt,frm)
for					(int cp=0;cp<cc->channels;++cp)
					{
						audio_data.put(frm->extended_data[cp],frm->linesize[cp]);
						this_audio.data_size+=frm->linesize[cp];
					}
					endDecode(cc,pkt,frm)
					ret=av_read_frame(fc,pkt);
				}
			startDecodeFlush(cc,frm)
for				(int cp=0;cp<cc->channels;++cp)
				{
					audio_data.put(frm->extended_data[cp],frm->linesize[cp]);
					this_audio.data_size+=frm->linesize[cp];
				}
				endDecodeFlush(cc,frm)
			this_audio.sample_count = this_audio.data_size / sizeof(sample);
			this_audio.sample_data = (sample*)(audio_data.to_array());
			audios.put(this_audio);
		}
	}
	avformat_close_input(&fc);
	av_free_packet(pkt);
	av_frame_free(&frm);
	avformat_free_context(fc);
	return audios.to_array_copy();
}

void pcm_audio::write(const char* dest_file)
{
	int ret = 0;
	AVPacket* pkt = nullptr;
	AVFrame* frm = nullptr;
	AVCodec* c = nullptr;
	AVCodecContext* cc = nullptr;
	AVFormatContext* format_context = avformat_alloc_context();
	if(!format_context)
	{
		logError(ret, "Allocate format context failed.");
		return;
	}
	//ret=avcodec_parameters_from_context((*(format_context->streams))->codecpar,vc->codec_context);
	if(ret < 0)
		logError(ret, "Copy parameters from video encoder context failed.");
	strcpy(format_context->filename, dest_file);
	format_context->oformat = av_guess_format(nullptr, dest_file, nullptr);
	//format_context->audio_codec_id=ac->codec->id;
	//addStream(format_context,vc->codec_context,vc->codec);
	ret = avio_open(&format_context->pb, dest_file, AVIO_FLAG_WRITE);
	if(ret < 0)
	{
		logError(ret, "Open the file failed.");
		return;
	}
	ret = avformat_write_header(format_context, nullptr);
	if(ret < 0)
	{
		logError(ret, "Write file header failed.");
		return;
	}
	frm = av_frame_alloc();
	if(!frm)
	{
		logError(ret, "Allocate frame failed.");
		return;
	}
	pkt = av_packet_alloc();
	if(!pkt)
	{
		logError(ret, "Allocate packet failed.");
		return;
	}
}

pcm_audio& pcm_audio::mix(pcm_audio& aud1, pcm_audio& aud2, long int pos_off)
{
	pcm_audio* mixed_audio = new pcm_audio(aud1.bit_rate, aud1.sample_rate, aud1.sample_size, getBiggerNumber(aud1.nb_channels, aud2.nb_channels), getMixedAudioSize(aud1.sample_count, aud2.sample_count, pos_off));
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
	return *mixed_audio;
}

void pcm_audio::mul_volume(float factor)
{
	sample* samples = (sample*)(sample_data);
	for(int pos = 0; pos < sample_count; ++pos)
	{
		sample& this_sample = *(samples + pos);
		this_sample.l = getS16SampleInt(this_sample.l * factor);
		this_sample.r = getS16SampleInt(this_sample.r * factor);
	}
}

short int** pcm_audio::depart_channels()
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

pcm_audio& pcm_audio::mul_speed(pcm_audio& audio, float factor)
{
	pcm_audio* speeded_audio = new pcm_audio(audio.bit_rate, audio.sample_rate, audio.sample_size, audio.nb_channels);
	audio_operation opt = AUDIO_ORDER_REGULAR;
	if(factor == 0)
	{
		return *speeded_audio;
	}
	if(factor < 0)
	{
		opt = AUDIO_ORDER_RESERVE;
		factor = -factor;
	}
	double dest_relative_sample_size = 1.0 / factor;
	int dest_data_sample_count = ceil(audio.sample_count * dest_relative_sample_size);
	sample* audio_data = (sample*)malloc(sizeof(sample) * dest_data_sample_count);
	speeded_audio->sample_count = dest_data_sample_count;
	speeded_audio->data_size = sizeof(sample) * dest_data_sample_count;
	double relative_pos = 0;
	long long last_relative_pos_q = 0;
	double dest_this_sample_l_sum = 0;
	double dest_this_sample_r_sum = 0;
	double cross_ratio = 0;
	double last_rest_cross_ratio = 1;
	int crossed_raw_sample_count = 0;
	int dest_pos = 0;
	int raw_pos = 0;
	switch(opt)
	{
	case AUDIO_ORDER_REGULAR:
		if(dest_relative_sample_size == 1)
		{
			for(raw_pos = 0, dest_pos = 0; raw_pos < audio.sample_count; ++raw_pos, ++dest_pos)
			{
				sample& this_raw_sample = audio.get_sample(raw_pos);
				sample& this_dest_sample = *(audio_data + dest_pos);
				this_dest_sample.l = this_raw_sample.l;
				this_dest_sample.r = this_raw_sample.r;
			}
			break;
		}
		raw_pos = 0;
		relative_pos += dest_relative_sample_size;
		do
		{
			sample& this_raw_sample = audio.get_sample(raw_pos);
			cross_ratio = relative_pos - (long int)relative_pos;
			if(relative_pos - last_relative_pos_q > 1)
			{
				if(dest_relative_sample_size < 1)
				{
					dest_this_sample_l_sum += this_raw_sample.l * (dest_relative_sample_size - cross_ratio);
					dest_this_sample_r_sum += this_raw_sample.r * (dest_relative_sample_size - cross_ratio);
					++crossed_raw_sample_count;
					sample& this_dest_sample = *(audio_data + dest_pos);
					++dest_pos;
					this_dest_sample.l = getS16SampleInt(dest_this_sample_l_sum / crossed_raw_sample_count);
					this_dest_sample.r = getS16SampleInt(dest_this_sample_r_sum / crossed_raw_sample_count);
					if(cross_ratio > 0)
					{
						crossed_raw_sample_count = 1;
						dest_this_sample_l_sum = cross_ratio * this_raw_sample.l;
						dest_this_sample_r_sum = cross_ratio * this_raw_sample.r;
					}
					else
					{
						crossed_raw_sample_count = 0;
						dest_this_sample_l_sum = 0;
						dest_this_sample_r_sum = 0;
					}
				}
				else if(dest_relative_sample_size > 1)
				{
					dest_this_sample_l_sum += this_raw_sample.l * last_rest_cross_ratio;
					dest_this_sample_r_sum += this_raw_sample.r * last_rest_cross_ratio;
					++crossed_raw_sample_count;
					sample& first_dest_sample = *(audio_data + dest_pos);
					++dest_pos;
					first_dest_sample.l = getS16SampleInt(dest_this_sample_l_sum / crossed_raw_sample_count);
					first_dest_sample.r = getS16SampleInt(dest_this_sample_r_sum / crossed_raw_sample_count);
					for(int i = 2; i < ceil(relative_pos) - last_relative_pos_q; ++i, ++dest_pos)
					{
						sample& same_dest_sample = *(audio_data + dest_pos);
						same_dest_sample.l = this_raw_sample.l;
						same_dest_sample.r = this_raw_sample.r;
					}
					last_rest_cross_ratio = 1 - cross_ratio;
					dest_this_sample_l_sum = this_raw_sample.l * cross_ratio;
					dest_this_sample_r_sum = this_raw_sample.r * cross_ratio;
					if(cross_ratio == 0)
					{
						crossed_raw_sample_count = 0;
						sample& last_dest_sample = *(audio_data + dest_pos);
						last_dest_sample.l = this_raw_sample.l;
						last_dest_sample.r = this_raw_sample.r;
						++dest_pos;
					}
					else
					{
						crossed_raw_sample_count = 1;
					}
				}
				goto CONTINUE_REG;
			}
			else
			{
				dest_this_sample_l_sum += this_raw_sample.l;
				dest_this_sample_r_sum += this_raw_sample.r;
				++crossed_raw_sample_count;
			}
			CONTINUE_REG: ++raw_pos;
			last_relative_pos_q = relative_pos;
			relative_pos += dest_relative_sample_size;
		}while(raw_pos < audio.sample_count);
		(*(audio_data + dest_data_sample_count - 1)).l = getS16SampleInt(dest_this_sample_l_sum / crossed_raw_sample_count);
		(*(audio_data + dest_data_sample_count - 1)).r = getS16SampleInt(dest_this_sample_r_sum / crossed_raw_sample_count);
		break;
	case AUDIO_ORDER_RESERVE:
		if(dest_relative_sample_size == 1)
		{
			for(raw_pos = audio.sample_count - 1, dest_pos = 0; raw_pos >= 0; --raw_pos, ++dest_pos)
			{
				sample& this_raw_sample = audio.get_sample(raw_pos);
				sample& this_dest_sample = *(audio_data + dest_pos);
				this_dest_sample.l = this_raw_sample.l;
				this_dest_sample.r = this_raw_sample.r;
			}
			break;
		}
		raw_pos = audio.sample_count - 1;
		relative_pos += dest_relative_sample_size;
		do
		{
			sample& this_raw_sample = audio.get_sample(raw_pos);
			cross_ratio = relative_pos - (long int)relative_pos;
			if(relative_pos - last_relative_pos_q > 1)
			{
				if(dest_relative_sample_size < 1)
				{
					dest_this_sample_l_sum += this_raw_sample.l * (dest_relative_sample_size - cross_ratio);
					dest_this_sample_r_sum += this_raw_sample.r * (dest_relative_sample_size - cross_ratio);
					++crossed_raw_sample_count;
					sample& this_dest_sample = *(audio_data + dest_pos);
					++dest_pos;
					this_dest_sample.l = getS16SampleInt(dest_this_sample_l_sum / crossed_raw_sample_count);
					this_dest_sample.r = getS16SampleInt(dest_this_sample_r_sum / crossed_raw_sample_count);
					if(cross_ratio > 0)
					{
						crossed_raw_sample_count = 1;
						dest_this_sample_l_sum = cross_ratio * this_raw_sample.l;
						dest_this_sample_r_sum = cross_ratio * this_raw_sample.r;
					}
					else
					{
						crossed_raw_sample_count = 0;
						dest_this_sample_l_sum = 0;
						dest_this_sample_r_sum = 0;
					}
				}
				else if(dest_relative_sample_size > 1)
				{
					dest_this_sample_l_sum += this_raw_sample.l * last_rest_cross_ratio;
					dest_this_sample_r_sum += this_raw_sample.r * last_rest_cross_ratio;
					++crossed_raw_sample_count;
					sample& first_dest_sample = *(audio_data + dest_pos);
					++dest_pos;
					first_dest_sample.l = getS16SampleInt(dest_this_sample_l_sum / crossed_raw_sample_count);
					first_dest_sample.r = getS16SampleInt(dest_this_sample_r_sum / crossed_raw_sample_count);
					for(int i = 2; i < ceil(relative_pos) - last_relative_pos_q; ++i, ++dest_pos)
					{
						sample& same_dest_sample = *(audio_data + dest_pos);
						same_dest_sample.l = this_raw_sample.l;
						same_dest_sample.r = this_raw_sample.r;
					}
					last_rest_cross_ratio = 1 - cross_ratio;
					dest_this_sample_l_sum = this_raw_sample.l * cross_ratio;
					dest_this_sample_r_sum = this_raw_sample.r * cross_ratio;
					if(cross_ratio == 0)
					{
						crossed_raw_sample_count = 0;
						sample& last_dest_sample = *(audio_data + dest_pos);
						last_dest_sample.l = this_raw_sample.l;
						last_dest_sample.r = this_raw_sample.r;
						++dest_pos;
					}
					else
					{
						crossed_raw_sample_count = 1;
					}
				}
				goto CONTINUE_RES;
			}
			else
			{
				dest_this_sample_l_sum += this_raw_sample.l;
				dest_this_sample_r_sum += this_raw_sample.r;
				++crossed_raw_sample_count;
			}
			CONTINUE_RES: --raw_pos;
			last_relative_pos_q = relative_pos;
			relative_pos += dest_relative_sample_size;
		}while(raw_pos >= 0);
		(*(audio_data + dest_data_sample_count - 1)).l = getS16SampleInt(dest_this_sample_l_sum / crossed_raw_sample_count);
		(*(audio_data + dest_data_sample_count - 1)).r = getS16SampleInt(dest_this_sample_r_sum / crossed_raw_sample_count);
		break;
	}
	speeded_audio->set_samples(audio_data);
	return *speeded_audio;
}

void andromeda::media::resample(void* src, void* dest, int start_pos, int end_pos, int stride)
{
	sample* this_src_sample = (sample*)src + start_pos;
	sample* this_dest_sample = (sample*)dest;
	for(int pos = start_pos; pos < end_pos; ++pos += stride, ++this_dest_sample += stride, ++this_src_sample)
		*this_dest_sample = *this_src_sample;
}

sample* andromeda::media::resample(void* src, int start_pos, int end_pos, int stride)
{
	sample* sampled_data = (sample*)malloc(sizeof(sample) * ceil((double)(1 + start_pos - end_pos) / (1 + stride)));
	resample(src, sampled_data, start_pos, end_pos, stride);
	return sampled_data;
}

void andromeda::media::resample(void* src, void* dest, int length, int stride)
{
	resample(src, dest, 0, length, stride);
}

sample* andromeda::media::resample(void* src, int length, int stride)
{
	return resample(src, 0, length, stride);
}
