龙源通用底盘
 链接机器人热点：
	名称： longyuan_wt_00001 密码：longyuansa
	TCP连接方式：TCP连接 10.168.1.10:31001
	配置网络 ：10.168.1.1  密码： longyuansa
编译：
   直接运行这两个命令：
   catkin_make --pkg disinfect_msg
   catkin_make --pkg disinfect_srvs
   catkin_make
   source devel/setup.sh

启动功能介绍：
    1.创建数据库
      rosrun slamservice currencyChassis_CreateSqlDatabaseMain_node

    2. 获取base_link相对于map的坐标，实时发布小车坐标

      rosrun slamservice currencyChassis_listenerCarBaselinkPose_node

    3.数据库调用
      rosrun slamservice currencyChassis_sqlDatabaseMain_node

    4.路点标记
      rosrun slamservice currencyChassis_savePoseByOdom_node

    5.设置初始点校正机器人位置 结合 cartographer_ros 算法
      rosrun slamservice currencyChassis_initial_pose_set_node

    6.导航逻辑
      rosrun slamservice currencyChassis_carStartServiceMain_node

    7.发送 movebase
      rosrun slamservice currencyChassis_car_action_node

    8.发送 位置标定
      rosrun slamservice currencyChassis_savePoseByOdom_node


    9.网络接口服务器
      rosrun slamservice currencyChassis_netWork_server_node


   10. 测试网络接口
      rosrun slamservice currencyChassis_netWork_send_test_node

   总启动：
      roslaunch slamservice currencyChassis_sendfoodCar.launch



===================================================================

1.路点值编辑
rosrun slamservice currencyChassis_KSDemoDlg_node
2.执行任务编辑
rosrun slamservice currencyChassis_KSDemoDlgTABLE_TASKPOSEMain_node
3.定时任务表编辑
rosrun slamservice currencyChassis_KSDemoDlgTaskMain_node
4.设置，字典编辑
rosrun slamservice currencyChassis_KSDemoDlgTABLE_DICTIONARESMain_node
===================================================================
仿真：
   roslaunch turtlebot3_gazebo turtlebot3_house.launch
   roslaunch turtlebot3_navigation turtlebot3_navigation.launch map_file:=/home/iimt/map.yaml

   roslaunch slamservice currencyChassis_sendfoodCar.launch
   roslaunch rosbridge_server rosbridge_websocket.launch

====================================================================

