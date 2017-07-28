#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/Eigenvalues>
#include <map>
#include <math.h>
#include <algorithm>
#include <time.h>
#include <stdio.h>
#include "wall.hpp"

using namespace Eigen;
using namespace std;

Vector2f average(vector<Wall*>& bin) {
  Vector2f total = Vector2f::Zero();
  for(auto &b : bin) {
    total += b->pos;
  }
  Vector2f avg = total * (1.0/(float)bin.size());
  return avg;
}

Vector2f pca(vector<Wall*>& bin) {
  Vector2f avg = average(bin);
  Vector2f var = Vector2f::Zero();
  for(auto &b : bin) {
    var += (b->pos - avg).cwiseAbs2();
  }
  var /= bin.size();
  float varx = var(0);
  float vary = var(1);
  float cvar = 0.0;
  for(auto &b : bin) {
    cvar += (b->pos(0) - avg(0)) * (b->pos(1) - avg(1));
  }
  cvar /= bin.size();
  MatrixXf cvarmat;
  cvarmat.resize(2,2);
  cvarmat << varx, cvar, cvar, vary;
  EigenSolver<MatrixXf> es(cvarmat,false);
  Vector2cf pvars = es.eigenvalues();
  return pvars.cwiseAbs();
}


void raw_to_wall(vector<float>& vertices, vector<float>& normals, vector<float>& crvts) {
  max_x=vertices[0];
  min_x=vertices[0];
  max_y=vertices[1];
  min_y=vertices[1];
  max_z=vertices[2];
  min_z=vertices[2];
  
  for (int i=1; i < vertices.size()/3; ++i) { // find extremes
    max_x = (vertices[3*i] > max_x ? vertices[3*i] : max_x);
    min_x = (vertices[3*i] < min_x ? vertices[3*i] : min_x);
    max_y = (vertices[3*i + 1] > max_y ? vertices[3*i + 1] : max_y);
    min_y = (vertices[3*i + 1] < min_y ? vertices[3*i + 1] : min_y);
    max_z = (vertices[3*i + 2] > max_z ? vertices[3*i + 2] : max_z);
    min_z = (vertices[3*i + 2] < min_z ? vertices[3*i + 2] : min_z);
  }
  
  x_len = (int)(max_x-min_x)/max_dist+1;
  y_len = (int)(max_y-min_y)/max_dist+1;
  angle_len = (int)(PI/max_angle)+1;
  total_len = x_len*y_len*angle_len;
  boxes.resize(total_len);
  
  int all_walls_size=0;
  for (int i=0; i < vertices.size()/3; ++i) {
    int j=0;
    if (std::abs(crvts[i]) < crvt_max and
        std::abs(normals[3*i + 2]) < nz_max and
        vertices[3*i + 2] < max_z - min_dist_from_ceil_and_floor and
        vertices[3*i + 2] > min_z + min_dist_from_ceil_and_floor) {
      all_walls_size++;
    }
  }
  all_walls.reserve(all_walls_size);
  int j=0;
  for (int i=0; i < vertices.size()/3; ++i) { // do filtering
    if (std::abs(crvts[i]) < crvt_max and
        std::abs(normals[3*i + 2]) < nz_max and
        vertices[3*i + 2] < max_z - min_dist_from_ceil_and_floor and
        vertices[3*i + 2] > min_z + min_dist_from_ceil_and_floor) {
      float temp_angle = atan2(-vertices[3*i+1],vertices[3*i]);
      all_walls.emplace_back( vertices[3*i], vertices[3*i+1], temp_angle<0 ? temp_angle+PI : temp_angle );
      unbinned_walls.push_back(&all_walls[j]);
      boxes[all_walls[j].get_box()].push_back(&all_walls[j]);
      //if (j<10) all_walls[j].print();
      j++;
    }
  }
}
