#include "Hand_3D.h"

Hand_3D::Hand_3D(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	m_pData = io::getInstance();
	m_bSurfOpen = false;

	QObject::connect(this, &Hand_3D::sigOpenSurf, m_pData, &io::read_surf_bfile);
	QObject::connect(this, &Hand_3D::sigFilledCloud, ui.openGLWidget, &MyOpenGLWidget::pushDisplay);
	
	connect(ui.actionFront, &QAction::triggered, this, &Hand_3D::onFrontView);
	connect(ui.actionSlide, &QAction::triggered, this, &Hand_3D::onSlideView);
	connect(ui.actionTop, &QAction::triggered, this, &Hand_3D::onTopView);
	connect(ui.actionopenFile, &QAction::triggered, this, &Hand_3D::onOpenFile);
}

void Hand_3D::onFrontView()
{
	ui.openGLWidget->setAngle(0.0, 0.0, 0.0);
}

void Hand_3D::onSlideView()
{
	ui.openGLWidget->setAngle(2.0, 0.0, 0.0);
}

void Hand_3D::onTopView()
{
	ui.openGLWidget->setAngle(0.0, 0.0, 2.0);
}

void Hand_3D::onOpenFile()
{
	QString strFileName = QFileDialog::getOpenFileName(
		this,
		"Open Document",
		QDir::currentPath(),
		"");

	if (!strFileName.isEmpty())
	{
		sigOpenSurf(strFileName.toLocal8Bit());
		m_bSurfOpen = true;
	}

	if (m_bSurfOpen)
		emit sigFilledCloud();

}
