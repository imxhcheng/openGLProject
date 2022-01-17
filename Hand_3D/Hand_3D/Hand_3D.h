#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Hand_3D.h"
#include <QAction>

class Hand_3D : public QMainWindow
{
    Q_OBJECT

public:
    Hand_3D(QWidget *parent = Q_NULLPTR);

public slots:
	void onFrontView();
	void onSlideView();
	void onTopView();
	void onOpenFile();

signals:
	void sigOpenSurf(const char*);
	void sigFilledCloud();//�����ƺ�·������ʾ����

private:
    Ui::Hand_3DClass ui;

	io* m_pData;
	bool m_bSurfOpen;// ƽ����ļ��Ƿ��

};
