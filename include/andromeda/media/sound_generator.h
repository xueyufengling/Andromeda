#ifndef ANDROMEDA_MEDIA_SOUNDGENERATOR
#define ANDROMEDA_MEDIA_SOUNDGENERATOR

#include <iostream>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include "pcm_audio.h"
#include "../util/string_utils.h"

namespace andromeda {
	namespace media {
		namespace procfunc {
			double constant(double v);
			double pass(double v);
			double sinwav(double v);
			template<int k=1>
			double linear(double v);
		}

		typedef double (*wave_func)(double t);

		class SingleSoundRawInfo
		{
		public:
			wave_func* wave_funcs;
			int wave_funcs_num;
			double **freq_amp_phrase_infos; //double frequency,double amplitude,double phrase.Plain data buffer for each function like this: f1,d1,a1,f2,d2,a2...
			int *freq_amp_phrase_infos_num; //info number of each function.

			SingleSoundRawInfo(wave_func* wave_funcs_arr=(wave_func*)andromeda::util::memasgn<>(malloc(sizeof(&procfunc::sinwav)),&procfunc::sinwav),int wave_funcs_arr_num=1,double **freq_amp_phrase_infos_arr=nullptr,int *freq_amp_phrase_infos_arr_num=nullptr);

			__attribute__((always_inline)) double* getInfoValuesForFuncAt(int func_pos)
			{
				return *(func_pos+freq_amp_phrase_infos);
			}

			__attribute__((always_inline)) double* getInfoValuesAt(int func_pos,int info_pos)
			{
				return 3*info_pos+getInfoValuesForFuncAt(func_pos);
			}

			__attribute__((always_inline)) double getInfoValueAt(int func_pos,int info_pos,int value_pos)
			{
				return *(double*)(value_pos+getInfoValuesAt(func_pos,info_pos));
			}

			__attribute__((always_inline))      wave_func getFuncAt(int func_pos)
			{
				return *(wave_funcs+func_pos);
			}

			__attribute__((always_inline)) double getPhraseAt(int func_pos,int info_pos)
			{
				return getInfoValueAt(func_pos,info_pos,2);
			}

			__attribute__((always_inline)) double getAmpAt(int func_pos,int info_pos)
			{
				return getInfoValueAt(func_pos,info_pos,1);
			}

			__attribute__((always_inline)) double getFreqAt(int func_pos,int info_pos)
			{
				return getInfoValueAt(func_pos,info_pos,0);
			}

			__attribute__((always_inline)) double getSingleValueInlined(int func_pos,int info_pos,double t)
			{
				return *(double*)(sizeof(double)+sizeof(double)*3*info_pos+*(sizeof(nullptr)*func_pos+(char**)freq_amp_phrase_infos))*(*(wave_func*)((char*)wave_funcs+sizeof(nullptr)*func_pos))(*(double*)(sizeof(double)*3*info_pos+*(sizeof(nullptr)*func_pos+(char**)freq_amp_phrase_infos))*(t+*(double*)(sizeof(double)*2+sizeof(double)*3*info_pos+*(sizeof(nullptr)*func_pos+(char**)freq_amp_phrase_infos))));
			}

			__attribute__((always_inline)) double getSingleValue(int func_pos,int info_pos,double t)
			{
				return getAmpAt(func_pos,info_pos)*(getFuncAt(func_pos))(getFreqAt(func_pos,info_pos)*(t+getPhraseAt(func_pos,info_pos)));
			}

			double getValue(double t);

			static SingleSoundRawInfo generateSinSingleNoteRawInfo(double base_freq,double* overtone_amps,int level);
			static SingleSoundRawInfo generateSinSingleNoteRawInfo(double* freqs,double* amps,int freq_num);
			static SingleSoundRawInfo generateSinSingleNoteRawInfo(double* freqs,double* amps,double* phrases,int freq_num);
		};

		class SoundRawTimeline
		{
		public:
			SoundRawTimeline(SingleSoundRawInfo* raw_infos_,wave_func* raw_info_proc_funcs_,int raw_infos_num_,double** time_infos_,int *time_infos_num_);

			SingleSoundRawInfo* raw_infos;
			wave_func* raw_info_proc_funcs;
			int raw_infos_num;
			double** time_infos; //double start time,double length
			int *time_infos_num;

		};

		class SoundGenerator
		{
		public:
			SoundGenerator(int bit_rate_=1024E3,int sample_size_=sizeof(Sample),int nb_channels_=2);

			PCMAudio generatePCMAudio(SoundRawTimeline* left_timeline,SoundRawTimeline* right_timeline,double total_time);
			//generatePCMAudioFreqAmp<int>(wave_func,int,int);

			inline void setBitRate(int br)
			{
				bit_rate=br;
			}

			inline int getBitRate(void)
			{
				return bit_rate;
			}
		protected:
			int bit_rate;
			int sample_rate;
			int sample_size;
			int nb_channels;

			struct TemplateFuncs
			{
				template<typename T>
				PCMAudio generatePCMAudioFreqAmp(wave_func wave_construct_func,T* freq_values,T*amp_values)
				{

				}
			};
		};
	}
}

#endif // ANDROMEDA_MEDIA_SOUNDGENERATOR
