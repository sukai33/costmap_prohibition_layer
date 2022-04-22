
#### 虚拟墙使用介绍

基于costmap_prohibition_layer 
git clone https://github.com/sukai33/costmap_prohibition_layer.git
git clone https://gitee.com/yongwangzhiqiankai/costmap_prohibition_layer.git
开源代码，魔改的支持数据库动态虚拟墙，支持虚拟墙动态刷新，支持代码中操作虚拟墙。

#### 软件架构

开发者 苏凯
欢迎交流：
    csdn博客：https://blog.csdn.net/qq_15204179?type=blog
    B站 再遇咖啡： https://b23.tv/Yvt8noQ
日期：2022-04-21
系统：ubuntu20.04
ros平台：noetic
开发工具 clion
项目路径：~/workspace/xuniqiang
数据库文件路径：
        ~/workspace/xuniqiang/db/TABLE_OR_ROOM_POSE.db
 
-------------------------------------------------------------------------

#### 安装教程

1.  直接下载到你的工作空间上编译
    1.0 把此工程与movebase项目放一起
    1.1 按顺序编译
	catkin_make --pkg disinfect_msg 
	catkin_make --pkg disinfect_srvs
	catkin_make
    1.2 或者一条命令直接编译
        source catkit_make_source.sh
	
2.  设置你的mve_base.launch文件

        <launch>
	  <!-- move_base -->
	  <node pkg="move_base" type="move_base" respawn="false" name="move_base" output="screen">
            <!-- 加这两行没有实际意义，因为点位是从数据库加载的。虚拟墙插件中可能加载yaml这部份代码没删，不加载prohibition_areas.yaml可能会报错。 -->
	    <rosparam file="$(find slamproject)/config/prohibition_areas.yaml" command="load" ns="global_costmap/costmap_prohibition_layer" />
            <rosparam file="$(find slamproject)/config/prohibition_areas.yaml" command="load" ns="local_costmap/costmap_prohibition_layer" />
	  </node>
	</launch>
3.  分别在global_costmap 和local_costmap 的文件中 plugins 下添加
    global_costmap_params.yaml 中：
	plugins:
	  - {name: costmap_prohibition_layer, type: "costmap_prohibition_layer_namespace::CostmapProhibitionLayer"}
    local_costmap_params.yaml 中：
	plugins:
	  - {name: costmap_prohibition_layer, type: "costmap_prohibition_layer_namespace::CostmapProhibitionLayer"}

#### 测试启动使用介绍：
 
    0. roscore
    
    1.创建数据库 [数据库只需要创建一次就可以了]
      rosrun slamproject currencyChassis_CreateSqlDatabaseMain_node
      
    2.数据库调用 [启动导航前优先启动]
      rosrun slamproject currencyChassis_sqlDatabaseMain_node
      
    3.虚拟墙qt界面编辑 [qt界面增加虚拟墙数据只支持 line，具体使用看图片]
      rosrun slamproject currencyChassis_KSDemoDlgTABLE_COSTMAPLAYERMain_node 
 
    4.刷新虚拟墙 【当数据库更改虚拟墙数据后需要发送topic重新加载虚拟墙】
	    4.1 引入头文件
	    #include <std_msgs/String.h>
	    4.2创建 Publisher
	    ros::Publisher   costmapProhibitionControl_pub = node.advertise<std_msgs::String>("/costmapProhibitionControl", 10);
	    4.3发送数据
	     std_msgs::String data;
	     data.data="refreshCostMapPlayer";
	     costmapProhibitionControl_pub.publish(data);


 
#### 代码中使用介绍：

    
       1.查询虚拟墙数据
            1.1 引入头文件
               #include <disinfect_srvs/testInfo_srvs.h>
               
            1.2 数据库操作
              ros::ServiceClient  SqlQueryClient = nh.serviceClient<disinfect_srvs::testInfo_srvs>("/QSqlQuery");
                
                
            1.3【默认条件查询 MerchantNumber=EMPTY MerchantName=EMPTY RobotNumber=EMPTY MapName=EMPTY Robotfloor=EMPTY，
                  需要改条件的话自己去数据库节点src/currencyChassis/currencyChassis_sqlDatabaseMain.cpp改源码，类中搜索 queryAllFromTable_CostMapPlayer 】
		disinfect_srvs::testInfo_srvs table_CostMapLayerService;
		table_CostMapLayerService.request.request_type="queryAllFromTable_CostMapPlayer";
		SqlQueryClient.call(table_CostMapLayerService);

		for (int i = 0; i <table_CostMapLayerService.response.result_Table_CostMapLayers.size() ; ++i) {
	
		 if(table_CostMapLayerService.response.result_Table_CostMapLayers[i].FigureType=="line"){//线
		      
		      string ax= table_CostMapLayerService.response.result_Table_CostMapLayers[i].POINT_A_X;
		      string ay= table_CostMapLayerService.response.result_Table_CostMapLayers[i].POINT_A_Y;
	  
		      string bx=  table_CostMapLayerService.response.result_Table_CostMapLayers[i].POINT_B_X;
		      string by= table_CostMapLayerService.response.result_Table_CostMapLayers[i].POINT_B_Y;
		      

		   }
                }
      
      2.通过id查询数据
            disinfect_srvs::testInfo_srvs service;
            service.request.request_type = "queryByIdFromTable_CostMapPlayer";//虚拟墙所有数据
            service.request.request_Table_CostMapLayer.Id=id.toInt();
            SqlQueryClient.call(service);
      
       3.通过id删除数据

                disinfect_srvs::testInfo_srvs service;
                service.request.request_type="deleteTaskPOSEFromTABLE_CostMapPlayer";//删除数据 ById
                service.request.request_Table_CostMapLayer.Id=id.toInt();
                SqlQueryClient.call(service);
       4.通过名字删除数据
                disinfect_srvs::testInfo_srvs service;
                service.request.request_type="deleteTaskPOSEFromTABLE_CostMapPlayerByFigureName";//删除数据 ByFigureName
                service.request.request_Table_CostMapLayer.FigureName=figureName.toStdString();
                SqlQueryClient.call(service);
                
       
       2.增加数据

                disinfect_srvs::testInfo_srvs service;
                service.request.request_type="saveTASKFromTABLE_CostMapPlayer";//读取数据库最大的一个id号
                service.request.request_Table_CostMapLayer.FigureType=figureType.toStdString();//
                service.request.request_Table_CostMapLayer.FigureName=figureName.toStdString();//
                service.request.request_Table_CostMapLayer.MerchantNumber=merchantNumber.toStdString(); //用EMPTY代替
                service.request.request_Table_CostMapLayer.MerchantName=merchantName.toStdString();//用EMPTY代替
                service.request.request_Table_CostMapLayer.RobotNumber=robotNumber.toStdString();//用EMPTY代替
                service.request.request_Table_CostMapLayer.MapName=mapName.toStdString(); //用EMPTY代替
                service.request.request_Table_CostMapLayer.Robotfloor=robotfloor.toStdString(); //用EMPTY代替
                service.request.request_Table_CostMapLayer.POINT_A_X=point_A_X.toStdString();//
                service.request.request_Table_CostMapLayer.POINT_A_Y=point_A_Y.toStdString(); //
                service.request.request_Table_CostMapLayer.POINT_B_X=point_B_X.toStdString();//
                service.request.request_Table_CostMapLayer.POINT_B_Y=point_B_Y.toStdString();//
                service.request.request_Table_CostMapLayer.POINT_C_X=point_C_X.toStdString(); //
                service.request.request_Table_CostMapLayer.POINT_C_Y=point_C_Y.toStdString(); //
                service.request.request_Table_CostMapLayer.POINT_D_X=point_D_X.toStdString(); //
                service.request.request_Table_CostMapLayer.POINT_D_Y=point_D_Y.toStdString(); //
                SqlQueryClient.call(service);
       
       3.更新数据
    
                disinfect_srvs::testInfo_srvs service;
                service.request.request_type="updateAllByIdFromTable_CostMapPlayer";
                service.request.request_Table_CostMapLayer.Id=id.toInt();
                service.request.request_Table_CostMapLayer.FigureType=figureType.toStdString();//
                service.request.request_Table_CostMapLayer.FigureName=figureName.toStdString();//
                service.request.request_Table_CostMapLayer.MerchantNumber=merchantNumber.toStdString(); //用EMPTY代替
                service.request.request_Table_CostMapLayer.MerchantName=merchantName.toStdString();//用EMPTY代替
                service.request.request_Table_CostMapLayer.RobotNumber=robotNumber.toStdString();//用EMPTY代替
                service.request.request_Table_CostMapLayer.MapName=mapName.toStdString(); // 用EMPTY代替
                service.request.request_Table_CostMapLayer.Robotfloor=robotfloor.toStdString(); //
                service.request.request_Table_CostMapLayer.POINT_A_X=point_A_X.toStdString();//
                service.request.request_Table_CostMapLayer.POINT_A_Y=point_A_Y.toStdString(); //
                service.request.request_Table_CostMapLayer.POINT_B_X=point_B_X.toStdString();//
                service.request.request_Table_CostMapLayer.POINT_B_Y=point_B_Y.toStdString();//
                service.request.request_Table_CostMapLayer.POINT_C_X=point_C_X.toStdString(); //
                service.request.request_Table_CostMapLayer.POINT_C_Y=point_C_Y.toStdString(); //
                service.request.request_Table_CostMapLayer.POINT_D_X=point_D_X.toStdString(); //
                service.request.request_Table_CostMapLayer.POINT_D_Y=point_D_Y.toStdString(); //

                SqlQueryClient.call(service);
       
       
       4.其它接口看 currencyChassis/currencyChassis_sqlDatabaseMain.cpp 源码
       
       
       
       5.刷新虚拟墙 【当数据库更改虚拟墙数据后需要发送topic重新加载虚拟墙】
	    5.1 引入头文件
	    #include <std_msgs/String.h>
	    5.2创建 Publisher
	    ros::Publisher   costmapProhibitionControl_pub = node.advertise<std_msgs::String>("/costmapProhibitionControl", 10);
	    5.3发送数据
	     std_msgs::String data;
	     data.data="refreshCostMapPlayer";
	     costmapProhibitionControl_pub.publish(data);
       
       
       
       
#### 项目实战通过 turtlebot3   仿真运行
    1.首先确定数据库是否存在，不存在就创建
      1.1 roscore
      1.2 创建数据库 [数据库只需要创建一次就可以了]
         rosrun slamproject currencyChassis_CreateSqlDatabaseMain_node
    2.启动数据库 [启动导航前优先启动]
      rosrun slamproject currencyChassis_sqlDatabaseMain_node
    
    3.启动turtlebot3
      roslaunch turtlebot3_gazebo turtlebot3_house.launch
      roslaunch turtlebot3_navigation turtlebot3_navigation.launch map_file:=/home/iimt/map.yaml
    4.启动虚拟墙编辑器，取2点，录入数据 [qt界面增加虚拟墙数据只支持 line，具体使用看图片]
      rosrun slamproject currencyChassis_KSDemoDlgTABLE_COSTMAPLAYERMain_node 
    5.启动rqt_publisher , 选择topic名字 /costmapProhibitionControl 发送 "refreshCostMapPlayer" 重新加载数据
       rosrun rqt_publisher rqt_publisher
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       







        
        



