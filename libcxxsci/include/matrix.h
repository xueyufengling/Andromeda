#ifndef ANDROMEDA_MA_TH_MA_TRIX
#define ANDROMEDA_MA_TH_MA_TRIX

#include "../traits/types.h"
#include <math.h>
#include <malloc.h>
#include "vector.h"

#include "../internal/signals.h"
#include <sstream>

namespace andromeda
{
namespace math
{
template<size_t _Row, size_t _Column, typename _T = float>
class matrix
{
public:
	vector<_Column, _T> elem[_Row]; //索引行、列。储存Row个Column维行向量
	typedef _T type;
	static const size_t row_num = _Row;
	static const size_t column_num = _Column;

	inline operator _T*()
	{
		return (_T*)elem;
	}

	inline operator vector<_Column, _T>*()
	{
		return elem;
	}

	template<typename _T2>
	inline operator matrix<_Row, _Column, _T2>()
	{
		matrix<_Row, _Column, _T2> cast_result;
		for(size_t i = 0; i < _Row; ++i)
			cast_result[i] = elem[i];
		return cast_result;
	}

	//提取行向量
	inline vector<_Column, _T>& operator[](size_t i)
	{
		return elem[i];
	}

	template<typename _T2>
	inline matrix<_Row, _Column, decltype(decl<_T>::val()+decl<_T2>::val())> operator+(matrix<_Row, _Column, _T2>& m)
	{
		return add(m);
	}

	template<typename _T2>
	inline matrix<_Row, _Column, decltype(decl<_T>::val()-decl<_T2>::val())> operator-(matrix<_Row, _Column, _T2>& m)
	{
		return sub(m);
	}

	template<typename _T2>
	inline matrix<_Row, _Column, decltype(decl<_T>::val()*decl<_T2>::val())> operator*(_T2 t)
	{
		return mul(t);
	}

	template<size_t _Row2, typename _T2>
	inline matrix<_Row, _Row2, decltype(decl<_T>::val()*decl<_T2>::val())> operator*(matrix<_Column, _Row2, _T2>& m)
	{
		return mul(m);
	}

	template<typename _T2>
	inline vector<_Row, decltype(decl<_T>::val()*decl<_T2>::val())> operator*(vector<_Column, _T2>& v)
	{
		return mul(v);
	}

	template<typename _T2>
	inline matrix<_Row, _Column, decltype(decl<_T>::val()/decl<_T2>::val())> operator/(_T2 t)
	{
		return div(t);
	}

	//右值处理
	template<typename _T2>
	inline matrix<_Row, _Column, decltype(decl<_T>::val()+decl<_T2>::val())> operator+(matrix<_Row, _Column, _T2>&& m)
	{
		return add(cast<matrix<_Row, _Column, _T2> >(m));
	}

	template<typename _T2>
	inline matrix<_Row, _Column, decltype(decl<_T>::val()-decl<_T2>::val())> operator-(matrix<_Row, _Column, _T2>&& m)
	{
		return sub(cast<matrix<_Row, _Column, _T2> >(m));
	}

	template<size_t _Row2, typename _T2>
	inline matrix<_Row, _Row2, decltype(decl<_T>::val()*decl<_T2>::val())> operator*(matrix<_Column, _Row2, _T2>&& m)
	{
		return mul(cast<matrix<_Row, _Column, _T2> >(m));
	}

	template<typename _T2>
	inline vector<_Row, decltype(decl<_T>::val()*decl<_T2>::val())> operator*(vector<_Column, _T2>&& v)
	{
		return mul(cast<vector<_Column, _T2> >(v));
	}

	//提取列向量
	vector<_Row, _T> column(int column)
	{
		vector<_Row, _T> row_vec;
		for(size_t i = 0; i < _Row; ++i)
			row_vec[i] = elem[i][column];
		return row_vec;
	}

	//提取行向量
	inline vector<_Column, _T> row(int row)
	{
		return elem[row];
	}

	matrix<_Column, _Row, _T> transpose() //转置
	{
		matrix<_Column, _Row, _T> transpose_result;
		for(size_t i = 0; i < _Column; ++i)
			for(size_t j = 0; j < _Row; ++j)
				transpose_result[i][j] = elem[j][i];
		return transpose_result;
	}

	template<typename _T2>
	matrix<_Row, _Column, decltype(decl<_T>::val()*decl<_T2>::val())> dot(matrix<_Row, _Column, _T2>& m) //按位相乘
	{
		matrix<_Row, _Column, decltype(decl<_T>::val()*decl<_T2>::val())> dot_result;
		for(size_t i = 0; i < _Row; ++i)
			for(size_t j = 0; j < _Column; ++j)
				dot_result[i][j] = elem[i][j] * m.elem[i][j];
		return dot_result;
	}

	template<typename _T2>
	matrix<_Row, _Column, decltype(decl<_T>::val()*decl<_T2>::val())> mul(_T2 t) //数乘
	{
		matrix<_Row, _Column, decltype(decl<_T>::val()*decl<_T2>::val())> mul_result;
		for(size_t i = 0; i < _Row; ++i)
			mul_result[i] = elem[i] * t;
		return mul_result;
	}

	template<size_t _Row2, typename _T2>
	matrix<_Row, _Row2, decltype(decl<_T>::val()*decl<_T2>::val())> mul(matrix<_Column, _Row2, _T2>& m)
	{
		matrix<_Row, _Row2, decltype(decl<_T>::val()*decl<_T2>::val())> mul_result;
		for(size_t i = 0; i < _Row; ++i)
		{
			vector<_Column, _T> row_vec = row(i);
			for(size_t j = 0; j < _Row2; ++j)
				mul_result[i][j] = row_vec.dot(m.column(j));
		}
		return mul_result;
	}

	template<typename _T2>
	vector<_Row, decltype(decl<_T>::val()*decl<_T2>::val())> mul(vector<_Column, _T2>& v)
	{
		vector<_Row, decltype(decl<_T>::val()*decl<_T2>::val())> mul_result;
		for(size_t i = 0; i < _Row; ++i)
		{
			vector<_Column, _T> row_vec = row(i);
			mul_result[i] = row_vec.dot(v);
		}
		return mul_result;
	}

	template<typename _T2>
	matrix<_Row, _Column, decltype(decl<_T>::val()/decl<_T2>::val())> div(_T2 t) //除法
	{
		matrix<_Row, _Column, decltype(decl<_T>::val()/decl<_T2>::val())> div_result;
		for(size_t i = 0; i < _Row; ++i)
			div_result[i] = elem[i] / t;
		return div_result;
	}

	template<typename _T2>
	matrix<_Row, _Column, decltype(decl<_T>::val()+decl<_T2>::val())> add(matrix<_Row, _Column, _T2>& m)
	{
		matrix<_Row, _Column, decltype(decl<_T>::val()+decl<_T2>::val())> add_result;
		for(size_t i = 0; i < _Row; ++i)
			add_result[i] = elem[i] + m.elem[i];
		return add_result;
	}

	template<typename _T2>
	matrix<_Row, _Column, decltype(decl<_T>::val()-decl<_T2>::val())> sub(matrix<_Row, _Column, _T2>& m)
	{
		matrix<_Row, _Column, decltype(decl<_T>::val()-decl<_T2>::val())> sub_result;
		for(size_t i = 0; i < _Row; ++i)
			sub_result[i] = elem[i] - m.elem[i];
		return sub_result;
	}

	static matrix<_Row, _Column, _T> zero()
	{
		matrix<_Row, _Column, _T> zero_mat;
		memset(&zero_mat.elem, 0, _Row * _Column * sizeof(_T));
		return zero_mat;
	}

	operator std::string()
	{
		std::ostringstream oss;
		for(size_t i = 0; i < _Row; ++i)
		{
			oss << elem[i][0];
			for(size_t j = 1; j < _Column; ++j)
				oss << ' ' << elem[i][j];
			oss << '\n';
		}
		return oss.str();
	}
};

//方阵特化
template<size_t _Rank, typename _T>
class square_matrix: public matrix<_Rank, _Rank, _T>
{
public:
	using matrix<_Rank, _Rank, _T>::elem; //索引行、列。储存_Rank个_Rank维行向量
	using matrix<_Rank, _Rank, _T>::type;
	using matrix<_Rank, _Rank, _T>::row;
	using matrix<_Rank, _Rank, _T>::column;
	using matrix<_Rank, _Rank, _T>::operator+;
	using matrix<_Rank, _Rank, _T>::operator-;
	using matrix<_Rank, _Rank, _T>::operator*;
	using matrix<_Rank, _Rank, _T>::operator/;

	template<typename _T2>
	inline square_matrix<_Rank, decltype(decl<_T>::val()+decl<_T2>::val())> operator+(square_matrix<_Rank, _T2>& m)
	{
		return add(m);
	}

	template<typename _T2>
	inline square_matrix<_Rank, decltype(decl<_T>::val()-decl<_T2>::val())> operator-(square_matrix<_Rank, _T2>& m)
	{
		return sub(m);
	}

	template<typename _T2>
	inline square_matrix<_Rank, decltype(decl<_T>::val()*decl<_T2>::val())> operator*(square_matrix<_Rank, _T2>& m)
	{
		return mul(m);
	}

	template<typename _T2>
	inline vector<_Rank, decltype(decl<_T>::val()*decl<_T2>::val())> operator*(vector<_Rank, _T2>& v)
	{
		return mul(v);
	}

	//右值处理
	template<typename _T2>
	inline square_matrix<_Rank, decltype(decl<_T>::val()+decl<_T2>::val())> operator+(square_matrix<_Rank, _T2>&& m)
	{
		return add(cast<square_matrix<_Rank, _T2> >(m));
	}

	template<typename _T2>
	inline square_matrix<_Rank, decltype(decl<_T>::val()-decl<_T2>::val())> operator-(square_matrix<_Rank, _T2>&& m)
	{
		return sub(cast<square_matrix<_Rank, _T2> >(m));
	}

	template<typename _T2>
	inline square_matrix<_Rank, decltype(decl<_T>::val()*decl<_T2>::val())> operator*(square_matrix<_Rank, _T2>&& m)
	{
		return mul(cast<square_matrix<_Rank, _T2> >(m));
	}

	template<typename _T2>
	inline vector<_Rank, decltype(decl<_T>::val()*decl<_T2>::val())> operator*(vector<_Rank, _T2>&& v)
	{
		return mul(cast<vector<_Rank, _T2> >(v));
	}

	//特化类独有
	inline constexpr size_t rank()
	{
		return _Rank;
	}

	auto det() -> decltype(decl<_T>::val()*decl<_T>::val()); //行列式懒得写了高阶的用不上

	auto trace() -> decltype(decl<_T>::val()+decl<_T>::val()) //迹
	{
		decltype(decl<_T>::val()+decl<_T>::val()) tr = 0;
		for(size_t i = 0; i < _Rank; ++i)
			tr += elem[i][i];
		return tr;
	}

	static square_matrix<_Rank, _T> identity()
	{
		square_matrix<_Rank, _T> id = cast<square_matrix<_Rank, _T> >(matrix<_Rank, _Rank, _T>::zero());
		for(size_t i = 0; i < _Rank; ++i)
			id[i][i] = 1;
		return id;
	}
};

//3x3方阵特化
template<typename _T>
class matrix3x3: public square_matrix<3, _T>
{
public:
	using matrix<3, 3, _T>::elem;
	using matrix<3, 3, _T>::type;
	using matrix<3, 3, _T>::row;
	using matrix<3, 3, _T>::column;
	using matrix<3, 3, _T>::operator+;
	using matrix<3, 3, _T>::operator-;
	using matrix<3, 3, _T>::operator*;
	using matrix<3, 3, _T>::operator/;

	template<typename _T2>
	inline matrix3x3<decltype(decl<_T>::val()+decl<_T2>::val())> operator+(matrix3x3<_T2>& m)
	{
		return add(m);
	}

	template<typename _T2>
	inline matrix3x3<decltype(decl<_T>::val()-decl<_T2>::val())> operator-(matrix3x3<_T2>& m)
	{
		return sub(m);
	}

	template<typename _T2>
	inline matrix3x3<decltype(decl<_T>::val()*decl<_T2>::val())> operator*(matrix3x3<_T2>& m)
	{
		return mul(m);
	}

	template<typename _T2>
	inline vector<3, decltype(decl<_T>::val()*decl<_T2>::val())> operator*(vector<3, _T2>& v)
	{
		return mul(v);
	}

	//右值处理
	template<typename _T2>
	inline matrix3x3<decltype(decl<_T>::val()+decl<_T2>::val())> operator+(matrix3x3<_T2>&& m)
	{
		return add(cast<matrix3x3<_T2> >(m));
	}

	template<typename _T2>
	inline matrix3x3<decltype(decl<_T>::val()-decl<_T2>::val())> operator-(matrix3x3<_T2>&& m)
	{
		return sub(cast<matrix3x3<_T2> >(m));
	}

	template<typename _T2>
	inline matrix3x3<decltype(decl<_T>::val()*decl<_T2>::val())> operator*(matrix3x3<_T2>&& m)
	{
		return mul(cast<matrix3x3<_T2> >(m));
	}

	template<typename _T2>
	inline vector<3, decltype(decl<_T>::val()*decl<_T2>::val())> operator*(vector<3, _T2>&& v)
	{
		return mul(cast<vector<3, _T2> >(v));
	}

	template<typename _T2>
	inline operator matrix<3, 3, _T2>()
	{
		_T2 m_elem[] =
				{
						(_T2)elem[0][0], (_T2)elem[0][1], (_T2)elem[0][2],
						(_T2)elem[1][0], (_T2)elem[1][1], (_T2)elem[1][2],
						(_T2)elem[2][0], (_T2)elem[2][1], (_T2)elem[2][2]
				};
		return cast<matrix<3, 3, _T2> >(m_elem);
	}

	inline vector<3, _T> column(int column)
	{
		_T v_elem[] =
				{
						elem[0][column], elem[1][column], elem[2][column]
				};
		return cast<vector<3, _T> >(v_elem);
	}

	inline vector<3, _T> extractRow(int row)
	{
		return elem[row];
	}

	inline matrix3x3<_T> transpose() //转置
	{
		_T m_elem[] =
				{
						elem[0][0], elem[1][0], elem[2][0],
						elem[0][1], elem[1][1], elem[2][1],
						elem[0][2], elem[1][2], elem[2][2]
				};
		return cast<matrix3x3<_T> >(m_elem);
	}

	template<typename _T2>
	inline matrix3x3<decltype(decl<_T>::val()*decl<_T2>::val())> dot(matrix<3, 3, _T2>& m) //按位相乘
	{
		typedef decltype(decl<_T>::val()*decl<_T2>::val()) _R;
		_R m_elem[] =
				{
						elem[0][0] * m.elem[0][0], elem[0][1] * m.elem[0][1], elem[0][2] * m.elem[0][2],
						elem[1][0] * m.elem[1][0], elem[1][1] * m.elem[1][1], elem[1][2] * m.elem[1][2],
						elem[2][0] * m.elem[2][0], elem[2][1] * m.elem[2][1], elem[2][2] * m.elem[2][2]
				};
		return cast<matrix3x3<_R> >(m_elem);
	}

	template<typename _T2>
	inline matrix3x3<decltype(decl<_T>::val()*decl<_T2>::val())> mul(_T2 t) //数乘
	{
		typedef decltype(decl<_T>::val()*decl<_T2>::val()) _R;
		_R m_elem[] =
				{
						elem[0][0] * t, elem[0][1] * t, elem[0][2] * t,
						elem[1][0] * t, elem[1][1] * t, elem[1][2] * t,
						elem[2][0] * t, elem[2][1] * t, elem[2][2] * t
				};
		return cast<matrix3x3<_R> >(m_elem);
	}

	template<typename _T2>
	inline matrix3x3<decltype(decl<_T>::val()*decl<_T2>::val())> mul(matrix<3, 3, _T2>& m)
	{
		typedef decltype(decl<_T>::val()*decl<_T2>::val()) _R;
		_R m_elem[] =
				{
						elem[0][0] * m.elem[0][0] + elem[0][1] * m.elem[1][0] + elem[0][2] * m.elem[2][0], elem[0][0] * m.elem[0][1] + elem[0][1] * m.elem[1][1] + elem[0][2] * m.elem[2][1], elem[0][0] * m.elem[0][2] + elem[0][1] * m.elem[1][2] + elem[0][2] * m.elem[2][2],
						elem[1][0] * m.elem[0][0] + elem[1][1] * m.elem[1][0] + elem[1][2] * m.elem[2][0], elem[1][0] * m.elem[0][1] + elem[1][1] * m.elem[1][1] + elem[1][2] * m.elem[2][1], elem[1][0] * m.elem[0][2] + elem[1][1] * m.elem[1][2] + elem[1][2] * m.elem[2][2],
						elem[2][0] * m.elem[0][0] + elem[2][1] * m.elem[1][0] + elem[2][2] * m.elem[2][0], elem[2][0] * m.elem[0][1] + elem[2][1] * m.elem[1][1] + elem[2][2] * m.elem[2][1], elem[2][0] * m.elem[0][2] + elem[2][1] * m.elem[1][2] + elem[2][2] * m.elem[2][2]
				};
		return cast<matrix3x3<_R> >(m_elem);
	}

	template<typename _T2>
	inline vector<3, decltype(decl<_T>::val()*decl<_T2>::val())> mul(vector<3, _T2>& v)
	{
		typedef decltype(decl<_T>::val()*decl<_T2>::val()) _R;
		_R m_elem[] =
				{
						elem[0][0] * v[0] + elem[0][1] * v[1] + elem[0][2] * v[2],
						elem[1][0] * v[0] + elem[1][1] * v[1] + elem[1][2] * v[2],
						elem[2][0] * v[0] + elem[2][1] * v[1] + elem[2][2] * v[2]
				};
		return cast<vector<3, _R> >(m_elem);
	}

	template<typename _T2>
	inline matrix3x3<decltype(decl<_T>::val()/decl<_T2>::val())> div(_T2 t) //除法
	{
		typedef decltype(decl<_T>::val()*decl<_T2>::val()) _R;
		_R m_elem[] =
				{
						elem[0][0] / t, elem[0][1] / t, elem[0][2] / t,
						elem[1][0] / t, elem[1][1] / t, elem[1][2] / t,
						elem[2][0] / t, elem[2][1] / t, elem[2][2] / t
				};
		return cast<matrix3x3<_R> >(m_elem);
	}

	template<typename _T2>
	inline matrix3x3<decltype(decl<_T>::val()+decl<_T2>::val())> add(matrix<3, 3, _T2>& m)
	{
		typedef decltype(decl<_T>::val()*decl<_T2>::val()) _R;
		_R m_elem[] =
				{
						elem[0][0] + m.elem[0][0], elem[0][1] + m.elem[0][1], elem[0][2] + m.elem[0][2],
						elem[1][0] + m.elem[1][0], elem[1][1] + m.elem[1][1], elem[1][2] + m.elem[1][2],
						elem[2][0] + m.elem[2][0], elem[2][1] + m.elem[2][1], elem[2][2] + m.elem[2][2]
				};
		return cast<matrix3x3<_R> >(m_elem);
	}

	template<typename _T2>
	inline matrix3x3<decltype(decl<_T>::val()-decl<_T2>::val())> sub(matrix<3, 3, _T2>& m)
	{
		typedef decltype(decl<_T>::val()*decl<_T2>::val()) _R;
		_R m_elem[] =
				{
						elem[0][0] - m.elem[0][0], elem[0][1] - m.elem[0][1], elem[0][2] - m.elem[0][2],
						elem[1][0] - m.elem[1][0], elem[1][1] - m.elem[1][1], elem[1][2] - m.elem[1][2],
						elem[2][0] - m.elem[2][0], elem[2][1] - m.elem[2][1], elem[2][2] - m.elem[2][2]
				};
		return cast<matrix3x3<_R> >(m_elem);
	}

	inline static matrix3x3<_T> zero()
	{
		_T m_elem[] =
				{
						0, 0, 0,
						0, 0, 0,
						0, 0, 0
				};
		return cast<matrix3x3<_T> >(m_elem);
	}

	inline auto det() -> decltype(decl<_T>::val()*decl<_T>::val())
	{
		return elem[0][0] * (elem[1][1] * elem[2][2] - elem[1][2] * elem[2][1]) + elem[0][1] * (elem[1][2] * elem[2][0] - elem[1][0] * elem[2][2]) + elem[0][2] * (elem[1][0] * elem[2][1] - elem[1][1] * elem[2][0]);
	}

	inline auto trace() -> decltype(decl<_T>::val()+decl<_T>::val()) //迹
	{
		return elem[0][0] + elem[1][1] + elem[2][2];
	}

	inline static matrix3x3<_T> identity()
	{
		_T m_elem[] =
				{
						1, 0, 0,
						0, 1, 0,
						0, 0, 1
				};
		return cast<matrix3x3<_T> >(m_elem);
	}
};

using matrix3x3f=matrix3x3<float>;

//4x4方阵特化
template<typename _T>
class matrix4x4: public square_matrix<4, _T>
{
public:
	using matrix<4, 4, _T>::elem;
	using matrix<4, 4, _T>::type;
	using matrix<4, 4, _T>::row;
	using matrix<4, 4, _T>::column;
	using matrix<4, 4, _T>::operator+;
	using matrix<4, 4, _T>::operator-;
	using matrix<4, 4, _T>::operator*;
	using matrix<4, 4, _T>::operator/;

	template<typename _T2>
	inline matrix4x4<decltype(decl<_T>::val()+decl<_T2>::val())> operator+(matrix4x4<_T2>& m)
	{
		return add(m);
	}

	template<typename _T2>
	inline matrix4x4<decltype(decl<_T>::val()-decl<_T2>::val())> operator-(matrix4x4<_T2>& m)
	{
		return sub(m);
	}

	template<typename _T2>
	inline matrix4x4<decltype(decl<_T>::val()*decl<_T2>::val())> operator*(matrix4x4<_T2>& m)
	{
		return mul(m);
	}

	template<typename _T2>
	inline vector<4, decltype(decl<_T>::val()*decl<_T2>::val())> operator*(vector<4, _T2>& v)
	{
		return mul(v);
	}

	template<typename _T2>
	inline vector<3, decltype(decl<_T>::val()*decl<_T2>::val())> operator*(vector<3, _T2>& v)
	{
		return mul(v);
	}

	//右值处理
	template<typename _T2>
	inline matrix4x4<decltype(decl<_T>::val()+decl<_T2>::val())> operator+(matrix4x4<_T2>&& m)
	{
		return add(cast<matrix4x4<_T2> >(m));
	}

	template<typename _T2>
	inline matrix4x4<decltype(decl<_T>::val()-decl<_T2>::val())> operator-(matrix4x4<_T2>&& m)
	{
		return sub(cast<matrix4x4<_T2> >(m));
	}

	template<typename _T2>
	inline matrix4x4<decltype(decl<_T>::val()*decl<_T2>::val())> operator*(matrix4x4<_T2>&& m)
	{
		return mul(cast<matrix4x4<_T2> >(m));
	}

	template<typename _T2>
	inline vector<4, decltype(decl<_T>::val()*decl<_T2>::val())> operator*(vector<4, _T2>&& v)
	{
		return mul(cast<vector<4, _T2> >(v));
	}

	template<typename _T2>
	inline vector<3, decltype(decl<_T>::val()*decl<_T2>::val())> operator*(vector<3, _T2>&& v)
	{
		return mul(cast<vector<3, _T2> >(v));
	}

	template<typename _T2>
	inline operator matrix<4,4,_T2>()
	{
		_T2 m_elem[] =
				{
						(_T2)elem[0][0], (_T2)elem[0][1], (_T2)elem[0][2], (_T2)elem[0][3],
						(_T2)elem[1][0], (_T2)elem[1][1], (_T2)elem[1][2], (_T2)elem[1][3],
						(_T2)elem[2][0], (_T2)elem[2][1], (_T2)elem[2][2], (_T2)elem[2][3],
						(_T2)elem[3][0], (_T2)elem[3][1], (_T2)elem[3][2], (_T2)elem[3][3],
				};
		return cast<matrix<4, 4, _T2> >(m_elem);
	}

	inline vector<4, _T> column(int column)
	{
		_T m_elem[] =
				{
						elem[0][column], elem[1][column], elem[2][column], elem[3][column]
				};
		return cast<vector<4, _T> >(m_elem);
	}

	inline matrix4x4<_T> transpose() //转置
	{
		_T m_elem[] =
				{
						elem[0][0], elem[1][0], elem[2][0], elem[3][0],
						elem[0][1], elem[1][1], elem[2][1], elem[3][1],
						elem[0][2], elem[1][2], elem[2][2], elem[3][2],
						elem[0][3], elem[1][3], elem[2][3], elem[3][3]
				};
		return cast<matrix4x4<_T> >(m_elem);
	}

	template<typename _T2>
	inline matrix4x4<decltype(decl<_T>::val()*decl<_T2>::val())> dot(matrix<4, 4, _T2>& m) //按位相乘
	{
		typedef decltype(decl<_T>::val()*decl<_T2>::val()) _R;
		_R m_elem[] =
				{
						elem[0][0] * m.elem[0][0], elem[0][1] * m.elem[0][1], elem[0][2] * m.elem[0][2], elem[0][3] * m.elem[0][3],
						elem[1][0] * m.elem[1][0], elem[1][1] * m.elem[1][1], elem[1][2] * m.elem[1][2], elem[1][3] * m.elem[1][3],
						elem[2][0] * m.elem[2][0], elem[2][1] * m.elem[2][1], elem[2][2] * m.elem[2][2], elem[2][3] * m.elem[2][3],
						elem[3][0] * m.elem[3][0], elem[3][1] * m.elem[3][1], elem[3][2] * m.elem[3][2], elem[3][3] * m.elem[3][3]
				};
		return cast<matrix4x4<_R> >(m_elem);
	}

	template<typename _T2>
	inline matrix4x4<decltype(decl<_T>::val()*decl<_T2>::val())> mul(_T2 t) //数乘
	{
		typedef decltype(decl<_T>::val()*decl<_T2>::val()) _R;
		_R m_elem[] =
				{
						elem[0][0] * t, elem[0][1] * t, elem[0][2] * t, elem[0][3] * t,
						elem[1][0] * t, elem[1][1] * t, elem[1][2] * t, elem[1][3] * t,
						elem[2][0] * t, elem[2][1] * t, elem[2][2] * t, elem[2][3] * t,
						elem[3][0] * t, elem[3][1] * t, elem[3][2] * t, elem[3][3] * t
				};
		return cast<matrix4x4<_R> >(m_elem);
	}

	template<typename _T2>
	inline matrix4x4<decltype(decl<_T>::val()*decl<_T2>::val())> mul(matrix<4, 4, _T2>& m)
	{
		typedef decltype(decl<_T>::val()*decl<_T2>::val()) _R;
		_R m_elem[] =
				{
						elem[0][0] * m.elem[0][0] + elem[0][1] * m.elem[1][0] + elem[0][2] * m.elem[2][0] + elem[0][3] * m.elem[3][0], elem[0][0] * m.elem[0][1] + elem[0][1] * m.elem[1][1] + elem[0][2] * m.elem[2][1] + elem[0][3] * m.elem[3][1], elem[0][0] * m.elem[0][2] + elem[0][1] * m.elem[1][2] + elem[0][2] * m.elem[2][2] + elem[0][3] * m.elem[3][2], elem[0][0] * m.elem[0][3] + elem[0][1] * m.elem[1][3] + elem[0][2] * m.elem[2][3] + elem[0][3] * m.elem[3][3],
						elem[1][0] * m.elem[0][0] + elem[1][1] * m.elem[1][0] + elem[1][2] * m.elem[2][0] + elem[1][3] * m.elem[3][0], elem[1][0] * m.elem[0][1] + elem[1][1] * m.elem[1][1] + elem[1][2] * m.elem[2][1] + elem[1][3] * m.elem[3][1], elem[1][0] * m.elem[0][2] + elem[1][1] * m.elem[1][2] + elem[1][2] * m.elem[2][2] + elem[1][3] * m.elem[3][2], elem[1][0] * m.elem[0][3] + elem[1][1] * m.elem[1][3] + elem[1][2] * m.elem[2][3] + elem[1][3] * m.elem[3][3],
						elem[2][0] * m.elem[0][0] + elem[2][1] * m.elem[1][0] + elem[2][2] * m.elem[2][0] + elem[2][3] * m.elem[3][0], elem[2][0] * m.elem[0][1] + elem[2][1] * m.elem[1][1] + elem[2][2] * m.elem[2][1] + elem[2][3] * m.elem[3][1], elem[2][0] * m.elem[0][2] + elem[2][1] * m.elem[1][2] + elem[2][2] * m.elem[2][2] + elem[2][3] * m.elem[3][2], elem[2][0] * m.elem[0][3] + elem[2][1] * m.elem[1][3] + elem[2][2] * m.elem[2][3] + elem[2][3] * m.elem[3][3],
						elem[3][0] * m.elem[0][0] + elem[3][1] * m.elem[1][0] + elem[3][2] * m.elem[2][0] + elem[3][3] * m.elem[3][0], elem[3][0] * m.elem[0][1] + elem[3][1] * m.elem[1][1] + elem[3][2] * m.elem[2][1] + elem[3][3] * m.elem[3][1], elem[3][0] * m.elem[0][2] + elem[3][1] * m.elem[1][2] + elem[3][2] * m.elem[2][2] + elem[3][3] * m.elem[3][2], elem[3][0] * m.elem[0][3] + elem[3][1] * m.elem[1][3] + elem[3][2] * m.elem[2][3] + elem[3][3] * m.elem[3][3],
				};
		return cast<matrix4x4<_T> >(m_elem);
	}

	template<typename _T2>
	inline vector<4, decltype(decl<_T>::val()*decl<_T2>::val())> mul(vector<4, _T2>& v)
	{
		typedef decltype(decl<_T>::val()*decl<_T2>::val()) _R;
		_R v_elem[] =
				{
						elem[0][0] * v[0] + elem[0][1] * v[1] + elem[0][2] * v[2] + elem[0][3] * v[3],
						elem[1][0] * v[0] + elem[1][1] * v[1] + elem[1][2] * v[2] + elem[1][3] * v[3],
						elem[2][0] * v[0] + elem[2][1] * v[1] + elem[2][2] * v[2] + elem[2][3] * v[3],
						elem[3][0] * v[0] + elem[3][1] * v[1] + elem[3][2] * v[2] + elem[3][3] * v[3]
				};
		return cast<vector<4, _R> >(v_elem);
	}

	template<typename _T2>
	inline vector<3, decltype(decl<_T>::val()*decl<_T2>::val())> mul(vector<3, _T2>& v) //差的维度补值为1
	{
		typedef decltype(decl<_T>::val()*decl<_T2>::val()) _R;
		_R v_elem[] =
				{
						elem[0][0] * v[0] + elem[0][1] * v[1] + elem[0][2] * v[2] + elem[0][3],
						elem[1][0] * v[0] + elem[1][1] * v[1] + elem[1][2] * v[2] + elem[1][3],
						elem[2][0] * v[0] + elem[2][1] * v[1] + elem[2][2] * v[2] + elem[2][3],
				};
		return cast<vector<3, _R> >(v_elem);
	}

	template<typename _T2>
	inline matrix4x4<decltype(decl<_T>::val()/decl<_T2>::val())> div(_T2 t) //除法
	{
		typedef decltype(decl<_T>::val()/decl<_T2>::val()) _R;
		_R m_elem[] =
				{
						elem[0][0] / t, elem[0][1] / t, elem[0][2] / t, elem[0][3] / t,
						elem[1][0] / t, elem[1][1] / t, elem[1][2] / t, elem[1][3] / t,
						elem[2][0] / t, elem[2][1] / t, elem[2][2] / t, elem[2][3] / t,
						elem[3][0] / t, elem[3][1] / t, elem[3][2] / t, elem[3][3] / t
				};
		return cast<matrix4x4<_R> >(m_elem);
	}

	template<typename _T2>
	inline matrix4x4<decltype(decl<_T>::val()+decl<_T2>::val())> add(matrix<4, 4, _T2>& m)
	{
		typedef decltype(decl<_T>::val()+decl<_T2>::val()) _R;
		_R m_elem[] =
				{
						elem[0][0] + m.elem[0][0], elem[0][1] + m.elem[0][1], elem[0][2] + m.elem[0][2], elem[0][3] + m.elem[0][3],
						elem[1][0] + m.elem[1][0], elem[1][1] + m.elem[1][1], elem[1][2] + m.elem[1][2], elem[1][3] + m.elem[1][3],
						elem[2][0] + m.elem[2][0], elem[2][1] + m.elem[2][1], elem[2][2] + m.elem[2][2], elem[2][3] + m.elem[2][3],
						elem[3][0] + m.elem[3][0], elem[3][1] + m.elem[3][1], elem[3][2] + m.elem[3][2], elem[3][3] + m.elem[3][3]
				};
		return cast<matrix4x4<_R> >(m_elem);
	}

	template<typename _T2>
	inline matrix4x4<decltype(decl<_T>::val()-decl<_T2>::val())> sub(matrix<4, 4, _T2>& m)
	{
		typedef decltype(decl<_T>::val()-decl<_T2>::val()) _R;
		_R m_elem[] =
				{
						elem[0][0] - m.elem[0][0], elem[0][1] - m.elem[0][1], elem[0][2] - m.elem[0][2], elem[0][3] - m.elem[0][3],
						elem[1][0] - m.elem[1][0], elem[1][1] - m.elem[1][1], elem[1][2] - m.elem[1][2], elem[1][3] - m.elem[1][3],
						elem[2][0] - m.elem[2][0], elem[2][1] - m.elem[2][1], elem[2][2] - m.elem[2][2], elem[2][3] - m.elem[2][3],
						elem[3][0] - m.elem[3][0], elem[3][1] - m.elem[3][1], elem[3][2] - m.elem[3][2], elem[3][3] - m.elem[3][3]
				};
		return cast<matrix4x4<_R> >(m_elem);
	}

	inline static matrix4x4<_T> zero()
	{
		_T m_elem[] =
				{
						0, 0, 0, 0,
						0, 0, 0, 0,
						0, 0, 0, 0,
						0, 0, 0, 0
				};
		return cast<matrix4x4<_T> >(m_elem);
	}

	inline auto det() -> decltype(decl<_T>::val()*decl<_T>::val())
	{
		return elem[0][0] * (elem[1][1] * elem[2][2] * elem[3][3] - elem[3][1] * elem[2][2] * elem[1][3]) + elem[0][1] * (elem[1][2] * elem[2][3] * elem[3][0] - elem[1][0] * elem[2][3] * elem[3][2]) + elem[0][2] * (elem[1][3] * elem[2][0] * elem[3][1] - elem[1][1] * elem[2][0] * elem[3][3]) + elem[0][3] * (elem[1][0] * elem[2][1] * elem[3][2] - elem[1][2] * elem[2][1] * elem[3][0]);
	}

	inline auto trace() -> decltype(decl<_T>::val()+decl<_T>::val()) //迹
	{
		return elem[0][0] + elem[1][1] + elem[2][2] + elem[3][3];
	}

	inline static matrix4x4<_T> identity()
	{
		_T m_elem[] =
				{
						1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						0, 0, 0, 1
				};
		return cast<matrix4x4<_T> >(m_elem);
	}
};

using matrix4x4f=matrix4x4<float>;
}
}
#endif//ANDROMEDA_MA_TH_MA_TRIX
