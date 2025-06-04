#include <andromeda/graphics/renderable_model.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <andromeda/util/array_list.h>
#include <andromeda/util/string_utils.h>
#include <andromeda/io/files.h>
#include <andromeda/util/log.h>

using namespace andromeda::graphics;
using namespace andromeda::util;
using namespace andromeda::io;

namespace andromeda {
	namespace graphics {
		static ArrayList<Texture2D*>textures_loaded(16); //已经加载过的纹理将不会再次加载
	}
}

static Renderable* loadTextures(const char* model_path,Renderable* renderable,aiMaterial* material,aiTextureType type)
{
	ArrayList<Texture2D>& textures=renderable->textures;
	unsigned int texture_count=material->GetTextureCount(type);
	if(!texture_count) //没有材质则直接返回
		return renderable;
	for(unsigned int idx=0;idx<texture_count;++idx)
	{
		aiString str; //获取材质名称
		material->GetTexture(type,idx,&str);
		const char* texture_path=str_join(getFileDir(model_path).c_str(),PATH_SEPARATOR,str.C_Str());
		bool loaded=false; //是否已经加载过
		for(size_t loaded_texture_idx=0;loaded_texture_idx<textures_loaded.length();++loaded_texture_idx)
		{
			Texture2D* loaded_texture=textures_loaded[loaded_texture_idx];
			if(std::strcmp(loaded_texture->getTexturePath(),texture_path)==0)
			{
				loaded=true;
				textures.add(*loaded_texture);
				break;
			}
		}
		if(!loaded)
		{
			Texture2D& texture=textures.add_placement_new(texture_path,(TextureType)type);
			textures_loaded.add(&texture);
		}
	}
	return renderable;
}

static Renderable loadRenderable(const char* model_path,aiMesh* mesh,const aiScene* scene,GeoStrategy geo_strategy,const char* attrib_order,const char* texture_types)
{
	Renderable renderable;
	ArrayList<float>& vertex_data=renderable.vertex_data;
	SplitStrings attribs=split(attrib_order,",");
	size_t attrib_num=attribs.length();
	for(unsigned int vertex_idx=0;vertex_idx<mesh->mNumVertices;++vertex_idx) //加载顶点数据
	{
		for(size_t attrib_idx=0;attrib_idx<attrib_num;++attrib_idx)
		{
			const char* attrib=attribs[attrib_idx];
			switch(attrib[0])
			//判断属性首字符
			{
			case 'V':
			case 'v':
				vertex_data.add(mesh->mVertices[vertex_idx].x,mesh->mVertices[vertex_idx].y,mesh->mVertices[vertex_idx].z);
				break;
			case 'N':
			case 'n':
				if(mesh->mNormals)
					vertex_data.add(mesh->mNormals[vertex_idx].x,mesh->mNormals[vertex_idx].y,mesh->mNormals[vertex_idx].z);
				break;
			case 'T':
			case 't':
				if(mesh->mTangents)
					vertex_data.add(mesh->mTangents[vertex_idx].x,mesh->mTangents[vertex_idx].y,mesh->mTangents[vertex_idx].z);
				break;
			case 'B':
			case 'b':
				if(mesh->mBitangents)
					vertex_data.add(mesh->mBitangents[vertex_idx].x,mesh->mBitangents[vertex_idx].y,mesh->mBitangents[vertex_idx].z);
				break;
			case 'C':
			case 'c':
			{
				int color_idx=0; //默认为0
				if(attrib[1]!='\0') //如果后面有数字则解析成对应的索引
					color_idx=std::stoi(attrib+1);
				aiColor4D* color=mesh->mColors[color_idx];
				if(color) //判断是否有颜色坐标
				{
					vertex_data.add(color[vertex_idx].r,color[vertex_idx].g,color[vertex_idx].b,color[vertex_idx].a);
				}
			}
				break;
			case 'X':
			case 'x':
			{
				int texcoord_idx=0; //默认为0
				if(attrib[1]!='\0') //如果后面有数字则解析成对应的索引
					texcoord_idx=std::stoi(attrib+1);
				aiVector3D* texcoord=mesh->mTextureCoords[texcoord_idx];
				if(texcoord) //判断是否有纹理坐标
				{
					vertex_data.add(texcoord[vertex_idx].x,texcoord[vertex_idx].y);
				}
			}
				break;
			}
		}
	}
	if(mesh->mMaterialIndex>=0&&texture_types!=nullptr) //如果有材质则加载材质
	{
		SplitStrings textures_types_str=split(texture_types,",");
		aiMaterial* material=scene->mMaterials[mesh->mMaterialIndex];
		for(size_t type_idx=0;type_idx<textures_types_str.length();++type_idx)
			loadTextures(model_path,&renderable,material,(aiTextureType)parseTextureType(textures_types_str[type_idx]));
	}
	renderable.geo_strategy=geo_strategy;
	return renderable;
}

static RenderableModel* laodNodeRenderable(const char* model_path,RenderableModel* model,aiNode* node,const aiScene* scene,GeoStrategy geo_strategy,const char* attrib_order,const char* texture_types)
{
	for(unsigned int idx=0;idx<node->mNumMeshes;++idx) //加载当前节点的全部Mesh
	{
		aiMesh* mesh=scene->mMeshes[node->mMeshes[idx]];
		model->renderable_list.push_back(loadRenderable(model_path,mesh,scene,geo_strategy,attrib_order,texture_types));
	}
	for(unsigned int children_idx=0;children_idx<node->mNumChildren;++children_idx) //遍历全部子节点
	{
		laodNodeRenderable(model_path,model,node->mChildren[children_idx],scene,geo_strategy,attrib_order,texture_types); //加载子节点的全部Mesh
	}
	return model;
}

RenderableModel* RenderableModel::load3DModel(const char* model_path,GeoStrategy geo_strategy,const char* attrib_order,const char* texture_types,int post_process)
{
	static Assimp::Importer model_importer;
	textures_loaded.clear();
	const aiScene* scene=model_importer.ReadFile(model_path,post_process);
	if(!scene||(scene->mFlags&AI_SCENE_FLAGS_INCOMPLETE)||!scene->mRootNode) //加载失败
	{
		LOG_ERROR("Loading model failed: ",model_importer.GetErrorString());
		return nullptr;
	}
	RenderableModel* model=new RenderableModel();
	laodNodeRenderable(model_path,model,scene->mRootNode,scene,geo_strategy,attrib_order,texture_types);
	model->model_path=model_path;
	return model;
}
