#include <andromeda/graphics/vertex_attribute.h>

#include<string>
#include <andromeda/util/string_utils.h>

using namespace andromeda::graphics;
using namespace andromeda::util;

VertexAttributeInfo::VertexAttributeInfo(const char* info)
{
	SplitStrings attrib_str=split(info,":");
	SplitStrings attrib_name_index=split(attrib_str[0],".");
	name=str_cpy(attrib_name_index[0]); //属性名称
	if(attrib_name_index.length()>1)
		index=std::stoi(attrib_str[1]);
	const char* attrib_data_info_str=attrib_str[1];
	register int pos=0;
	if(attrib_data_info_str[0]=='n') //需要规范化
	{
		pos=1;
		normalized=true;
	}
	num=char_to_int_0_to_9(attrib_data_info_str[pos]);
	type=getGLType(attrib_data_info_str+(++pos));
	type_size=getGLTypeSize(type);
	int type_str_len=0;
	switch(type)//可选divisor
	{
	case GL_BYTE:
	case GL_SHORT:
	case GL_INT:
	case GL_FLOAT:
	case GL_DOUBLE:
		type_str_len=1;
		break;
	case GL_UNSIGNED_BYTE:
	case GL_UNSIGNED_SHORT:
	case GL_UNSIGNED_INT:
#ifdef GL_2_BYTES
	case GL_2_BYTES:
#endif
#ifdef GL_3_BYTES
	case GL_3_BYTES:
#endif
#ifdef GL_4_BYTES
	case GL_4_BYTES:
#endif
		type_str_len=2;
		break;
	}
	if(attrib_data_info_str[pos+=type_str_len]=='/')//有divisor
		divisor=std::stoi(attrib_data_info_str+(++pos));
}

void VertexAttribute::setAttribute(const char* attrib_str)
{
	SplitStrings attrib_str_arr=split(attrib_str,","); //析构时自动释放内存
	size_t offset=0;
	for(int i=0;i<attrib_str_arr.length();++i)
	{
		VertexAttributeInfo& attrib_info=attribs.add_placement_new(attrib_str_arr[i]);
		if(attrib_info.index==-1)
			attrib_info.index=i;
		attrib_info.offset=offset;
		offset+=attrib_info.num*attrib_info.type_size; //下一个顶点（如果存在）的初始位置
	}
	vertex_size=offset;
}

VertexAttributeInfo VertexAttribute::getVertexAttributeInfo(const char* attrib_name)
{
	for(int i=0;i<attribs.length();++i)
		if(strcmp(attribs[i].name,attrib_name))
			return attribs[i];
	return VertexAttributeInfo(); //无效查询
}
