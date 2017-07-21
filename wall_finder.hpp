#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/Eigenvalues>
#include <map>
#include <math.h>
#include <algorithm>
#include <time.h>
#include <stdio.h>

using namespace Eigen;

void merge_bins(std::vector< ParametrizedLine<float,2> >& binout, std::vector< ParametrizedLine<float,2> >& binin) {
  while (!binin.empty()) {
    binout.push_back(binin.back());
    binin.pop_back();
  }
}

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

bool approx_same(ParametrizedLine<float,2>& a, ParametrizedLine<float,2>& b, float smax_angle, float cmax_angle, float max_dist) {
  return ( std::abs(a.distance(b.pointAt(0)))/(a.pointAt(0)-b.pointAt(0)).norm()<0.5*smax_angle and std::abs((a.pointAt(1)-a.pointAt(0)).dot( (b.pointAt(1)-b.pointAt(0)))) > cmax_angle and (a.pointAt(0)-b.pointAt(0)).norm()<max_dist );
}

void raw_to_wall(std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& crvts, std::vector< ParametrizedLine<float,2> >& walls, float crvt_max, float nz_max, float min_dist_from_ceil_and_floor) {
  Vector2f vertex;
  float ceilheight=vertices[2];
  float floorheight=vertices[2];
  for (int i=1; i<vertices.size()/3; i+=1) { // find height of ceiling and floor
    if (vertices[3*i+2]>ceilheight)
      ceilheight = vertices[3*i+2];
    if (vertices[3*i+2]<floorheight)
      floorheight = vertices[3*i+2];
  }
  for (int i=0; i<vertices.size()/3; i+=1) { // do filtering
    if ( std::abs(crvts[i])<crvt_max and std::abs(normals[3*i+2])<nz_max and vertices[3*i+2]<ceilheight-min_dist_from_ceil_and_floor and vertices[3*i+2]>floorheight+min_dist_from_ceil_and_floor ) {
      vertex(0) = vertices[3*i];
      vertex(1) = vertices[3*i+1];
      Vector2f dir (normals[3*i+1],-normals[3*i]);
      ParametrizedLine<float, 2> line (vertex,dir.normalized());
      walls.push_back(line);
    }
  }
}

void walls_to_bins(std::vector< ParametrizedLine<float,2> >& all_walls, std::vector< std::vector< ParametrizedLine<float,2> > >& wall_bins_stage_2, float max_angle, float max_dist, int min_bin_size ) {
  float cmax_angle = std::abs(cos(max_angle));
  float smax_angle = std::abs(sin(max_angle));
  std::vector< std::vector< ParametrizedLine<float,2> > > wall_bins_stage_1;
  bool taken = false;
  int bin;
  for (int i=0; i<all_walls.size()/4; i++) {
    taken=false;
    for (int j=0; j<wall_bins_stage_1.size() and !taken; j++) {
      for (int k=0; k<wall_bins_stage_1[j].size(); k++) {
        if (approx_same(all_walls[i], wall_bins_stage_1[j][k], smax_angle, cmax_angle, max_dist)) {
          wall_bins_stage_1[j].push_back(all_walls[i]);
          taken=true;
          bin=j;
          break;
        }
      }
    }
    
    if (taken) {
      for (int j=bin+1; j<wall_bins_stage_1.size(); j++) {
        for (int k=0; k<wall_bins_stage_1[j].size(); k++) {
          if (approx_same(all_walls[i], wall_bins_stage_1[j][k], smax_angle, cmax_angle, max_dist)) {
            merge_bins(wall_bins_stage_1[bin],wall_bins_stage_1[j]);
            break;
          }
        }
      }
    }
    else {
      std::vector< ParametrizedLine<float, 2> > bin_to_add;
      bin_to_add.push_back(all_walls[i]);
      wall_bins_stage_1.push_back(bin_to_add);
    }
  }
  // remove bins that don't have half of what they would need to be on track (min_bin_size/4 /2)
  for (int i=0; i<wall_bins_stage_1.size(); i++) {
    Vector2f pvars = pca(wall_bins_stage_1[i]);
    if (wall_bins_stage_1[i].size()>min_bin_size/8)
      wall_bins_stage_2.push_back(wall_bins_stage_1[i]);
  }
  // switch to only-add-to-existing-bins mode
  for (int i=all_walls.size()/4; i<all_walls.size(); i++) {
    taken=false;
    for (int j=0; j<wall_bins_stage_2.size() and !taken; j++) {
      for (int k=0; k<wall_bins_stage_2[j].size(); k++) {
        if (approx_same(all_walls[i], wall_bins_stage_2[j][k], smax_angle, cmax_angle, max_dist)) {
          wall_bins_stage_2[j].push_back(all_walls[i]);
          taken=true;
          bin=j;
          break;
        }
      }
    }
    if (taken) {
      for (int j=bin+1; j<wall_bins_stage_2.size(); j++) {
        for (int k=0; k<wall_bins_stage_2[j].size(); k++) {
          if (approx_same(all_walls[i], wall_bins_stage_2[j][k], smax_angle, cmax_angle, max_dist)) {
            merge_bins(wall_bins_stage_2[bin],wall_bins_stage_2[j]);
            break;
          }
        }
      }
    }
  }
}



