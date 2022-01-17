#pragma once

#include "global_header.h"
#include "io.h"

#include <QDebug>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

class MyOpenGLWidget : public QOpenGLWidget,QOpenGLFunctions_3_3_Core
{
	Q_OBJECT

public:
	explicit MyOpenGLWidget(QWidget *parent);
	~MyOpenGLWidget();

	void initAxis();
	void initNet();

	//----------界面重载函数----------//
public:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();

	void setAngle(float yaw,float roll,float pitch);
protected slots:
	void teardownGL();

	//----------显示数组----------//
public:
	bool is_cloud_view;//点云是否可见
	bool is_pose_view;//位姿是否可见
	bool is_background_view;//地面网格是否可见

	void recv_display_view(int);//接收不同的显示指令
	void recv_net_size(int);//接收地面网格分辨率

public:
	float* m_pHandPos;	//点数组
	float* m_pBonePos;	//点数组
	int* m_pBoneIndice;		//骨骼点的索引数组

	float* m_pAxis;//坐标轴数组
	int* m_pAxisIndice;//坐标轴索引数组

	//需要更新为三面的网格数组
	float* m_Net;//地面网格数组
	int* m_NetIndice;//地面网格数组索引

	void pushDisplay();//填充点云和路径点数组

//-----------场景绘制------------//
private:
	void drawAxis();//绘制坐标轴
	void drawBackGround();//绘制场景及网格
	void draw3DScence();//绘制点云场景

	void drawHand();
	void drawBones();

	void mainDrawFunc(const unsigned int, float*, int*, const int, const int, int, int, int, bool, bool);//自定义的绘制主函数

//----------场景缩放、平移及旋转相关-----------//
public:
	//1 鼠标及滚轮消息
	void wheelEvent(QWheelEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

	//2 键盘消息
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);

	//3 成员变量
	Point3d translate_inc;//表示平移量
	QPoint leftButtonClicked;//鼠标左键按下状态下的上一时刻坐标
	QPoint rightButtonClicked;//鼠标右键按下状态下的上一时刻坐标

	int m_iIsmouseclicked;//标识鼠标是否按下, 0表示松开状态, 1表示按下左键, 2表示按下右键
	int is_view_up_scence;

	float yawAngle;//偏航角
	float rollAngle;//横滚角
	float pitchAngle;//俯仰角，鼠标右键控制

	float zoom;//缩放比例

	//4 自定义函数
	void translate_and_rotate(const unsigned int ID);

	//----------着色器相关----------//
private:
	int countID;//表示当前操作的程序ID
	unsigned int shaderID[4];//程序ID, 0表示场景，1表示坐标系，2表示Bones
	string vShaderSource;//顶点着色器
	string fShaderSource;//片段着色器

	void getShaderSource(const char*, string&);//从资源文件中读取两个着色器
	void shaderProgram();//着色器主程序
	void shaderProgramCloud();//着色器主程序
	void bindShader();//绑定着色器
	bool checkCompileErrors(unsigned int shader, string type);//检查编译错误

//-------------文件相关----------//
private:
	io* m_pData;

	/*渲染点*/
	QOpenGLShaderProgram *program;
	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer vbo;

	int nPointsShaderID;
};
