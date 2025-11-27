#ifndef ANDROMEDA_MATH_MATRIX
#define ANDROMEDA_MATH_MATRIX

#include <type_traits>
#include <math.h>
#include <malloc.h>
#include <sstream>
#include "vector.h"
#include "../traits/Types.h"

namespace andromeda
{
namespace math
{
template<int Row, int Column, typename T = float>
class matrix
{
public:
	vector<Column, T> elem[Row]; //索引行、列。储存Row个Column维行向量
	typedef T type;
	static const int row_num = Row;
	static const int column_num = Column;

	inline operator T*()
	{
		return (T*)elem;
	}

	inline operator vector<Column,T>*()
	{
		return elem;
	}

	template<typename T2>
	inline operator matrix<Row,Column,T2>()
	{
		matrix<Row, Column, T2> cast_result;
		for(int i = 0; i < Row; ++i)
			cast_result[i] = elem[i];
		return cast_result;
	}

	//提取行向量
	inline vector<Column, T>& operator[](int i)
	{
		return elem[i];
	}

	inline operator std::string() //转化为字符串
	{
		return to_string();
	}

	inline operator char*() //转化为C字符串
	{
		return to_string().c_str();
	}

	template<typename T2>
	inline matrix<Row, Column, decltype(std::declval<T>()+std::declval<T2>())> operator +(matrix<Row, Column, T2>& m)
	{
		return add(m);
	}

	template<typename T2>
	inline matrix<Row, Column, decltype(std::declval<T>()-std::declval<T2>())> operator -(matrix<Row, Column, T2>& m)
	{
		return sub(m);
	}

	template<typename T2>
	inline matrix<Row, Column, decltype(std::declval<T>()*std::declval<T2>())> operator *(T2 t)
	{
		return mul(t);
	}

	template<int Row2, typename T2>
	inline matrix<Row, Row2, decltype(std::declval<T>()*std::declval<T2>())> operator *(matrix<Column, Row2, T2>& m)
	{
		return mul(m);
	}

	template<typename T2>
	inline vector<Row, decltype(std::declval<T>()*std::declval<T2>())> operator *(vector<Column, T2>& v)
	{
		return mul(v);
	}

	template<typename T2>
	inline matrix<Row, Column, decltype(std::declval<T>()/std::declval<T2>())> operator /(T2 t)
	{
		return div(t);
	}

	//右值处理
	template<typename T2>
	inline matrix<Row, Column, decltype(std::declval<T>()+std::declval<T2>())> operator +(matrix<Row, Column, T2>&& m)
	{
		return add(cast<matrix<Row, Column, T2> >(m));
	}

	template<typename T2>
	inline matrix<Row, Column, decltype(std::declval<T>()-std::declval<T2>())> operator -(matrix<Row, Column, T2>&& m)
	{
		return sub(cast<matrix<Row, Column, T2> >(m));
	}

	template<int Row2, typename T2>
	inline matrix<Row, Row2, decltype(std::declval<T>()*std::declval<T2>())> operator *(matrix<Column, Row2, T2>&& m)
	{
		return mul(cast<matrix<Row, Column, T2> >(m));
	}

	template<typename T2>
	inline vector<Row, decltype(std::declval<T>()*std::declval<T2>())> operator *(vector<Column, T2>&& v)
	{
		return mul(cast<vector<Column, T2> >(v));
	}

	//提取列向量
	vector<Row, T> column(int column)
	{
		vector<Row, T> row_vec;
		for(int i = 0; i < Row; ++i)
			row_vec[i] = elem[i][column];
		return row_vec;
	}

	//提取行向量
	inline vector<Column, T> row(int row)
	{
		return elem[row];
	}

	matrix<Column, Row, T> transpose() //转置
	{
		matrix<Column, Row, T> transpose_result;
		for(int i = 0; i < Column; ++i)
			for(int j = 0; j < Row; ++j)
				transpose_result[i][j] = elem[j][i];
		return transpose_result;
	}

	template<typename T2>
	matrix<Row, Column, decltype(std::declval<T>()*std::declval<T2>())> dot(matrix<Row, Column, T2>& m) //按位相乘
	{
		matrix<Row, Column, decltype(std::declval<T>()*std::declval<T2>())> dot_result;
		for(int i = 0; i < Row; ++i)
			for(int j = 0; j < Column; ++j)
				dot_result[i][j] = elem[i][j] * m.elem[i][j];
		return dot_result;
	}

	template<typename T2>
	matrix<Row, Column, decltype(std::declval<T>()*std::declval<T2>())> mul(T2 t) //数乘
	{
		matrix<Row, Column, decltype(std::declval<T>()*std::declval<T2>())> mul_result;
		for(int i = 0; i < Row; ++i)
			mul_result[i] = elem[i] * t;
		return mul_result;
	}

	template<int Row2, typename T2>
	matrix<Row, Row2, decltype(std::declval<T>()*std::declval<T2>())> mul(matrix<Column, Row2, T2>& m)
	{
		matrix<Row, Row2, decltype(std::declval<T>()*std::declval<T2>())> mul_result;
		for(int i = 0; i < Row; ++i)
		{
			vector<Column, T> row_vec = row(i);
			for(int j = 0; j < Row2; ++j)
				mul_result[i][j] = row_vec.dot(m.column(j));
		}
		return mul_result;
	}

	template<typename T2>
	vector<Row, decltype(std::declval<T>()*std::declval<T2>())> mul(vector<Column, T2>& v)
	{
		vector<Row, decltype(std::declval<T>()*std::declval<T2>())> mul_result;
		for(int i = 0; i < Row; ++i)
		{
			vector<Column, T> row_vec = row(i);
			mul_result[i] = row_vec.dot(v);
		}
		return mul_result;
	}

	template<typename T2>
	matrix<Row, Column, decltype(std::declval<T>()/std::declval<T2>())> div(T2 t) //除法
	{
		matrix<Row, Column, decltype(std::declval<T>()/std::declval<T2>())> div_result;
		for(int i = 0; i < Row; ++i)
			div_result[i] = elem[i] / t;
		return div_result;
	}

	template<typename T2>
	matrix<Row, Column, decltype(std::declval<T>()+std::declval<T2>())> add(matrix<Row, Column, T2>& m)
	{
		matrix<Row, Column, decltype(std::declval<T>()+std::declval<T2>())> add_result;
		for(int i = 0; i < Row; ++i)
			add_result[i] = elem[i] + m.elem[i];
		return add_result;
	}

	template<typename T2>
	matrix<Row, Column, decltype(std::declval<T>()-std::declval<T2>())> sub(matrix<Row, Column, T2>& m)
	{
		matrix<Row, Column, decltype(std::declval<T>()-std::declval<T2>())> sub_result;
		for(int i = 0; i < Row; ++i)
			sub_result[i] = elem[i] - m.elem[i];
		return sub_result;
	}

	static matrix<Row, Column, T> zero()
	{
		matrix<Row, Column, T> zero_mat;
		memset(&zero_mat.elem, 0, Row * Column * sizeof(T));
		return zero_mat;
	}

	std::string to_string()
	{
		std::ostringstream oss;
		for(int i = 0; i < Row; ++i)
		{
			oss << elem[i][0];
			for(int j = 1; j < Column; ++j)
				oss << ' ' << elem[i][j];
			oss << '\n';
		}
		return oss.str();
	}
};

//方阵特化
template<int Rank, typename T>
class square_matrix: public matrix<Rank, Rank, T>
{
public:
	using matrix<Rank, Rank, T>::elem; //索引行、列。储存Rank个Rank维行向量
	using matrix<Rank, Rank, T>::type;
	using matrix<Rank, Rank, T>::row;
	using matrix<Rank, Rank, T>::column;
	using matrix<Rank, Rank, T>::operator+;
	using matrix<Rank, Rank, T>::operator-;
	using matrix<Rank, Rank, T>::operator*;
	using matrix<Rank, Rank, T>::operator/;

	template<typename T2>
	inline square_matrix<Rank, decltype(std::declval<T>()+std::declval<T2>())> operator +(square_matrix<Rank, T2>& m)
	{
		return add(m);
	}

	template<typename T2>
	inline square_matrix<Rank, decltype(std::declval<T>()-std::declval<T2>())> operator -(square_matrix<Rank, T2>& m)
	{
		return sub(m);
	}

	template<typename T2>
	inline square_matrix<Rank, decltype(std::declval<T>()*std::declval<T2>())> operator *(square_matrix<Rank, T2>& m)
	{
		return mul(m);
	}

	template<typename T2>
	inline vector<Rank, decltype(std::declval<T>()*std::declval<T2>())> operator *(vector<Rank, T2>& v)
	{
		return mul(v);
	}

	//右值处理
	template<typename T2>
	inline square_matrix<Rank, decltype(std::declval<T>()+std::declval<T2>())> operator +(square_matrix<Rank, T2>&& m)
	{
		return add(cast<square_matrix<Rank, T2> >(m));
	}

	template<typename T2>
	inline square_matrix<Rank, decltype(std::declval<T>()-std::declval<T2>())> operator -(square_matrix<Rank, T2>&& m)
	{
		return sub(cast<square_matrix<Rank, T2> >(m));
	}

	template<typename T2>
	inline square_matrix<Rank, decltype(std::declval<T>()*std::declval<T2>())> operator *(square_matrix<Rank, T2>&& m)
	{
		return mul(cast<square_matrix<Rank, T2> >(m));
	}

	template<typename T2>
	inline vector<Rank, decltype(std::declval<T>()*std::declval<T2>())> operator *(vector<Rank, T2>&& v)
	{
		return mul(cast<vector<Rank, T2> >(v));
	}

	//特化类独有
	inline constexpr int rank()
	{
		return Rank;
	}

	auto det() -> decltype(std::declval<T>()*std::declval<T>()); //行列式懒得写了高阶的用不上

	auto trace() -> decltype(std::declval<T>()+std::declval<T>()) //迹
	{
		decltype(std::declval<T>()+std::declval<T>()) tr = 0;
		for(int i = 0; i < Rank; ++i)
			tr += elem[i][i];
		return tr;
	}

	static square_matrix<Rank, T> identity()
	{
		square_matrix<Rank, T> id = cast<square_matrix<Rank, T> >(matrix<Rank, Rank, T>::zero());
		for(int i = 0; i < Rank; ++i)
			id[i][i] = 1;
		return id;
	}
};

//3x3方阵特化
template<typename T>
class matrix3x3: public square_matrix<3, T>
{
public:
	using matrix<3, 3, T>::elem;
	using matrix<3, 3, T>::type;
	using matrix<3, 3, T>::row;
	using matrix<3, 3, T>::column;
	using matrix<3, 3, T>::operator+;
	using matrix<3, 3, T>::operator-;
	using matrix<3, 3, T>::operator*;
	using matrix<3, 3, T>::operator/;

	template<typename T2>
	inline matrix3x3<decltype(std::declval<T>()+std::declval<T2>())> operator +(matrix3x3<T2>& m)
	{
		return add(m);
	}

	template<typename T2>
	inline matrix3x3<decltype(std::declval<T>()-std::declval<T2>())> operator -(matrix3x3<T2>& m)
	{
		return sub(m);
	}

	template<typename T2>
	inline matrix3x3<decltype(std::declval<T>()*std::declval<T2>())> operator *(matrix3x3<T2>& m)
	{
		return mul(m);
	}

	template<typename T2>
	inline vector<3, decltype(std::declval<T>()*std::declval<T2>())> operator *(vector<3, T2>& v)
	{
		return mul(v);
	}

	//右值处理
	template<typename T2>
	inline matrix3x3<decltype(std::declval<T>()+std::declval<T2>())> operator +(matrix3x3<T2>&& m)
	{
		return add(cast<matrix3x3<T2> >(m));
	}

	template<typename T2>
	inline matrix3x3<decltype(std::declval<T>()-std::declval<T2>())> operator -(matrix3x3<T2>&& m)
	{
		return sub(cast<matrix3x3<T2> >(m));
	}

	template<typename T2>
	inline matrix3x3<decltype(std::declval<T>()*std::declval<T2>())> operator *(matrix3x3<T2>&& m)
	{
		return mul(cast<matrix3x3<T2> >(m));
	}

	template<typename T2>
	inline vector<3, decltype(std::declval<T>()*std::declval<T2>())> operator *(vector<3, T2>&& v)
	{
		return mul(cast<vector<3, T2> >(v));
	}

	template<typename T2>
	inline operator matrix<3,3,T2>()
	{
		T2 m_elem[] =
				{
						(T2)elem[0][0], (T2)elem[0][1], (T2)elem[0][2],
						(T2)elem[1][0], (T2)elem[1][1], (T2)elem[1][2],
						(T2)elem[2][0], (T2)elem[2][1], (T2)elem[2][2]
				};
		return cast<matrix<3, 3, T2> >(m_elem);
	}

	inline vector<3, T> column(int column)
	{
		T v_elem[] =
				{
						elem[0][column], elem[1][column], elem[2][column]
				};
		return cast<vector<3, T> >(v_elem);
	}

	inline vector<3, T> extractRow(int row)
	{
		return elem[row];
	}

	inline matrix3x3<T> transpose() //转置
	{
		T m_elem[] =
				{
						elem[0][0], elem[1][0], elem[2][0],
						elem[0][1], elem[1][1], elem[2][1],
						elem[0][2], elem[1][2], elem[2][2]
				};
		return cast<matrix3x3<T> >(m_elem);
	}

	template<typename T2>
	inline matrix3x3<decltype(std::declval<T>()*std::declval<T2>())> dot(matrix<3, 3, T2>& m) //按位相乘
	{
		typedef decltype(std::declval<T>()*std::declval<T2>()) R;
		R m_elem[] =
				{
						elem[0][0] * m.elem[0][0], elem[0][1] * m.elem[0][1], elem[0][2] * m.elem[0][2],
						elem[1][0] * m.elem[1][0], elem[1][1] * m.elem[1][1], elem[1][2] * m.elem[1][2],
						elem[2][0] * m.elem[2][0], elem[2][1] * m.elem[2][1], elem[2][2] * m.elem[2][2]
				};
		return cast<matrix3x3<R> >(m_elem);
	}

	template<typename T2>
	inline matrix3x3<decltype(std::declval<T>()*std::declval<T2>())> mul(T2 t) //数乘
	{
		typedef decltype(std::declval<T>()*std::declval<T2>()) R;
		R m_elem[] =
				{
						elem[0][0] * t, elem[0][1] * t, elem[0][2] * t,
						elem[1][0] * t, elem[1][1] * t, elem[1][2] * t,
						elem[2][0] * t, elem[2][1] * t, elem[2][2] * t
				};
		return cast<matrix3x3<R> >(m_elem);
	}

	template<typename T2>
	inline matrix3x3<decltype(std::declval<T>()*std::declval<T2>())> mul(matrix<3, 3, T2>& m)
	{
		typedef decltype(std::declval<T>()*std::declval<T2>()) R;
		R m_elem[] =
				{
						elem[0][0] * m.elem[0][0] + elem[0][1] * m.elem[1][0] + elem[0][2] * m.elem[2][0], elem[0][0] * m.elem[0][1] + elem[0][1] * m.elem[1][1] + elem[0][2] * m.elem[2][1], elem[0][0] * m.elem[0][2] + elem[0][1] * m.elem[1][2] + elem[0][2] * m.elem[2][2],
						elem[1][0] * m.elem[0][0] + elem[1][1] * m.elem[1][0] + elem[1][2] * m.elem[2][0], elem[1][0] * m.elem[0][1] + elem[1][1] * m.elem[1][1] + elem[1][2] * m.elem[2][1], elem[1][0] * m.elem[0][2] + elem[1][1] * m.elem[1][2] + elem[1][2] * m.elem[2][2],
						elem[2][0] * m.elem[0][0] + elem[2][1] * m.elem[1][0] + elem[2][2] * m.elem[2][0], elem[2][0] * m.elem[0][1] + elem[2][1] * m.elem[1][1] + elem[2][2] * m.elem[2][1], elem[2][0] * m.elem[0][2] + elem[2][1] * m.elem[1][2] + elem[2][2] * m.elem[2][2]
				};
		return cast<matrix3x3<R> >(m_elem);
	}

	template<typename T2>
	inline vector<3, decltype(std::declval<T>()*std::declval<T2>())> mul(vector<3, T2>& v)
	{
		typedef decltype(std::declval<T>()*std::declval<T2>()) R;
		R m_elem[] =
				{
						elem[0][0] * v[0] + elem[0][1] * v[1] + elem[0][2] * v[2],
						elem[1][0] * v[0] + elem[1][1] * v[1] + elem[1][2] * v[2],
						elem[2][0] * v[0] + elem[2][1] * v[1] + elem[2][2] * v[2]
				};
		return cast<vector<3, R> >(m_elem);
	}

	template<typename T2>
	inline matrix3x3<decltype(std::declval<T>()/std::declval<T2>())> div(T2 t) //除法
	{
		typedef decltype(std::declval<T>()*std::declval<T2>()) R;
		R m_elem[] =
				{
						elem[0][0] / t, elem[0][1] / t, elem[0][2] / t,
						elem[1][0] / t, elem[1][1] / t, elem[1][2] / t,
						elem[2][0] / t, elem[2][1] / t, elem[2][2] / t
				};
		return cast<matrix3x3<R> >(m_elem);
	}

	template<typename T2>
	inline matrix3x3<decltype(std::declval<T>()+std::declval<T2>())> add(matrix<3, 3, T2>& m)
	{
		typedef decltype(std::declval<T>()*std::declval<T2>()) R;
		R m_elem[] =
				{
						elem[0][0] + m.elem[0][0], elem[0][1] + m.elem[0][1], elem[0][2] + m.elem[0][2],
						elem[1][0] + m.elem[1][0], elem[1][1] + m.elem[1][1], elem[1][2] + m.elem[1][2],
						elem[2][0] + m.elem[2][0], elem[2][1] + m.elem[2][1], elem[2][2] + m.elem[2][2]
				};
		return cast<matrix3x3<R> >(m_elem);
	}

	template<typename T2>
	inline matrix3x3<decltype(std::declval<T>()-std::declval<T2>())> sub(matrix<3, 3, T2>& m)
	{
		typedef decltype(std::declval<T>()*std::declval<T2>()) R;
		R m_elem[] =
				{
						elem[0][0] - m.elem[0][0], elem[0][1] - m.elem[0][1], elem[0][2] - m.elem[0][2],
						elem[1][0] - m.elem[1][0], elem[1][1] - m.elem[1][1], elem[1][2] - m.elem[1][2],
						elem[2][0] - m.elem[2][0], elem[2][1] - m.elem[2][1], elem[2][2] - m.elem[2][2]
				};
		return cast<matrix3x3<R> >(m_elem);
	}

	inline static matrix3x3<T> zero()
	{
		T m_elem[] =
				{
						0, 0, 0,
						0, 0, 0,
						0, 0, 0
				};
		return cast<matrix3x3<T> >(m_elem);
	}

	inline auto det() -> decltype(std::declval<T>()*std::declval<T>())
	{
		return elem[0][0] * (elem[1][1] * elem[2][2] - elem[1][2] * elem[2][1]) + elem[0][1] * (elem[1][2] * elem[2][0] - elem[1][0] * elem[2][2]) + elem[0][2] * (elem[1][0] * elem[2][1] - elem[1][1] * elem[2][0]);
	}

	inline auto trace() -> decltype(std::declval<T>()+std::declval<T>()) //迹
	{
		return elem[0][0] + elem[1][1] + elem[2][2];
	}

	inline static matrix3x3<T> identity()
	{
		T m_elem[] =
				{
						1, 0, 0,
						0, 1, 0,
						0, 0, 1
				};
		return cast<matrix3x3<T> >(m_elem);
	}
};

using matrix3x3f=matrix3x3<float>;

//4x4方阵特化
template<typename T>
class matrix4x4: public square_matrix<4, T>
{
public:
	using matrix<4, 4, T>::elem;
	using matrix<4, 4, T>::type;
	using matrix<4, 4, T>::row;
	using matrix<4, 4, T>::column;
	using matrix<4, 4, T>::operator+;
	using matrix<4, 4, T>::operator-;
	using matrix<4, 4, T>::operator*;
	using matrix<4, 4, T>::operator/;

	template<typename T2>
	inline matrix4x4<decltype(std::declval<T>()+std::declval<T2>())> operator +(matrix4x4<T2>& m)
	{
		return add(m);
	}

	template<typename T2>
	inline matrix4x4<decltype(std::declval<T>()-std::declval<T2>())> operator -(matrix4x4<T2>& m)
	{
		return sub(m);
	}

	template<typename T2>
	inline matrix4x4<decltype(std::declval<T>()*std::declval<T2>())> operator *(matrix4x4<T2>& m)
	{
		return mul(m);
	}

	template<typename T2>
	inline vector<4, decltype(std::declval<T>()*std::declval<T2>())> operator *(vector<4, T2>& v)
	{
		return mul(v);
	}

	template<typename T2>
	inline vector<3, decltype(std::declval<T>()*std::declval<T2>())> operator *(vector<3, T2>& v)
	{
		return mul(v);
	}

	//右值处理
	template<typename T2>
	inline matrix4x4<decltype(std::declval<T>()+std::declval<T2>())> operator +(matrix4x4<T2>&& m)
	{
		return add(cast<matrix4x4<T2> >(m));
	}

	template<typename T2>
	inline matrix4x4<decltype(std::declval<T>()-std::declval<T2>())> operator -(matrix4x4<T2>&& m)
	{
		return sub(cast<matrix4x4<T2> >(m));
	}

	template<typename T2>
	inline matrix4x4<decltype(std::declval<T>()*std::declval<T2>())> operator *(matrix4x4<T2>&& m)
	{
		return mul(cast<matrix4x4<T2> >(m));
	}

	template<typename T2>
	inline vector<4, decltype(std::declval<T>()*std::declval<T2>())> operator *(vector<4, T2>&& v)
	{
		return mul(cast<vector<4, T2> >(v));
	}

	template<typename T2>
	inline vector<3, decltype(std::declval<T>()*std::declval<T2>())> operator *(vector<3, T2>&& v)
	{
		return mul(cast<vector<3, T2> >(v));
	}

	template<typename T2>
	inline operator matrix<4,4,T2>()
	{
		T2 m_elem[] =
				{
						(T2)elem[0][0], (T2)elem[0][1], (T2)elem[0][2], (T2)elem[0][3],
						(T2)elem[1][0], (T2)elem[1][1], (T2)elem[1][2], (T2)elem[1][3],
						(T2)elem[2][0], (T2)elem[2][1], (T2)elem[2][2], (T2)elem[2][3],
						(T2)elem[3][0], (T2)elem[3][1], (T2)elem[3][2], (T2)elem[3][3],
				};
		return cast<matrix<4, 4, T2> >(m_elem);
	}

	inline vector<4, T> column(int column)
	{
		T m_elem[] =
				{
						elem[0][column], elem[1][column], elem[2][column], elem[3][column]
				};
		return cast<vector<4, T> >(m_elem);
	}

	inline matrix4x4<T> transpose() //转置
	{
		T m_elem[] =
				{
						elem[0][0], elem[1][0], elem[2][0], elem[3][0],
						elem[0][1], elem[1][1], elem[2][1], elem[3][1],
						elem[0][2], elem[1][2], elem[2][2], elem[3][2],
						elem[0][3], elem[1][3], elem[2][3], elem[3][3]
				};
		return cast<matrix4x4<T> >(m_elem);
	}

	template<typename T2>
	inline matrix4x4<decltype(std::declval<T>()*std::declval<T2>())> dot(matrix<4, 4, T2>& m) //按位相乘
	{
		typedef decltype(std::declval<T>()*std::declval<T2>()) R;
		R m_elem[] =
				{
						elem[0][0] * m.elem[0][0], elem[0][1] * m.elem[0][1], elem[0][2] * m.elem[0][2], elem[0][3] * m.elem[0][3],
						elem[1][0] * m.elem[1][0], elem[1][1] * m.elem[1][1], elem[1][2] * m.elem[1][2], elem[1][3] * m.elem[1][3],
						elem[2][0] * m.elem[2][0], elem[2][1] * m.elem[2][1], elem[2][2] * m.elem[2][2], elem[2][3] * m.elem[2][3],
						elem[3][0] * m.elem[3][0], elem[3][1] * m.elem[3][1], elem[3][2] * m.elem[3][2], elem[3][3] * m.elem[3][3]
				};
		return cast<matrix4x4<R> >(m_elem);
	}

	template<typename T2>
	inline matrix4x4<decltype(std::declval<T>()*std::declval<T2>())> mul(T2 t) //数乘
	{
		typedef decltype(std::declval<T>()*std::declval<T2>()) R;
		R m_elem[] =
				{
						elem[0][0] * t, elem[0][1] * t, elem[0][2] * t, elem[0][3] * t,
						elem[1][0] * t, elem[1][1] * t, elem[1][2] * t, elem[1][3] * t,
						elem[2][0] * t, elem[2][1] * t, elem[2][2] * t, elem[2][3] * t,
						elem[3][0] * t, elem[3][1] * t, elem[3][2] * t, elem[3][3] * t
				};
		return cast<matrix4x4<R> >(m_elem);
	}

	template<typename T2>
	inline matrix4x4<decltype(std::declval<T>()*std::declval<T2>())> mul(matrix<4, 4, T2>& m)
	{
		typedef decltype(std::declval<T>()*std::declval<T2>()) R;
		R m_elem[] =
				{
						elem[0][0] * m.elem[0][0] + elem[0][1] * m.elem[1][0] + elem[0][2] * m.elem[2][0] + elem[0][3] * m.elem[3][0], elem[0][0] * m.elem[0][1] + elem[0][1] * m.elem[1][1] + elem[0][2] * m.elem[2][1] + elem[0][3] * m.elem[3][1], elem[0][0] * m.elem[0][2] + elem[0][1] * m.elem[1][2] + elem[0][2] * m.elem[2][2] + elem[0][3] * m.elem[3][2], elem[0][0] * m.elem[0][3] + elem[0][1] * m.elem[1][3] + elem[0][2] * m.elem[2][3] + elem[0][3] * m.elem[3][3],
						elem[1][0] * m.elem[0][0] + elem[1][1] * m.elem[1][0] + elem[1][2] * m.elem[2][0] + elem[1][3] * m.elem[3][0], elem[1][0] * m.elem[0][1] + elem[1][1] * m.elem[1][1] + elem[1][2] * m.elem[2][1] + elem[1][3] * m.elem[3][1], elem[1][0] * m.elem[0][2] + elem[1][1] * m.elem[1][2] + elem[1][2] * m.elem[2][2] + elem[1][3] * m.elem[3][2], elem[1][0] * m.elem[0][3] + elem[1][1] * m.elem[1][3] + elem[1][2] * m.elem[2][3] + elem[1][3] * m.elem[3][3],
						elem[2][0] * m.elem[0][0] + elem[2][1] * m.elem[1][0] + elem[2][2] * m.elem[2][0] + elem[2][3] * m.elem[3][0], elem[2][0] * m.elem[0][1] + elem[2][1] * m.elem[1][1] + elem[2][2] * m.elem[2][1] + elem[2][3] * m.elem[3][1], elem[2][0] * m.elem[0][2] + elem[2][1] * m.elem[1][2] + elem[2][2] * m.elem[2][2] + elem[2][3] * m.elem[3][2], elem[2][0] * m.elem[0][3] + elem[2][1] * m.elem[1][3] + elem[2][2] * m.elem[2][3] + elem[2][3] * m.elem[3][3],
						elem[3][0] * m.elem[0][0] + elem[3][1] * m.elem[1][0] + elem[3][2] * m.elem[2][0] + elem[3][3] * m.elem[3][0], elem[3][0] * m.elem[0][1] + elem[3][1] * m.elem[1][1] + elem[3][2] * m.elem[2][1] + elem[3][3] * m.elem[3][1], elem[3][0] * m.elem[0][2] + elem[3][1] * m.elem[1][2] + elem[3][2] * m.elem[2][2] + elem[3][3] * m.elem[3][2], elem[3][0] * m.elem[0][3] + elem[3][1] * m.elem[1][3] + elem[3][2] * m.elem[2][3] + elem[3][3] * m.elem[3][3],
				};
		return cast<matrix4x4<T> >(m_elem);
	}

	template<typename T2>
	inline vector<4, decltype(std::declval<T>()*std::declval<T2>())> mul(vector<4, T2>& v)
	{
		typedef decltype(std::declval<T>()*std::declval<T2>()) R;
		R v_elem[] =
				{
						elem[0][0] * v[0] + elem[0][1] * v[1] + elem[0][2] * v[2] + elem[0][3] * v[3],
						elem[1][0] * v[0] + elem[1][1] * v[1] + elem[1][2] * v[2] + elem[1][3] * v[3],
						elem[2][0] * v[0] + elem[2][1] * v[1] + elem[2][2] * v[2] + elem[2][3] * v[3],
						elem[3][0] * v[0] + elem[3][1] * v[1] + elem[3][2] * v[2] + elem[3][3] * v[3]
				};
		return cast<vector<4, R> >(v_elem);
	}

	template<typename T2>
	inline vector<3, decltype(std::declval<T>()*std::declval<T2>())> mul(vector<3, T2>& v) //差的维度补值为1
	{
		typedef decltype(std::declval<T>()*std::declval<T2>()) R;
		R v_elem[] =
				{
						elem[0][0] * v[0] + elem[0][1] * v[1] + elem[0][2] * v[2] + elem[0][3],
						elem[1][0] * v[0] + elem[1][1] * v[1] + elem[1][2] * v[2] + elem[1][3],
						elem[2][0] * v[0] + elem[2][1] * v[1] + elem[2][2] * v[2] + elem[2][3],
				};
		return cast<vector<3, R> >(v_elem);
	}

	template<typename T2>
	inline matrix4x4<decltype(std::declval<T>()/std::declval<T2>())> div(T2 t) //除法
	{
		typedef decltype(std::declval<T>()/std::declval<T2>()) R;
		R m_elem[] =
				{
						elem[0][0] / t, elem[0][1] / t, elem[0][2] / t, elem[0][3] / t,
						elem[1][0] / t, elem[1][1] / t, elem[1][2] / t, elem[1][3] / t,
						elem[2][0] / t, elem[2][1] / t, elem[2][2] / t, elem[2][3] / t,
						elem[3][0] / t, elem[3][1] / t, elem[3][2] / t, elem[3][3] / t
				};
		return cast<matrix4x4<R> >(m_elem);
	}

	template<typename T2>
	inline matrix4x4<decltype(std::declval<T>()+std::declval<T2>())> add(matrix<4, 4, T2>& m)
	{
		typedef decltype(std::declval<T>()+std::declval<T2>()) R;
		R m_elem[] =
				{
						elem[0][0] + m.elem[0][0], elem[0][1] + m.elem[0][1], elem[0][2] + m.elem[0][2], elem[0][3] + m.elem[0][3],
						elem[1][0] + m.elem[1][0], elem[1][1] + m.elem[1][1], elem[1][2] + m.elem[1][2], elem[1][3] + m.elem[1][3],
						elem[2][0] + m.elem[2][0], elem[2][1] + m.elem[2][1], elem[2][2] + m.elem[2][2], elem[2][3] + m.elem[2][3],
						elem[3][0] + m.elem[3][0], elem[3][1] + m.elem[3][1], elem[3][2] + m.elem[3][2], elem[3][3] + m.elem[3][3]
				};
		return cast<matrix4x4<R> >(m_elem);
	}

	template<typename T2>
	inline matrix4x4<decltype(std::declval<T>()-std::declval<T2>())> sub(matrix<4, 4, T2>& m)
	{
		typedef decltype(std::declval<T>()-std::declval<T2>()) R;
		R m_elem[] =
				{
						elem[0][0] - m.elem[0][0], elem[0][1] - m.elem[0][1], elem[0][2] - m.elem[0][2], elem[0][3] - m.elem[0][3],
						elem[1][0] - m.elem[1][0], elem[1][1] - m.elem[1][1], elem[1][2] - m.elem[1][2], elem[1][3] - m.elem[1][3],
						elem[2][0] - m.elem[2][0], elem[2][1] - m.elem[2][1], elem[2][2] - m.elem[2][2], elem[2][3] - m.elem[2][3],
						elem[3][0] - m.elem[3][0], elem[3][1] - m.elem[3][1], elem[3][2] - m.elem[3][2], elem[3][3] - m.elem[3][3]
				};
		return cast<matrix4x4<R> >(m_elem);
	}

	inline static matrix4x4<T> zero()
	{
		T m_elem[] =
				{
						0, 0, 0, 0,
						0, 0, 0, 0,
						0, 0, 0, 0,
						0, 0, 0, 0
				};
		return cast<matrix4x4<T> >(m_elem);
	}

	inline auto det() -> decltype(std::declval<T>()*std::declval<T>())
	{
		return elem[0][0] * (elem[1][1] * elem[2][2] * elem[3][3] - elem[3][1] * elem[2][2] * elem[1][3]) + elem[0][1] * (elem[1][2] * elem[2][3] * elem[3][0] - elem[1][0] * elem[2][3] * elem[3][2]) + elem[0][2] * (elem[1][3] * elem[2][0] * elem[3][1] - elem[1][1] * elem[2][0] * elem[3][3]) + elem[0][3] * (elem[1][0] * elem[2][1] * elem[3][2] - elem[1][2] * elem[2][1] * elem[3][0]);
	}

	inline auto trace() -> decltype(std::declval<T>()+std::declval<T>()) //迹
	{
		return elem[0][0] + elem[1][1] + elem[2][2] + elem[3][3];
	}

	inline static matrix4x4<T> identity()
	{
		T m_elem[] =
				{
						1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						0, 0, 0, 1
				};
		return cast<matrix4x4<T> >(m_elem);
	}
};

using matrix4x4f=matrix4x4<float>;
}
}
#endif//ANDROMEDA_MATH_MATRIX
