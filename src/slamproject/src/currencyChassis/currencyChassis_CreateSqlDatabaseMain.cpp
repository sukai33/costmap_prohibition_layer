//
// Created by iimt on 21-3-20.
//

/**
 * 苏凯
 * 22-04-21
 * 创建数据库sqllite
 * rosrun slamproject currencyChassis_CreateSqlDatabaseMain_node
 */

#include <QtWidgets/QApplication>
#include <QCoreApplication>
#include <QDebug>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <ros/ros.h>
#include <ros/package.h>
#include <netdb.h>

using namespace std;
std::string MerchantNumber;//商户编号
std::string MerchantName;//商户名称
std::string dbPath = "~/workspace/xuniqiang/src/slamproject/db/TABLE_OR_ROOM_POSE.db";
//主机名
string hostname="iimt";


int main(int argc, char *argv[]) {


    //节点名
    string nodeName = "currencyChassisskSDemoDlgCreateMainNode";
    //初始化节点
    ros::init(argc,argv,nodeName);


    std::string home = getenv("HOME");//
    dbPath =home+"/workspace/xuniqiang/src/slamproject/db/TABLE_OR_ROOM_POSE.db";

      //创建节点
    ros::NodeHandle node;

    QApplication a(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    cout<<"dbPath: "<<dbPath;
    db.setDatabaseName(QString::fromStdString(dbPath));
    if (!db.open()) {
        qDebug()<<"TABLE_OR_ROOM_POSE.db";
        return 0;
    }

/**************************使用QSqlQuery操作数据库**************************/
    QString connectionName = "slamtecKSDemiDIgCreateMaingui";
    QSqlQuery query;    //执行操作类对象

    //创建 点位值 路点值 表格
    QSqlQuery sql_query(connectionName);


    //虚拟墙点位储存     图形类型 FigureType-> point：点 至少1个点数据, line：线 至少2个点数据, polygons:多边形 至少3个点数据

    if(!sql_query.exec("create table TABLE_COSTMAPLAYER(Id int primary key,FigureType text,FigureName text, MerchantNumber text, MerchantName text,RobotNumber text,MapName text,Robotfloor text,Point_A_X text, Point_A_Y text, Point_B_X text, Point_B_Y text, Point_C_X text, Point_C_Y text, Point_D_X  text, Point_D_Y  text)"))
    {
        qDebug() << "Error: Fail to create TABLE_COSTMAPLAYER";
    }
    else
    {
        qDebug() << "TABLE_TASK TABLE_COSTMAPLAYER!";
    }


}

