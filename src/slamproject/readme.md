送餐机器人:双轮差速导诊车
slamproject
系统ubunt16.04
依赖：opencv3.4.9
开发工具 clion
项目路径：/home/iimt/workspace/workspace_ros_guidance
静态资源说明：
  filepath：
	相机内参：
	   filepath/camera_calibration.yaml
	棋盘格图片与小车pose关联的文件
	   filepath/extrinsic_param_car_in_eye.xml
	相机相对于小车的坐标
	   filepath/calibration_ex_params_car_in_eye.xml
	不同二维码在基坐标下的坐标
	   filepath/target_qrcode_position_with_base_*.yml
        餐桌在基坐标下的坐标(作废，用数据库)
	   filepath/target_foodcar_position_with_base_*.yml
        配置餐桌(作废，用数据库)：
           visualOrientation/param/params.yaml  tables：按示例格式填写，遵守 名字tablea:->table*的顺序填 值不限制格式，tablea不要填对应的餐桌，a号默认是0位置；
        配置送餐车到达目标点后的等待时间：
           visualOrientation/param/params.yaml  waitGoalTime：单位毫秒
  images：
        棋盘格图片
        注意：images文件夹中不允许有其它无关的文件，注意保存的图片文件名字格式；
  路点数据，数据库文件：
        /home/iimt/workspace/workspace_ros_guidance/src/visualOrien/visualOrientation/db/TABLE_OR_ROOM_POSE.db
  地图更改位置：
      src/controlMaster/luanch/amcl.launch
      <arg name="map_path" default="$(find controlMaster)/map/indoor.yaml"/>
-------------------------------------------------------------------------
qt：serialport安装
   sudo apt-get install ros-kinetic-qt-create
   sudo apt-get install ros-kinetic-qt-build
   sudo apt install libqt5serialport5-dev
二维码识别：
    二维码安装包：
         sudo apt-get install ros-kinetic-ar-track-alvar
    创建二维码：
         新建一个目录，cd进目录中：
	     rosrun ar_track_alvar createMarker -s 5 1
             rosrun ar_track_alvar createMarker -s 5 2
             rosrun ar_track_alvar createMarker -s 5 3
   启动二维码识：
             roslaunch robot_vision usb_cam_with_calibration.launch 【需要在launch中加载内参标定文件，注意文件路径及文件名字】
             roslaunch robot_vision ar_track_camera.launch  【改二维码尺寸】
             rostopic echo /ar_pose_marker  
             rostopic echo /visualization_marker

视觉定位纠偏执行步骤:
	1.内参标定
         标定包安装：sudo apt-get install ros-kinetic-camera-calibration	  
	 roslaunch robot_vision usb_cam.launch 
	 rosrun camera_calibration cameracalibrator.py --size 6x9 --square 20     image:=/usb_cam/image_raw
        【标定文件在cd /tmp 中解压改名：camera_calibration.yaml cp到/home/iimt/workspace/workspace_ros_guidance/src/robot_vision目录下】
	2.外参数据采集 src/saveImgeAndCarPose.cpp
	 roslaunch robot_vision usb_cam.launch
	 roslaunch visualOrientation saveImgeAndCarPose.launch
 
	3.外参标定 src/CarInEyeCalibration.cpp
	 rosrun visualOrientation carInEyeCalibration_node
         注意：images文件夹中不允许有其它无关的文件，注意保存的图片文件名字格式；
	注意：
	   需要收到小车当前pose及二维码pose

       4.标定：二维码位置，餐桌位置，初始化小车姿态：收到小车pose得到二维码在基坐标的pose及得到小车在基座标的pose：【先启动小车】
            roslaunch robot_vision usb_cam_with_calibration.launch 【注意加载内参文件】
            roslaunch robot_vision ar_track_camera.launch  【注意改二维码尺寸】
            roslaunch visualOrientation sendOdomClient_sql.launch  
       注意：
           第一次运行需要创建数据库：
           rosrun visualOrientation KSDemoDlgCreate_node
	   需要收到小车当前pose及二维码pose
       目标位置手工输入编辑：
           rosrun visualOrientation KSDemoDlg_node
     5.送餐演示demo
            roslaunch robot_vision usb_cam_with_calibration.launch 【注意加载内参文件】
            roslaunch robot_vision ar_track_camera.launch  【注意改二维码尺寸】
            roslaunch visualOrientation sendFoodCarDemo.launch  （自动更新小车姿态）
6.自动识别二维码调整小车位置信息位置 (方差大于1阀值时才会触发调整小车姿态)
	     roslaunch robot_vision usb_cam_with_calibration.launch 【注意加载内参文件】
	     roslaunch robot_vision ar_track_camera.launch  【注意改二维码尺寸】
	     roslaunch visualOrientation automaticComputerInitialPose.launch
	  
7. 启动导诊车ui
        roslaunch visualOrientation sendGuidanceCar.launch
         
日志：
   日志路径：
       ~/data/log/


----------------car---------------------------------
1.通过获取tf坐标转换获取小车坐标并实时发布出去 src/ListenerCarBaseLinkPose.cpp
        rosrun visualOrientation listenerCarBaselinkPose_node
2.发送小车pose得到二维码在基坐标的pose及得到小车在基座标的pose
        rosrun visualOrientation sendOdomClient_node

-------------------------校准角校准-------------线速度校准-----------------
1.校准角速度
rosrun zxcar_nav calibrate_angular.py
执行完成之后,根据提示执行如下命令:
rosrun rqt_reconfigure rqt_reconfigure
点击start_test 转360度
先记下手机指南针第一次的度数，比如：静止时第一次手机指南针度数50度，如果点击start_test后，小车转动后停下的度数超过50，那么就是转够了一圈有多，如果小车转动后停下的度数小于50，那么就是没有转够一圈。分以下两种情况：
情况1：小车停下后度数假如是60度,那么小车实际上是转了370度。
此时角速度比例系数 anguler_scale= 370/360 = 1.028
情况2：小车停下后度数假如是40度,那么小车实际上是转了350度。
此时角速度比例系数 anguler_scale= 350/360 = 0.972
 
2.校准线速度
rosrun zxcar_nav calibrate_linear.py
执行完成之后,根据提示执行如下命令:
rosrun rqt_reconfigure rqt_reconfigure

点击start_test,进行第一次的走一米测试，测量小车走的距离。
假如小车走了1.1米，那么线速度的比例系数 linear_scale = 1.1/1=1.1
假如小车走了0.9米，那么线速度的比例系数 linear_scale = 0.9/1= 0.9
bringup.launch


修改代码：
	anguler/home/iimt/workspace/workspace_ros_foodCar/src/motorControl/src/motorControl.cpp
	odomMsg.twist.twist.linear.x = AvergeVel*linear_scale;
	odomMsg.twist.twist.angular.z = deltaThVel*anguler_scale;
----------------test---------------------------------

获取基坐标下小车的坐标demo：src/getOdomClient_test.cpp
启动：
	rosrun visualOrientation getOdomClient_test_node
 发送serviceClient小车坐标：
         request：
	        1.serviceClient
		2.消息名字 serviceName = "target_odom_service";
		3.消息类型：qRcode_srvs::qRcode
         response：得到小车在基坐标下的位置

模拟发送小车位置的数据demo：src/sendOdomClient_test.cpp
启动：
        rosrun visualOrientation sendOdomClient_test_node
        server：
	        1.serviceClient
		2.消息名字serviceName = "target_qrcode_service"
		3.消息类型：qRcode_srvs::qRcode
       response：ok

       topic:
                1.publisher
                2.消息名字 topicName_odom = "/car_odom";
                3.消息类型： qRcode_msg::qRcode_msg

           
-------------------------------------------------------------------------
usb_cam 启动报错：v412-ctl not found
        sudo apt-get install v4l-utils   
控制台
	sudo apt-get install terminator
	打开多终端同步输入
	win + G
	关闭多终端同步输入
	win + shift + G
更新命令
	sudo apt-get update
	sudo apt-get upgrade
	sudo apt-get dist-upgrade
	sudo apt-get -f install
标定板：
  角点顺序，红橙黄绿青蓝紫的顺序
  红色为x坐标
  y的方向是相机摆的方向
rosrun rqt_service_caller rqt_service_caller
turtlebot-3仿真
         启动：
             roslaunch turtlebot3_gazebo turtlebot3_empty_world.launch
	     roslaunch turtlebot3_gazebo turtlebot3_world.launch
             roslaunch turtlebot3_gazebo turtlebot3_house.launch
         控制：
             roslaunch turtlebot3_teleop turtlebot3_teleop_key.launch
         Turtlebot开始根据激光雷达的信号开始自主规划避障:
             export TURTLEBOT3_MODEL=waffle
             roslaunch turtlebot3_gazebo turtlebot3_simulation.launch
         打开rviz
             export TURTLEBOT3_MODEL=waffle
             roslaunch turtlebot3_gazebo turtlebot3_gazebo_rviz.launch
        rostopic type /odom
        rosmsg show nav_msgs/Odometry

       开启实时建图功能,算法选择gmapping, 还有多种slam算法, 如cartographer,hector_slam
	roslaunch turtlebot3_slam turtlebot3_slam.launch slam_methods:=gmapping
       尽可能保证地图闭合,然后保存地图为文件
       rosrun map_server map_saver -f ~/map

       打开自动导航功能
       roslaunch turtlebot3_navigation turtlebot3_navigation.launch map_file:=/home/iimt/map.yaml
       初始化小车pose       
       rostopic echo /initialpose
二、配置文件
        move_base使用前需要配置一些参数：运行成本、机器人半径、到达目标位置的距离，机器人移动的速度，这些参数都在rbx1_nav包的以下几个配置文件中：
        • base_local_planner_params.yaml
        • costmap_common_params.yaml
        • global_costmap_params.yaml
        • local_costmap_params.yaml


小乌龟：
   启动： 
      rosrun turtlesim turtlesim_node
      rosrun turtlesim turtlesim turtle_teleop_key
      rostopic list
      rostopic pub /turtle1/cmd_vel
      rostopic pub /turtle1/pose
      rosservice type /spawn
      rossrv show turtlesim/Spawn
      
rviz
   启动：
      rosrun rviz rviz
      选择add，加tf
      选择Fixed Frame，选择发布的tf话题
  保存启动文件：
      File->saveConfigAs 选择一个目录存放，改好名字
      <node pkg="rviz" type="rviz" name="rviz" args="-d $(find demo_tf)/rviz/demo.rviz"></node>









        
        



