#ifndef ANDROMEDA_MATH_OPERATIONS
#define ANDROMEDA_MATH_OPERATIONS

#define M_PI 3.14159265358979323846

#define getRadianTangent(x,y) (x==0?1.57079632679489661923:(float)(atan(y/x)))
#define getLength3DVector(x,y,z) ((float)sqrt(x*x+y*y+z*z))
#define getBiggerNumber(x,y) (x>y?x:y)
#define getSmallerNumber(x,y) (x<y?x:y)
#define getAbsoluteValue(n) (n<0?-n:n)

#endif //ANDROMEDA_MATH_OPERATIONS
