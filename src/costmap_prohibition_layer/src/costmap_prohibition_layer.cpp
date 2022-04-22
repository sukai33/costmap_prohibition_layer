/*********************************************************************
 *
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2016,
 *  TU Dortmund - Institute of Control Theory and Systems Engineering.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the institute nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 * Author: Stephan Kurzawe
 * sukai 注释 2022-03-26
 *********************************************************************/

#include <costmap_prohibition_layer/costmap_prohibition_layer.h>
#include <pluginlib/class_list_macros.h>

PLUGINLIB_EXPORT_CLASS(costmap_prohibition_layer_namespace::CostmapProhibitionLayer, costmap_2d::Layer)

using costmap_2d::LETHAL_OBSTACLE;

namespace costmap_prohibition_layer_namespace
{
    
CostmapProhibitionLayer::CostmapProhibitionLayer() : _dsrv(NULL)
{
}

CostmapProhibitionLayer::~CostmapProhibitionLayer()
{
    if (_dsrv!=NULL)
        delete _dsrv;
}

void CostmapProhibitionLayer::onInitialize()
{
  ros::NodeHandle nh("~/" + name_);
  current_ = true;

  _dsrv = new dynamic_reconfigure::Server<CostmapProhibitionLayerConfig>(nh);
  dynamic_reconfigure::Server<CostmapProhibitionLayerConfig>::CallbackType cb =
      boost::bind(&CostmapProhibitionLayer::reconfigureCB, this, _1, _2);
  _dsrv->setCallback(cb);

  // get a pointer to the layered costmap and save resolution
  costmap_2d::Costmap2D *costmap = layered_costmap_->getCostmap();
  _costmap_resolution = costmap->getResolution();

  //todo sukai 这里加载虚拟墙数据库
    //数据库操作
   SqlQueryClient = nh.serviceClient<disinfect_srvs::testInfo_srvs>("/QSqlQuery");
    //更改字典数据
   //SqlQueryClient.waitForExistence();
    //虚拟墙操作 data：refreshCostMapPlayer
    costmapProhibitionControl_sub = nh.subscribe<std_msgs::String>("/costmapProhibitionControl", 10, &CostmapProhibitionLayer::callBack_costmapProhibitionControl, this);


   // set initial bounds
  _min_x = _min_y = _max_x = _max_y = 0;
  
  // reading the prohibition areas out of the namespace of this plugin!
  // e.g.: "move_base/global_costmap/prohibition_layer/prohibition_areas"


  std::string params = "prohibition_areas";
    //todo 苏凯 加载虚拟墙数据
    if (!parseProhibitionListFromSql())
        ROS_ERROR_STREAM("sukai Reading prohibition areas from sql TABLE_COSTMAPLAYER'" << "' failed!");

    //todo  作废 用上面的 （原）
 // if (!parseProhibitionListFromYaml(&nh, params))
  //  ROS_ERROR_STREAM("Reading prohibition areas from '" << nh.getNamespace() << "/" << params << "' failed!");


  //========================================================================
  _fill_polygons = true;
  nh.param("fill_polygons", _fill_polygons, _fill_polygons);

  //todo （原） compute map bounds for the current set of prohibition areas. 更新costmap区域，updateBounds的参数
  computeMapBounds();
  
  ROS_INFO("CostmapProhibitionLayer initialized.");
}

void CostmapProhibitionLayer::reconfigureCB(CostmapProhibitionLayerConfig &config, uint32_t level)
{
  enabled_ = config.enabled;
  _fill_polygons = config.fill_polygons;
}

//更新costmap区域，updateBounds的参数 robot_x,robot_y,robot_yaw 是机器人的当前位姿，不需要我们人为的去设定，ROS会自动传递这几个参数
// https://blog.csdn.net/heyijia0327/article/details/42241831
void CostmapProhibitionLayer::updateBounds(double robot_x, double robot_y, double robot_yaw, 
                                           double *min_x, double *min_y, double *max_x, double *max_y)
{
    if (!enabled_)
        return;
    
    std::lock_guard<std::mutex> l(_data_mutex);
    
    if (_prohibition_points.empty() && _prohibition_polygons.empty()){
      //  std::cout<<"=====sukai===================_prohibition_points.empty() && _prohibition_polygons.empty()======================="<<std::endl;
        return;
    }
    //todo sukai
   // mark_x_ = robot_x + cos(robot_yaw);
   // mark_y_ = robot_y + sin(robot_yaw);


    *min_x = std::min(*min_x, _min_x);
    *min_y = std::min(*min_y, _min_y);
    *max_x = std::max(*max_x, _max_x);
    *max_y = std::max(*max_y, _max_y);

}
//更新costmap的值
void CostmapProhibitionLayer::updateCosts(costmap_2d::Costmap2D &master_grid, int min_i, int min_j, int max_i, int max_j)
{
  if (!enabled_)
    return;

  std::lock_guard<std::mutex> l(_data_mutex);
  
  // set costs of polygons
  for (int i = 0; i < _prohibition_polygons.size(); ++i)
  {
      setPolygonCost(master_grid, _prohibition_polygons[i], LETHAL_OBSTACLE, min_i, min_j, max_i, max_j, _fill_polygons);
  }
      
  // set cost of points
  for (int i = 0; i < _prohibition_points.size(); ++i)
  {
    unsigned int mx;
    unsigned int my;
    if (master_grid.worldToMap(_prohibition_points[i].x, _prohibition_points[i].y, mx, my))
    {
      master_grid.setCost(mx, my, LETHAL_OBSTACLE);
    }
  }
}

void CostmapProhibitionLayer::computeMapBounds()
{
  std::lock_guard<std::mutex> l(_data_mutex);
    
  // reset bounds
  _min_x = _min_y = _max_x = _max_y = 0;
    
  // iterate polygons
  for (int i = 0; i < _prohibition_polygons.size(); ++i)
  {
    for (int j=0; j < _prohibition_polygons.at(i).size(); ++j)
    {
      double px = _prohibition_polygons.at(i).at(j).x;
      double py = _prohibition_polygons.at(i).at(j).y;
      _min_x = std::min(px, _min_x);
      _min_y = std::min(py, _min_y);
      _max_x = std::max(px, _max_x);
      _max_y = std::max(py, _max_y);
    }
  }

  // iterate points
  for (int i = 0; i < _prohibition_points.size(); ++i)
  {
      double px = _prohibition_points.at(i).x;
      double py = _prohibition_points.at(i).y;
      _min_x = std::min(px, _min_x);
      _min_y = std::min(py, _min_y);
      _max_x = std::max(px, _max_x);
      _max_y = std::max(py, _max_y);
  }
}


void CostmapProhibitionLayer::setPolygonCost(costmap_2d::Costmap2D &master_grid, const std::vector<geometry_msgs::Point>& polygon, unsigned char cost,
                                             int min_i, int min_j, int max_i, int max_j, bool fill_polygon)
{
    std::vector<PointInt> map_polygon;
    for (unsigned int i = 0; i < polygon.size(); ++i)
    {
        PointInt loc;
        master_grid.worldToMapNoBounds(polygon[i].x, polygon[i].y, loc.x, loc.y);
        map_polygon.push_back(loc);
    }

    std::vector<PointInt> polygon_cells;

    // get the cells that fill the polygon
    rasterizePolygon(map_polygon, polygon_cells, fill_polygon);

    // set the cost of those cells
    for (unsigned int i = 0; i < polygon_cells.size(); ++i)
    {
        int mx = polygon_cells[i].x;
        int my = polygon_cells[i].y;
        // check if point is outside bounds
        if (mx < min_i || mx >= max_i)
            continue;
        if (my < min_j || my >= max_j)
            continue;
        master_grid.setCost(mx, my, cost);
    }
}


void CostmapProhibitionLayer::polygonOutlineCells(const std::vector<PointInt>& polygon, std::vector<PointInt>& polygon_cells)
  {
     for (unsigned int i = 0; i < polygon.size() - 1; ++i)
     {
       raytrace(polygon[i].x, polygon[i].y, polygon[i + 1].x, polygon[i + 1].y, polygon_cells);
     }
     if (!polygon.empty())
     {
       unsigned int last_index = polygon.size() - 1;
       // we also need to close the polygon by going from the last point to the first
       raytrace(polygon[last_index].x, polygon[last_index].y, polygon[0].x, polygon[0].y, polygon_cells);
     }
  }

void CostmapProhibitionLayer::raytrace(int x0, int y0, int x1, int y1, std::vector<PointInt>& cells)
{
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    PointInt pt;
    pt.x = x0;
    pt.y = y0;
    int n = 1 + dx + dy;
    int x_inc = (x1 > x0) ? 1 : -1;
    int y_inc = (y1 > y0) ? 1 : -1;
    int error = dx - dy;
    dx *= 2;
    dy *= 2;
        
    for (; n > 0; --n)
    {
        cells.push_back(pt);

        if (error > 0)
        {
            pt.x += x_inc;
            error -= dy;
        }
        else
        {
            pt.y += y_inc;
            error += dx;
        }
    }
}


void CostmapProhibitionLayer::rasterizePolygon(const std::vector<PointInt>& polygon, std::vector<PointInt>& polygon_cells, bool fill)
{
    // this implementation is a slighly modified version of Costmap2D::convexFillCells(...)

    //we need a minimum polygon of a traingle
    if(polygon.size() < 3)
        return;

    //first get the cells that make up the outline of the polygon
    polygonOutlineCells(polygon, polygon_cells);

    if (!fill)
        return;

    //quick bubble sort to sort points by x
    PointInt swap;
    unsigned int i = 0;
    while(i < polygon_cells.size() - 1)
    {
        if(polygon_cells[i].x > polygon_cells[i + 1].x)
        {
            swap = polygon_cells[i];
            polygon_cells[i] = polygon_cells[i + 1];
            polygon_cells[i + 1] = swap;

            if(i > 0)
            --i;
        }
        else
            ++i;
        }

        i = 0;
        PointInt min_pt;
        PointInt max_pt;
        int min_x = polygon_cells[0].x;
        int max_x = polygon_cells[(int)polygon_cells.size() -1].x;

        //walk through each column and mark cells inside the polygon
        for(int x = min_x; x <= max_x; ++x)
        {
            if(i >= (int)polygon_cells.size() - 1)
                break;

            if(polygon_cells[i].y < polygon_cells[i + 1].y)
            {
                min_pt = polygon_cells[i];
                max_pt = polygon_cells[i + 1];
            }
            else
            {
                min_pt = polygon_cells[i + 1];
                max_pt = polygon_cells[i];
            }

            i += 2;
            while(i < polygon_cells.size() && polygon_cells[i].x == x)
            {
                if(polygon_cells[i].y < min_pt.y)
                    min_pt = polygon_cells[i];
                else if(polygon_cells[i].y > max_pt.y)
                    max_pt = polygon_cells[i];
                ++i;
            }

            PointInt pt;
            //loop though cells in the column
            for(int y = min_pt.y; y < max_pt.y; ++y)
            {
                pt.x = x;
                pt.y = y;
                polygon_cells.push_back(pt);
            }
        }
  }


//2. todo sukai load prohibition positions out of the sql server 加载
bool CostmapProhibitionLayer::parseProhibitionListFromSql()
    {
        bool ret_val = true;

         //数据库设计 costmapProhibitionLayer
        //FigureType:point point_A_x point_A_y
        //FigureType:line point_A_x point_A_y point_B_x point_B_y
        //FigureType:polygons point_A_x point_A_y point_B_x point_B_y point_C_x point_C_y point_D_x point_D_y
       //设计数据库【x,y】,【x,y】
       //查询虚拟墙数据
        disinfect_srvs::testInfo_srvs table_CostMapLayerService;
        table_CostMapLayerService.request.request_type="queryAllFromTable_CostMapPlayer";
        SqlQueryClient.call(table_CostMapLayerService);

        if(table_CostMapLayerService.response.result_Table_CostMapLayers.size()<=0){
            //std::cout<<"0. ============================table_CostMapLayerService.response.result_Table_CostMapLayers.size()================================: "<<std::to_string(table_CostMapLayerService.response.result_Table_CostMapLayers.size())<<std::endl;
            std::cout<<"===================虚拟墙数据库中没有数据请检查数据库： 【 TABLE_COSTMAPLAYER 】 rosrun slamservice currencyChassis_KSDemoDlgTABLE_COSTMAPLAYERMain_node 虚拟墙 ================"<<std::endl;
            return false;
        }

        //2.（判断当前任务路点是否被占用）通过任务数据查询路点数据，获取路点数据的状态值判断是否需要去备用点等待 【取餐点最后处理，等其它点都走完后处理】
        for (int i = 0; i <table_CostMapLayerService.response.result_Table_CostMapLayers.size() ; ++i) {
            std::vector<geometry_msgs::Point> vector_to_add;
           if(table_CostMapLayerService.response.result_Table_CostMapLayers[i].FigureType=="point"){//点

               geometry_msgs::Point point;
               point.x =atof(table_CostMapLayerService.response.result_Table_CostMapLayers[i].POINT_A_X.c_str());
               point.y =atof(table_CostMapLayerService.response.result_Table_CostMapLayers[i].POINT_A_Y.c_str());
               point.z = 0.0;
               _prohibition_points.push_back(point);
               //std::cout<<"1.FigureType==point 1.point.x: "<<std::to_string(point.x)<<std::endl;
              // std::cout<<"1.FigureType==point 1.point.y: "<<std::to_string(point.y)<<std::endl;
           }else if(table_CostMapLayerService.response.result_Table_CostMapLayers[i].FigureType=="line"){//线
               geometry_msgs::Point point_A;
               point_A.x =atof(table_CostMapLayerService.response.result_Table_CostMapLayers[i].POINT_A_X.c_str());
               point_A.y =atof(table_CostMapLayerService.response.result_Table_CostMapLayers[i].POINT_A_Y.c_str());
               point_A.z = 0.0;
               vector_to_add.push_back(point_A);
               std::cout<<"2.  FigureType==line point_A.x: "<<std::to_string(point_A.x)<<std::endl;
               std::cout<<"2.  FigureType==line point_A.y: "<<std::to_string(point_A.y)<<std::endl;
               geometry_msgs::Point point_B;
               point_B.x =atof(table_CostMapLayerService.response.result_Table_CostMapLayers[i].POINT_B_X.c_str());
               point_B.y =atof(table_CostMapLayerService.response.result_Table_CostMapLayers[i].POINT_B_Y.c_str());
               point_B.z = 0.0;
               vector_to_add.push_back(point_B);
               //todo dayin
               std::cout<<"2.  FigureType==line  point_B.x: "<<std::to_string(point_B.x)<<std::endl;
               std::cout<<"2.  FigureType==line  point_B.y: "<<std::to_string(point_B.y)<<std::endl;
               // calculate the normal vector for AB  计算AB的法向量
               geometry_msgs::Point point_N;
               point_N.x = point_B.y - point_A.y;
               point_N.y = point_A.x - point_B.x;
               //todo dayin
               std::cout<<"2. line point_N.x: "<<std::to_string(point_N.x)<<std::endl;
               std::cout<<"2. line point_N.y: "<<std::to_string(point_N.y)<<std::endl;
               // get the absolute value of N to normalize and get 获取N的绝对值以进行规格化并获取
               // it to the length of the costmap resolution 它与costmap分辨率的长度相同
               double abs_N = sqrt(pow(point_N.x, 2) + pow(point_N.y, 2));
               std::cout<<"2.a line abs_N: "<<std::to_string(abs_N)<<std::endl;
               point_N.x = point_N.x / abs_N * _costmap_resolution;
               point_N.y = point_N.y / abs_N * _costmap_resolution;
               std::cout<<"2. line point_N.x: point_N.x / abs_N * _costmap_resolution "<<std::to_string(point_N.x)<<std::endl;
               std::cout<<"2. line point_N.y: point_N.y / abs_N * _costmap_resolution "<<std::to_string(point_N.y)<<std::endl;
               // calculate the new points to get a polygon which can be filled 计算新点以获得可填充的多边形
               geometry_msgs::Point point;
               point.x = point_A.x + point_N.x;
               point.y = point_A.y + point_N.y;
               vector_to_add.push_back(point);
               std::cout<<"2. line point.x: "<<std::to_string(point.x)<<std::endl;
               std::cout<<"2. line point.y: "<<std::to_string(point.y)<<std::endl;
               point.x = point_B.x + point_N.x;
               point.y = point_B.y + point_N.y;
               std::cout<<"2. line point.x: "<<std::to_string(point.x)<<std::endl;
               std::cout<<"2. line point.y: "<<std::to_string(point.y)<<std::endl;
               vector_to_add.push_back(point);

               _prohibition_polygons.push_back(vector_to_add);

           }else if(table_CostMapLayerService.response.result_Table_CostMapLayers[i].FigureType=="polygons"){//多边形
               // add a polygon with any number of points 添加具有任意数量点的多边形
               geometry_msgs::Point point_A;
               point_A.x =atof(table_CostMapLayerService.response.result_Table_CostMapLayers[i].POINT_A_X.c_str());
               point_A.y =atof(table_CostMapLayerService.response.result_Table_CostMapLayers[i].POINT_A_Y.c_str());
               point_A.z = 0.0;
               vector_to_add.push_back(point_A);
              // std::cout<<"3.  FigureType==polygons point_A.x: "<<std::to_string(point_A.x)<<std::endl;
              // std::cout<<"3.  FigureType==polygons point_A.y: "<<std::to_string(point_A.y)<<std::endl;
               geometry_msgs::Point point_B;
               point_B.x =atof(table_CostMapLayerService.response.result_Table_CostMapLayers[i].POINT_B_X.c_str());
               point_B.y =atof(table_CostMapLayerService.response.result_Table_CostMapLayers[i].POINT_B_Y.c_str());
               point_B.z = 0.0;
               vector_to_add.push_back(point_B);
               //std::cout<<"3.  FigureType==polygons point_B.x: "<<std::to_string(point_B.x)<<std::endl;
               //std::cout<<"3.  FigureType==polygons point_B.y: "<<std::to_string(point_B.y)<<std::endl;
               geometry_msgs::Point point_C;
               point_C.x =atof(table_CostMapLayerService.response.result_Table_CostMapLayers[i].POINT_C_X.c_str());
               point_C.y =atof(table_CostMapLayerService.response.result_Table_CostMapLayers[i].POINT_C_Y.c_str());
               point_C.z = 0.0;
               vector_to_add.push_back(point_C);
              // std::cout<<"3.  FigureType==polygons point_C.x: "<<std::to_string(point_C.x)<<std::endl;
              // std::cout<<"3.  FigureType==polygons point_C.y: "<<std::to_string(point_C.y)<<std::endl;
               geometry_msgs::Point point_D;
               point_D.x =atof(table_CostMapLayerService.response.result_Table_CostMapLayers[i].POINT_D_X.c_str());
               point_D.y =atof(table_CostMapLayerService.response.result_Table_CostMapLayers[i].POINT_D_Y.c_str());
               point_D.z = 0.0;
               vector_to_add.push_back(point_D);
               //std::cout<<"3.  FigureType==polygons point_D.x: "<<std::to_string(point_D.x)<<std::endl;
               //std::cout<<"3.  FigureType==polygons point_D.y: "<<std::to_string(point_D.y)<<std::endl;
               _prohibition_polygons.push_back(vector_to_add);
           }

        }

        return ret_val;
    }


    // load prohibition positions out of the rosparam server
bool CostmapProhibitionLayer::parseProhibitionListFromYaml(ros::NodeHandle *nhandle, const std::string &param)
{
  std::lock_guard<std::mutex> l(_data_mutex);
  std::unordered_map<std::string, geometry_msgs::Pose> map_out;

  XmlRpc::XmlRpcValue param_yaml;

  bool ret_val = true;

  if (nhandle->getParam(param, param_yaml))
  {
    if (param_yaml.getType() == XmlRpc::XmlRpcValue::TypeArray)  // list of goals
    {
      for (int i = 0; i < param_yaml.size(); ++i)
      {
        if (param_yaml[i].getType() == XmlRpc::XmlRpcValue::TypeArray)
        {
          std::vector<geometry_msgs::Point> vector_to_add;

          /* **************************************
           * differ between points and polygons
           * lines get to a polygon with the resolution
           * of the costmap
           **************************************** */

          // add a point
          if (param_yaml[i].size() == 1)
          {
            geometry_msgs::Point point;
            ret_val = getPoint(param_yaml[i][0], point);
            _prohibition_points.push_back(point);
          }
          // add a line
          else if (param_yaml[i].size() == 2)
          {
            if (param_yaml[i][0].getType() == XmlRpc::XmlRpcValue::TypeDouble ||
	      param_yaml[i][0].getType() == XmlRpc::XmlRpcValue::TypeInt)
            {
              // add a lonely point
              geometry_msgs::Point point;
              ret_val = getPoint(param_yaml[i], point);
              _prohibition_points.push_back(point);
            }
            else
            {
              // add a line!
              geometry_msgs::Point point_A;
              ret_val = getPoint(param_yaml[i][0], point_A);
              vector_to_add.push_back(point_A);

              geometry_msgs::Point point_B;
              ret_val = getPoint(param_yaml[i][1], point_B);
              vector_to_add.push_back(point_B);

              // calculate the normal vector for AB
              geometry_msgs::Point point_N;
              point_N.x = point_B.y - point_A.y;
              point_N.y = point_A.x - point_B.x;

              // get the absolute value of N to normalize and get
              // it to the length of the costmap resolution
              double abs_N = sqrt(pow(point_N.x, 2) + pow(point_N.y, 2));
              point_N.x = point_N.x / abs_N * _costmap_resolution;
              point_N.y = point_N.y / abs_N * _costmap_resolution;

              // calculate the new points to get a polygon which can be filled
              geometry_msgs::Point point;
              point.x = point_A.x + point_N.x;
              point.y = point_A.y + point_N.y;
              vector_to_add.push_back(point);

              point.x = point_B.x + point_N.x;
              point.y = point_B.y + point_N.y;
              vector_to_add.push_back(point);

              _prohibition_polygons.push_back(vector_to_add);
            }
          }
          // add a point or add a polygon
          else if (param_yaml[i].size() >= 3)
          {
            // add a polygon with any number of points
            for (int j = 0; j < param_yaml[i].size(); ++j)
            {
              geometry_msgs::Point point;
              ret_val = getPoint(param_yaml[i][j], point);
              vector_to_add.push_back(point);
            }
            _prohibition_polygons.push_back(vector_to_add);
          }
        }
        else
        {
          ROS_ERROR_STREAM("Prohibition Layer:" << param << " with index " << i << " is not correct.");
          ret_val = false;
        }
      }
    }
    else
    {
      ROS_ERROR_STREAM("Prohibition Layer: " << param << "struct is not correct.");
      ret_val = false;
    }
  }
  else
  {
    ROS_ERROR_STREAM("Prohibition Layer: Cannot read " << param << " from parameter server");
    ret_val = false;
  }
  return ret_val;
}

// get a point out of the XML Type into a geometry_msgs::Point
bool CostmapProhibitionLayer::getPoint(XmlRpc::XmlRpcValue &val, geometry_msgs::Point &point)
{
  try
  {
    // check if there a two values for the coordinate
    if (val.getType() == XmlRpc::XmlRpcValue::TypeArray && val.size() == 2)
    {
      auto convDouble = [](XmlRpc::XmlRpcValue &val) -> double
      {
        if (val.getType() == XmlRpc::XmlRpcValue::TypeInt)  // XmlRpc cannot cast int to double
          return int(val);
        return val;  // if not double, an exception is thrown;
      };

      point.x = convDouble(val[0]);
      point.y = convDouble(val[1]);
      point.z = 0.0;
      return true;
    }
    else
    {
      ROS_ERROR_STREAM("Prohibition_Layer: A point has to consist two values!");
      return false;
    }
  }
  catch (const XmlRpc::XmlRpcException &ex)
  {
    ROS_ERROR_STREAM("Prohibition Layer: Cannot add current point: " << ex.getMessage());
    return false;
  }
}

//虚拟墙操作 data:start stop refresh
void CostmapProhibitionLayer::callBack_costmapProhibitionControl(std_msgs::String msg){





        try {
             std::cout<<"1. 虚拟墙操作 refresh"<<std::endl;
            //虚拟墙操作 refresh
             if(msg.data=="refreshCostMapPlayer"){
                 _prohibition_points.clear();
                 _prohibition_polygons.clear();
                 parseProhibitionListFromSql();
                 // compute map bounds for the current set of prohibition areas.
                 computeMapBounds();
            }

          std::cout<<"2. 虚拟墙操作 refresh"<<std::endl;
            //log日志
        }catch (std::exception e){
            e.what();
//优先放入队列

            std::cout<<"SocketData QTcpServer 获取小车当前的运行状态描述error: " <<e.what()<<std::endl;
        }

    }


}  // end namespace
// ROS 之 利用超声波障碍物层紧急避障
//  原文链接：https://blog.csdn.net/weixin_40639095/article/details/108489152

//在Costmap_2d里面插入Prohibition_layer(禁止区域层) https://blog.csdn.net/qq_41995710/article/details/88750354
