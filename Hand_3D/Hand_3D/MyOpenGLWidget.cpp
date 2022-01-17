#include "MyOpenGLWidget.h"

//
//GLSL3.0�汾��,������attribute�ؼ��֣��Լ�varying�ؼ��֣������Ա���ͳһ��in/out��Ϊǰ�ùؼ���
#define GL_VERSION  "#version 330 core\n"
#define GLCHA(x)  #@x           //�ӵ�����,��x��Ϊ�ַ�
#define GLSTR(x)  #x            //��˫���ţ���x��Ϊ�ַ���
#define GET_GLSTR(x) GL_VERSION#x


const char *vsrc = GET_GLSTR(
	layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 ourColor;
void main(void)
{
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	ourColor = aColor;
}
);

const char *fsrc = GET_GLSTR(

	out vec4 FragColor;
in vec3 ourColor;
void main(void)
{
	FragColor = vec4(ourColor, 1.0f);
}
);


const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -30.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 2.0f, 0.0f);
 
//�����������
const int netWidth = 800;//��800��
const int netHeight = 600;//��600��
int netResolution = 50;//����ֱ���Ϊ20��

const float PI = 3.141593;

MyOpenGLWidget::MyOpenGLWidget(QWidget *parent)
	: QOpenGLWidget(parent)
{

	setMouseTracking(true);

	//����OpenGL�İ汾��Ϣ
	QSurfaceFormat format;
	format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setVersion(3, 3);
	setFormat(format);

	is_cloud_view = is_pose_view = is_background_view = true;

	countID = 0;
	zoom = 1.0;
	m_iIsmouseclicked = 0;
	is_view_up_scence = 0;

	yawAngle = 0.0;
	rollAngle = 0.0;

	translate_inc.x = 0;
	translate_inc.y = -0.6;

	m_pData = io::getInstance();

	initAxis();
	initNet();

	
}

MyOpenGLWidget::~MyOpenGLWidget()
{
	if (!m_pAxis)
		delete[] m_pAxis;

	if (!m_pAxisIndice)
		delete[] m_pAxisIndice;

	if (!m_Net)
		delete[] m_Net;

	if (!m_NetIndice)
		delete[] m_NetIndice;

	if (!m_pHandPos)
		delete[] m_pHandPos;

	if (!m_pBonePos)
		delete[] m_pBonePos;

	if (!m_pBoneIndice)
		delete[] m_pBoneIndice;
}

void MyOpenGLWidget::initAxis()
{
	// �������������
	m_pAxis = new float[36];
	m_pAxisIndice = new int[6];
	for (size_t i = 0; i < 36; ++i)
		m_pAxis[i] = 0;

	m_pAxis[18] = m_pAxis[25] = m_pAxis[32] = 0.08f;
	m_pAxis[3] = m_pAxis[10] = m_pAxis[17] = 
	m_pAxis[21] = m_pAxis[28] = m_pAxis[35] = 1.0f;

	for (int i = 0; i < 3; ++i) 
	{
		m_pAxisIndice[2 * i] = i;
		m_pAxisIndice[2 * i + 1] = i + 3;
	}

	for (int i = 0; i < 3; ++i) 
	{

		qDebug() <<  m_pAxisIndice[2 * i];
		qDebug()<< m_pAxisIndice[2 * i + 1];
	}

}

void MyOpenGLWidget::initNet()
{
	// �����������
	int w = netWidth / netResolution + 1;
	int h = netWidth / netResolution + 1;

	m_Net = new float[6 * (w + h - 2)];
	m_NetIndice = new int[2 * (w + h)];

	Rect rect;
	rect.left = -2.0 * (w / 2.0) / (w - 1);
	rect.right = 2.0 * (w / 2.0) / (w - 1);

	rect.top = 2.0 * (h / 2.0) / (h - 1);
	rect.bottom = -2.0 * (h / 2.0) / (h - 1);

	double delta_x = (rect.right - rect.left) / (w - 1);
	double delta_z = (rect.top - rect.bottom) / (h - 1);

	//����ѹ������ֻ�洢�ھ��α����ϵĶ���
	float cal_x = 0.0, cal_z = 0.0;
	for (int i = 0; i < 2 * (w + h - 2); ++i)
	{
		if (i < w)
		{//��һ��
			cal_x = rect.left + delta_x * i;
			cal_z = rect.bottom;
		}
		else
		{
			if (i < w + 2 * (h - 2))
			{//�м�
				static int count = 1;
				if (0 == ((i - w) % 2))
				{//��1��
					cal_x = rect.left;
				}
				else
				{//���һ��
					cal_x = rect.right;
				}
				cal_z = rect.bottom + delta_z * count;

				if (1 == ((i - w) % 2)) count++;
			}
			else
			{//���һ��
				int count = i - w - 2 * (h - 2);
				cal_x = rect.left + delta_x * count;
				cal_z = rect.top;
			}
		}

		m_Net[3 * i] = cal_x;
		m_Net[3 * i + 1] = 0.0;
		m_Net[3 * i + 2] = cal_z;
	}
	//������������
	int front = 0, rear = 0;
	for (size_t i = 0; i < w + h; ++i)
	{
		if (i < w)
		{
			front = i;
			rear = front + w + 2 * (h - 2);
		}
		else if (i == w)
		{
			m_NetIndice[2 * i] = 0;
			m_NetIndice[2 * i + 1] = w - 1;
			continue;
		}
		else if ((w + h - 1) == i)
		{
			front = w + 2 * (h - 2);
			rear = 2 * (w + h - 2) - 1;
		}
		else
		{
			static int count = w;
			front = count;
			rear = front + 1;
			count += 2;
		}
		m_NetIndice[2 * i] = front;
		m_NetIndice[2 * i + 1] = rear;
	}
}

void MyOpenGLWidget::initializeGL()
{
#if 1

	// Ϊ��ǰ������ʼ��OpenGL����
	initializeOpenGLFunctions();

	// ������ɫ������
	shaderProgram();
	shaderProgramCloud();
#endif

#if 0
	// Ϊ��ǰ������ʼ��OpenGL����
	initializeOpenGLFunctions();


	getShaderSource(":/Shaders/shaders/cloud.vert", vShaderSource);
	getShaderSource(":/Shaders/shaders/cloud.frag", fShaderSource);

	const char* vertexShaderSource = vShaderSource.c_str();
	const char* fragmentShaderSource = fShaderSource.c_str();

	// �༭������ɫ��
	unsigned int vertex, fragment;

	// ������ɫ��
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexShaderSource, NULL);
	glCompileShader(vertex);

	// �����ɫ���������
	checkCompileErrors(vertex, "VERTEX");

	// Ƭ����ɫ��
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragment);

	// �����ɫ���������
	checkCompileErrors(fragment, "FRAGMENT");

	// ������ɫ������
	nShaderID = glCreateProgram();
	glAttachShader(nShaderID, vertex);
	glAttachShader(nShaderID, fragment);
	glLinkProgram(nShaderID);

	// ������Ӵ���
	checkCompileErrors(nShaderID, "PROGRAM");

	// ɾ����ɫ��
	glDeleteShader(vertex);
	glDeleteShader(fragment);


	
#endif

}

void MyOpenGLWidget::resizeGL(int width, int height)
{
	// Currently we are not handling width/height changes
	(void)width;
	(void)height;
}

void MyOpenGLWidget::paintGL()
{
#if 1

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// ������Ȳ���
	glEnable(GL_DEPTH_TEST);

	// ��ȾShader
	drawAxis();
	drawBackGround();
	draw3DScence();


#endif

#if 0

	// ����
   // glViewport(0, 0, width(), height());
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT);

	vbo.create();
	vbo.bind();              //�󶨵���ǰ��OpenGL������,
	vbo.allocate(cloud, 6 * data->cloud.size() * sizeof(GLfloat));
	vbo.setUsagePattern(QOpenGLBuffer::StreamDraw);  //����Ϊһ���޸ģ����ʹ��


	//5.��ʼ��VAO,���ö�������״̬(���㣬���ߣ����������)
	vao.create();
	vao.bind();

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//6.������ж���
	vao.release();
	vbo.release();

	// ��ȾShader
	glUseProgram(nShaderID);
	translate_and_rotate(2);


	vao.bind();      //�󶨼���vao

	glPointSize(3);
	glDrawArrays(GL_POINTS, 0, data->cloud.size());    //����3������,��ʽΪ������
	//glDrawArrays(GL_LINE_LOOP, 0, 3);    //����3������,��ʽΪ������

	vao.release();       //���

#endif

}

void MyOpenGLWidget::setAngle(float yaw, float roll, float pitch)
{
	yawAngle = yaw;
	rollAngle = roll;
	pitchAngle = pitch;

	update();
}

void MyOpenGLWidget::teardownGL()
{
	// Currently we have no data to teardown
}

void MyOpenGLWidget::recv_display_view(int cmd)
{
	switch (cmd) {
	case 0:
		is_cloud_view = true;
		break;
	case 1:
		is_cloud_view = false;
		break;
	case 2:
		is_pose_view = true;
		break;
	case 3:
		is_pose_view = false;
		break;
	case 4:
		is_background_view = true;
		break;
	case 5:
		is_background_view = false;
		break;
	default:
		break;
	}
	update();
}

void MyOpenGLWidget::recv_net_size(int value)
{
	netResolution = value;
	update();
}

void MyOpenGLWidget::pushDisplay()
{
	//����ϵת�� ���ֵ�����
	vector<Point3d> vecRightHand;
	for (int i=0;i<m_pData->vecPos.size();i++)
	{
		Point3d pos3D;
		pos3D.x = m_pData->vecPos[i].x;
		pos3D.y = m_pData->vecPos[i].y;
		pos3D.z = -1*m_pData->vecPos[i].z;

		vecRightHand.push_back(pos3D);
	}

	//�ҵ���С�ĵ�
	float min_fX = vecRightHand[0].x;
	float min_fY = vecRightHand[0].y;
	float min_fZ = vecRightHand[0].z;
	for (int i = 0; i < vecRightHand.size(); i++)
	{
		if (vecRightHand[i].x < min_fX)
		{
			min_fX = vecRightHand[i].x;
		}

		if (vecRightHand[i].y < min_fY)
		{
			min_fY = vecRightHand[i].y;
		}

		if (vecRightHand[i].z < min_fZ)
		{
			min_fZ = vecRightHand[i].z;
		}
	}

	//���еĵ����ƽ��
	for (int i = 0; i < vecRightHand.size(); i++)
	{
		vecRightHand[i].x = vecRightHand[i].x - min_fX+50;
		vecRightHand[i].y = vecRightHand[i].y - min_fY+50;
		vecRightHand[i].z = vecRightHand[i].z - min_fZ+50;
	}

	// ����������
	m_pHandPos = new float[6 * vecRightHand.size()];
	m_pBonePos = new float[3 * vecRightHand.size()];
	float xx = 0.0, yy = 0.0, zz = 0.0;
	for (size_t i = 0; i < vecRightHand.size(); ++i) 
	{
		m_pHandPos[6 * i] = vecRightHand[i].x;
		m_pHandPos[6 * i + 1] = vecRightHand[i].y;
		m_pHandPos[6 * i + 2] = vecRightHand[i].z;

		m_pHandPos[6 * i + 3] = 1.0;
		m_pHandPos[6 * i + 4] = 1.0;
		m_pHandPos[6 * i + 5] = 1.0;

		xx = (xx > fabs(vecRightHand[i].x)) ? xx : fabs(vecRightHand[i].x);
		yy = (yy > fabs(vecRightHand[i].y)) ? yy : fabs(vecRightHand[i].y);
		zz = (zz > fabs(vecRightHand[i].z)) ? zz : fabs(vecRightHand[i].z);
	}

	for (size_t i = 0; i < vecRightHand.size(); ++i)
	{
		m_pHandPos[6 * i] /= (xx+50);
		m_pHandPos[6 * i + 1] /= (yy+50);
		m_pHandPos[6 * i + 2] /= (zz+50);


		m_pBonePos[3 * i] = m_pHandPos[6 * i];
		m_pBonePos[3 * i+1] = m_pHandPos[6 * i+1];
		m_pBonePos[3 * i+2] = m_pHandPos[6 * i+2];


		if (i == 0)
		{
			m_pHandPos[6 * i + 3] = 1.0;
			m_pHandPos[6 * i + 4] = 0.6;
			m_pHandPos[6 * i + 5] = 0.0;
		}
		else if (1 <= i && i <= 4)
		{
			m_pHandPos[6 * i + 3] = 1.0;
			m_pHandPos[6 * i + 4] = 0.0;
			m_pHandPos[6 * i + 5] = 0.0;
		}
		else if (5 <= i && i <= 8)
		{
			m_pHandPos[6 * i + 3] = 1.0;
			m_pHandPos[6 * i + 4] = 1.0;
			m_pHandPos[6 * i + 5] = 0.0;
		}
		else if (9 <= i && i <= 12)
		{
			m_pHandPos[6 * i + 3] = 0.0;
			m_pHandPos[6 * i + 4] = 0.0;
			m_pHandPos[6 * i + 5] = 1.0;
		}
		else if (13 <= i && i <= 16)
		{
			m_pHandPos[6 * i + 3] = 0.0;
			m_pHandPos[6 * i + 4] = 1.0;
			m_pHandPos[6 * i + 5] = 0.0;
		}
		else if (17 <= i && i <= 20)
		{
			m_pHandPos[6 * i + 3] = 1,0;
			m_pHandPos[6 * i + 4] = 0.0;
			m_pHandPos[6 * i + 5] = 1.0;
		}
	}

	//���������߶�
	m_pBoneIndice = new int[2 * (4 * 5)];
	for (int i = 1; i <= 4*5*2; ++i) 
	{
		if (i % 2 == 0)
		{
			m_pBoneIndice[i-1] = i / 2;
		}
		else
		{
			if (i%8 == 1)
			{
				m_pBoneIndice[i-1] = 0;
			}
			else
			{
				m_pBoneIndice[i-1] = i / 2;
			}
		}
	}

	//for (int i = 0; i < 4 * 5 * 2; ++i)
	//{
	//	qDebug() << m_pBoneIndice[i];
	//}

	//ˢ����ʾ
	update();
}

void MyOpenGLWidget::drawAxis()
{
	int vlen = 6 * 6 * sizeof(float);
	int ilen = 6 * sizeof(int);
	int glen = 6;
	mainDrawFunc(1, m_pAxis, m_pAxisIndice, vlen, ilen, glen, 3, 6, true, true);
}

void MyOpenGLWidget::draw3DScence()
{
	if (m_pData->vecPos.size() > 0)
	{
		drawBones();
		drawHand();
	}
}

void MyOpenGLWidget::drawBackGround()
{
	if (false == is_background_view)
		return;
	int w = netWidth / netResolution + 1;
	int h = netWidth / netResolution + 1;

	int vlen = 6 * (w + h - 2) * sizeof(float);
	int glen = 2 * (w + h);
	int ilen = glen * sizeof(int);

	mainDrawFunc(0, m_Net, m_NetIndice, vlen, ilen, glen, 3, 3, true, false);
}

void MyOpenGLWidget::drawHand()
{
	vbo.create();
	vbo.bind();              //�󶨵���ǰ��OpenGL������,
	vbo.allocate(m_pHandPos, 6 * m_pData->vecPos.size() * sizeof(GLfloat));
	vbo.setUsagePattern(QOpenGLBuffer::StreamDraw);  //����Ϊһ���޸ģ����ʹ��

	//5.��ʼ��VAO,���ö�������״̬(���㣬���ߣ����������)
	vao.create();
	vao.bind();

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//6.������ж���
	vao.release();
	vbo.release();

	// ��ȾShader
	glUseProgram(nPointsShaderID);
	translate_and_rotate(2);
	vao.bind();      //�󶨼���vao

	glPointSize(4);
	glDrawArrays(GL_POINTS, 0, m_pData->vecPos.size());    //����3������,��ʽΪ������

	vao.release();       //���
}

void MyOpenGLWidget::drawBones()
{
	int vlen = 3 * m_pData->vecPos.size() * sizeof(float);
	int glen = 2 * (4 * 5);
	int ilen = glen * sizeof(int);

	mainDrawFunc(0, m_pBonePos, m_pBoneIndice, vlen, ilen, glen, 3, 3, true, false);
}

//�Զ���Ļ��Ƴ�����������
/*�������ͣ�
 * ����1: ����ID
 * ����2-3: �����������������
 * ����4-6: �����������Դ�С����һ���������������������ڼ���������ɣ����������鳤�Ⱥ�������Ƶ�ͼԪ��Ŀ
 * ����7-8: ��������ƫ��������������ƫ����
 * ����9-10: �Ƿ�ʹ���������Ƿ�ʹ��layout = 1�˿�
 */
void MyOpenGLWidget::mainDrawFunc(const unsigned int ID, 
	float* vertices, int* indices, 
	const int vsize, const int isize, int gnumber,
	int vbias, int ibias,
	bool is_index_need, bool is_port_1)
{
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// ��
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vsize, vertices, GL_STATIC_DRAW);

	if (true == is_index_need) 
	{
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, isize, indices, GL_STATIC_DRAW);
	}

	// λ��0
	glVertexAttribPointer(0, vbias, GL_FLOAT, GL_FALSE, ibias * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// ��ɫ1
	if (true == is_port_1) 
	{
		glVertexAttribPointer(1, vbias, GL_FLOAT, GL_FALSE, ibias * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(shaderID[ID]);
	translate_and_rotate(ID);

	glBindVertexArray(VAO);

	if (ID < 2)
	{
		glDrawElements(GL_LINES, gnumber, GL_UNSIGNED_INT, 0);
	}
	else
	{
		qDebug() << gnumber;
		glPointSize(2);
		glDrawArrays(GL_POINTS, 0, gnumber);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	if (m_pData->vecPos.size() > 0)
		glDeleteBuffers(1, &EBO);
}

void MyOpenGLWidget::wheelEvent(QWheelEvent *event)
{
	if (event->delta() > 0)
		zoom *= 1.05;
	else
		zoom *= 0.95;
	if (zoom < 0.2)
		zoom = 0.2;
	update();
}

void MyOpenGLWidget::mousePressEvent(QMouseEvent *event)
{
	if (Qt::LeftButton == event->button()) {
		m_iIsmouseclicked = 1;
		leftButtonClicked = event->pos();
	}
	if (Qt::RightButton == event->button()) {
		rightButtonClicked = event->pos();
		m_iIsmouseclicked = 2;
	}
}

void MyOpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
	m_iIsmouseclicked = 0;
	update();
}

void MyOpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (1 == m_iIsmouseclicked) 
	{
		float x = event->pos().x() - leftButtonClicked.x();
		float y = event->pos().y() - leftButtonClicked.y();

		yawAngle += (PI * x / this->rect().width());
		rollAngle += (PI * y / this->rect().height());

		leftButtonClicked = event->pos();
		update();
	}
	else if (2 == m_iIsmouseclicked) 
	{
		float x = event->pos().x() - rightButtonClicked.x();
		if (x > 0)
			yawAngle += 0.05;
		if (x < 0)
			yawAngle -= 0.05;

		rightButtonClicked = event->pos();
		update();
	}
}

void MyOpenGLWidget::keyPressEvent(QKeyEvent *event)
{
	if (Qt::Key_Up == event->key())
		translate_inc.y += (0.05f / zoom);
	if (Qt::Key_Down == event->key())
		translate_inc.y -= (0.05f / zoom);
	if (Qt::Key_Left == event->key())
		translate_inc.x -= (0.05f / zoom);
	if (Qt::Key_Right == event->key())
		translate_inc.x += (0.05f / zoom);
	update();
}

void MyOpenGLWidget::keyReleaseEvent(QKeyEvent *event)
{
	update();
}

void MyOpenGLWidget::translate_and_rotate(const unsigned int ID)
{
	// ͶӰ����
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	// �ӱ任���󣬸�������
	glm::mat4 view = glm::mat4();

	// ģ�ͱ任���󣬸���ƽ�ƺ���ת
	glm::mat4 model = glm::mat4();

	if (1 == ID) 
	{
		view = glm::lookAt(cameraPos, cameraFront, cameraUp);
		model = glm::translate(model, glm::vec3(-0.45f, -0.35f, glm::length(cameraPos) - 1.0f));
	}
	else 
	{
		model = glm::translate(model, glm::vec3(translate_inc.x, translate_inc.y, 0));
		view = glm::lookAt(cameraPos / zoom, cameraFront, cameraUp);
	}

	model = glm::rotate(model, rollAngle, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, yawAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, pitchAngle, glm::vec3(0.0f, 0.0f, 1.0f));

	unsigned int modelLoc = glGetUniformLocation(shaderID[ID], "model");
	unsigned int viewLoc = glGetUniformLocation(shaderID[ID], "view");
	unsigned int projectLoc = glGetUniformLocation(shaderID[ID], "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, &projection[0][0]);
}

void MyOpenGLWidget::getShaderSource(const char* path, string& str)
{
	QFile file(path);
	file.open(QIODevice::ReadOnly);
	QTextStream in(&file);
	// ���ı�����ȡ���ַ����У�
	str = in.readAll().toStdString();
	file.close();
}

void MyOpenGLWidget::shaderProgram()
{
	//��ȡ��ɫ��������Ϣ
	//������ɫ��
	getShaderSource(":/Shaders/shaders/scence.vert", vShaderSource);
	getShaderSource(":/Shaders/shaders/scence.frag", fShaderSource);
	bindShader();

	// ��������ɫ��
	getShaderSource(":/Shaders/shaders/axis.vert", vShaderSource);
	getShaderSource(":/Shaders/shaders/axis.frag", fShaderSource);
	bindShader();


	// Pose��ɫ��
	getShaderSource(":/Shaders/shaders/pose.vert", vShaderSource);
	getShaderSource(":/Shaders/shaders/pose.frag", fShaderSource);
	bindShader();
}

void MyOpenGLWidget::shaderProgramCloud()
{
	getShaderSource(":/Shaders/shaders/cloud.vert", vShaderSource);
	getShaderSource(":/Shaders/shaders/cloud.frag", fShaderSource);

	const char* vertexShaderSource = vShaderSource.c_str();
	const char* fragmentShaderSource = fShaderSource.c_str();

	// �༭������ɫ��
	unsigned int vertex, fragment;

	// ������ɫ��
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexShaderSource, NULL);
	glCompileShader(vertex);

	// �����ɫ���������
	checkCompileErrors(vertex, "VERTEX");

	// Ƭ����ɫ��
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragment);

	// �����ɫ���������
	checkCompileErrors(fragment, "FRAGMENT");

	// ������ɫ������
	nPointsShaderID = glCreateProgram();
	glAttachShader(nPointsShaderID, vertex);
	glAttachShader(nPointsShaderID, fragment);
	glLinkProgram(nPointsShaderID);

	// ������Ӵ���
	checkCompileErrors(nPointsShaderID, "PROGRAM");

	// ɾ����ɫ��
	glDeleteShader(vertex);
	glDeleteShader(fragment);

}

void MyOpenGLWidget::bindShader()
{
	if (countID > 3) countID = 3;
	const char* vertexShaderSource = vShaderSource.c_str();
	const char* fragmentShaderSource = fShaderSource.c_str();

	// �༭������ɫ��
	unsigned int vertex, fragment;

	// ������ɫ��
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexShaderSource, NULL);
	glCompileShader(vertex);

	// �����ɫ���������
	checkCompileErrors(vertex, "VERTEX");

	// Ƭ����ɫ��
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragment);

	// �����ɫ���������
	checkCompileErrors(fragment, "FRAGMENT");

	// ������ɫ������
	shaderID[countID] = glCreateProgram();
	glAttachShader(shaderID[countID], vertex);
	glAttachShader(shaderID[countID], fragment);
	glLinkProgram(shaderID[countID]);

	// ������Ӵ���
	checkCompileErrors(shaderID[countID], "PROGRAM");

	// ɾ����ɫ��
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	countID++;
}

bool MyOpenGLWidget::checkCompileErrors(unsigned int shader, string type)
{
	int success;
	char infoLog[512];
	if (type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "---" << infoLog << endl;
			return false;
		}
	}
	else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 512, NULL, infoLog);
			cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "---" << infoLog << endl;
			return false;
		}
	}
	return true;
}
