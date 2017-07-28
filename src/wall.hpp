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
const int min_bin_size = 0;
const float max_var_ratio = 0.1;

int total_len;

const float max_dist = 0.3; // points must be within this distance of each other to go in the same bin. this and the next are the important parameters
const float max_angle = 1.0;

const float max_dist_sq = max_dist*max_dist;
const float sin_max_angle = sin(max_angle);

float max_x, min_x, max_y, min_y, max_z, min_z;
int x_len, y_len, angle_len;

int bin;

class Wall {
  public:
    Vector2f pos;
    float angle;
    Wall(float,float,float);
    Wall(Vector2f,float);
    Wall();
    int get_box();
    void binnize();
    void print();
};

vector<Wall*> empty_bin;

vector<Wall> all_walls;
vector<Wall*> unbinned_walls;
vector< vector<Wall*> > untrimmed_bins;
vector< vector<Wall*> > bins;
//vector<Wall*> * boxes;
vector< vector<Wall*> > boxes;

vector<Wall*> next_layer;


int get_box_idx(int x, int y, int angle) {
  int ret = angle*x_len*y_len + y*x_len + x;
  if (0<=ret and ret<total_len) return ret;
  else {
    cout << "GOT WRONG INDEX: " << x << "," << y << "," << angle << " --> " << ret << endl;
    exit(1);
  }
}

int get_box_idx(float x, float y, float angle) {
  return get_box_idx((int)((x-min_x)/max_dist),(int)((y-min_y)/max_dist),(int)(angle/max_angle));
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

Wall::Wall() { }

int Wall::get_box() {
  return get_box_idx(this->pos(0),this->pos(1),this->angle);
}

void Wall::print() {
  cout << this << ":\t" << "( " << this->pos(0) << ", " << this->pos(1) << ", " << this->angle << " ):\t" << this->get_box() << endl;
}

void Wall::binnize() {
  int box_x = (int)((this->pos(0)-min_x)/max_dist);
  int box_y = (int)((this->pos(1)-min_y)/max_dist);
  int box_angle = (int)(this->angle/max_angle);
  int box = get_box_idx(box_x,box_y,box_angle);
  next_layer.pop_back();
  untrimmed_bins[bin].push_back(this);
  
  for (int i=0; i<unbinned_walls.size(); i++) {
    if (unbinned_walls[i] == this) {
      unbinned_walls.erase(unbinned_walls.begin()+i);//should be erasing from the back!
      break;
    }
  }
  
  for (int i=-1; i<=1; i++) { for (int j=-1; j<=1; j++) { for (int k=-1; k<=1; k++) {
    box_x = (int)((this->pos(0)-min_x)/max_dist)+i;
    box_y = (int)((this->pos(1)-min_y)/max_dist)+j;
    box_angle = (int)(this->angle/max_angle)+k;
    if (0<=box_x and box_x<x_len and 0<=box_y and box_y<y_len and 0<=box_angle and box_angle<angle_len) {
      box = get_box_idx(box_x,box_y,box_angle);
      for (int l=0; l < boxes[box].size(); l++) {
        Rotation2D<float> rot (-(this->angle));
        float dist_sq = (this->pos - boxes[box][l]->pos).cwiseAbs2().sum();
        if ( dist_sq < max_dist_sq and (std::abs(this->angle - boxes[box][l]->angle) < max_angle or PI-std::abs(this->angle - boxes[box][l]->angle) < max_angle) and std::abs((rot*(boxes[box][l]->pos - this->pos))(1))/sqrt(dist_sq) < 0.5*sin_max_angle ) {
          next_layer.push_back(boxes[box][l]);
          boxes[box].erase(boxes[box].begin()+l);
          l--;
        }
      }
    }
  } } }
}
