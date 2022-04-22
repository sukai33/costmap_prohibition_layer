
/**
 * sukai
 * 数据库 sqllite
 *
 * 22-04-21
 * 数据库操作
 * rosrun slamproject currencyChassis_sqlDatabaseMain_node
 * 这个类会自己启动 mapserver 节点
 */


#include <QtWidgets/QApplication>
#include <QCoreApplication>
#include <QDebug>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <ros/ros.h>
#include <ros/package.h>
#include "disinfect_srvs/testInfo_srvs.h"
#include "disinfect_msg/testInfo.h"
#include <disinfect_srvs/Table_costmaplayer_srvs.h>
#include <std_msgs/String.h>
#include <unistd.h>
#include <QTimer>
#include <queue>
#include <mutex>
#include <boost/thread/thread.hpp>
#include <netdb.h>
#include <time.h>




using namespace std;
std::string dbPath = "/workspace/xuniqiang/src/slamproject/db/TABLE_OR_ROOM_POSE.db";//

int time_int=50;

struct SqlSendMessage
{

    disinfect_srvs::testInfo_srvsRequest request;
    disinfect_srvs::testInfo_srvsResponse response;
};



//指令队列
queue<SqlSendMessage> instructionQueue;
mutex *m = new mutex;
//操作sql日志
void logWriteSql(string type,int id,string status){
    try{


    }catch (std::exception e){
       // ROS_ERROR(e.what());

    }

}
//操作sql日志
void logWriteSqlString(string data){
    try{


    }catch (std::exception e){
      //  ROS_ERROR(e.what());

    }

}



// ============start============虚拟墙=======：TABLE_COSTMAPLAYER=========FigureType-> point：点 至少1个点数据, line：线 至少2个点数据, polygons:多边形 至少3个点数据==========================================


void query_disinfect_msg_Table_CostMapPlayer(disinfect_msg::Table_CostMapLayer &t,QSqlQuery &query){

    t.Id=query.value("Id").toInt();//执行路点任务id号
    t.FigureType = query.value("FigureType").toString().toStdString();//图形类型 point：点 至少1个点数据, line：线 至少2个点数据, polygons:多边形 至少3个点数据
    t.FigureName = query.value("FigureName").toString().toStdString();//虚拟墙线或点名字
    t.MerchantName = query.value("MerchantName").toString().toStdString();// 商户名称
    t.MerchantNumber = query.value("MerchantNumber").toString().toStdString();// 商户编号 客户编号 比如龙源或者某个餐饮店比如海底捞1号店【每家店铺可能包含10台机器人，它们在同一个场景运行所以它们的客户编号都是一致的（方便多机协作）】
    t.MapName = query.value("MapName").toString().toStdString();//地图名字
    t.Robotfloor = query.value("Robotfloor").toString().toStdString();//地图楼层
    t.RobotNumber = query.value("RobotNumber").toString().toStdString();//自研底盘机器人编号： longyuan_wt_00001；思岚机器人编号 SLAMWARE-812370； WT5LS103194： 云迹 机器人编号 ；
    t.POINT_A_X = query.value("POINT_A_X").toString().toStdString();//第一个点位A
    t.POINT_A_Y = query.value("POINT_A_Y").toString().toStdString();//第一个点位A
    t.POINT_B_X = query.value("POINT_B_X").toString().toStdString();//第二个点位B
    t.POINT_B_Y = query.value("POINT_B_Y").toString().toStdString();//第二个点位B
    t.POINT_C_X = query.value("POINT_C_X").toString().toStdString();//第三个点位C
    t.POINT_C_Y = query.value("POINT_C_Y").toString().toStdString();//第三个点位C
    t.POINT_D_X = query.value("POINT_D_X").toString().toStdString();//第四个点位D
    t.POINT_D_Y = query.value("POINT_D_Y").toString().toStdString();//第四个点位D

}
//删除数据ByFigureName
bool deleteTaskPOSEFromTABLE_CostMapPlayerByFigureName(disinfect_srvs::testInfo_srvsRequest &request){
    bool fig= false;
    try{
        QString connectionName = "currencyChassisdeleteTaskPOSEFromTABLE_CostMapPlayerByFigureName";

        QSqlQuery query(connectionName);

//============================5.1 更新数据库 ============================

//更新数据 DELETE FROM COMPANY WHERE ID = 7
        QString deleteSql ="DELETE FROM TABLE_COSTMAPLAYER WHERE FigureName='"+QString::fromStdString(request.request_Table_CostMapLayer.FigureName)+"'";
        //qDebug()<<"deleteSql: "<<deleteSql;
        //更改
        query.prepare(deleteSql);
        fig=query.exec();

    }catch (std::exception e){
        //   ROS_ERROR(e.what());

    }
    return fig;
}
//删除数据byId
bool deleteTaskPOSEFromTABLE_CostMapPlayer(disinfect_srvs::testInfo_srvsRequest &request){
    bool fig= false;
    try{
        QString connectionName = "currencyChassisdeleteTaskPOSEFromCostMapPlayer";

        QSqlQuery query(connectionName);

//============================5.1 更新数据库 ============================

//更新数据 DELETE FROM COMPANY WHERE ID = 7
        QString deleteSql ="DELETE FROM TABLE_COSTMAPLAYER WHERE Id="+QString::number(request.request_Table_CostMapLayer.Id);
        //qDebug()<<"deleteSql: "<<deleteSql;

        //qDebug()<<"==================request.request_TaskPOSE.Id==============: "<<request.request_TaskPOSE.Id;
//更改
        query.prepare(deleteSql);
        fig=query.exec();

    }catch (std::exception e){
     //   ROS_ERROR(e.what());

    }
    return fig;
}
//读取数据库最大的一个id号
void queryMaximunIdFromTABLE_CostMapPlayer(disinfect_srvs::testInfo_srvsResponse &response){

    try {
        QString connectionName = "currencyChassisQueryMaximunIdFromCostMapPlayer";

        QSqlQuery query(connectionName);

        //sukai 查询数据
        query.prepare("SELECT * FROM TABLE_COSTMAPLAYER order by Id desc limit 1");
        query.exec();
        QSqlRecord recode1 = query.record();        //recode保存查询到一些内容信息，如表头、列数等等
        int column1 = recode1.count();            //获取读取结果的列数
        QString s11 = recode1.fieldName(0);        //获取第0列的列名
        int id;
        while (query.next()) {
            id = query.value("Id").toInt();
        }

        if(id<0 || id> 2754433){//刚建数据库的id数据有问题
            id=0;
        }else{
            id=id+1;
        }
        response.result_maximunId=id;

    }catch (exception e){
        //qDebug()<<"数据库操作异常请检查文件是否存在！" ,"send";
    }

}
//保存虚拟墙数据
bool saveTASKFromTABLE_CostMapPlayer(disinfect_srvs::testInfo_srvsRequest &request){
    bool fig= false;
    try{
        //查询字典数据
       // selectAllFromTABLE_DICTIONARES();
        //qDebug()<<"===========================保存数据 ===========================";
        QString connectionName = "currencyChassissaveTASKFromCostMapPlayer";

        QSqlQuery query(connectionName);
        int id=0; //自增长id
         if(request.request_Table_CostMapLayer.Id<=0){
             //sukai 查询数据,id自增
             query.prepare("SELECT * FROM TABLE_COSTMAPLAYER order by Id desc limit 1");
             query.exec();
             QSqlRecord recode1 = query.record();        //recode保存查询到一些内容信息，如表头、列数等等
             int column1 = recode1.count();            //获取读取结果的列数
             QString s11 = recode1.fieldName(0);        //获取第0列的列名

             while (query.next()) {
                 id = query.value("Id").toInt();
             }

             if(id<0 || id> 2754433){//刚建数据库的id数据有问题
                 id=0;
             }else{
                 id=id+1;
             }
         }else{
             id =request.request_Table_CostMapLayer.Id;
         }

        //============================ 保存数据库 ============================
        //  //虚拟墙点位储存     图形类型 FigureType-> point：点 至少1个点数据, line：线 至少2个点数据, polygons:多边形 至少3个点数据
        QString installSql ="INSERT INTO TABLE_COSTMAPLAYER (Id, FigureType,FigureName,MerchantName, MerchantNumber,MapName,Robotfloor,RobotNumber,POINT_A_X,POINT_A_Y,POINT_B_X,POINT_B_Y,POINT_C_X,POINT_C_Y,POINT_D_X,POINT_D_Y) VALUES (:Id,:FigureType,:FigureName, :MerchantName, :MerchantNumber,:MapName,:Robotfloor,:RobotNumber,:POINT_A_X,:POINT_A_Y,:POINT_B_X,:POINT_B_Y,:POINT_C_X,:POINT_C_Y,:POINT_D_X,:POINT_D_Y)";

        //qDebug()<<"installSql: "<<installSql;
        //插入数据
        query.prepare(installSql);

        query.bindValue(":Id", id);//id唯一号
        if(QString::fromStdString(request.request_Table_CostMapLayer.FigureType)!="EMPTY"){
            query.bindValue(":FigureType", QString::fromStdString(request.request_Table_CostMapLayer.FigureType));//图形类型 point：点 至少1个点数据, line：线 至少2个点数据, polygons:多边形 至少3个点数据
        }
        if(QString::fromStdString(request.request_Table_CostMapLayer.FigureName)!="EMPTY"){
            query.bindValue(":FigureName", QString::fromStdString(request.request_Table_CostMapLayer.FigureName));//虚拟墙线或点名字
        }
        //通过字典获取
       // query.bindValue(":MerchantName", QString::fromStdString(MerchantName));// 商户名称
       // query.bindValue(":MerchantNumber",QString::fromStdString(MerchantNumber));// 商户编号 客户编号 比如龙源或者某个餐饮店比如海底捞1号店【每家店铺可能包含10台机器人，它们在同一个场景运行所以它们的客户编号都是一致的（方便多机协作）】
       // query.bindValue(":MapName", QString::fromStdString(MapName));//地图名字
       // query.bindValue(":Robotfloor",QString::fromStdString(Robotfloor) );//地图楼层
       // query.bindValue(":RobotNumber", QString::fromStdString(RobotNumber));//自研底盘机器人编号： longyuan_wt_00001；思岚机器人编号 SLAMWARE-812370； WT5LS103194： 云迹 机器人编号 ；
       //--------------------------------------------------
        query.bindValue(":MerchantName", "EMPTY");// 商户名称
        query.bindValue(":MerchantNumber","EMPTY");// 商户编号 客户编号 比如龙源或者某个餐饮店比如海底捞1号店【每家店铺可能包含10台机器人，它们在同一个场景运行所以它们的客户编号都是一致的（方便多机协作）】
        query.bindValue(":MapName", "EMPTY");//地图名字
        query.bindValue(":Robotfloor","EMPTY" );//地图楼层
        query.bindValue(":RobotNumber", "EMPTY");//自研底盘机器人编号： longyuan_wt_00001；思岚机器人编号 SLAMWARE-812370； WT5LS103194： 云迹 机器人编号 ；



        if(QString::fromStdString(request.request_Table_CostMapLayer.POINT_A_X)!="EMPTY"){
            query.bindValue(":POINT_A_X", QString::fromStdString(request.request_Table_CostMapLayer.POINT_A_X));//第一个点位A

        }
        if(QString::fromStdString(request.request_Table_CostMapLayer.POINT_A_Y)!="EMPTY"){
            query.bindValue(":POINT_A_Y", QString::fromStdString(request.request_Table_CostMapLayer.POINT_A_Y));//第一个点位A

        }
        if(QString::fromStdString(request.request_Table_CostMapLayer.POINT_B_X)!="EMPTY"){
            query.bindValue(":POINT_B_X", QString::fromStdString(request.request_Table_CostMapLayer.POINT_B_X));//第二个点位B

        }
        if(QString::fromStdString(request.request_Table_CostMapLayer.POINT_B_Y)!="EMPTY"){
            query.bindValue(":POINT_B_Y", QString::fromStdString(request.request_Table_CostMapLayer.POINT_B_Y));//第二个点位B

        }
        if(QString::fromStdString(request.request_Table_CostMapLayer.POINT_C_X)!="EMPTY"){
            query.bindValue(":POINT_C_X", QString::fromStdString(request.request_Table_CostMapLayer.POINT_C_X));//第三个点位C

        }
        if(QString::fromStdString(request.request_Table_CostMapLayer.POINT_C_Y)!="EMPTY"){
            query.bindValue(":POINT_C_Y", QString::fromStdString(request.request_Table_CostMapLayer.POINT_C_Y));//第三个点位C

        }
        if(QString::fromStdString(request.request_Table_CostMapLayer.POINT_D_X)!="EMPTY"){
            query.bindValue(":POINT_D_X", QString::fromStdString(request.request_Table_CostMapLayer.POINT_D_X));//第四个点位D

        }
        if(QString::fromStdString(request.request_Table_CostMapLayer.POINT_D_Y)!="EMPTY"){
            query.bindValue(":POINT_D_Y", QString::fromStdString(request.request_Table_CostMapLayer.POINT_D_Y));//第四个点位D
        }


        fig = query.exec();


        //qDebug()<<"===========================end 保存数据 ===========================";
    }catch (std::exception e){
    //    ROS_ERROR(e.what());

    }
    return fig;

}

//查询 FigureName
void queryByFigureNameFromTable_CostMapPlayer(int &count,disinfect_srvs::testInfo_srvsRequest &request,disinfect_srvs::testInfo_srvsResponse &response){

    try {
        ////查询字典数据
       // selectAllFromTABLE_DICTIONARES();
        QString connectionName = "currencyChassisQueryAllByRobotNumberFromCostMapPlayer";
        QSqlQuery query(connectionName);    //执行操作类对象
        //查询数据
        QString sql=" FROM TABLE_COSTMAPLAYER WHERE FigureName='"+QString::fromStdString(request.request_Table_CostMapLayer.FigureName)+"'";
        query.prepare("SELECT * "+sql);
        //qDebug()<<"sql: "<<sql;

        query.exec();    //执行
        QSqlRecord recode = query.record();        //recode保存查询到一些内容信息，如表头、列数等等
        int column = recode.count();            //获取读取结果的列数
        QString s1 = recode.fieldName(0);        //获取第0列的列名
        int index=0;
        while (query.next()) {

            disinfect_msg::Table_CostMapLayer t;
            query_disinfect_msg_Table_CostMapPlayer(t,query);
            response.result_Table_CostMapLayers.push_back(t);
            index++;
        }

        query.exec("SELECT count(*) "+sql);
        query.next();
        count = query.value(0).toInt();
        response.result_count=count;

    }catch (exception e){
        //qDebug()<<"数据库操作异常请检查文件是否存在！" ,"send";
    }

}


//查询 Id
void queryByIdFromTable_CostMapPlayer(int &count,disinfect_srvs::testInfo_srvsRequest &request,disinfect_srvs::testInfo_srvsResponse &response){

    try {
        ////查询字典数据
        //selectAllFromTABLE_DICTIONARES();
        QString connectionName = "currencyChassisQueryByIdFromTable_CostMapPlayer";
        QSqlQuery query(connectionName);    //执行操作类对象
        //查询数据
        QString sql=" FROM TABLE_COSTMAPLAYER WHERE Id="+QString::number(request.request_Table_CostMapLayer.Id);
        query.prepare("SELECT * "+sql);
        //qDebug()<<"sql: "<<sql;

        query.exec();    //执行
        QSqlRecord recode = query.record();        //recode保存查询到一些内容信息，如表头、列数等等
        int column = recode.count();            //获取读取结果的列数
        QString s1 = recode.fieldName(0);        //获取第0列的列名
        int index=0;
        while (query.next()) {

            disinfect_msg::Table_CostMapLayer t;
            query_disinfect_msg_Table_CostMapPlayer(t,query);
            response.result_Table_CostMapLayers.push_back(t);
            index++;
        }

        query.exec("SELECT count(*) "+sql);
        query.next();
        count = query.value(0).toInt();
        response.result_count=count;

    }catch (exception e){
        //qDebug()<<"数据库操作异常请检查文件是否存在！" ,"send";
    }

}



//查询 RobotNumber：机器编号 Order做升序排序查询
void queryAllFromTable_CostMapPlayer(int &count,disinfect_srvs::testInfo_srvsRequest &request,disinfect_srvs::testInfo_srvsResponse &response){





    try {
        //查询字典数据
        //selectAllFromTABLE_DICTIONARES();
        QString connectionName = "currencyChassisQueryAllFromCostMapPlayer";
        QSqlQuery query(connectionName);    //执行操作类对象

        //查询数据
      //  QString sql=" FROM TABLE_COSTMAPLAYER WHERE  " ;
        QString sql=" FROM TABLE_COSTMAPLAYER   " ;
        //sql=  sql+" RobotNumber='"+QString::fromStdString(RobotNumber)+"'";//机器编号
       // sql=  sql+" MerchantNumber='"+QString::fromStdString(dictionariesMap["merchantNumber"])+"'";// 商户编号 海底捞
        //sql=  sql+" AND MapName='"+QString::fromStdString(dictionariesMap["mapName"])+"'";//地图名字
       // sql=  sql+" AND Robotfloor='"+QString::fromStdString(dictionariesMap["robotfloor"])+"'";//楼层

      //   sql=  sql+" MerchantNumber='EMPTY'";// 商户编号 海底捞
      //   sql=  sql+" AND MapName='EMPTY'";//地图名字
     //    sql=  sql+" AND Robotfloor='EMPTY'";//楼层

//        sql=  sql+" MerchantNumber=EMPTY";// 商户编号 海底捞
//        sql=  sql+" AND MapName=EMPTY";//地图名字
//        sql=  sql+" AND Robotfloor=EMPTY";//楼层

        sql=  sql+" ORDER BY Id ASC";
        query.prepare("SELECT * "+sql);

        qDebug()<<"sql: "<<sql;

        query.exec();    //执行
        QSqlRecord recode = query.record();        //recode保存查询到一些内容信息，如表头、列数等等
        int column = recode.count();            //获取读取结果的列数
        QString s1 = recode.fieldName(0);        //获取第0列的列名
        int index=0;
        while (query.next()) {
            disinfect_msg::Table_CostMapLayer t;
            query_disinfect_msg_Table_CostMapPlayer(t,query);
            response.result_Table_CostMapLayers.push_back(t);
            index++;
        }

        query.exec("SELECT count(*) "+sql);
        query.next();
        count = query.value(0).toInt();
        response.result_count=count;

    }catch (exception e){
        //qDebug()<<"数据库操作异常请检查文件是否存在！" ,"send";
    }

}

//更新TABLE_COSTMAPLAYER所有数据数据数据byFigureName
bool updateAllByFigureNameFromTable_CostMapPlayer(disinfect_srvs::testInfo_srvsRequest &request){
    bool fig= false;
    try{
        QString connectionName = "currencyChassisupdateAllByIdFromCostMapPlayer";

        QSqlQuery query(connectionName);

//============================5.1 更新数据库 ============================
//虚拟墙点位储存     图形类型 FigureType-> point：点 至少1个点数据, line：线 至少2个点数据, polygons:多边形 至少3个点数据
        QString updateSql ="UPDATE TABLE_COSTMAPLAYER SET ";
        if(QString::fromStdString(request.request_Table_CostMapLayer.FigureType)!="EMPTY"){
            updateSql =   updateSql   +"FigureType= '"+ QString::fromStdString(request.request_Table_CostMapLayer.FigureType)+"', ";
        }

//        updateSql =   updateSql   +"   MerchantNumber='EMPTY'";
//        updateSql =   updateSql   +"', MerchantName='EMPTY'";
//        updateSql =   updateSql   +"', RobotNumber='EMPTY'";
//        updateSql =   updateSql   +"', MapName='EMPTY'";
//        updateSql =   updateSql   +"', Robotfloor='EMPTY'";
        if(QString::fromStdString(request.request_Table_CostMapLayer.POINT_A_X)!="EMPTY"){
            updateSql =   updateSql   +"', POINT_A_X= '"+QString::fromStdString(request.request_Table_CostMapLayer.POINT_A_X);
        }

        if(QString::fromStdString(request.request_Table_CostMapLayer.POINT_A_Y)!="EMPTY"){
            updateSql =   updateSql   +"', POINT_A_Y= '"+QString::fromStdString(request.request_Table_CostMapLayer.POINT_A_Y);

        }
        if(QString::fromStdString(request.request_Table_CostMapLayer.POINT_B_X)!="EMPTY"){
            updateSql =   updateSql   +"', POINT_B_X= '"+QString::fromStdString(request.request_Table_CostMapLayer.POINT_B_X);

        }
        if(QString::fromStdString(request.request_Table_CostMapLayer.POINT_B_Y)!="EMPTY"){
            updateSql =   updateSql   +"', POINT_B_Y= '"+QString::fromStdString(request.request_Table_CostMapLayer.POINT_B_Y);

        }
        if(QString::fromStdString(request.request_Table_CostMapLayer.POINT_C_X)!="EMPTY"){
            updateSql =   updateSql   +"', POINT_C_X= '"+QString::fromStdString(request.request_Table_CostMapLayer.POINT_C_X);

        }
        if(QString::fromStdString(request.request_Table_CostMapLayer.POINT_C_Y)!="EMPTY"){
            updateSql =   updateSql   +"', POINT_C_Y= '"+QString::fromStdString(request.request_Table_CostMapLayer.POINT_C_Y);

        }
        if(QString::fromStdString(request.request_Table_CostMapLayer.POINT_D_X)!="EMPTY"){
            updateSql =   updateSql   +"', POINT_D_X= '"+QString::fromStdString(request.request_Table_CostMapLayer.POINT_D_X);

        }
        if(QString::fromStdString(request.request_Table_CostMapLayer.POINT_D_Y)!="EMPTY"){
            updateSql =   updateSql   +"', POINT_D_Y= '"+QString::fromStdString(request.request_Table_CostMapLayer.POINT_D_Y);

        }
        updateSql =   updateSql   +"' WHERE FigureName='"+QString::fromStdString(request.request_Table_CostMapLayer.FigureName)+"'";
        qDebug()<<"updateSql: "<<updateSql;

        //qDebug()<<"==================request.request_Table_Task.Id==============: "<<request.request_Table_Task.Id;
//更改
        query.prepare(updateSql);
        fig = query.exec();

    }catch (std::exception e){
        //   ROS_ERROR(e.what());

    }
    return fig;
}



//更新TABLE_COSTMAPLAYER所有数据数据数据byID
bool updateAllByIdFromTable_CostMapPlayer(disinfect_srvs::testInfo_srvsRequest &request){
    bool fig= false;
    try{
        QString connectionName = "currencyChassisupdateAllByIdFromCostMapPlayer";

        QSqlQuery query(connectionName);

//============================5.1 更新数据库 ============================
//虚拟墙点位储存     图形类型 FigureType-> point：点 至少1个点数据, line：线 至少2个点数据, polygons:多边形 至少3个点数据
        QString updateSql ="UPDATE TABLE_COSTMAPLAYER SET ";
        if(QString::fromStdString(request.request_Table_CostMapLayer.FigureType)!="EMPTY"){
            updateSql =   updateSql   +"FigureType= '"+ QString::fromStdString(request.request_Table_CostMapLayer.FigureType)+"', ";
        }
        if(QString::fromStdString(request.request_Table_CostMapLayer.FigureName)!="EMPTY"){
            updateSql =   updateSql   +"FigureName= '"+ QString::fromStdString(request.request_Table_CostMapLayer.FigureName)+"', ";
        }


//        updateSql =   updateSql   +"   MerchantNumber= '"+QString::fromStdString(request.request_Table_CostMapLayer.MerchantNumber);
//        updateSql =   updateSql   +"', MerchantName= '"+QString::fromStdString(request.request_Table_CostMapLayer.MerchantName);
//        updateSql =   updateSql   +"', RobotNumber= '"+QString::fromStdString(request.request_Table_CostMapLayer.RobotNumber);
//        updateSql =   updateSql   +"', MapName= '"+QString::fromStdString(request.request_Table_CostMapLayer.MapName);
//        updateSql =   updateSql   +"', Robotfloor= '"+QString::fromStdString(request.request_Table_CostMapLayer.Robotfloor);


        if(QString::fromStdString(request.request_Table_CostMapLayer.POINT_A_X)!="EMPTY"){
            updateSql =   updateSql   +"', POINT_A_X= '"+QString::fromStdString(request.request_Table_CostMapLayer.POINT_A_X);
        }

        if(QString::fromStdString(request.request_Table_CostMapLayer.POINT_A_Y)!="EMPTY"){
            updateSql =   updateSql   +"', POINT_A_Y= '"+QString::fromStdString(request.request_Table_CostMapLayer.POINT_A_Y);

        }
        if(QString::fromStdString(request.request_Table_CostMapLayer.POINT_B_X)!="EMPTY"){
            updateSql =   updateSql   +"', POINT_B_X= '"+QString::fromStdString(request.request_Table_CostMapLayer.POINT_B_X);

        }
        if(QString::fromStdString(request.request_Table_CostMapLayer.POINT_B_Y)!="EMPTY"){
            updateSql =   updateSql   +"', POINT_B_Y= '"+QString::fromStdString(request.request_Table_CostMapLayer.POINT_B_Y);

        }
        if(QString::fromStdString(request.request_Table_CostMapLayer.POINT_C_X)!="EMPTY"){
            updateSql =   updateSql   +"', POINT_C_X= '"+QString::fromStdString(request.request_Table_CostMapLayer.POINT_C_X);

        }
        if(QString::fromStdString(request.request_Table_CostMapLayer.POINT_C_Y)!="EMPTY"){
            updateSql =   updateSql   +"', POINT_C_Y= '"+QString::fromStdString(request.request_Table_CostMapLayer.POINT_C_Y);

        }
        if(QString::fromStdString(request.request_Table_CostMapLayer.POINT_D_X)!="EMPTY"){
            updateSql =   updateSql   +"', POINT_D_X= '"+QString::fromStdString(request.request_Table_CostMapLayer.POINT_D_X);

        }
        if(QString::fromStdString(request.request_Table_CostMapLayer.POINT_D_Y)!="EMPTY"){
            updateSql =   updateSql   +"', POINT_D_Y= '"+QString::fromStdString(request.request_Table_CostMapLayer.POINT_D_Y);

        }
        updateSql =   updateSql   +"' WHERE Id="+QString::number(request.request_Table_CostMapLayer.Id);
        qDebug()<<"updateSql: "<<updateSql;

        //qDebug()<<"==================request.request_Table_Task.Id==============: "<<request.request_Table_Task.Id;
//更改
        query.prepare(updateSql);
        fig = query.exec();

    }catch (std::exception e){
     //   ROS_ERROR(e.what());

    }
    return fig;
}



// ============end===================虚拟墙：TABLE_COSTMAPLAYER===================================================

//加入队列
void addSqlQueue(disinfect_srvs::testInfo_srvsRequest &request,disinfect_srvs::testInfo_srvsResponse &response){
    SqlSendMessage sqlSendMessage;
    sqlSendMessage.request=request;
    sqlSendMessage.response=response;
    instructionQueue.push(sqlSendMessage);

}
//处理队列中的数据
void executeSqlfun(ros::Publisher   &costmapProhibitionControl_pub){
    try {

        if(instructionQueue.empty()  )
            return;

        //互锁 当获取锁时，如果其他线程持有该锁，无可用锁资源，直接返回false，这时候线程不用阻塞等待，可以先去做其他事情；
        bool tryLock = m->try_lock();
        //互锁 当获取锁时，如果其他线程持有该锁，无可用锁资源，直接返回false，这时候线程不用阻塞等待，可以先去做其他事情；
        if(!tryLock)
            return;

        bool fig= false;

        //2.取出一条指令中的夹抓状态与定时器时间
        SqlSendMessage sqlSendMessage = instructionQueue.front();

//==============================================================================================

    if(sqlSendMessage.request.request_type=="saveTASKFromTABLE_CostMapPlayer"){//保存虚拟墙数据
            //***********************************************************************************************************
           // ============start============虚拟墙=======：TABLE_COSTMAPLAYER=========FigureType-> point：点 至少1个点数据, line：线 至少2个点数据, polygons:多边形 至少3个点数据==========================================
            fig= saveTASKFromTABLE_CostMapPlayer(sqlSendMessage.request);
            //刷新虚拟墙 data：refreshCostMapPlayer
            std_msgs::String data;
            data.data="refreshCostMapPlayer";
            costmapProhibitionControl_pub.publish(data);
        }else if( sqlSendMessage.request.request_type=="deleteTaskPOSEFromTABLE_CostMapPlayer"){//删除数据byId
            fig= deleteTaskPOSEFromTABLE_CostMapPlayer(sqlSendMessage.request);
            //刷新虚拟墙 data：refreshCostMapPlayer
            std_msgs::String data;
            data.data="refreshCostMapPlayer";
            costmapProhibitionControl_pub.publish(data);

        }else if( sqlSendMessage.request.request_type=="deleteTaskPOSEFromTABLE_CostMapPlayerByFigureName"){//删除数据ByFigureName
            fig= deleteTaskPOSEFromTABLE_CostMapPlayerByFigureName(sqlSendMessage.request);
            //刷新虚拟墙 data：refreshCostMapPlayer
            std_msgs::String data;
            data.data="refreshCostMapPlayer";
            costmapProhibitionControl_pub.publish(data);
        }else if(sqlSendMessage.request.request_type=="updateAllByIdFromTable_CostMapPlayer"){//更新TABLE_COSTMAPLAYER所有数据数据数据byID
            fig= updateAllByIdFromTable_CostMapPlayer(sqlSendMessage.request);
            //刷新虚拟墙 data：refreshCostMapPlayer
            std_msgs::String data;
            data.data="refreshCostMapPlayer";
            costmapProhibitionControl_pub.publish(data);

        }else if(sqlSendMessage.request.request_type=="updateAllByFigureNameFromTable_CostMapPlayer"){//更新TABLE_COSTMAPLAYER所有数据数据数据byFigureName
            fig= updateAllByFigureNameFromTable_CostMapPlayer(sqlSendMessage.request);
            //刷新虚拟墙 data：refreshCostMapPlayer
            std_msgs::String data;
            data.data="refreshCostMapPlayer";
            costmapProhibitionControl_pub.publish(data);

        }else{
            //其它数据默认 true
            fig=true;
        }

        //5.删除队列中的这条指令,sql 操作成功就删除否则在执行一次，直到执行成功
        if(fig){
            instructionQueue.pop();
          //  std::cout<<"6.sukai 队列  yes  executeSql====sinstructionQueue.pop()" <<std::endl;
        }else{
          //  std::cout<<"7.sukai 队列 NO  executeSql====sinstructionQueue.pop()" <<std::endl;

        }


    }catch (std::exception e){
        //互锁
          m->unlock();
        e.what();
        std::cout<<"QTcpServer 数据发送给所有客户端: " <<e.what()<<std::endl;
    }

    //互锁
       m->unlock();
}

//数据库操作 disinfect_srvs/testInfo_srvs.h disinfect_srvs::testInfo_srvs::Request,disinfect_srvs::testInfo_srvs::Response
bool callback_QSqlQuery(disinfect_srvs::testInfo_srvs::Request &request  , disinfect_srvs::testInfo_srvs::Response &response,QTimer*  executeTimer) {

    try {

        int count=0;

        if( QString::fromStdString(request.request_type).contains("query")){
        m->lock();

         if(request.request_type=="queryMaximunIdFromTABLE_CostMapPlayer"){//读取ABLE_CostMapPlayer数据库最大的一个id号
                queryMaximunIdFromTABLE_CostMapPlayer(response);
                //cout<<"queryMaximunIdFromTABLE_CostMapPlayer 查询 by Id"<<endl;
                //日志
                //logWriteSql("queryMaximunIdFromTABLE_CostMapPlayer 查询 by Id" ,-1,"");
            }else if(request.request_type=="queryByIdFromTable_CostMapPlayer"){//读取ABLE_CostMapPlayer by Id
                queryByIdFromTable_CostMapPlayer(count,request,response);
                //cout<<"queryByIdFromTable_CostMapPlayer 查询 by Id"<<endl;
                //日志
                //logWriteSql("queryByIdFromTable_CostMapPlayer 查询 by Id" ,-1,"");


            }else if(request.request_type=="queryByFigureNameFromTable_CostMapPlayer"){//读取ABLE_CostMapPlayer by Id
                queryByFigureNameFromTable_CostMapPlayer(count,request,response);
                //cout<<"queryByFigureNameFromTable_CostMapPlayer 查询 by Id"<<endl;
                //日志
                //logWriteSql("queryByFigureNameFromTable_CostMapPlayer 查询 by Id" ,-1,"");

                //查询 FigureName
            }else if(request.request_type=="queryAllFromTable_CostMapPlayer"){//读取ABLE_CostMapPlayer
                queryAllFromTable_CostMapPlayer(count,request,response);
                //cout<<"queryAllFromTable_CostMapPlayer 查询 by Id"<<endl;
                //日志
                //logWriteSql("queryAllFromTable_CostMapPlayer 查询 by Id" ,-1,"");
            }
            m->unlock();
           // executeTimer->start();
        }else if (!QString::fromStdString(request.request_type).contains("query")){
            //除查询外所有的数据库操作都加入队列执行
            SqlSendMessage sqlSendMessage;
            sqlSendMessage.request=request;
            sqlSendMessage.response=response;
            //   cout<<"1.加入队列request.request_type: "<<request.request_type<<endl;
            //1.加入队列
            addSqlQueue(request,response);
            //   cout<<"2.加入队列sqlSendMessage.request: "<<sqlSendMessage.request<<endl;
            int nutime = (int)(1 / ((double)(time_int + 10) / 1000));//计算时间
             ros::Rate r(nutime);
             r.sleep();
        }

    }catch (std::exception e){
      //  ROS_ERROR(e.what());
        //互锁
        m->unlock();

    }

    response.result="ok";
    return true;
}

//定时任务从队列中执行sql
void executeSql( ros::Publisher   &costmapProhibitionControl_pub){
    //从队列中执行sql
    executeSqlfun(costmapProhibitionControl_pub);

}

// ============end==================数据库操作all===================================================
//ros::spin
void update(){
    ros::spinOnce();
    //处理消息
    if(ros::isShuttingDown()){
        QApplication::quit();
    }

}


//检查数据库文件(所有类型)是否存在
// 1:存在 0:不存在
int IsFileExist(const char* path)
{
    return !access(path, F_OK);
}


//3秒后检查数据库是否存在，不存在则建立
void startTimerfun(QTimer  *startTimer){
    // string dbpath1="/home/"+hostname+"/slam/TABLE_OR_ROOM_POSE.db";
//    std::string home = getenv("HOME");//
//    dbPath=home+"/workspace/xuniqiang/src/slamproject/db/TABLE_OR_ROOM_POSE.db";
//
//        startTimer->stop();
//        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
//        //cout<<"dbPath: "<<dbPath;
//        db.setDatabaseName(QString::fromStdString(dbPath));
//        if (!db.open()) {
//            qDebug()<<"!db.open() TABLE_OR_ROOM_POSE.db";
//
//        }else{
//            qDebug()<<"open() TABLE_OR_ROOM_POSE.db";
//
//        }


}



//todo sukai
int main(int argc, char *argv[]) {

        //节点名
        string nodeName = "currencyChassis_sqlDatabaseMain_node";
        //初始化节点
        ros::init(argc,argv,nodeName);
        //创建节点
        ros::NodeHandle node;
        /**************************使用service 获取数据**************************/
    //获取主机名称
  //  char hname[128];
   // gethostname(hname, sizeof(hname));
//    cout<<"hname:"<<hname<<endl;
    //    //主机名
  //  hostname=string(hname);//iimt
   // dbPath="/home/"+hostname+"/slam/TABLE_OR_ROOM_POSE.db";


        std::string home = getenv("HOME");//
         dbPath=home+"/workspace/xuniqiang/src/slamproject/db/TABLE_OR_ROOM_POSE.db";

    //std::string home = getenv("HOME");// /home/iimt
    //dbPath=home+"/slam/TABLE_OR_ROOM_POSE.db";


    std::cout<<"=========dbPath " <<dbPath<<std::endl;

    //刷新虚拟墙 data：refreshCostMapPlayer
    ros::Publisher   costmapProhibitionControl_pub = node.advertise<std_msgs::String>("/costmapProhibitionControl", 10);


    //======end================================ 注册码验证 ==============================================================

    QApplication app(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    //cout<<"dbPath: "<<dbPath;
    db.setDatabaseName(QString::fromStdString(dbPath));
    if (!db.open()) {
        qDebug()<<"!db.open() TABLE_OR_ROOM_POSE.db";

    }else{
        qDebug()<<"open() TABLE_OR_ROOM_POSE.db";

    }

    QTimer*  updateTimer = new QTimer();
    updateTimer->setInterval(100);
    QObject::connect(updateTimer, &QTimer::timeout,  &update);
    updateTimer->start();

    //轮询检查队列中是否存在需要执行的sql操作
    QTimer* executeTimer = new QTimer();
    executeTimer->setInterval(time_int);
    QObject::connect(executeTimer, &QTimer::timeout,  boost::bind(&executeSql,costmapProhibitionControl_pub));
    executeTimer->start();


    //3秒后检查数据库是否存在，不存在则建立
    QTimer*  startTimer = new QTimer();
    startTimer->setInterval(3000);
    // QObject::connect(updateTimer, &QTimer::timeout,  &startTimer);
    QObject::connect(startTimer, &QTimer::timeout, boost::bind(&startTimerfun, startTimer));

    //数据库操作 disinfect_srvs::testInfo_srvsRequest &request  , disinfect_srvs::testInfo_srvsResponse &response
    const ros::ServiceServer &SqlQueryServer = node.advertiseService<disinfect_srvs::testInfo_srvs::Request,disinfect_srvs::testInfo_srvs::Response>("/QSqlQuery", boost::bind(&callback_QSqlQuery,_1,_2,executeTimer));



    return app.exec();
}

