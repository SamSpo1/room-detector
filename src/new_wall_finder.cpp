#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/Eigenvalues>
#include <map>
#include <math.h>
#include <algorithm>
#include <time.h>
#include <stdio.h>
#include "rtabmap_reader.hpp"
#include "new_wall_finder.hpp"

/* organization:
 * 1. load 3D data from ply file into verts, norms, and crvts (declared in rtabmap_reader.h)
 * 2. put walls into bins based on approx_same function
 * 
 * */

using namespace Eigen;
using namespace std;


// parameters for displaying output
const int linenum = 100; // only display 100 randomly chosen walls from each bin
const float linelen = 0.2; // display lines with length 0.1


int main(int argc, char *argv[])
{
  if (argc != 2) { cout << "Usage: ./new_wall_finder *.ply" << endl; return -1; }
	
  cout << "sizeof(Wall*) = " << sizeof(Wall*) << ", sizeof(int) = " << sizeof(int) << endl;
  
  // 1-- load .ply file into verts, norms, and crvts, then populate all_walls
  vector<float> verts;
  vector<uint8_t> colors;
  vector<float> norms;
  vector<float> crvts;
  read_ply_file(argv[1], verts, colors, norms, crvts);
  raw_to_wall(verts, norms, crvts);
  
  // 2-- put walls in bins
  timepoint before=now();
  for (bin=0; !unbinned_walls.empty(); bin++) { // try bin=0
    untrimmed_bins.push_back(empty_bin);
    next_layer.push_back(unbinned_walls[0]);
    //bin=i;
    while (!next_layer.empty()) next_layer.back()->binnize();
    Vector2f pvars = pca(untrimmed_bins[bin]);
    if (untrimmed_bins.back().size() > min_bin_size and
        min(pvars(0),pvars(1))/max(pvars(0),pvars(1)) < max_var_ratio) {
      bins.push_back(untrimmed_bins[bin]);
      cout << "Finished making bin " << bin << " of size " << untrimmed_bins[bin].size() << "        " << endl;
    }
    else if (bin%100==0) cout << "Finished making bin " << bin << " of size " << untrimmed_bins[bin].size() << "        \r" << flush;
  }
  timepoint after=now();
  cout << "Putting " << all_walls.size() << " walls in " << bins.size() << " bins..." << (double)chrono::duration_cast<std::chrono::seconds>(after - before).count() << "s" << "                        " << endl;
  
  
  std::string outfilename = "tests/nfw_mbs"+std::to_string(min_bin_size)+"md"+std::to_string(max_dist)+"ma"+std::to_string(max_angle)+".txt";
  ofstream outfile;
  outfile.open(outfilename);
  srand(time(NULL));
  
  // save all bins
  for (int i=0; i<bins.size(); i++) {
    cout << "Bin size: " << bins[i].size() << endl;
    Vector2f avg = average(bins[i]);
    cout << "Average: (" << avg(0) << "," << avg(1) << ")" << endl;
    random_shuffle(bins[i].begin(), bins[i].end());
    for (int j=0; j<linenum and j<bins[i].size(); j++) {
      outfile << bins[i][j]->pos(0) << endl;
      outfile << bins[i][j]->pos(1) << endl;
      outfile << bins[i][j]->pos(0)+cos(bins[i][j]->angle)*linelen << endl;
      outfile << bins[i][j]->pos(1)+sin(bins[i][j]->angle)*linelen << endl;
    }
  }
  
  outfile.close();
  system(("python src/view.py "+outfilename).c_str());
  
  return 0;
}

