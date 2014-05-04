

#include "Vector.h"

Vector3::Vector3(void)
{
}

Vector3::Vector3(float X, float Y, float Z){
    this->x = X;
    this->y = Y;
    this->z = Z;
}
float Vector3::dotProduct(Vector3 &rhs){
    return (x * rhs.x +
            y * rhs.y +
            z * rhs.z);
}
void Vector3::set(float x,float y,float z){
    this->x=x;
    this->y=y;
    this->z=z;
}
// Returns the length of the vector
float Vector3::Length(){
    return sqrt(x * x + y * y + z * z);
}
    char * Vector3::toString(){
        char * result=new char[100];
        sprintf(result, "%.4f %.4f %.4f", x,y,z);
        return result;

    }
Vector3 Vector3::crossProduct(Vector3 &rhs)
{
    return Vector3( y * rhs.z - z * rhs.y,
                 z * rhs.x - x * rhs.z,
                 x * rhs.y - y * rhs.x);
}
// Normalizes the vector
Vector3 Vector3::Normalize(){

    float length = this->Length();

    if(length != 0){
        x = x/length;
        y = y/length;
        z = z/length;
    }
}

Vector3::~Vector3(void)
{
}

