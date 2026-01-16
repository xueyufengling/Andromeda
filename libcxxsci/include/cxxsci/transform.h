#ifndef _CXXSCI_TRANSFORM
#define _CXXSCI_TRANSFORM

#include <math.h>

#include <cxxsci/vector.h>
#include <cxxsci/matrix.h>
#include <cxxsci/quaternion.h>

//采用右手坐标系
namespace cxxsci
{
//缩放矩阵
template<int _Rank = 4, typename _T = float>
inline square_matrix<_Rank, _T> scale(_T scale_factor)
{
	return cast<square_matrix<_Rank, _T> >(square_matrix<_Rank, _T>::identity() * scale_factor);
}

//平移矩阵，矩阵维度比矢量大1
template<int _Rank, typename _T = float>
inline square_matrix<_Rank + 1, _T> shift(vector<_Rank, _T>& shift_v)
{
	square_matrix<_Rank + 1, _T> shift_mat = square_matrix<_Rank + 1, _T>::identify();
	for(int i = 0; i <= _Rank; ++i)
		shift_mat[i][_Rank] = shift_v[i];
	return shift_mat;
}

template<typename _T = float>
inline matrix4x4<_T> shift(vector<3, _T> shift_v)
{
	_T m_elem[] =
			{
					1, 0, 0, shift_v[0],
					0, 1, 0, shift_v[1],
					0, 0, 1, shift_v[2],
					0, 0, 0, 1
			};
	return cast<matrix4x4<_T> >(m_elem);
}

template<typename _T = float>
inline matrix4x4<_T> Rx(_T rad)
{
	_T m_elem[] =
			{
					1, 0, 0, 0,
					0, cos(rad), -sin(rad), 0,
					0, sin(rad), cos(rad), 0,
					0, 0, 0, 1
			};
	return cast<matrix4x4<_T> >(m_elem);
}

template<typename _T = float>
inline matrix4x4<_T> Ry(_T rad)
{
	_T m_elem[] =
			{
					cos(rad), 0, sin(rad), 0,
					0, 1, 0, 0,
					-sin(rad), 0, cos(rad), 0,
					0, 0, 0, 1
			};
	return cast<matrix4x4<_T> >(m_elem);
}

template<typename _T = float>
inline matrix4x4<_T> Rz(_T rad)
{
	_T m_elem[] =
			{
					cos(rad), -sin(rad), 0, 0,
					sin(rad), cos(rad), 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1
			};
	return cast<matrix4x4<_T> >(m_elem);
}

//外在旋转
template<typename _T = float>
inline matrix4x4<_T> Rxyz(_T rad_x, _T rad_y, _T rad_z)
{
	return Rx(rad_z) * Ry(rad_y) * Rz(rad_x);
}

//内在旋转
template<typename _T = float>
inline matrix4x4<_T> Rzyx(_T pitch, _T yaw, _T roll)
{
	return Rz(roll) * Ry(yaw) * Rx(pitch);
}

//绕轴转动，四元数转换为旋转矩阵
template<typename _T = float>
inline matrix4x4<_T> R(vector<3, _T> axis, _T rad)
{
	return quaternion<_T>::R(axis, rad);
}

//OpenGL的投影矩阵。NDC为左手坐标系，世界坐标系为右手坐标系，near和far应该取负数，向z轴负方向看去
template<typename _T = float>
inline matrix4x4<_T> frustum(_T z_near, _T z_far, _T fov, _T left, _T right, _T top, _T bottom)
{
	_T m_elem[] = {
			2 * z_near / (right - left), 0, (right + left) / (right - left), 0,
			0, 2 * z_near / (top - bottom), (top + bottom) / (top - bottom), 0,
			0, 0, -(z_far + z_near) / (z_far - z_near), -2 * z_near * z_far / (z_far - z_near),
			0, 0, -1, 0
	};
	return cast<matrix4x4<_T> >(m_elem);
}

//OpenGL的投影矩阵。NDC为左手坐标系，世界坐标系为右手坐标系，znear和zfar取正
template<typename _T = float>
inline matrix4x4<_T> frustum(_T znear, _T zfar, _T fov, _T aspect_ratio)
{
	_T m_elem[] = {
			1 / (tan(fov / 2) * aspect_ratio), 0, 0, 0,
			0, 1 / tan(fov / 2), 0, 0,
			0, 0, -(zfar + znear) / (zfar - znear), -2 * znear * zfar / (zfar - znear),
			0, 0, -1, 0
	};
	return cast<matrix4x4<_T> >(m_elem);
}

}

#endif//_CXXSCI_TRANSFORM
