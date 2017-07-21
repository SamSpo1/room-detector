#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <map>
#include <math.h>
#include <stdlib.h>

#include "rtabmap_reader.h"

using namespace Eigen;

const int min_bin_size = 500;
const float max_dist = 0.5;
const float max_angle = 0.05;
float cmax_angle = std::abs(cos(max_angle));


ParametrizedLine<float, 2> pca( std::vector<Vector2f>& curr_verts ) {
  Vector2f tot=Vector2f::Zero();
  for (int i=0; i<curr_verts.size(); i++)
    tot = tot + curr_verts[i];
  
  


void merge_bins(std::vector< ParametrizedLine<float, 2> >& binout, std::vector< ParametrizedLine<float, 2> >& binin) {
  while (!binin.empty()) {
    binout.push_back(binin.back());
    binin.pop_back();
  }
}

bool approxSame(ParametrizedLine<float,2>& a, ParametrizedLine<float,2>& b) {
  return ( (a.pointAt(0)-b.pointAt(0)).norm()<max_dist and std::abs((a.pointAt(1)-a.pointAt(0)).dot( (b.pointAt(1)-b.pointAt(0)))) > cmax_angle );
}


int main(int argc, char *argv[])
{
	if (argc != 2) { std::cout << "Usage: ./pca_find_walls plyfile" << std::endl; return -1; }
	read_ply_file(argv[1]);
	
	std::vector<Vector2f> vertices;
  Vector2f vertex;
  for (int i=0; i<verts.size()/3; i+=1) {
    if (crvts[i] < 0.1) {
      vertex(0) = verts[3*i];
      vertex(1) = verts[3*i+1];
      vertices.push_back(vertex);
    }
  }
  

  
  srand(chrono::system_clock::now().time_since_epoch().count());
  
  std::vector< ParametrizedLine<float, 2> > all_walls;
  for (int i=0; i<1000; i++) { // parameter (next line is the same parameter too)
    int v = ((float)rand()/(float)RAND_MAX)*1000;
    std::vector<Vector2f> curr_verts;
    for (int j=0; j<vertices.size(); j++) {
      if ( (vertices[j]-vertices[v]).norm()<0.1 ) // parameter
        curr_verts.push_back(vertices[j]);
    }
    
    all_walls.push_back(pca(curr_verts));
  }
      
  
  /*for (int i=0; i<100; i++) {
    for (int j=0; j<100; j++) {
    */  
    
    

  
  std::vector< std::vector< ParametrizedLine<float, 2> > > wall_bins;
  
  std::cout << "Putting " << vertices.size() << " walls in bins..." << std::endl;
  bool taken=false;
  int bin;
  for (int i=0; i<all_walls.size(); i++) {
    taken=false;
    for (int j=0; j<wall_bins.size() and !taken; j++) {
      for (int k=0; k<wall_bins[j].size(); k++) {
        if (approxSame(all_walls[i], wall_bins[j][k])) {
          wall_bins[j].push_back(all_walls[i]);
          taken=true;
          bin=j;
          break;
        }
      }
    }
    
    if (taken) {
      for (int j=bin+1; j<wall_bins.size(); j++) {
        for (int k=0; k<wall_bins[j].size(); k++) {
          if (approxSame(all_walls[i], wall_bins[j][k])) {
            merge_bins(wall_bins[bin],wall_bins[j]);
            break;
          }
        }
      }
    }
    
    else {
      std::vector< ParametrizedLine<float, 2> > bin_to_add;
      bin_to_add.push_back(all_walls[i]);
      wall_bins.push_back(bin_to_add);
    }
  }
  
  
  std::vector< std::vector< ParametrizedLine<float, 2> > > trimmed_wall_bins;
  std::cout << "Trimming bins..." << std::endl;
  for (int i=0; i<wall_bins.size(); i++) {
    if (wall_bins[i].size()>min_bin_size)
      trimmed_wall_bins.push_back(wall_bins[i]);
  }
  
  
  for (int i=0; i<trimmed_wall_bins.size(); i++) {
    std::cout << "x0= " << trimmed_wall_bins[i][0].pointAt(0)(0) << "\ty0= " << trimmed_wall_bins[i][0].pointAt(0)(1) << std::endl;
    std::cout << "x1= " << trimmed_wall_bins[i][0].pointAt(1)(0) << "\ty1= " << trimmed_wall_bins[i][0].pointAt(1)(1) << std::endl;
  }
  
  return 0;
}
