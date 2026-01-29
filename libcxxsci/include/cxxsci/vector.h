#ifndef _CXXSCI_VECTOR
#define _CXXSCI_VECTOR

#include <tplmp/tplmp.h>
#include <math.h>
#include <malloc.h>

#include <sstream>

namespace cxxsci
{
template<size_t _Dim, typename _T = float>
class vector //列向量，初始化使用vector vec={x,y,z,w...};
{
public:
	_T coord[_Dim];
	typedef _T type;
	static const size_t dim = _Dim;

	inline operator _T*()
	{
		return coord;
	}

	template<typename _T2>
	inline operator vector<_Dim,_T2>()
	{
		vector<_Dim, _T2> cast_result;
		for(int i = 0; i < _Dim; ++i)
			cast_result[i] = (_T2)coord[i];
		return cast_result;
	}

	inline _T& operator[](int i)
	{
		return coord[i];
	}

	template<typename _T2>
	inline vector<_Dim, decltype(tplmp::decl<_T>::val()+tplmp::decl<_T2>::val())> operator+(vector<_Dim, _T2>& v)
	{
		return add(v);
	}

	template<typename _T2>
	inline vector<_Dim, decltype(tplmp::decl<_T>::val()-tplmp::decl<_T2>::val())> operator-(vector<_Dim, _T2>& v)
	{
		return sub(v);
	}

	inline vector<_Dim, _T> operator-()
	{
		vector<_Dim, _T> minus;
		for(int i = 0; i < _Dim; ++i)
			minus[i] = -coord[i];
		return minus;
	}

	template<typename _T2>
	vector<_Dim, decltype(tplmp::decl<_T>::val()*tplmp::decl<_T2>::val())> operator*(_T2 t)
	{
		return mul(t);
	}

	template<typename _T2>
	inline auto operator*(vector<_Dim, _T2> v) -> decltype(tplmp::decl<_T>::val()*tplmp::decl<_T2>::val())
	{
		return dot(v);
	}

	template<typename _T2>
	vector<_Dim, decltype(tplmp::decl<_T>::val()/tplmp::decl<_T2>::val())> operator/(_T2 t)
	{
		return div(t);
	}

	template<typename _T2>
	auto dot(vector<_Dim, _T2> v) -> decltype(tplmp::decl<_T>::val()*tplmp::decl<_T2>::val()) //点乘
	{
		decltype(tplmp::decl<_T>::val()*tplmp::decl<_T2>::val()) dot_result = 0;
		for(size_t i = 0; i < _Dim; ++i)
			dot_result += coord[i] * v.coord[i];
		return dot_result;
	}

	auto len() -> decltype(std::sqrt(std::declval<decltype(tplmp::decl<_T>::val()*tplmp::decl<_T>::val())>())) //矢量长度
	{
		return std::sqrt(dot(*this));
	}

	vector<_Dim, decltype(tplmp::decl<_T>::val()/std::declval<decltype(((vector<_Dim,_T>*)0)->len())>())> unit() //单位矢量
	{
		return div(len());
	}

	template<typename _T2>
	vector<_Dim, decltype(tplmp::decl<_T>::val()*tplmp::decl<_T2>::val())> mul(_T2 t) //数乘
	{
		vector<_Dim, decltype(tplmp::decl<_T>::val()*tplmp::decl<_T2>::val())> mul_result;
		for(size_t i = 0; i < _Dim; ++i)
			mul_result[i] = coord[i] * t;
		return mul_result;
	}

	template<typename _T2>
	vector<_Dim, decltype(tplmp::decl<_T>::val()/tplmp::decl<_T2>::val())> div(_T2 t) //除法
	{
		vector<_Dim, decltype(tplmp::decl<_T>::val()/tplmp::decl<_T2>::val())> div_result;
		for(int i = 0; i < _Dim; ++i)
			div_result[i] = coord[i] / t;
		return div_result;
	}

	template<typename _T2>
	vector<_Dim, decltype(tplmp::decl<_T>::val()+tplmp::decl<_T2>::val())> add(vector<_Dim, _T2>& v)
	{
		vector<_Dim, decltype(tplmp::decl<_T>::val()+tplmp::decl<_T2>::val())> add_result;
		for(int i = 0; i < _Dim; ++i)
			add_result[i] = coord[i] + v.coord[i];
		return add_result;
	}

	template<typename _T2>
	vector<_Dim, decltype(tplmp::decl<_T>::val()-tplmp::decl<_T2>::val())> sub(vector<_Dim, _T2>& v)
	{
		vector<_Dim, decltype(tplmp::decl<_T>::val()-tplmp::decl<_T2>::val())> sub_result;
		for(int i = 0; i < _Dim; ++i)
			sub_result[i] = coord[i] - v.coord[i];
		return sub_result;
	}

	static vector<_Dim, _T> zero()
	{
		vector<_Dim, _T> zero_vec;
		memset(&zero_vec.coord, 0, _Dim * sizeof(_T));
		return zero_vec;
	}

	operator std::string()
	{
		std::ostringstream oss;
		oss << '(';
		for(size_t i = 0; i < _Dim; ++i)
		{
			oss << coord[i];
			if(i < _Dim - 1)
				oss << ',';
		}
		oss << ')';
		return oss.str();
	}
};

template<typename _T>
class vector3: public vector<3, _T>
{
public:
	using vector<3, _T>::coord;

	vector3(_T x = 0, _T y = 0, _T z = 0)
	{
		coord[0] = x;
		coord[1] = y;
		coord[2] = z;
	}

	inline operator _T*()
	{
		return (_T*)&coord;
	}

	template<typename _T2>
	inline operator vector3<_T2>()
	{
		return
		{	(_T2)coord[0],(_T2)coord[1],(_T2)coord[2]};
	}

	template<typename _T2>
	inline operator vector<3, _T2>()
	{
		return
		{	(_T2)coord[0],(_T2)coord[1],(_T2)coord[2]};
	}

	template<typename _T2>
	inline vector3<_T>& operator=(const vector<3, _T2>& vec)
	{
		coord[0] = (_T)coord[0];
		coord[1] = (_T)coord[1];
		coord[2] = (_T)coord[2];
		return *this;
	}

	inline _T& operator[](int i)
	{
		return coord[i];
	}

	template<typename _T2>
	inline vector3<decltype(tplmp::decl<_T>::val()+tplmp::decl<_T2>::val())> operator+(vector3<_T2>& v)
	{
		return add(v);
	}

	template<typename _T2>
	inline vector3<decltype(tplmp::decl<_T>::val()-tplmp::decl<_T2>::val())> operator-(vector3<_T2>& v)
	{
		return sub(v);
	}

	inline vector3<_T> operator-()
	{
		return
		{	-coord[0],-coord[1],-coord[2]};
	}

	template<typename _T2>
	vector3<decltype(tplmp::decl<_T>::val()*tplmp::decl<_T2>::val())> operator*(_T2 t)
	{
		return mul(t);
	}

	template<typename _T2>
	inline auto operator*(vector3<_T2> v) -> decltype(tplmp::decl<_T>::val()*tplmp::decl<_T2>::val())
	{
		return dot(v);
	}

	template<typename _T2>
	vector3<decltype(tplmp::decl<_T>::val()/tplmp::decl<_T2>::val())> operator/(_T2 t)
	{
		return div(t);
	}

	template<typename _T2>
	auto dot(vector3<_T2> v) -> decltype(tplmp::decl<_T>::val()*tplmp::decl<_T2>::val()) //点乘
	{
		return coord[0] * v.coord[0] + coord[1] * v.coord[1] + coord[2] * v.coord[2];
	}

	auto len() -> decltype(std::sqrt(std::declval<decltype(tplmp::decl<_T>::val()*tplmp::decl<_T>::val())>())) //矢量长度
	{
		return std::sqrt(dot(*this));
	}

	vector3<decltype(tplmp::decl<_T>::val()/std::declval<decltype(((vector3<_T>*)0)->len())>())> unit() //单位矢量
	{
		return div(len());
	}

	template<typename _T2>
	vector3<decltype(tplmp::decl<_T>::val()*tplmp::decl<_T2>::val())> mul(_T2 t) //数乘
	{
		return
		{	coord[0]*t,coord[1]*t,coord[2]*t};
	}

	template<typename _T2>
	vector3<decltype(tplmp::decl<_T>::val()/tplmp::decl<_T2>::val())> div(_T2 t) //除法
	{
		return
		{	coord[0]/t,coord[1]/t,coord[2]/t};
	}

	template<typename _T2>
	vector3<decltype(tplmp::decl<_T>::val()+tplmp::decl<_T2>::val())> add(vector3<_T2>& v)
	{
		return
		{	coord[0]+v.coord[0],coord[1]+v.coord[1],coord[2]+v.coord[2]};
	}

	template<typename _T2>
	vector3<decltype(tplmp::decl<_T>::val()-tplmp::decl<_T2>::val())> sub(vector3<_T2>& v)
	{
		return
		{	coord[0]-v.coord[0],coord[1]-v.coord[1],coord[2]-v.coord[2]};
	}

	template<typename _T2>
	vector3<decltype(tplmp::decl<_T>::val()*tplmp::decl<_T2>::val())> cross(vector3<_T2>& v) //叉乘
	{
		return
		{	coord[1]*v.coord[2]-coord[2]*v.coord[1],coord[2]*v.coord[0]-coord[0]*v.coord[2],coord[0]*v.coord[1]-coord[1]*v.coord[0]};
	}

	static vector3<_T> zero()
	{
		return
		{	0,0,0};
	}
};

using vector2f=vector<2, float>;
using vector3f=vector3<float>;
using Vector4f=vector<4, float>;

}

#endif//_CXXSCI_VECTOR