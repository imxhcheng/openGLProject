#include "io.h"
io* io::m_pInstance = new io;

io::io(){
    m_sreFilePath.clear();
}

io* io::getInstance(){
    return m_pInstance;
}

void io::read_bfile(vector<Point3d>& vec){
    //cout << filepath << endl;

    ifstream fin(m_sreFilePath.c_str(), ios::in | ios::binary | ios::app);
    if(!fin.is_open()){
        cout << "fail to read file, please check whether it exists!" << endl;
        return;
    }
    Point3d node;
    while(fin.read((char*)&node, sizeof(Point3d)))
        vec.push_back(node);

    fin.close();
}

void io::read_txtfile(vector<Point3d> &vec)
{
	QFile txtFile(QString::fromLocal8Bit(m_sreFilePath.c_str()));

	if (txtFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{

		QString strTxt = txtFile.readAll();

		QStringList lstTxt = strTxt.split(" ");

		for (int i = 0; i < lstTxt.size() / 3; i++)
		{
			Point3d pos;

			pos.x = lstTxt[i].toFloat();
			pos.y = lstTxt[i + 1].toFloat();
			pos.z = lstTxt[i + 2].toFloat();

			vec.push_back(pos);
		}
	}
}


void io::read_surf_bfile(const char* path){
    m_sreFilePath = path;
    read_txtfile(vecPos);
}

void io::read_corner_bfile(const char* path){
    m_sreFilePath = path;
    read_bfile(vecPos);
}

void io::clear()
{
    vecPos.clear();
}
