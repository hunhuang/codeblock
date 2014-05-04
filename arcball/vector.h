#include <math.h>
#include <stdio.h>
class Vector3
{
public:
    Vector3(void);
    Vector3(float X, float Y, float Z);
    ~Vector3(void);
    float Length();
    Vector3 Normalize();
    float x,y,z;
    float dotProduct(Vector3 &);
    Vector3 crossProduct(Vector3 &);
    char * toString();
    void set(float,float,float);
};
