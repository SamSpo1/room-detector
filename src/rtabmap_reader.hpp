#include <thread>
#include <chrono>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include "tinyply.h"

using namespace tinyply;

typedef std::chrono::time_point<std::chrono::high_resolution_clock> timepoint;
std::chrono::high_resolution_clock c;

inline std::chrono::time_point<std::chrono::high_resolution_clock> now()
{
	return c.now();
}

void read_ply_file(const std::string & filename, std::vector<float>& verts, std::vector<uint8_t>& colors, std::vector<float>& norms, std::vector<float>& crvts)
{
	try
	{
		std::ifstream ss(filename, std::ios::binary);

		// parse the ASCII header fields
		PlyFile file(ss);

		for (auto e : file.get_elements())
		{
			std::cout << "element - " << e.name << " (" << e.size << ")" << std::endl;
			for (auto p : e.properties)
			{
				std::cout << "\tproperty - " << p.name << " (" << PropertyTable[p.propertyType].str << ")" << std::endl;
			}
		}
		std::cout << std::endl;

		for (auto c : file.comments)
		{
			std::cout << "Comment: " << c << std::endl;
		}
		
		
		uint32_t vertexCount, colorCount, normalCount, curvatureCount;
		vertexCount = colorCount = normalCount = curvatureCount = 0;

		// the count returns the number of instances of the property group. The vectors
		// above will be resized into a multiple of the property group size as
		// they are "flattened"... i.e. verts = {x, y, z, x, y, z, ...}
		vertexCount = file.request_properties_from_element("vertex", { "x", "y", "z" }, verts);
		colorCount = file.request_properties_from_element("vertex", { "red", "green", "blue" }, colors);
		normalCount = file.request_properties_from_element("vertex", { "nx", "ny", "nz" }, norms);
		curvatureCount = file.request_properties_from_element("vertex", { "curvature" }, crvts);

		// now populate the vectors...
		timepoint before = now();
		file.read(ss);
		timepoint after = now();

		std::cout << "Loading took " << (double)std::chrono::duration_cast<std::chrono::seconds>(after - before).count() << "s" << std::endl;
	}

	catch (const std::exception & e)
	{
		std::cerr << "Caught exception: " << e.what() << std::endl;
	}
}
