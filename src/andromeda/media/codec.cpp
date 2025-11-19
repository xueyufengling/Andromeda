#include <andromeda/media/codec.h>

using namespace andromeda::media;

bool codec::initialize(AVCodecID id)
{
	encoded_frame_count = 0;
	is_buf_alloc = false;
	avcodec = avcodec_find_encoder(id);
	if(!avcodec)
	{
		LogError("codec find encoder ", id, " failed.");
		return false;
	}
	codec_context = avcodec_alloc_context3(avcodec);
	codec_context->codec_id = id;
	codec_context->pix_fmt = AV_PIX_FMT_YUV444P;
	codec_context->width = 0;
	codec_context->height = 0;
	if(!codec_context)
	{
		std::cout << "Allocate encoder context failed." << std::endl;
		return false;
	}
	return true;
}
