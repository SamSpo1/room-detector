#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <math.h>
#include <vector>
#include <map>

#define PI 3.14159265

using namespace std;
using namespace Eigen;

float max_dist, max_dist_sq, max_angle;
size_t x_len, y_len, angle_len;
float sin_max_angle;

class Wall {
  public:
    static void init(float,float,float[]);

    Vector2f pos;
    float angle;
    Wall(float,float,float);
    Wall(Vector2f,float);
    void bin(vector< vector<Wall*> >&,int);
};

vector<Wall*> * boxes;

size_t get_box_idx(size_t x, size_t y, size_t angle) {
  return angle*x_len*y_len + y*x_len + x;
}

size_t get_box_idx(float x, float y, float angle) {
  return x_len*y_len*(angle/max_angle) + x_len*y/(max_dist) + x/max_dist;
}

void Wall::init(float in_max_dist, float in_max_angle, float extremes[]) {
  max_dist = in_max_dist;
  max_dist_sq = in_max_dist*in_max_dist;
  max_angle = in_max_angle;
  sin_max_angle = sin(max_angle);
  x_len = (extremes[1]-extremes[0])/max_dist;
  y_len = (extremes[3]-extremes[2])/max_dist;
  angle_len = PI/max_angle;
  const size_t total_len = x_len*y_len*angle_len;
  boxes = new vector<Wall*> [total_len];
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

void Wall::bin(vector< vector<Wall*> >& bins, int bin) {
  static vector<Wall*> next_layer {this};
  size_t box = get_box_idx(this->pos(0),this->pos(1),this->angle);
  for (size_t i=0; i<next_layer.size(); i++) {
    if (next_layer[i]==this) next_layer.erase(next_layer.begin()+i);
  }
  for (size_t i=0; i<boxes[box].size(); i++) {
    if (boxes[box][i] == this) boxes[box].erase(boxes[box].begin()+i);
  }
  bins[bin].push_back(this);
  
  for (int i=-1; i<=1; i++) {
    for (int j=-1; j<=1; j++) {
      for (int k=-1; k<=1; k++) {
        size_t box = get_box_idx(this->pos(0)/max_dist+i,this->pos(1)/max_dist+j,this->angle/max_angle+k);
        for (int l=0; l < boxes[box].size(); l++) {
          Rotation2D<float> rot (-this->angle);
          float dist = (this->pos - boxes[box][l]->pos).cwiseAbs2().sum();
          if ( dist < max_dist_sq and std::abs(this->angle - boxes[box][l]->angle) < max_angle and std::abs((rot*(boxes[box][l]->pos))(1))/dist < sin_max_angle )
            next_layer.push_back(boxes[box][l]);
        }
      }
    }
  }
  for (int i=0; i<next_layer.size(); i++)
    next_layer[i]->bin(bins,bin);
}

int main() {
  return 0;
}
