//
// Created by iimt on 21-3-20.
//

/**
 * sukai
 * 虚拟墙编辑
 * 22-01-15
 * 第一次执行前需要 首先执行 rosrun slamservice currencyChassis_CreateSqlDatabaseMain_node 创建数据库
 *
 * rosrun slamservice currencyChassis_KSDemoDlgTABLE_COSTMAPLAYERMain_node
 *  point：点 至少1个点数据, line：线 至少2个点数据, polygons:多边形 至少3个点数据
 */

//#include "KsTestDemo.h"
#include <QtWidgets/QApplication>
#include <QCoreApplication>

#include "currencyChassis_KSDemoDlgTABLE_COSTMAPLAYER.h"
#include <ros/ros.h>
#include <ros/package.h>
using namespace std;

//std::string dbPath = ros::package::getPath("visualorientation")+"/db/TABLE_OR_ROOM_POSE.db";
std::string dbPath ="~/workspace/xuniqiang/src/slamproject/db/TABLE_OR_ROOM_POSE.db";
//主机名
string hostname="iimt";
int main(int argc, char *argv[])
{

    //节点名
    string nodeName = "currencyChassis_KSDemoDlgTABLE_COSTMAPLAYERMain_node";
    //初始化节点
    ros::init(argc,argv,nodeName);

    //获取主机名称

    char hname[128];
    gethostname(hname, sizeof(hname));
    cout<<"hname:"<<hname<<endl;
    //主机名
   // hostname=string(hname);//iimt
    //dbPath ="/home/"+hostname+"/slam/TABLE_OR_ROOM_POSE.db";
    std::string home = getenv("HOME");// /home/iimt
    dbPath =home+"/workspace/xuniqiang/src/slamproject/db/TABLE_OR_ROOM_POSE.db";
    //创建节点
    ros::NodeHandle node;
    QApplication a(argc, argv);

    SlamtecKSDemoDlgCostMap dlg(QString::fromStdString(dbPath), "TABLE_COSTMAPLAYER");    //这里我们在生成KSDemoDlg类的时候，在构造函数中传入sqlite数据库名CONFIG.DB和想要操作的表T_USER_MANAGE

    dlg.show();    //显示一下就OK
    if(ros::isShuttingDown()){
        QApplication::quit();
    }
    return a.exec();
}
