#ifndef IO_H
#define IO_H
#include "global_header.h"

class io : public QObject{
    Q_OBJECT
public:
    io();
private:
    static io* m_pInstance;
public:
    static io* getInstance();
public:
    void read_pose_bfile(const char*);
    void read_surf_bfile(const char*);
    void read_corner_bfile(const char*);

    void clear();
private:
	void read_bfile(vector<Point3d> &vec);//读取二进制文件
	void read_txtfile(vector<Point3d> &vec);//读取二进制文件
public:
    vector<Point3d> vecPos;
private:
    string m_sreFilePath;
};

#endif // IO_H
