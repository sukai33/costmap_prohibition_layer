cmake_minimum_required(VERSION 3.0.2)
project(slamproject)

add_compile_options(-std=c++11 )
add_compile_options(-std=c++14 )

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -O3 -mtune=native ")





#######################################################################################################




message(PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}")
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rosmsg
  rospy
  std_msgs
        sensor_msgs
        cv_bridge
        image_transport
        tf
      
        disinfect_msg
        disinfect_srvs
        geometry_msgs

)

find_package(OpenCV REQUIRED )


set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets PrintSupport Network SerialPort Sql WebSockets WebChannel  REQUIRED)






################################################
## Declare ROS messages, services and actions ##
################################################

## To declare and build messages, services or actions from within this
## package, follow these steps:
## * Let MSG_DEP_SET be the set of packages whose message types you use in
##   your messages/services/actions (e.g. std_msgs, actionlib_msgs, ...).
## * In the file package.xml:
##   * add a build_depend tag for "message_generation"
##   * add a build_depend and a exec_depend tag for each package in MSG_DEP_SET
##   * If MSG_DEP_SET isn't empty the following dependency has been pulled in
##     but can be declared for certainty nonetheless:
##     * add a exec_depend tag for "message_runtime"
## * In this file (CMakeLists.txt):
##   * add "message_generation" and every package in MSG_DEP_SET to
##     find_package(catkin REQUIRED COMPONENTS ...)
##   * add "message_runtime" and every package in MSG_DEP_SET to
##     catkin_package(CATKIN_DEPENDS ...)
##   * uncomment the add_*_files sections below as needed
##     and list every .msg/.srv/.action file to be processed
##   * uncomment the generate_messages entry below
##   * add every package in MSG_DEP_SET to generate_messages(DEPENDENCIES ...)

## Generate messages in the 'msg' folder
# add_message_files(
#   FILES
#   Message1.msg
#   Message2.msg
# )

## Generate services in the 'srv' folder
# add_service_files(
#   FILES
#   Service1.srv
#   Service2.srv
# )

## Generate actions in the 'action' folder
# add_action_files(
#   FILES
#   Action1.action
#   Action2.action
# )

## Generate added messages and services with any dependencies listed here
# generate_messages(
#   DEPENDENCIES
#   std_msgs  # Or other packages containing msgs
# )

################################################
## Declare ROS dynamic reconfigure parameters ##
################################################

## To declare and build dynamic reconfigure parameters within this
## package, follow these steps:
## * In the file package.xml:
##   * add a build_depend and a exec_depend tag for "dynamic_reconfigure"
## * In this file (CMakeLists.txt):
##   * add "dynamic_reconfigure" to
##     find_package(catkin REQUIRED COMPONENTS ...)
##   * uncomment the "generate_dynamic_reconfigure_options" section below
##     and list every .cfg file to be processed

## Generate dynamic reconfigure parameters in the 'cfg' folder
# generate_dynamic_reconfigure_options(
#   cfg/DynReconf1.cfg
#   cfg/DynReconf2.cfg
# )

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if your package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES visualOrientation
#  CATKIN_DEPENDS roscpp rosmsg rospy
#  DEPENDS system_lib
        #CATKIN_DEPENDS geometry_msgs roscpp rosmsg rospy  tf
        CATKIN_DEPENDS geometry_msgs roscpp rosmsg rospy  tf
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations




include_directories(
 include
  ${catkin_INCLUDE_DIRS}
      ${OpenCV_INCLUDE_DIRS}
        ${Qt5Core_INCLUDE_DIRS}
        ${Qt5Gui_INCLUDE_DIRS}
        ${Qt5Widgets_INCLUDE_DIRS}
        ${Qt5Network_INCLUDE_DIRS}
        ${Qt5SerialPort_INCLUDE_DIRS}
        ${Qt5Sql_INCLUDE_DIRS}
        ${Qt5WebSockets_INCLUDE_DIRS}
        ${Qt5WebChannel_INCLUDE_DIRS}


)
# /usr/lib/x86_64-linux-gnu/libpthread.a

#message(STATUS ${Qt5WebChannel_INCLUDE_DIRS})

#        ${Qt5websockets_INCLUDE_DIRS}
include_directories(/usr/include/eigen3)


##########################################################################

add_executable(currencyChassis_KSDemoDlgTABLE_COSTMAPLAYERMain_node src/currencyChassis/currencyChassis_KSDemoDlgTABLE_COSTMAPLAYERMain.cpp src/currencyChassis/currencyChassis_KSDemoDlgTABLE_COSTMAPLAYER.cpp)

add_executable(currencyChassis_CreateSqlDatabaseMain_node src/currencyChassis/currencyChassis_CreateSqlDatabaseMain.cpp)
add_executable(currencyChassis_sqlDatabaseMain_node src/currencyChassis/currencyChassis_sqlDatabaseMain.cpp)







#####################################################################

## Rename C++ executable without prefix
## The above recommended prefix causes long target names, the following renames the
## target back to the shorter version for ease of user use
## e.g. "rosrun someones_pkg node" instead of "rosrun someones_pkg someones_pkg_node"
# set_target_properties(${PROJECT_NAME}_node PROPERTIES OUTPUT_NAME node PREFIX "")

## Add cmake target dependencies of the executable
## same as for the library above
# add_dependencies(${PROJECT_NAME}_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
# qt加载的库
SET(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5OpenGL_LIBRARIES}
        ${Qt5Network_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Qml_LIBRARIES}  ${Qt5SerialPort_LIBRARIES} ${Qt5Sql_LIBRARIES}  ${Qt5WebSockets_LIBRARIES}  ${Qt5WebChannel_LIBRARIES}  )

message(STATUS "    Qt5: ${QT_LIBRARIES}")
## Specify libraries to link a library or executable target against

##########################################################################
###############currencyChassis#############龙源############################



target_link_libraries(
        currencyChassis_KSDemoDlgTABLE_COSTMAPLAYERMain_node
        #${OpenCV_LIBS}
        ${util}
        ${tinyxml}
        ${catkin_LIBRARIES}
        ${QT_LIBRARIES}
)





target_link_libraries(
        currencyChassis_CreateSqlDatabaseMain_node
        #${OpenCV_LIBS}
        ${util}
        ${tinyxml}
        ${catkin_LIBRARIES}
        ${QT_LIBRARIES}
)
target_link_libraries(
        currencyChassis_sqlDatabaseMain_node
        #${OpenCV_LIBS}
        ${util}
        ${tinyxml}
        ${catkin_LIBRARIES}
        ${QT_LIBRARIES}
)





##########################################################################



#############
## Install ##
#############

# all install targets should use catkin DESTINATION variables
# See http://ros.org/doc/api/catkin/html/adv_user_guide/variables.html

## Mark executable scripts (Python etc.) for installation
## in contrast to setup.py, you can choose the destination
# catkin_install_python(PROGRAMS
#   scripts/my_python_script
#   DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

## Mark executables for installation
## See http://docs.ros.org/melodic/api/catkin/html/howto/format1/building_executables.html
# install(TARGETS ${PROJECT_NAME}_node
#   RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

## Mark libraries for installation
## See http://docs.ros.org/melodic/api/catkin/html/howto/format1/building_libraries.html
# install(TARGETS ${PROJECT_NAME}
#   ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION}
# )

## Mark cpp header files for installation
# install(DIRECTORY include/${PROJECT_NAME}/
#   DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
#   FILES_MATCHING PATTERN "*.h"
#   PATTERN ".svn" EXCLUDE
# )

## Mark other files for installation (e.g. launch and bag files, etc.)
# install(FILES
#   # myfile1
#   # myfile2
#   DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
# )

#############
## Testing ##
#############

## Add gtest based cpp test target and link libraries
# catkin_add_gtest(${PROJECT_NAME}-test test/test_visualOrientation.cpp)
# if(TARGET ${PROJECT_NAME}-test)
#   target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME})
# endif()
#
## Add folders to be run by python nosetests
# catkin_add_nosetests(test)


#c++ 打包 | 把所有node节点名称加入 install
install(TARGETS
         currencyChassis_KSDemoDlgTABLE_COSTMAPLAYERMain_node
         currencyChassis_CreateSqlDatabaseMain_node
        currencyChassis_sqlDatabaseMain_node
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}

)

#静态资源打包
install(DIRECTORY launch
DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
PATTERN ".svn" EXCLUDE)
install(DIRECTORY filepath param script/source images db config
        DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)

#catkin_install_python(PROGRAMS src/ laser2pc.py
# DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
