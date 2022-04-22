#!/bin/bash
#sukai 2022-04-21
# source catkit_make_source.sh
#catkin_make --pkg yocs_msgs
catkin_make --pkg disinfect_msg  
catkin_make --pkg disinfect_srvs
catkin_make
if [[ $? != 0 ]]; then
echo 'catkin_make :error'
else
echo 'catkin_make :ok'
source devel/setup.bash
echo 'source devel/setup.bash :ok'
fi
