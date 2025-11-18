#include <andromeda/util/string_utils.h>

#include <string.h>
#include <sstream>

using namespace andromeda::util;

bool andromeda::util::parse_bool(const char* value)
{
	return strcmp(value,"true")?true:false;
}

const char* andromeda::util::str_cpy(const char* str)
{
	size_t len=strlen(str);
	char* cpy=(char*)malloc(sizeof(char)*(len+1));
	*(cpy+len)='\0';
	memcpy(cpy,str,len);
	return (const char*)cpy;
}

const char* andromeda::util::str_arr_join(const char** str_arr,int count)
{
	std::ostringstream oss;
	for(int i=0;i<count;++i)
		oss<<*(str_arr+i);
	return oss.str().c_str();
}

split_strings andromeda::util::split(const char* str,const char* delim)
{
	array_list<const char*> arr(16);
	char* cpy=(char*)str_cpy(str); //尽管返回了许多子字符串，但由于这些字符串紧密位于同一块内存cpy中，释放内存时应只释放第一个子串的指针，即调用free((void*)arr[0])！切勿对每个子串调用free()，否则会出错
	char* splited_str=strtok(cpy,delim); //strtok会改变原字符串，因此要先拷贝
	while(splited_str)
	{
		arr.add((const char*)splited_str);
		splited_str=strtok(0,delim);
	}
	return *(split_strings*)&arr;
}

void andromeda::util::pack_bits_to_bytes(unsigned char* bits,int bits_start_pos,unsigned char* bytes,int bytes_start_pos,long int bits_length)
{
	unsigned char byte;
	int bit_count,byte_index=0,bit_index=0;
	while(bit_index<bits_length)
	{
		bit_count=0;
		byte=0;
		while(bit_count<8&&bit_index<bits_length)
		{
			byte|=(*(bits+bits_start_pos+bit_index)<<(8-bit_count-1));
			++bit_count;
			++bit_index;
		}
		*(bytes+bytes_start_pos+byte_index)=byte;
		++byte_index;
	}
}

const char* andromeda::util::int_to_string_dec(long long int num)
{
	char* str=nullptr;
	int num_len=1;
	int limit=0;
	char num_ch='+';
	if(num<0)
	{
		num_ch='-';
		num=-num;
		++limit;
	}
	register int tmp=num;
	while(tmp>=10)
	{
		tmp/=10;
		++num_len;
	}
	int pos=num_len;
	switch(num_ch)
	{
	case '-':
		str=new char[num_len+2];
		str[0]='-';
		str[num_len+1]='\0';
		break;
	case '+':
		str=new char[num_len+1];
		str[num_len]='\0';
		--pos;
		break;
	}
	for(tmp=num;pos>=limit;tmp/=10,--pos)
		str[pos]='0'+(tmp%10);
	return (const char*)str;
}

const char* andromeda::util::str_join(const char* str1,const char* str2)
{
	register int len=strlen(str1)+strlen(str2)+1;
	char* str=new char[len];
	str[len-1]='\0';
	strcpy(str,str1);
	strcpy(str+strlen(str1),str2);
	return (const char*)str;
}

const char* andromeda::util::str_join(const char* str1,char ch)
{
	register int len=strlen(str1)+2;
	char* str=new char[len];
	str[len-1]='\0';
	str[len-2]=ch;
	strcpy(str,str1);
	return (const char*)str;
}

const char* andromeda::util::str_join(char ch,const char* str1)
{
	register int len=strlen(str1)+2;
	char* str=new char[len];
	str[len-1]='\0';
	str[0]=ch;
	strcpy(str+1,str1);
	return (const char*)str;
}
