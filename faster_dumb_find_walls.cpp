#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <map>
#include <math.h>
#include <algorithm>
#include <time.h>

#include "rtabmap_reader.h"

using namespace Eigen;

const int min_bin_size = 100;
<<<<<<< HEAD
const float max_dist = 0.2;
const float max_angle = 0.5;
=======
const float max_dist = 0.5;
const float max_angle = 1;
>>>>>>> 390608de2db7cde9e3fb099b06c8729cbd8c04e3
float cmax_angle = std::abs(cos(max_angle));
float smax_angle = std::abs(sin(max_angle));
std::string outfilename = "tests/fdfw_mbs"+std::to_string(min_bin_size)+"md"+std::to_string(max_dist)+"ma"+std::to_string(max_angle)+".txt";
const int linenum = 100;
const float linelen = 0.1;

<<<<<<< HEAD
Vector2f pca_check_if_wall(std::vector< ParametrizedLine<float,2> >& bin) {
  Vector2f total = Vector2f::Zero();
  for (int i=0; i<bin.size(); i++) {
    total += bin[i].pointAt(0);
  }
  Vector2f avg = total * (1.0/(float)bin.size());
  return avg;
}
=======

void sort_walls(std::vector< ParametrizedLine<float,2> >& walls) {
	
	
>>>>>>> 390608de2db7cde9e3fb099b06c8729cbd8c04e3

void merge_bins(std::vector< ParametrizedLine<float,2> >& binout, std::vector< ParametrizedLine<float,2> >& binin) {
  while (!binin.empty()) {
    binout.push_back(binin.back());
    binin.pop_back();
  }
}

bool approxSame(ParametrizedLine<float,2>& a, ParametrizedLine<float,2>& b) {
<<<<<<< HEAD
  return ( std::abs(a.distance(b.pointAt(0)))/(a.pointAt(0)-b.pointAt(0)).norm()<0.5*smax_angle and std::abs((a.pointAt(1)-a.pointAt(0)).dot( (b.pointAt(1)-b.pointAt(0)))) > cmax_angle and (a.pointAt(0)-b.pointAt(0)).norm()<max_dist );
}


int main(int argc, char *argv[])
{
	if (argc != 2) { std::cout << "Usage: ./faster_dumb_find_walls plyfile" << std::endl; return -1; }
	read_ply_file(argv[1]);
	
	std::vector<Vector2f> vertices;
=======
  return ( std::abs(a.distance(b.pointAt(0)))/(a.pointAt(0)-b.pointAt(0)).norm()<0.5*smax_angle and (a.pointAt(0)-b.pointAt(0)).norm()<max_dist );
}



int main(int argc, char *argv[])
{
  if (argc != 2) { std::cout << "Usage: ./dumb_find_walls plyfile" << std::endl; return -1; }
  read_ply_file(argv[1]);
  std::vector<Vector2f> vertices;
>>>>>>> 390608de2db7cde9e3fb099b06c8729cbd8c04e3
  std::vector<Vector2f> normals;
  
  Vector2f vertex;
  Vector2f normal;
  std::cout << "Generating walls from " << verts.size() << " points..." << std::endl;
  float ceilheight=verts[2];
  float floorheight=verts[2];
  for (int i=0; i<verts.size()/3; i+=1) {
    if (verts[3*i+2]>ceilheight)
      ceilheight = verts[3*i+2];
    if (verts[3*i+2]<floorheight)
      floorheight = verts[3*i+2];
  }
<<<<<<< HEAD
  std::cout << "ceil height: " << ceilheight << "; floor height: " << floorheight << std::endl;
=======
  
>>>>>>> 390608de2db7cde9e3fb099b06c8729cbd8c04e3
  for (int i=0; i<verts.size()/3; i+=1) {
    if ( std::abs(crvts[i])<0.0005 and std::abs(norms[3*i+2])<0.0005 and verts[3*i+2]<ceilheight-0.05 and verts[3*i+2]>floorheight+0.05 ) { // 4 parameters
      vertex(0) = verts[3*i];
      vertex(1) = verts[3*i+1];
      vertices.push_back(vertex);
      normal(0) = norms[3*i];
      normal(1) = norms[3*i+1];
      normals.push_back(normal.normalized());
    }
  }
  
  std::vector< ParametrizedLine<float,2> > all_walls;
  
  for (int i=0; i<vertices.size(); i++) {
    Vector2f dir (normals[i](1),-normals[i](0));
    ParametrizedLine<float, 2> line (vertices[i],dir);
	all_walls.push_back(line);
  }
  
  std::vector< std::vector< ParametrizedLine<float,2> > > wall_bins;
  
  timepoint before=now();
  std::cout << "Putting " << vertices.size() << " walls in bins...";
  cout.flush();
  bool taken=false;
  int bin;
<<<<<<< HEAD
  for (int i=0; i<all_walls.size()/4; i++) {
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
  for (int i=0; i<wall_bins.size(); i++) {
    if (wall_bins[i].size()>min_bin_size/16)
      trimmed_wall_bins.push_back(wall_bins[i]);
  }
  for (int i=all_walls.size()/4; i<all_walls.size(); i++) {
=======
  for (int i=0; i<all_walls.size(); i++) {
>>>>>>> 390608de2db7cde9e3fb099b06c8729cbd8c04e3
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
  timepoint after=now();
  std::cout << (double)std::chrono::duration_cast<std::chrono::seconds>(after - before).count() << "s" << std::endl;
  
  std::vector< std::vector< ParametrizedLine<float,2> > > trimmed_wall_bins;
  std::cout << "Trimming bins..." << std::endl;
  for (int i=0; i<wall_bins.size(); i++) {
    if (wall_bins[i].size()>min_bin_size)
      trimmed_wall_bins.push_back(wall_bins[i]);
  }
  
  ofstream outfile;
  outfile.open(outfilename);
  srand(time(NULL));
  
  std::vector< ParametrizedLine<float, 2> > empty;
  int curmax = 0;
  int l;
  for (int n=0; n<4; n++) {
    curmax=0;
    for (int i=0; i<trimmed_wall_bins.size(); i++) {
      if (trimmed_wall_bins[i].size()>curmax) {
        curmax=trimmed_wall_bins[i].size();
        l=i;
      }
    }
    std::cout << "Bin size: " << trimmed_wall_bins[l].size() << std::endl;
<<<<<<< HEAD
    Vector2f avg = pca_check_if_wall(trimmed_wall_bins[l]);
    std::cout << "Average: (" << avg(0) << "," << avg(1) << ")" << std::endl;
=======
>>>>>>> 390608de2db7cde9e3fb099b06c8729cbd8c04e3
    std::random_shuffle(trimmed_wall_bins[l].begin(), trimmed_wall_bins[l].end());
    for (int j=0; j<linenum and j<trimmed_wall_bins[l].size(); j++) {
      outfile << trimmed_wall_bins[l][j].pointAt(0)(0) << std::endl;
      outfile << trimmed_wall_bins[l][j].pointAt(0)(1) << std::endl;
      outfile << trimmed_wall_bins[l][j].pointAt(linelen)(0) << std::endl;
      outfile << trimmed_wall_bins[l][j].pointAt(linelen)(1) << std::endl;
    }
    trimmed_wall_bins[l]=empty;
  }
  
  /*
  for (int i=0; i<trimmed_wall_bins.size(); i++) {
    std::cout << "Bin size: " << trimmed_wall_bins[i].size() << std::endl;
    std::random_shuffle(trimmed_wall_bins[i].begin(), trimmed_wall_bins[i].end());
    for (int j=0; j<linenum and j<trimmed_wall_bins[i].size(); j++) {
      outfile << trimmed_wall_bins[i][j].pointAt(0)(0) << std::endl;
      outfile << trimmed_wall_bins[i][j].pointAt(0)(1) << std::endl;
      outfile << trimmed_wall_bins[i][j].pointAt(linelen)(0) << std::endl;
      outfile << trimmed_wall_bins[i][j].pointAt(linelen)(1) << std::endl;
    }
  }
  */
  outfile.close();
  system(("python view.py "+outfilename).c_str());
  
  return 0;
}
