#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <math.h>
#include <vector>
#include <map>
#include <iostream>

#define PI 3.14159265

using namespace std;
using namespace Eigen;

// "With four parameters I can fit a wall, and with five I can give it windows" -- Darknet YOLO, on elephants in the BWI lab
// parameters involved in finding walls
const float crvt_max = 0.0005;
const float nz_max = 0.0005;
const float min_dist_from_ceil_and_floor = 0.05;
const int min_bin_size = 200;
const float max_var_ratio = 0.1;

const float max_dist = 0.3; // points must be within this distance of each other to go in the same bin. this and the next are the important parameters
const float max_angle = 1.0;

const float max_dist_sq = max_dist*max_dist;
const float sin_max_angle = sin(max_angle);

float max_x, min_x, max_y, min_y, max_z, min_z;
size_t x_len, y_len, angle_len;

class Wall {
  public:
    Vector2f pos;
    float angle;
    Wall(float,float,float);
    Wall(Vector2f,float);
    void bin(size_t);
};

vector<Wall*> empty_bin;

vector<Wall> all_walls;
vector<Wall*> unbinned_walls;
vector< vector<Wall*> > untrimmed_bins;
vector< vector<Wall*> > bins;
vector<Wall*> * boxes;

vector<Wall*> next_layer;

size_t get_box_idx(size_t x, size_t y, size_t angle) {
  return angle*x_len*y_len + y*x_len + x;
}

size_t get_box_idx(int x, int y, int angle) {
  return angle*x_len*y_len + y*x_len + x;
}

size_t get_box_idx(float x, float y, float angle) {
  return x_len*y_len*(angle/max_angle) + x_len*y/(max_dist) + x/max_dist;
}

Wall::Wall(float x, float y, float angle) {
  this->pos(0) = x;
  this->pos(1) = y;
  this->angle = fmod(angle,PI);
}

Wall::Wall(Vector2f pos, float angle) {
  this->pos = pos;
  this->angle = fmod(angle,PI);
}

void Wall::bin(size_t bin) {
  static size_t box;
  box = get_box_idx(this->pos(0),this->pos(1),this->angle);
  cout << "box=" << box << endl;
  for (size_t i=0; i<next_layer.size(); i++) {
    if (next_layer[i]==this) next_layer.erase(next_layer.begin()+i);
  }
  for (size_t i=0; i<boxes[box].size(); i++) {
    if (boxes[box][i] == this) boxes[box].erase(boxes[box].begin()+i);
  }
  for (size_t i=0; i<unbinned_walls.size(); i++) {
    if (unbinned_walls[i] == this) unbinned_walls.erase(unbinned_walls.begin()+i);//should be erasing from the back!
  }
  untrimmed_bins[bin].push_back(this);
  
  for (int i=-1; i<=1; i++) {
    for (int j=-1; j<=1; j++) {
      for (int k=-1; k<=1; k++) {
        int box_x = this->pos(0)/max_dist+i;
        int box_y = this->pos(1)/max_dist+j;
        int box_angle = this->angle/max_angle+k;
        if (0<=box_x and box_x<x_len and 0<=box_y and box_y<y_len and 0<=box_angle and box_angle<angle_len) {
          box = get_box_idx(box_x,box_y,box_angle);
          for (int l=0; l < boxes[box].size(); l++) {
            Rotation2D<float> rot (-this->angle);
            float dist = (this->pos - boxes[box][l]->pos).cwiseAbs2().sum();
            if ( dist < max_dist_sq and std::abs(this->angle - boxes[box][l]->angle) < max_angle and std::abs((rot*(boxes[box][l]->pos))(1))/dist < sin_max_angle )
              next_layer.push_back(boxes[box][l]);
          }
        }
      }
    }
  }
  for (size_t i=0; i<next_layer.size(); i++) next_layer[i]->bin(bin);
}

Wall wall_maker(float x, float y, float angle) {
  Wall wall(x,y,angle);
  return wall;
}
