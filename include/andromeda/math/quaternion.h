#ifndef ANDROMEDA_MATH_QUATERNION
#define ANDROMEDA_MATH_QUATERNION

#include "vector.h"
#include "matrix.h"

namespace andromeda {
	namespace math {
		template<typename T=float>
		class Quaternion
		{
		public:
			T w,x,y,z;

			Quaternion(T w,T x,T y,T z)
			{
				this->w=w;
				this->x=x;
				this->y=y;
				this->z=z;
			}

			Quaternion(T w,Vector<3,T> v)
			{
				this->w=w;
				this->x=v[0];
				this->y=v[1];
				this->z=v[2];
			}

			Quaternion conj()
			{
				return Quaternion(w,-x,-y,-z);
			}

			template<typename T2>
			Quaternion<decltype(std::declval<T>()+std::declval<T2>())> operator+(Quaternion<T2> q)
			{
				return Quaternion<decltype(std::declval<T>()+std::declval<T2>())>(w+q.w,x+q.x,y+q.y,z+q.z);
			}

			template<typename T2>
			Quaternion<decltype(std::declval<T>()-std::declval<T2>())> operator-(Quaternion<T2> q)
			{
				return Quaternion<decltype(std::declval<T>()-std::declval<T2>())>(w-q.w,x-q.x,y-q.y,z-q.z);
			}

			template<typename T2>
			Quaternion<decltype(std::declval<T>()*std::declval<T2>())> operator*(Quaternion<T2> q)
			{
				return Quaternion<decltype(std::declval<T>()*std::declval<T2>())>(w*q.w-x*q.x-y*q.y-z*q.z,y*q.z-q.y*z+w*q.x+q.w*x,z*q.x-q.z*x+w*q.y+q.w*y,x*q.y-q.x*y+w*q.z+q.w*z);
			}

			static Vector<3,T> rotate(Vector<3,T> v,Vector<3,T> axis,T rad)
			{
				auto p=Quaternion(0,v);
				auto q=Quaternion(cos(rad/2.0f),axis*sin(rad/2.0f));
				auto r=q*p*q.conj();
				return {r.x,r.y,r.z};
			}

			//绕轴转动，四元数转换为旋转矩阵
			static inline Matrix4x4<T> R(Vector<3,T> axis,T rad)
			{
				T m_elem[]=
				{
					cos(rad)+axis[0]*axis[0]*(1-cos(rad)),axis[0]*axis[1]*(1-cos(rad))-axis[2]*sin(rad),axis[0]*axis[2]*(1-cos(rad))+axis[1]*sin(rad),0,
					axis[0]*axis[1]*(1-cos(rad))+axis[2]*sin(rad),cos(rad)+axis[1]*axis[1]*(1-cos(rad)),axis[1]*axis[2]*(1-cos(rad))-axis[0]*sin(rad),0,
					axis[0]*axis[2]*(1-cos(rad))-axis[1]*sin(rad),axis[1]*axis[2]*(1-cos(rad))+axis[0]*sin(rad),cos(rad)+axis[2]*axis[2]*(1-cos(rad)),0,
					0,0,0,1
				};
				return andromeda::traits::cast<Matrix4x4<T> >(m_elem);
			}
		};
	}
}

#endif//ANDROMEDA_MATH_QUATERNION
