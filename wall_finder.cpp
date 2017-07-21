#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/Eigenvalues>
#include <map>
#include <math.h>
#include <algorithm>
#include <time.h>
#include <stdio.h>
#include "rtabmap_reader.hpp"
#include "wall_finder.hpp"

/* organization:
 * 1. load 3D data from ply file into verts, norms, and crvts (declared in rtabmap_reader.h)
 * 2. TODO: filter out or correct bad RTABMAP normal data
 * 3. filter verts by curvature, normal z-component, and distance-from-ceiling-and-floor, put filtered data into all_walls using ParametrizedLine structure from Eigen
 * 4. put walls into bins based on approx_same function
 * 5. filter out bins that are too small or too glob-y
 * 
 * */



using namespace Eigen;

// "With four parameters I can fit a wall, and with five I can give it windows" -- Darknet YOLO, on elephants in the BWI lab
// parameters involved in finding walls
const float crvt_max = 0.0005;
const float nz_max = 0.0005;
const float min_dist_from_ceil_and_floor = 0.05;
const int min_bin_size = 200;
const float max_dist = 0.3; // points must be within this distance of each other to go in the same bin. this and the next are the important parameters
const float max_angle = 0.5;


// parameters for displaying output
const int linenum = 100; // only display 100 randomly chosen walls from each bin
const float linelen = 0.1; // display lines with length 0.1


int main(int argc, char *argv[])
{
  if (argc != 2) { std::cout << "Usage: ./dumb_find_walls plyfile" << std::endl; return -1; }
	
  // 1-- load .ply file into verts, norms, and crvts
  std::vector<float> verts;
  std::vector<uint8_t> colors;
  std::vector<float> norms;
  std::vector<float> crvts;
  read_ply_file(argv[1], verts, colors, norms, crvts);
	
  // 2-- TODO: improve data?
  
  // 3-- filter into all_walls
  std::cout << "Generating walls from " << verts.size() << " points..." << std::endl;
  std::vector< ParametrizedLine<float,2> > all_walls;
  raw_to_wall(verts, norms, crvts, all_walls, crvt_max, nz_max, min_dist_from_ceil_and_floor);
  
  // 4-- put walls in bins
  std::cout << "Putting " << all_walls.size() << " walls in bins...";
  cout.flush();
  timepoint before=now();
  std::vector< std::vector< ParametrizedLine<float,2> > > wall_bins;
  walls_to_bins(all_walls, wall_bins, max_angle, max_dist, min_bin_size);
  timepoint after=now();
  std::cout << (double)std::chrono::duration_cast<std::chrono::seconds>(after - before).count() << "s" << std::endl;
  
  // 5-- filter out bins with fewer than min_bin_size walls or eigenvalue ratio too large
  std::cout << "Trimming bins..." << std::endl;
  std::vector< std::vector< ParametrizedLine<float,2> > > trimmed_wall_bins;
  for (int i=0; i<wall_bins.size(); i++) {
    Vector2f pvars = pca(wall_bins[i]);
    if (wall_bins[i].size()>min_bin_size and min(pvars(0),pvars(1))/max(pvars(0),pvars(1)) < 0.1)
      trimmed_wall_bins.push_back(wall_bins[i]);
  }
  
  std::string outfilename = "tests/dfw_mbs"+std::to_string(min_bin_size)+"md"+std::to_string(max_dist)+"ma"+std::to_string(max_angle)+".txt";
  ofstream outfile;
  outfile.open(outfilename);
  srand(time(NULL));
  
  /*
  // save top 4 bins
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
  
  // save all bins
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
