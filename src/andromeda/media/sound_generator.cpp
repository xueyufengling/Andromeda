#include <andromeda/media/sound_generator.h>

#include <andromeda/math/operations.h>

using namespace andromeda::media;

double procfunc::pass(double v)
{
	return v>1?1:v;
}

double procfunc::constant(double v)
{
	return 1;
}

double procfunc::sinwav(double v)
{
	return sin(2*M_PI*v);
}

template<int k>
double procfunc::linear(double v)
{
	return k*v;
}

SoundGenerator::SoundGenerator(int bit_rate_,int sample_size_,int nb_channels_) :
		bit_rate(bit_rate_), sample_rate(bit_rate_/sample_size_/8), sample_size(sample_size_), nb_channels(nb_channels_)
{
}

SoundRawTimeline::SoundRawTimeline(SingleSoundRawInfo* raw_infos_,wave_func* raw_info_proc_funcs_,int raw_infos_num_,double** time_infos_,int* time_infos_num_) :
		raw_infos(raw_infos_), raw_info_proc_funcs(raw_info_proc_funcs_==nullptr?new wave_func[1]{&procfunc::constant}:raw_info_proc_funcs_), raw_infos_num(raw_infos_num_), time_infos(time_infos_), time_infos_num(time_infos_num_)
{
}

SingleSoundRawInfo::SingleSoundRawInfo(wave_func* wave_funcs_arr,int wave_funcs_arr_num,double** freq_amp_phrase_infos_arr,int* freq_amp_phrase_infos_arr_num) :
		wave_funcs(wave_funcs_arr), wave_funcs_num(wave_funcs_arr_num), freq_amp_phrase_infos(freq_amp_phrase_infos_arr), freq_amp_phrase_infos_num(freq_amp_phrase_infos_arr_num)
{
}

double SingleSoundRawInfo::getValue(double t)
{
	if(freq_amp_phrase_infos_num==nullptr)
		return 0;
	double v=0;
	register int c_funcs=wave_funcs_num;
	while(--c_funcs>=0)
	{
		register int c_infos=*(freq_amp_phrase_infos_num+c_funcs);
		while(--c_infos>=0)
		{
			v+=getSingleValueInlined(c_funcs,c_infos,t);
		}
	}
	return v;
}

SingleSoundRawInfo SingleSoundRawInfo::generateSinSingleNoteRawInfo(double* freqs,double* amps,double* phrases,int freq_num)
{
	double* infos=(double*)malloc(sizeof(double)*3*freq_num);
	wave_func* funcs=(wave_func*)malloc(sizeof(&procfunc::sinwav));
	*funcs=procfunc::sinwav;
	SingleSoundRawInfo note(funcs,1,&infos,new int(freq_num));
	register int c_level=0;
	while(c_level<freq_num)
	{
		*(infos+3*c_level)=*(freqs+c_level);
		*(infos+3*c_level+1)=*(amps+c_level);
		*(infos+3*c_level+2)=*(phrases+c_level);
		++c_level;
	}
	return note;
}

SingleSoundRawInfo SingleSoundRawInfo::generateSinSingleNoteRawInfo(double* freqs,double* amps,int freq_num)
{
	return generateSinSingleNoteRawInfo(freqs,amps,(double*)memset(malloc(sizeof(double)*freq_num),0,sizeof(double)*freq_num),freq_num);
}

SingleSoundRawInfo SingleSoundRawInfo::generateSinSingleNoteRawInfo(double base_freq,double* overtone_amps,int level)
{
	double* freqs=(double*)malloc(sizeof(double)*level);
	for(int i=0;i<level;)
	{
		*(freqs+i)=base_freq*(++i);
	}
	return generateSinSingleNoteRawInfo(freqs,overtone_amps,level);
}

PCMAudio SoundGenerator::generatePCMAudio(SoundRawTimeline* left_timeline,SoundRawTimeline* right_timeline,double total_time)
{
	if(total_time<=0||!(left_timeline||right_timeline))
		return PCMAudio();
	double delta_t_per_idx=1.0/sample_rate;
	int arr_len=total_time*bit_rate/8;
	signed short int* audio_data=(signed short int*)malloc(arr_len);
	memset(audio_data,0,arr_len);
	register int info_pos=0,start_offset=0;
	SoundRawTimeline* timeline=left_timeline;
	bool lr_flag=true;
	loop:while(info_pos<timeline->raw_infos_num)
	{
		register int time_pos=0;
		double* time_info;
		SingleSoundRawInfo* ssr;
		ssr=(timeline->raw_infos)+info_pos;
		while(time_pos<*(timeline->time_infos_num+info_pos))
		{
			time_info=*(timeline->time_infos+time_pos);
			if(*time_info<=total_time)
			{
				int start_idx=2*sample_rate**time_info,end_idx=start_idx+2*sample_rate**(time_info+1);
				for(double dt=0;(start_idx<end_idx)&&(start_idx<arr_len);++start_idx,dt+=delta_t_per_idx)
				{
					*(audio_data+start_offset+start_idx)=(signed short int)((*(wave_func*)(timeline->raw_info_proc_funcs+info_pos))(dt)*(ssr->getValue(dt))*32768); //32768=2^15
				}
			}
			++time_pos;
		}
		++info_pos;
	}
	if(lr_flag)
	{
		start_offset=1;
		timeline=right_timeline;
		lr_flag=false;
		goto loop;
	}
	return PCMAudio(bit_rate,sample_rate,sample_size,nb_channels,arr_len,(Sample*)audio_data);
}
