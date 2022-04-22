虚拟墙


 navigation里如何写自己的全局路径规划插件教程
http://wiki.ros.org/navigation/Tutorials/Writing%20A%20Global%20Path%20Planner%20As%20Plugin%20in%20ROS
https://blog.csdn.net/heyijia0327/article/details/42241831
https://blog.csdn.net/qq_41995710/article/details/88750354

# costmap_prohibition_layer
ROS-Package that implements a costmap layer to add prohibited areas to the costmap-2D by a user configuration.

Build status of the *kinetic-devel* branch:
- Travis (Ubuntu Trusty): [![Build Status](https://travis-ci.org/rst-tu-dortmund/costmap_prohibition_layer.svg?branch=kinetic-devel)](https://travis-ci.org/rst-tu-dortmund/costmap_prohibition_layer)

禁止层，虚拟墙
https://blog.csdn.net/qq_41995710/article/details/88750354
在Costmap_2d里面插入Prohibition_layer(禁止区域层)

他们都喊我sao G 2019-03-25 16:37:10  2036  收藏 19
版权
在Costmap_2d里面插入Prohibition_layer(禁止区域层)
简介
costmap_2d在navigaition里面是一个很重要的板块，通常默认的是三层地图（静态/障碍/膨胀层）叠加在一起，构成整个完整的代价地图，但是我们也可以插入一些自定义的图层，然后实现自己定制化的功能，基础的教程大家可以参考costmap_2d中插入自定义层。本文的主要目的是来告诉大家如何在代价地图中插入Prohibition_layer（禁止层），然后在已经建好的地图中设立禁止通行区域，关于prohibition_layer具体的作用大家可以参照roswiki上的解释。

具体实现步骤
1　代码下载与编译
　　之前我们学习插入simple_layer的时候是在自己的工作空间（catkin_ws）中建立一个pakage，然后再去自己创建.cpp文件和.h头文件，再去改cmakelist，添加.xml文件，在这个教程中我们不需要这么麻烦，我们可以直接在github上把prohibition_layer的包下载下来(源码下载地址戳这里).，解压后放在自己的工作空间中的　src　文件夹里面，然后在终端输入：

$ cd catkin_ws("catkin_ws"替换成你的工作空间名称)
$ catkin_make
1
2
编译完成以后在终端中输入
编译完成以后，可以查看ROS系统里是否已经有了这个layer 插件。在终端中输入：


rospack plugins --attrib=plugin costmap_2d
1
当你发现终端中输出：
这就表明prohibition_layer已经是一个可供使用的地图插件了．

２　在代价地图中插入prohibition_layer
　　完成步骤１后说明prohibition_layer已经可以供costmap使用了，现在我们要做的就是把prohibition_layer插入到costmap中去．
　　我们在自己的代价地图参数配置文件夹中找到　global_costmap_params.yaml　和　local_costmap_params.yaml　，打开这两个文档，分别在两个文档的末尾做修改，修改后应该是下面的内容：

 plugins:
      - {name: static_map,       type: "costmap_2d::StaticLayer"}
      - {name: obstacles,        type: "costmap_2d::VoxelLayer"}
      - {name: costmap_prohibition_layer,        type: "costmap_prohibition_layer_namespace::CostmapProhibitionLayer"}     
      - {name: inflation_layer,        type: "costmap_2d::InflationLayer"}

1
2
3
==========================================================================================
      下面我们来把创建的simple_layer，放入全局global_costmap中。要想使得ROS接受你的插件，要在参数中用plugins指明，也就是主要修改move_base中涉及到costmap的yaml文件，下面给出我的修改：

1.costmap_common_params.yaml

obstacle_range: 2.5
raytrace_range: 3.0
robot_radius: 0.49
inflation_radius: 0.1
max_obstacle_height: 0.6
min_obstacle_height: 0.0
obstacles:
   observation_sources: scan
   scan: {data_type: LaserScan, topic: /scan, marking: true, clearing: true, expected_update_rate: 0}
一定要注意，这里添加了一个obstales，说明障碍物层的数据来源scan，"obstacles："的作用是强调命名空间。

2.global_costmap_params.yaml

global_costmap:
   global_frame: /map
   robot_base_frame: /base_footprint
   update_frequency: 1.0
   publish_frequency: 0
   static_map: true
   rolling_window: false
   resolution: 0.01
   transform_tolerance: 1.0
   map_type: costmap
   plugins:
      - {name: static_map,       type: "costmap_2d::StaticLayer"}
      - {name: obstacles,        type: "costmap_2d::VoxelLayer"}
      - {name: simplelayer,        type: "simple_layer_namespace::SimpleLayer"}     
      - {name: inflation_layer,        type: "costmap_2d::InflationLayer"}
3.local_costmap_params.yaml

local_costmap:
   global_frame: /map
   robot_base_frame: /base_footprint
   update_frequency: 3.0
   publish_frequency: 1.0
   static_map: false
   rolling_window: true
   width: 6.0
   height: 6.0
   resolution: 0.01
   transform_tolerance: 1.0
   map_type: costmap

————————————————

原文链接：https://blog.csdn.net/heyijia0327/article/details/42241831

===========================================================================================
5
6
３　修改launch文件
　　完成插入地图后我们还需要修改自己的launch文件，我是在自己的move_base.launch 中插入的，这个大家根据自己的需求，你用的carlike就修改carlike的launch文件．打开自己的launch文件中插入如下内容:

<rosparam file="$(find turtlebot3_navigation)/param/prohibition_areas.yaml" command="load" ns="global_costmap/costmap_prohibition_layer" />
<rosparam file="$(find turtlebot3_navigation)/param/prohibition_areas.yaml" command="load" ns="local_costmap/costmap_prohibition_layer" />
1
2
其中的＂turtlebot3_navigation＂需要根据自己的情况修改，你打开自己的launch文件对照上下文就知道怎么更改了．

４　设置禁止区域坐标参数
　　完成以上步骤所有的配置工作就剩下最后一步了，设置禁止区域坐标参数．
　　首先需要在参数配置文件夹（就是和　global_costmap_params.yaml　以及　local_costmap_params.yaml　相同位置的文件夹）中创建新的文档，命名为　＂prohibition_areas.yaml＂．
　　然后在prohibition_areas.yaml文档中输入:

prohibition_areas:
#定义一个禁止点
 - [17.09, -6.388]
# 定义一个禁止通行的线
 - [[8.33, 2.11],
    [8.26, 5.11]]
# 定义一个禁止通行的区域
 - [[-11.15, -15.614],
    [-12.35, -13.89],
    [-10.05, -12.218]]
 
1
2
3
4
5
6
7
8
9
10
11
注意事项：
１　一定要严格按照上述格式来设置坐标，我之前遇到的格式错误导致不能识别禁止区域坐标情形有
　　（１）：坐标前的短横线没对齐
　　（２）：定义禁止区域或者禁止线，两坐标之间缺少了逗号
２　你可以同时定义多个禁止点／多个禁止线／多个禁止区域，或者混合定义多个点／线／区域．
３　禁止区域的坐标如何设置就需要你根据自己建的图来测量，这里就不赘述了．

５　结语
完成以上所有内容，你再次打开自己的导航就能够在rviz中看到自己的设置的禁止区域了，这里贴出我之前做的结果：
这是我在实验室里面建立的地图，图中的红色三角形是我设立的禁止区域，矩形框是我画的四条禁止线，小车现在在矩形框内，当我把导航目标设置在矩形框外的时候，结果显示的是无法规划路线的．
————————————————
版权声明：本文为CSDN博主「他们都喊我sao G」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/qq_41995710/article/details/88750354
https://blog.csdn.net/qq_41995710/article/details/88750354

 navigation里如何写自己的全局路径规划插件教程
http://wiki.ros.org/navigation/Tutorials/Writing%20A%20Global%20Path%20Planner%20As%20Plugin%20in%20ROS
https://blog.csdn.net/heyijia0327/article/details/42241831
https://blog.csdn.net/qq_41995710/article/details/88750354
http://wiki.ros.org/costmap_prohibition_layer
move_base 分层代价地图的作用（翻译）
https://guyuehome.com/28201
