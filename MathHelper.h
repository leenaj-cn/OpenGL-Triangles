#ifndef MATH_HELPER
#define MATH_HELPER 

#include <GL\freeglut.h>
#include <math.h>
#include <iostream>

using namespace std;

class MathHelper{
public:
    static void matrixMult4x4 (GLfloat* result, GLfloat* m1, GLfloat* m2);
    static void matrixMult4x4Column (GLfloat* result, GLfloat* m1, GLfloat* m2);
    //单位矩阵
    static void makeIdentity(GLfloat* result);

    static void makeTranslate(GLfloat* result, GLfloat x, GLfloat y, GLfloat z);
    static void translateMatrixBy(GLfloat* result, GLfloat x, GLfloat y, GLfloat z);

    static void makeRotateX(GLfloat* result, GLfloat rotation);
    static void makeRotateY(GLfloat* result, GLfloat rotation);
    static void makeRotateZ(GLfloat* result, GLfloat roration);

    static void makeScale(GLfloat* result, GLfloat x, GLfloat y, GLfloat z);

    static void makePerspectiveMatrix(GLfloat* result, GLfloat fov, GLfloat aspect, GLfloat nearPlane, GLfloat farPlane);

    static void copyMatrix(GLfloat* src, GLfloat* dest);
    static void print4x4Matrix(GLfloat* mat);

};
#endif