#ifndef ANDROMEDA_GRAPHICS_RENDERABLEMODEL
#define ANDROMEDA_GRAPHICS_RENDERABLEMODEL

#include <vector>
#include "renderable.h"

#include <assimp/postprocess.h>

namespace andromeda
{
namespace graphics
{
//等同于Model，由多个Renderable对象组成的模型
class renderable_model
{
private:
	const char* model_path = nullptr;
	public:
	std::vector<renderable> renderable_list; //渲染对象列表，将从索引0开始依次渲染

	renderable_model() = default;

	inline const char* get_model_path()
	{
		return model_path;
	}

	inline void instance(const char* id = nullptr)
	{
		for(size_t idx = 0; idx < renderable_list.size(); ++idx)
			renderable_list[idx].instance(id);
	}

	inline void buffer_vertex_data()
	{
		for(size_t idx = 0; idx < renderable_list.size(); ++idx)
			renderable_list[idx].buffer_vertex_data();
	}

	inline void load_textures(bool generate_minimap = true, bool release_image = true)
	{
		for(size_t idx = 0; idx < renderable_list.size(); ++idx)
			renderable_list[idx].load_textures(generate_minimap, release_image);
	}

	__attribute__((always_inline)) inline void draw() //依次绘制renderable_list中所有对象，每个对象只绘制一次
	{
		for(size_t idx = 0; idx < renderable_list.size(); ++idx)
			renderable_list[idx].draw();
	}

	/*
	 * 加载3D模型
	 * attrib_order为不区分大小写的字符串，每个字符代表一种属性。V顶点，N法线，T顶点切线Tangents，B顶点负切线Bitangents，C颜色，X纹理坐标，如果不存在该属性，则忽略
	 * 不同属性以','隔开，部分属性名称(C、T)后可选加数字，例如T0表示索引0的纹理坐标，T1表示索引1的纹理坐标，不加数字默认为索引0，加了但不存在该索引则忽略该索引。
	 * texture_types为要加载的纹理贴图类型，详细种类缩写见Texture2D.cpp中的parseTextureType()
	 */
	static renderable_model* load_model(const char* model_path, geometry_strategy geo_strategy, const char* attrib_order = "V,X", const char* texture_types = "Co", int post_process = aiProcess_Triangulate | aiProcess_FlipUVs);
};
}
}

#endif//ANDROMEDA_GRAPHICS_RENDERABLEMODEL
