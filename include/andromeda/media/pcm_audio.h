#ifndef ANDROMEDA_MEDIA_PCMAUDIO
#define ANDROMEDA_MEDIA_PCMAUDIO

#include <ffmpeg/libavcodec/avcodec.h>

#define GetS16SampleInt(i) (i<-32768?-32768:(i>32767?32767:i))
#define SampleLinear(s,e,t,i) (GetS16SampleInt((s+(double)(e-s)/t*i)))
#define GetMixedAudioSize(s1,s2,off) (s2+off>s1?s2+off:s1)

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

class pcm_audio
{
public:
	typedef struct sample
	{
		signed short int l, r;

		sample(signed short int lv = 0, signed short int rv = 0) :
				l(lv), r(rv)
		{
		}

		inline sample operator+(const sample& other)
		{
			return sample(GetS16SampleInt(l + other.l), GetS16SampleInt(r + other.r));
		}

		inline bool operator==(sample& s)
		{
			return (l == s.l) && (r == s.r);
		}

		inline bool operator!=(sample& s)
		{
			return (l != s.l) || (r != s.r);
		}
	} sample;

protected:
	sample* sample_data;

public:
	int bit_rate;
	int sample_rate;
	size_t sample_size;
	size_t nb_channels;
	size_t sample_count;
	size_t data_size;

	pcm_audio(int br = 320000, int sr = 0, size_t sample_size = 0, size_t nbc = 0, size_t data_size = 0, pcm_audio::sample* data = nullptr) :
			bit_rate(br), sample_rate(sr), sample_size(sample_size), nb_channels(nbc), data_size(data_size), sample_data(data), sample_count(data_size / sample_size)
	{
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

	void mul_volume(float factor);
	short int** depart_channels();

	static pcm_audio& mix(pcm_audio& aud1, pcm_audio& aud2, long int pos_off);
	static pcm_audio& mul_speed(pcm_audio& audio, float factor);
	static pcm_audio* read(const char* file);
	void write(const char* dest_file); //有问题，没写完
};

extern void resample(void* src, void* dest, int start_pos, int end_pos, int stride);
extern pcm_audio::sample* resample(void* src, int start_pos, int end_pos, int stride);
extern void resample(void* src, void* dest, int length, int stride);
extern pcm_audio::sample* resample(void* src, int length, int stride);

inline int sizeof_sample(AVSampleFormat fmt)
{
	switch(fmt)
	{
	case AV_SAMPLE_FMT_S16:
		case AV_SAMPLE_FMT_S16P:
		return 16;
	case AV_SAMPLE_FMT_S32:
		case AV_SAMPLE_FMT_S32P:
		return 32;
	case AV_SAMPLE_FMT_S64:
		case AV_SAMPLE_FMT_S64P:
		return 64;
	case AV_SAMPLE_FMT_U8:
		case AV_SAMPLE_FMT_U8P:
		return 8;
	default:
		return -1;
	}
}

}
}

#endif // ANDROMEDA_MEDIA_PCMAUDIO
