#include <andromeda/media/audio_factory.h>

#include <math.h>
#include <iostream>
#include <andromeda/media/codec.h>
#include <andromeda/media/pcm_audio.h>
#include <andromeda/util/linked_chunk_buffer.h>

using namespace andromeda::media;

void andromeda::media::resample(void* src,void* dest,int start_pos,int end_pos,int stride)
{
	Sample* this_src_sample=(Sample*)src+start_pos;
	Sample* this_dest_sample=(Sample*)dest;
	for(int pos=start_pos;pos<end_pos;++pos+=stride,++this_dest_sample+=stride,++this_src_sample)
		*this_dest_sample=*this_src_sample;
}

Sample* andromeda::media::resample(void* src,int start_pos,int end_pos,int stride)
{
	Sample* sampled_data=(Sample*)malloc(sizeof(Sample)*ceil((double)(1+start_pos-end_pos)/(1+stride)));
	resample(src,sampled_data,start_pos,end_pos,stride);
	return sampled_data;
}

void andromeda::media::resample(void* src,void* dest,int length,int stride)
{
	resample(src,dest,0,length,stride);
}

Sample* andromeda::media::resample(void* src,int length,int stride)
{
	return resample(src,0,length,stride);
}
