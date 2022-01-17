#ifndef GLOBAL_HEADER_H
#define GLOBAL_HEADER_H

//c++头文件
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <sstream>
using namespace std;

//Qt相关头文件
#include <QDir>
#include <QFile>
#include <QMenu>
#include <QString>
#include <QTextStream>
#include <QMouseEvent>
#include <QFileDialog>
#include <QSpinBox>
#include <QMessageBox>

//openGL窗口相关文件
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>

//glm头文件
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Point3d{
public:
    Point3d(){}
    Point3d(float xx, float yy, float zz):x(xx),y(yy),z(zz){}
public:
    float x;
    float y;
    float z;
    float intensity;

    int index;
};

struct Rect{
    float top;
    float bottom;

    float left;
    float right;
};

#endif // GLOBAL_HEADER_H
