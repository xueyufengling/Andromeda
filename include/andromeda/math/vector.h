#ifndef ANDROMEDA_MATH_VECTOR
#define ANDROMEDA_MATH_VECTOR

#include <type_traits>
#include <math.h>
#include <malloc.h>
#include <sstream>

namespace andromeda
{
namespace math
{
template<int Dim, typename T = float>
class vector //列向量，初始化使用Vector vec={x,y,z,w...};
{
public:
	T coord[Dim];
	typedef T type;
	static const int dim = Dim;

	inline operator T*()
	{
		return coord;
	}

	template<typename T2>
	inline operator vector<Dim,T2>()
	{
		vector<Dim, T2> cast_result;
		for(int i = 0; i < Dim; ++i)
			cast_result[i] = (T2)coord[i];
		return cast_result;
	}

	inline T& operator[](int i)
	{
		return coord[i];
	}

	inline operator std::string() //转化为字符串
	{
		return toString();
	}

	inline operator const char*() //转化为C字符串
	{
		return toString().c_str();
	}

	template<typename T2>
	inline vector<Dim, decltype(std::declval<T>()+std::declval<T2>())> operator +(vector<Dim, T2>& v)
	{
		return add(v);
	}

	template<typename T2>
	inline vector<Dim, decltype(std::declval<T>()-std::declval<T2>())> operator -(vector<Dim, T2>& v)
	{
		return sub(v);
	}

	inline vector<Dim, T> operator-()
	{
		vector<Dim, T> minus;
		for(int i = 0; i < Dim; ++i)
			minus[i] = -coord[i];
		return minus;
	}

	template<typename T2>
	vector<Dim, decltype(std::declval<T>()*std::declval<T2>())> operator *(T2 t)
	{
		return mul(t);
	}

	template<typename T2>
	inline auto operator *(vector<Dim, T2> v) -> decltype(std::declval<T>()*std::declval<T2>())
	{
		return dot(v);
	}

	template<typename T2>
	vector<Dim, decltype(std::declval<T>()/std::declval<T2>())> operator /(T2 t)
	{
		return div(t);
	}

	template<typename T2>
	auto dot(vector<Dim, T2> v) -> decltype(std::declval<T>()*std::declval<T2>()) //点乘
	{
		decltype(std::declval<T>()*std::declval<T2>()) dot_result = 0;
		for(int i = 0; i < Dim; ++i)
			dot_result += coord[i] * v.coord[i];
		return dot_result;
	}

	auto len() -> decltype(std::sqrt(std::declval<decltype(std::declval<T>()*std::declval<T>())>())) //矢量长度
	{
		return std::sqrt(dot(*this));
	}

	vector<Dim, decltype(std::declval<T>()/std::declval<decltype(((vector<Dim,T>*)0)->len())>())> unit() //单位矢量
	{
		return div(len());
	}

	template<typename T2>
	vector<Dim, decltype(std::declval<T>()*std::declval<T2>())> mul(T2 t) //数乘
	{
		vector<Dim, decltype(std::declval<T>()*std::declval<T2>())> mul_result;
		for(int i = 0; i < Dim; ++i)
			mul_result[i] = coord[i] * t;
		return mul_result;
	}

	template<typename T2>
	vector<Dim, decltype(std::declval<T>()/std::declval<T2>())> div(T2 t) //除法
	{
		vector<Dim, decltype(std::declval<T>()/std::declval<T2>())> div_result;
		for(int i = 0; i < Dim; ++i)
			div_result[i] = coord[i] / t;
		return div_result;
	}

	template<typename T2>
	vector<Dim, decltype(std::declval<T>()+std::declval<T2>())> add(vector<Dim, T2>& v)
	{
		vector<Dim, decltype(std::declval<T>()+std::declval<T2>())> add_result;
		for(int i = 0; i < Dim; ++i)
			add_result[i] = coord[i] + v.coord[i];
		return add_result;
	}

	template<typename T2>
	vector<Dim, decltype(std::declval<T>()-std::declval<T2>())> sub(vector<Dim, T2>& v)
	{
		vector<Dim, decltype(std::declval<T>()-std::declval<T2>())> sub_result;
		for(int i = 0; i < Dim; ++i)
			sub_result[i] = coord[i] - v.coord[i];
		return sub_result;
	}

	static vector<Dim, T> zero()
	{
		vector<Dim, T> zero_vec;
		memset(&zero_vec.coord, 0, Dim * sizeof(T));
		return zero_vec;
	}

	std::string toString()
	{
		std::ostringstream oss;
		oss << '(' << coord[0];
		for(int i = 1; i < Dim; ++i)
			oss << ',' << coord[i];
		oss << ')';
		return oss.str();
	}
};

template<typename T>
class vector<3, T> //初始化使用Vector vec={x,y,z};特化版有叉乘运算
{
public:
	T coord[3];
	typedef T type;
	static const int dim = 3;

	inline operator T*()
	{
		return (T*)&coord;
	}

	template<typename T2>
	inline operator vector<3,T2>()
	{
		return
		{	(T2)coord[0],(T2)coord[1],(T2)coord[2]};
	}

	inline T& operator[](int i)
	{
		return coord[i];
	}

	inline operator std::string() //转化为字符串
	{
		return toString();
	}

	inline operator const char*() //转化为C字符串
	{
		return toString().c_str();
	}

	template<typename T2>
	inline vector<3, decltype(std::declval<T>()+std::declval<T2>())> operator +(vector<3, T2>& v)
	{
		return add(v);
	}

	template<typename T2>
	inline vector<3, decltype(std::declval<T>()-std::declval<T2>())> operator -(vector<3, T2>& v)
	{
		return sub(v);
	}

	inline vector<3, T> operator-()
	{
		return
		{	-coord[0],-coord[1],-coord[2]};
	}

	template<typename T2>
	vector<3, decltype(std::declval<T>()*std::declval<T2>())> operator *(T2 t)
	{
		return mul(t);
	}

	template<typename T2>
	inline auto operator *(vector<3, T2> v) -> decltype(std::declval<T>()*std::declval<T2>())
	{
		return dot(v);
	}

	template<typename T2>
	vector<3, decltype(std::declval<T>()/std::declval<T2>())> operator /(T2 t)
	{
		return div(t);
	}

	template<typename T2>
	auto dot(vector<3, T2> v) -> decltype(std::declval<T>()*std::declval<T2>()) //点乘
	{
		return coord[0] * v.coord[0] + coord[1] * v.coord[1] + coord[2] * v.coord[2];
	}

	auto len() -> decltype(std::sqrt(std::declval<decltype(std::declval<T>()*std::declval<T>())>())) //矢量长度
	{
		return std::sqrt(dot(*this));
	}

	vector<3, decltype(std::declval<T>()/std::declval<decltype(((vector<3,T>*)0)->len())>())> unit() //单位矢量
	{
		return div(len());
	}

	template<typename T2>
	vector<3, decltype(std::declval<T>()*std::declval<T2>())> mul(T2 t) //数乘
	{
		return
		{	coord[0]*t,coord[1]*t,coord[2]*t};
	}

	template<typename T2>
	vector<3, decltype(std::declval<T>()/std::declval<T2>())> div(T2 t) //除法
	{
		return
		{	coord[0]/t,coord[1]/t,coord[2]/t};
	}

	template<typename T2>
	vector<3, decltype(std::declval<T>()+std::declval<T2>())> add(vector<3, T2>& v)
	{
		return
		{	coord[0]+v.coord[0],coord[1]+v.coord[1],coord[2]+v.coord[2]};
	}

	template<typename T2>
	vector<3, decltype(std::declval<T>()-std::declval<T2>())> sub(vector<3, T2>& v)
	{
		return
		{	coord[0]-v.coord[0],coord[1]-v.coord[1],coord[2]-v.coord[2]};
	}

	template<typename T2>
	vector<3, decltype(std::declval<T>()*std::declval<T2>())> cross(vector<3, T2>& v) //叉乘
	{
		return
		{	coord[1]*v.coord[2]-coord[2]*v.coord[1],coord[2]*v.coord[0]-coord[0]*v.coord[2],coord[0]*v.coord[1]-coord[1]*v.coord[0]};
	}

	static vector<3, T> zero()
	{
		return
		{	0,0,0};
	}

	std::string toString()
	{
		std::ostringstream oss;
		oss << '(' << coord[0] << ',' << coord[1] << ',' << coord[2] << ')';
		return oss.str();
	}
};

using vector2f=vector<2,float>;
using vector3f=vector<3,float>;
using Vector4f=vector<4,float>;
}
}

#endif//ANDROMEDA_MATH_VECTOR