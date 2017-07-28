// visualize *.ply file: take number_of_lines random vertices and save the walls they generate to outputfile

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <map>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string>

#include "rtabmap_reader.hpp"

using namespace Eigen;


int main(int argc, char *argv[])
{
	if (argc != 5) { std::cout << "Usage: ./viewtest plyfile outputfile number_of_lines line_length" << std::endl; return -1; }
	
  int numlines = std::stoi((string)argv[3]);
  float linelen = std::stof((string)argv[4]);
  
  std::vector<float> verts;
  std::vector<uint8_t> colors;
  std::vector<float> norms;
  std::vector<float> crvts;
  read_ply_file(argv[1], verts, colors, norms, crvts);
  std::string outfilename (argv[2]);
	
	std::vector<Vector2f> vertices;
  std::vector<Vector2f> normals;
  
  Vector2f vertex;
  Vector2f normal;
  std::cout << "Generating walls from " << verts.size() << " points..." << std::endl;
  for (int i=0; i<verts.size()/3; i+=1) {
    if ( std::abs(crvts[i])<0.0005 and std::abs(norms[3*i+2])<0.0005 ) {
      vertex(0) = verts[3*i];
      vertex(1) = verts[3*i+1];
      vertices.push_back(vertex);
      normal(0) = norms[3*i];
      normal(1) = norms[3*i+1];
      normals.push_back(normal.normalized());
    }
  }
  
  std::vector< ParametrizedLine<float, 2> > all_walls;
  
  for (int i=0; i<vertices.size(); i++) {
    Vector2f dir (normals[i](1),-normals[i](0));
    ParametrizedLine<float, 2> line (vertices[i],dir);
    all_walls.push_back(line);
  }
  
  ofstream outfile;
  outfile.open(outfilename);
  
  std::cout << "Shuffling..." << std::endl;
  std::srand ( unsigned ( std::time(0) ) );
  std::random_shuffle( all_walls.begin(), all_walls.end() );
  for (int i=0; i<numlines; i++) {
    outfile << all_walls[i].pointAt(0)(0) << std::endl;
    outfile << all_walls[i].pointAt(0)(1) << std::endl;
    outfile << all_walls[i].pointAt(linelen)(0) << std::endl;
    outfile << all_walls[i].pointAt(linelen)(1) << std::endl;
  }
  
  outfile.close();
  
  return 0;
}
