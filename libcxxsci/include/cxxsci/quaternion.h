#ifndef _CXXSCI_QUATERNION
#define _CXXSCI_QUATERNION

#include <cxxsci/vector.h>
#include <cxxsci/matrix.h>

namespace cxxsci
{
template<typename T = float>
class quaternion
{
public:
	T w, x, y, z;

	quaternion(T w, T x, T y, T z)
	{
		this->w = w;
		this->x = x;
		this->y = y;
		this->z = z;
	}

	quaternion(T w, vector<3, T> v)
	{
		this->w = w;
		this->x = v[0];
		this->y = v[1];
		this->z = v[2];
	}

	quaternion conj()
	{
		return quaternion(w, -x, -y, -z);
	}

	template<typename T2>
	quaternion<decltype(std::declval<T>()+std::declval<T2>())> operator+(quaternion<T2> q)
	{
		return quaternion<decltype(std::declval<T>()+std::declval<T2>())>(w + q.w, x + q.x, y + q.y, z + q.z);
	}

	template<typename T2>
	quaternion<decltype(std::declval<T>()-std::declval<T2>())> operator-(quaternion<T2> q)
	{
		return quaternion<decltype(std::declval<T>()-std::declval<T2>())>(w - q.w, x - q.x, y - q.y, z - q.z);
	}

	template<typename T2>
	quaternion<decltype(std::declval<T>()*std::declval<T2>())> operator*(quaternion<T2> q)
	{
		return quaternion<decltype(std::declval<T>()*std::declval<T2>())>(w * q.w - x * q.x - y * q.y - z * q.z, y * q.z - q.y * z + w * q.x + q.w * x, z * q.x - q.z * x + w * q.y + q.w * y, x * q.y - q.x * y + w * q.z + q.w * z);
	}

	static vector<3, T> rotate(vector<3, T> v, vector<3, T> axis, T rad)
	{
		auto p = quaternion(0, v);
		auto q = quaternion(cos(rad / 2.0f), axis * sin(rad / 2.0f));
		auto r = q * p * q.conj();
		return
		{	r.x,r.y,r.z};
	}

	//绕轴转动，四元数转换为旋转矩阵
	static inline matrix4x4<T> R(vector<3, T> axis, T rad)
	{
		T m_elem[] =
				{
						cos(rad) + axis[0] * axis[0] * (1 - cos(rad)), axis[0] * axis[1] * (1 - cos(rad)) - axis[2] * sin(rad), axis[0] * axis[2] * (1 - cos(rad)) + axis[1] * sin(rad), 0,
						axis[0] * axis[1] * (1 - cos(rad)) + axis[2] * sin(rad), cos(rad) + axis[1] * axis[1] * (1 - cos(rad)), axis[1] * axis[2] * (1 - cos(rad)) - axis[0] * sin(rad), 0,
						axis[0] * axis[2] * (1 - cos(rad)) - axis[1] * sin(rad), axis[1] * axis[2] * (1 - cos(rad)) + axis[0] * sin(rad), cos(rad) + axis[2] * axis[2] * (1 - cos(rad)), 0,
						0, 0, 0, 1
				};
		return tplmp::cast<matrix4x4<T> >(m_elem);
	}
};

}

#endif//_CXXSCI_QUATERNION
