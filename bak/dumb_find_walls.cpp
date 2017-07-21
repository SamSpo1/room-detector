#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/Eigenvalues>
#include <map>
#include <math.h>
#include <algorithm>
#include <time.h>
#include <stdio.h>

#include "rtabmap_reader.h"

using namespace Eigen;

const int min_bin_size = 250;
const float max_dist = 0.1;
const float max_angle = 0.8;
float cmax_angle = std::abs(cos(max_angle));
float smax_angle = std::abs(sin(max_angle));
std::string outfilename = "tests/dfw_mbs"+std::to_string(min_bin_size)+"md"+std::to_string(max_dist)+"ma"+std::to_string(max_angle)+".txt";
const int linenum = 100;
const float linelen = 0.1;

Vector2f average(std::vector< ParametrizedLine<float,2> >& bin) {
  Vector2f total = Vector2f::Zero();
  for (int i=0; i<bin.size(); i++) {
    total += bin[i].pointAt(0);
  }
  Vector2f avg = total * (1.0/(float)bin.size());
  return avg;
}

Vector2f pca(std::vector< ParametrizedLine<float,2> >& bin) {
  Vector2f avg = average(bin);
  
  Vector2f var = Vector2f::Zero();
  for (int i=0; i<bin.size(); i++)
    var += (bin[i].pointAt(0)-avg).cwiseAbs2();
  var = var*(1.0/(float)bin.size());
  float varx = var(0);
  float vary = var(1);
  float cvar = 0.0;
  for (int i=0; i<bin.size(); i++)
    cvar += (bin[i].pointAt(0)(0)-avg(0))*(bin[i].pointAt(0)(1)-avg(1));
  cvar /= (float)bin.size();
  MatrixXf cvarmat;
  cvarmat.resize(2,2);
  cvarmat << varx, cvar, cvar, vary;
  EigenSolver<MatrixXf> es(cvarmat,false);
  Vector2cf pvars = es.eigenvalues();
  return pvars.cwiseAbs();
}

void merge_bins(std::vector< ParametrizedLine<float,2> >& binout, std::vector< ParametrizedLine<float,2> >& binin) {
  while (!binin.empty()) {
    binout.push_back(binin.back());
    binin.pop_back();
  }
}

bool approxSame(ParametrizedLine<float,2>& a, ParametrizedLine<float,2>& b) {
  return ( std::abs(a.distance(b.pointAt(0)))/(a.pointAt(0)-b.pointAt(0)).norm()<0.5*smax_angle and std::abs((a.pointAt(1)-a.pointAt(0)).dot( (b.pointAt(1)-b.pointAt(0)))) > cmax_angle and (a.pointAt(0)-b.pointAt(0)).norm()<max_dist );
}


int main(int argc, char *argv[])
{
	if (argc != 2) { std::cout << "Usage: ./dumb_find_walls plyfile" << std::endl; return -1; }
	read_ply_file(argv[1]);
	
	std::vector<Vector2f> vertices;
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
  std::cout << "ceil height: " << ceilheight << "; floor height: " << floorheight << std::endl;
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
  timepoint after=now();
  std::cout << (double)std::chrono::duration_cast<std::chrono::seconds>(after - before).count() << "s" << std::endl;
  
  std::vector< std::vector< ParametrizedLine<float,2> > > trimmed_wall_bins;
  std::cout << "Trimming bins..." << std::endl;
  for (int i=0; i<wall_bins.size(); i++) {
    Vector2f pvars = pca(wall_bins[i]);
    if (wall_bins[i].size()>min_bin_size and min(pvars(0),pvars(1))/max(pvars(0),pvars(1)) < 0.1)
      trimmed_wall_bins.push_back(wall_bins[i]);
  }
  
  ofstream outfile;
  outfile.open(outfilename);
  srand(time(NULL));
  
  /*
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
    Vector2f avg = pca_check_if_wall(trimmed_wall_bins[l]);
    std::cout << "Average: (" << avg(0) << "," << avg(1) << ")" << std::endl;
    std::random_shuffle(trimmed_wall_bins[l].begin(), trimmed_wall_bins[l].end());
    for (int j=0; j<linenum and j<trimmed_wall_bins[l].size(); j++) {
      outfile << trimmed_wall_bins[l][j].pointAt(0)(0) << std::endl;
      outfile << trimmed_wall_bins[l][j].pointAt(0)(1) << std::endl;
      outfile << trimmed_wall_bins[l][j].pointAt(linelen)(0) << std::endl;
      outfile << trimmed_wall_bins[l][j].pointAt(linelen)(1) << std::endl;
    }
    trimmed_wall_bins[l]=empty;
  }
  */
  
  for (int i=0; i<trimmed_wall_bins.size(); i++) {
    std::cout << "Bin size: " << trimmed_wall_bins[i].size() << std::endl;
    Vector2f avg = average(trimmed_wall_bins[i]);
    std::cout << "Average: (" << avg(0) << "," << avg(1) << ")" << std::endl;
    std::random_shuffle(trimmed_wall_bins[i].begin(), trimmed_wall_bins[i].end());
    for (int j=0; j<linenum and j<trimmed_wall_bins[i].size(); j++) {
      outfile << trimmed_wall_bins[i][j].pointAt(0)(0) << std::endl;
      outfile << trimmed_wall_bins[i][j].pointAt(0)(1) << std::endl;
      outfile << trimmed_wall_bins[i][j].pointAt(linelen)(0) << std::endl;
      outfile << trimmed_wall_bins[i][j].pointAt(linelen)(1) << std::endl;
    }
  }
  
  outfile.close();
  system(("python view.py "+outfilename).c_str());
  
  return 0;
}
