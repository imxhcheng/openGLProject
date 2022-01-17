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

	//----------�������غ���----------//
public:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();

	void setAngle(float yaw,float roll,float pitch);
protected slots:
	void teardownGL();

	//----------��ʾ����----------//
public:
	bool is_cloud_view;//�����Ƿ�ɼ�
	bool is_pose_view;//λ���Ƿ�ɼ�
	bool is_background_view;//���������Ƿ�ɼ�

	void recv_display_view(int);//���ղ�ͬ����ʾָ��
	void recv_net_size(int);//���յ�������ֱ���

public:
	float* m_pHandPos;	//������
	float* m_pBonePos;	//������
	int* m_pBoneIndice;		//���������������

	float* m_pAxis;//����������
	int* m_pAxisIndice;//��������������

	//��Ҫ����Ϊ�������������
	float* m_Net;//������������
	int* m_NetIndice;//����������������

	void pushDisplay();//�����ƺ�·��������

//-----------��������------------//
private:
	void drawAxis();//����������
	void drawBackGround();//���Ƴ���������
	void draw3DScence();//���Ƶ��Ƴ���

	void drawHand();
	void drawBones();

	void mainDrawFunc(const unsigned int, float*, int*, const int, const int, int, int, int, bool, bool);//�Զ���Ļ���������

//----------�������š�ƽ�Ƽ���ת���-----------//
public:
	//1 ��꼰������Ϣ
	void wheelEvent(QWheelEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

	//2 ������Ϣ
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);

	//3 ��Ա����
	Point3d translate_inc;//��ʾƽ����
	QPoint leftButtonClicked;//����������״̬�µ���һʱ������
	QPoint rightButtonClicked;//����Ҽ�����״̬�µ���һʱ������

	int m_iIsmouseclicked;//��ʶ����Ƿ���, 0��ʾ�ɿ�״̬, 1��ʾ�������, 2��ʾ�����Ҽ�
	int is_view_up_scence;

	float yawAngle;//ƫ����
	float rollAngle;//�����
	float pitchAngle;//�����ǣ�����Ҽ�����

	float zoom;//���ű���

	//4 �Զ��庯��
	void translate_and_rotate(const unsigned int ID);

	//----------��ɫ�����----------//
private:
	int countID;//��ʾ��ǰ�����ĳ���ID
	unsigned int shaderID[4];//����ID, 0��ʾ������1��ʾ����ϵ��2��ʾBones
	string vShaderSource;//������ɫ��
	string fShaderSource;//Ƭ����ɫ��

	void getShaderSource(const char*, string&);//����Դ�ļ��ж�ȡ������ɫ��
	void shaderProgram();//��ɫ��������
	void shaderProgramCloud();//��ɫ��������
	void bindShader();//����ɫ��
	bool checkCompileErrors(unsigned int shader, string type);//���������

//-------------�ļ����----------//
private:
	io* m_pData;

	/*��Ⱦ��*/
	QOpenGLShaderProgram *program;
	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer vbo;

	int nPointsShaderID;
};
