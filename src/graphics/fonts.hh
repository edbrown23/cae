#pragma once

#include <tuple>
#include <string>
#include <vector>
#include <cstdint>

namespace graphics { namespace fonts {

struct UVLocation {
	float u;
	float v;
};

struct Metrics {
	int width;
	int height;
	int offset_x;
	int offset_y;
	int advance;
};

struct PixelData {
	unsigned char val;
};

struct CharMapData {
	struct Metadata {
		int image_width;
		int image_height;
		int space_width;
	} md;
	std::vector<PixelData> pixel_data;
	std::vector<UVLocation> char_to_uv_locations;
	std::vector<Metrics> char_to_metrics;
};

CharMapData get_char_map_data(std::string const& path, int px_sz);

} }
