// © Joseph Cameron - All Rights Reserved

#include "volume_block_model.h"

#include <gdk/game_loop.h>
#include <gdk/graphics_constraints.h>
#include <gdk/graphics_context.h>
#include <gdk/graphics_exception.h>
#include <gdk/scene.h>
#include <gdk/texture_data.h>
#include <gdk/webgl1es2_context.h>
#include <gdk/webgl1es2_shader_program.h>
#include <gdk/webgl1es2_texture.h>
#include <jfc/cube_array.h>
#include <jfc/glfw_window.h>
#include <jfc/to_array.h>

#include <GLFW/glfw3.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <map>
#include <set>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace jfc;
using namespace gdk;
using namespace gdk::ext::volume;

static const auto PNG = jfc::to_array<texture_data::encoded_byte>({
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
	0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20,
	0x08, 0x06, 0x00, 0x00, 0x00, 0x73, 0x7a, 0x7a, 0xf4, 0x00, 0x00, 0x00,
	0x01, 0x73, 0x52, 0x47, 0x42, 0x01, 0xd9, 0xc9, 0x2c, 0x7f, 0x00, 0x00,
	0x00, 0x04, 0x67, 0x41, 0x4d, 0x41, 0x00, 0x00, 0xb1, 0x8f, 0x0b, 0xfc,
	0x61, 0x05, 0x00, 0x00, 0x00, 0x20, 0x63, 0x48, 0x52, 0x4d, 0x00, 0x00,
	0x7a, 0x26, 0x00, 0x00, 0x80, 0x84, 0x00, 0x00, 0xfa, 0x00, 0x00, 0x00,
	0x80, 0xe8, 0x00, 0x00, 0x75, 0x30, 0x00, 0x00, 0xea, 0x60, 0x00, 0x00,
	0x3a, 0x98, 0x00, 0x00, 0x17, 0x70, 0x9c, 0xba, 0x51, 0x3c, 0x00, 0x00,
	0x00, 0x06, 0x62, 0x4b, 0x47, 0x44, 0x00, 0x23, 0x00, 0xaf, 0x00, 0xce,
	0xd1, 0xda, 0x1b, 0x41, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73,
	0x00, 0x00, 0x0e, 0xa6, 0x00, 0x00, 0x0e, 0xa6, 0x01, 0x87, 0xdc, 0xa9,
	0x6f, 0x00, 0x00, 0x00, 0x07, 0x74, 0x49, 0x4d, 0x45, 0x07, 0xe9, 0x05,
	0x12, 0x01, 0x0f, 0x0b, 0xde, 0xe7, 0x5f, 0xfe, 0x00, 0x00, 0x09, 0x35,
	0x49, 0x44, 0x41, 0x54, 0x58, 0xc3, 0x45, 0x96, 0xc9, 0x8e, 0xec, 0x48,
	0x72, 0x45, 0x8f, 0x39, 0x9d, 0x74, 0xce, 0x11, 0x91, 0x53, 0xbd, 0xaa,
	0x6c, 0x48, 0xbd, 0xd0, 0x52, 0x80, 0xb6, 0x02, 0xfa, 0x17, 0xfa, 0x1b,
	0xf4, 0x37, 0xfa, 0xc9, 0xee, 0x2e, 0x09, 0xe8, 0x7a, 0x43, 0xbd, 0xcc,
	0xc8, 0x88, 0x48, 0xce, 0x83, 0xd3, 0xad, 0x17, 0x4c, 0xbc, 0x5a, 0x10,
	0x04, 0x08, 0x82, 0x74, 0xb3, 0x7b, 0xee, 0x35, 0x93, 0xff, 0xfa, 0x9f,
	0xff, 0xd6, 0xb8, 0x34, 0xf8, 0x6e, 0xc3, 0x66, 0x82, 0x18, 0x61, 0x6e,
	0x02, 0x51, 0x22, 0x84, 0x4d, 0x61, 0x83, 0xc8, 0x09, 0x49, 0x01, 0x73,
	0x0f, 0x04, 0x70, 0x15, 0x4c, 0x8d, 0xa2, 0x1b, 0xfc, 0xf5, 0xfc, 0x3b,
	0x75, 0x16, 0xa3, 0xaa, 0x34, 0xe3, 0x8a, 0x18, 0xa1, 0x72, 0x96, 0xc9,
	0x6f, 0x00, 0x38, 0x1b, 0xf1, 0xd2, 0x2c, 0xfc, 0x54, 0x27, 0xbc, 0xb4,
	0x0b, 0x8f, 0x55, 0x82, 0x00, 0x2f, 0xed, 0xc2, 0x43, 0x95, 0x60, 0x09,
	0xca, 0x36, 0x05, 0x8c, 0x95, 0xfd, 0x47, 0x09, 0xa8, 0x1a, 0x50, 0x25,
	0x12, 0x21, 0x6c, 0x42, 0x58, 0x03, 0x73, 0x07, 0xc5, 0x9d, 0xe0, 0x57,
	0x01, 0x01, 0x89, 0x20, 0xce, 0x95, 0xaa, 0xb3, 0x9c, 0xaa, 0x84, 0x4b,
	0x3b, 0x63, 0x8c, 0xf0, 0x6f, 0x0f, 0x05, 0xd7, 0x6e, 0xa6, 0x19, 0x36,
	0x5c, 0x2c, 0x2c, 0x3e, 0xf0, 0x53, 0x9d, 0x70, 0x57, 0x3b, 0x54, 0xa1,
	0x9b, 0x3d, 0xa5, 0xb3, 0x3c, 0x94, 0x09, 0xaf, 0xcd, 0x8c, 0x89, 0x52,
	0x21, 0xb2, 0xe0, 0x17, 0x65, 0x78, 0xdd, 0x58, 0x27, 0x45, 0x0c, 0xcc,
	0xa3, 0x92, 0x96, 0x20, 0x46, 0x31, 0x56, 0xc8, 0x6a, 0x01, 0x40, 0x01,
	0x13, 0x02, 0x21, 0x28, 0x4b, 0x0b, 0xdf, 0x9b, 0x99, 0x4b, 0x33, 0xd3,
	0x4e, 0x9e, 0x2c, 0x8e, 0xf8, 0xe7, 0xb9, 0x27, 0x28, 0x3c, 0x56, 0x09,
	0x46, 0xa0, 0x4a, 0x63, 0x00, 0xae, 0xed, 0xc2, 0xb9, 0x5b, 0x28, 0x92,
	0x88, 0x6e, 0xf2, 0x44, 0x06, 0x9e, 0x2a, 0x87, 0xf1, 0x93, 0x12, 0x02,
	0xc4, 0x99, 0x90, 0x54, 0xc2, 0x34, 0x28, 0x12, 0x02, 0xae, 0x30, 0x4c,
	0x1d, 0xdc, 0xff, 0xac, 0x44, 0x89, 0xb0, 0x05, 0xe8, 0x6f, 0x8a, 0x6e,
	0x8a, 0x75, 0x82, 0x75, 0x42, 0x94, 0x09, 0x4f, 0xb5, 0x03, 0xa0, 0x74,
	0x96, 0xb7, 0x6e, 0x61, 0x0b, 0xca, 0x6b, 0x3b, 0x23, 0x22, 0x94, 0x69,
	0x8c, 0x11, 0xa1, 0x9d, 0x56, 0x82, 0x06, 0x1e, 0xca, 0x84, 0x7e, 0xf6,
	0x94, 0xa9, 0xe5, 0x54, 0x39, 0xfa, 0xc5, 0x63, 0xc4, 0x80, 0x1f, 0x03,
	0xe5, 0x3d, 0x20, 0x82, 0x06, 0xb0, 0xb9, 0xe0, 0xa7, 0x40, 0xd8, 0xa0,
	0x6f, 0x84, 0x38, 0x55, 0x96, 0x5e, 0xf1, 0xa3, 0xb2, 0x4e, 0x30, 0xf4,
	0xc2, 0x36, 0x2b, 0xdb, 0x18, 0x68, 0xa7, 0x95, 0x97, 0x66, 0x06, 0xe0,
	0xa9, 0x76, 0x1c, 0xf3, 0x98, 0xc7, 0xd2, 0x11, 0x54, 0xe9, 0x67, 0xcf,
	0xb1, 0x4a, 0x40, 0x84, 0x76, 0xda, 0x50, 0x94, 0xa0, 0xf0, 0xda, 0x2e,
	0xbc, 0x35, 0x13, 0x5b, 0x50, 0x8c, 0x06, 0xc5, 0x55, 0x42, 0x7b, 0x86,
	0x6d, 0x51, 0x6c, 0x02, 0xd5, 0x51, 0x89, 0x12, 0x08, 0x6b, 0x60, 0x6c,
	0x94, 0x10, 0xf6, 0xe7, 0xc5, 0x49, 0x88, 0x9d, 0xb2, 0xf6, 0x01, 0x57,
	0x82, 0x71, 0x86, 0xca, 0xc5, 0x3c, 0x54, 0x09, 0xdd, 0xe4, 0x01, 0x38,
	0x96, 0x09, 0xdd, 0xec, 0xf7, 0x76, 0x3b, 0xcb, 0xad, 0x9d, 0x29, 0x9d,
	0x65, 0x58, 0x36, 0xce, 0xed, 0x42, 0x9d, 0xee, 0xfa, 0xb7, 0x93, 0x67,
	0x5a, 0x03, 0xc6, 0x66, 0x06, 0xa2, 0x9d, 0xf8, 0x38, 0x05, 0x0d, 0x70,
	0x7b, 0x81, 0xb0, 0x40, 0x76, 0x14, 0xb6, 0x15, 0xe6, 0x9b, 0x52, 0xde,
	0xc1, 0xb2, 0x08, 0x4b, 0xaf, 0x64, 0x27, 0xc1, 0x26, 0xe0, 0xe7, 0xc0,
	0x6b, 0xb7, 0xd0, 0x2f, 0x1b, 0x85, 0xb3, 0xb4, 0x93, 0xe7, 0xda, 0x2e,
	0x00, 0x3c, 0x94, 0x31, 0x02, 0x34, 0x93, 0x47, 0x55, 0x11, 0x81, 0x87,
	0xca, 0x21, 0x46, 0x18, 0x16, 0x4f, 0x95, 0xc6, 0x3c, 0xd5, 0x0e, 0x13,
	0xbc, 0x92, 0xc4, 0x60, 0x22, 0xf0, 0x33, 0x6c, 0x7e, 0xb7, 0x5b, 0x50,
	0x48, 0x9c, 0x90, 0x1f, 0x40, 0x45, 0x78, 0xff, 0x1d, 0xc2, 0xa6, 0x48,
	0x2c, 0x08, 0xca, 0xba, 0x08, 0xae, 0x8e, 0x78, 0xac, 0x12, 0xfe, 0xfd,
	0xb1, 0x60, 0x5c, 0x3c, 0xa5, 0x8b, 0x76, 0x1e, 0x52, 0x4b, 0xbf, 0x6c,
	0x28, 0xec, 0xd0, 0xcd, 0x9e, 0xd4, 0x1a, 0xfa, 0xc5, 0x73, 0x2a, 0xdd,
	0x0f, 0x30, 0xbb, 0xc9, 0x63, 0xc2, 0x02, 0xed, 0x39, 0x60, 0x13, 0x25,
	0x3f, 0xee, 0xa4, 0x6f, 0x8b, 0x12, 0x27, 0x4a, 0xdf, 0x28, 0x63, 0x07,
	0xae, 0xdc, 0x6d, 0x27, 0x06, 0xee, 0x7f, 0x81, 0xf1, 0x5d, 0x31, 0x1a,
	0x58, 0xbb, 0x8d, 0x6e, 0xf2, 0x5c, 0xda, 0x79, 0xef, 0xc0, 0xec, 0xf9,
	0xde, 0xcc, 0xc8, 0x07, 0x94, 0xaf, 0xed, 0xfc, 0xc3, 0x76, 0x87, 0x2c,
	0x46, 0x15, 0x7e, 0x3b, 0xf7, 0x04, 0x55, 0xce, 0xed, 0x42, 0xe1, 0x22,
	0x8c, 0xec, 0x87, 0x66, 0x9d, 0x04, 0xef, 0x21, 0xb2, 0x3b, 0x84, 0x41,
	0x85, 0xf1, 0xaa, 0x6c, 0xab, 0xe2, 0x67, 0x88, 0x1d, 0xf8, 0x41, 0xb9,
	0x7e, 0x83, 0x28, 0x11, 0xba, 0xb7, 0xbd, 0x5b, 0x85, 0x8b, 0x78, 0x6d,
	0x16, 0x14, 0xa8, 0xd3, 0x98, 0x2c, 0x89, 0x08, 0x0a, 0xed, 0xb4, 0xf2,
	0x50, 0x26, 0xa8, 0xc2, 0x5b, 0xbf, 0x72, 0xaa, 0x1c, 0x87, 0x2c, 0xa6,
	0x48, 0x2c, 0x02, 0xb8, 0x58, 0xe8, 0x66, 0x8f, 0x41, 0x95, 0xea, 0xc9,
	0xb0, 0x2d, 0x4a, 0x14, 0x76, 0xad, 0x8c, 0x28, 0xeb, 0x10, 0xc8, 0x0f,
	0x90, 0xd5, 0x42, 0x58, 0x95, 0xb1, 0x55, 0x4c, 0x0c, 0xcb, 0xa0, 0xac,
	0xc3, 0x07, 0x07, 0x1f, 0x1f, 0x49, 0xec, 0x7e, 0x3f, 0x55, 0x09, 0x55,
	0x6a, 0x39, 0xb7, 0x33, 0xaa, 0xd0, 0xcf, 0xbb, 0xd6, 0x8f, 0x65, 0xc2,
	0xad, 0x9b, 0x51, 0x55, 0xda, 0xd9, 0xa3, 0xfc, 0x91, 0x0f, 0xc6, 0x58,
	0xc1, 0xaf, 0x0a, 0x02, 0x7d, 0xab, 0x58, 0x07, 0x49, 0x2e, 0x3f, 0x92,
	0x71, 0x5b, 0x61, 0x9d, 0xc0, 0x66, 0x06, 0x13, 0xed, 0xf0, 0xd5, 0x4f,
	0x42, 0xec, 0x04, 0x55, 0xa8, 0x9c, 0x45, 0x64, 0x6f, 0xf9, 0xad, 0x5d,
	0xfe, 0x80, 0x4d, 0xa0, 0x70, 0xfb, 0x61, 0x82, 0xea, 0x0f, 0x18, 0xc7,
	0x65, 0xa3, 0x9b, 0xd6, 0x1f, 0x21, 0x65, 0x54, 0x61, 0xbc, 0x2a, 0x49,
	0x69, 0xf0, 0x33, 0xac, 0x33, 0x2c, 0x03, 0x58, 0x07, 0xcb, 0x04, 0x69,
	0x06, 0xe9, 0xc1, 0xc0, 0x16, 0xd0, 0x4d, 0x21, 0x12, 0xd6, 0x15, 0xe6,
	0x69, 0x67, 0xa2, 0x9b, 0x3d, 0x65, 0x62, 0x01, 0x68, 0x67, 0x4f, 0x08,
	0x4a, 0x37, 0xaf, 0x94, 0xce, 0x12, 0x19, 0xc3, 0x63, 0xed, 0xf6, 0xaa,
	0x55, 0x51, 0x20, 0x4b, 0x0c, 0x55, 0x16, 0x73, 0x2c, 0x1d, 0xfd, 0xec,
	0x31, 0xba, 0x41, 0x52, 0x19, 0xea, 0x07, 0x25, 0x3d, 0x18, 0xfc, 0xa4,
	0x68, 0x50, 0xd2, 0x5a, 0x20, 0xc0, 0xe2, 0x05, 0xe7, 0x14, 0x8c, 0x50,
	0x3d, 0xee, 0x73, 0x60, 0x38, 0x07, 0xfc, 0xa4, 0xac, 0xb3, 0xee, 0x55,
	0xf6, 0xcb, 0x1f, 0x15, 0x77, 0xeb, 0x47, 0xfb, 0x37, 0x8e, 0xe5, 0x3e,
	0x78, 0xe6, 0x25, 0x50, 0xba, 0xdd, 0x19, 0xa5, 0xb3, 0xf0, 0x01, 0x63,
	0x91, 0x44, 0x98, 0x10, 0x94, 0x6d, 0x52, 0xfa, 0x1b, 0x64, 0x25, 0x18,
	0x2b, 0xac, 0x23, 0xa8, 0xdf, 0x33, 0x61, 0xed, 0x03, 0xdd, 0x55, 0x99,
	0x1b, 0x65, 0x68, 0xa1, 0x28, 0x41, 0x62, 0x83, 0x89, 0x20, 0xad, 0xa0,
	0x5f, 0x36, 0x1e, 0x2b, 0xc7, 0x5d, 0xb5, 0x57, 0x74, 0x5f, 0xc6, 0xd4,
	0xd9, 0x47, 0xfe, 0x77, 0x0b, 0xcd, 0xb4, 0x72, 0x5f, 0x26, 0x7b, 0x56,
	0x24, 0x11, 0xe7, 0x6e, 0x07, 0xb6, 0x70, 0x96, 0x66, 0xf2, 0x18, 0xc2,
	0x3e, 0x60, 0xa6, 0x9b, 0x32, 0x8d, 0xa0, 0xdb, 0x3e, 0x66, 0x9b, 0xef,
	0x8a, 0x9f, 0x95, 0xb0, 0x07, 0x1c, 0x26, 0x82, 0xb5, 0x57, 0xfa, 0xab,
	0x12, 0x17, 0x82, 0x7a, 0x65, 0x19, 0x76, 0xed, 0xdb, 0x69, 0xe5, 0xfa,
	0x61, 0xc5, 0xf3, 0x47, 0x10, 0x55, 0xa9, 0x45, 0x55, 0x19, 0x96, 0xf0,
	0xc3, 0x8a, 0x46, 0x76, 0x6e, 0xda, 0x69, 0x05, 0x60, 0x5c, 0x03, 0xf6,
	0x2f, 0x87, 0xff, 0x24, 0xcf, 0x73, 0xaa, 0xb2, 0xa4, 0x28, 0x0a, 0xba,
	0xbe, 0xa7, 0x6d, 0x5b, 0xda, 0xb6, 0xe5, 0x4f, 0xcf, 0xcf, 0x00, 0x64,
	0x59, 0x06, 0x02, 0x5f, 0xbf, 0xfe, 0xce, 0xf1, 0x50, 0xd3, 0xb4, 0x2d,
	0xcd, 0xd2, 0x90, 0xd7, 0x05, 0xff, 0x9b, 0xff, 0x05, 0x93, 0xe5, 0x98,
	0xc4, 0x11, 0xe5, 0x19, 0x12, 0x59, 0xe6, 0xf3, 0x15, 0xd6, 0x01, 0x55,
	0x88, 0xfe, 0x9c, 0x11, 0x15, 0x39, 0xb6, 0x2c, 0x99, 0xbf, 0x7f, 0x07,
	0x04, 0xdf, 0xb7, 0x08, 0x02, 0x3f, 0x09, 0xd6, 0xb9, 0x94, 0xa2, 0x28,
	0xe8, 0x87, 0x81, 0xbe, 0x1f, 0x78, 0x7a, 0x7a, 0xa4, 0x28, 0x0a, 0xd6,
	0x75, 0xa5, 0x1f, 0x06, 0xee, 0xef, 0xef, 0x18, 0xc6, 0x91, 0xa6, 0x69,
	0x48, 0x53, 0xc7, 0xbc, 0x2c, 0xb8, 0x24, 0xe1, 0xe1, 0xfe, 0x81, 0xf3,
	0xdb, 0x19, 0xc9, 0x1c, 0x61, 0x1a, 0x41, 0x03, 0xea, 0x57, 0x74, 0x0b,
	0x44, 0x71, 0x4c, 0x54, 0x3d, 0x40, 0x6c, 0x41, 0x15, 0xbc, 0x67, 0x3e,
	0xbf, 0x11, 0xbc, 0x47, 0x54, 0x71, 0x4f, 0x4f, 0xe0, 0x03, 0x22, 0x82,
	0x4d, 0x92, 0x98, 0xb2, 0x28, 0x18, 0x87, 0x91, 0x79, 0x99, 0xf9, 0xed,
	0xf3, 0x67, 0x4e, 0xc7, 0x23, 0xce, 0x39, 0x2e, 0xd7, 0x2b, 0x97, 0xeb,
	0x95, 0x3c, 0xcb, 0x78, 0x7e, 0xfe, 0x05, 0x80, 0xa1, 0x1f, 0xc8, 0xf3,
	0x9c, 0xeb, 0xed, 0x46, 0x96, 0x65, 0x44, 0x89, 0x43, 0xad, 0x25, 0x2c,
	0x0b, 0x61, 0x1c, 0xf7, 0x9d, 0x61, 0xb1, 0x84, 0x79, 0x24, 0xaa, 0x0e,
	0xe8, 0xb6, 0x41, 0x08, 0xc4, 0x59, 0xc6, 0x12, 0xa5, 0x24, 0x2e, 0xc2,
	0xb8, 0x18, 0x82, 0xb2, 0xb6, 0x0d, 0x76, 0x9a, 0x66, 0x3e, 0x7f, 0xf9,
	0xc2, 0xdd, 0xe9, 0x44, 0x5e, 0x64, 0xf4, 0xfd, 0x40, 0x3f, 0x0c, 0xa4,
	0x69, 0xca, 0x7f, 0x3c, 0xdc, 0x73, 0xb9, 0x5c, 0x41, 0xe1, 0xcb, 0xd7,
	0x6f, 0xd4, 0x65, 0x49, 0x5d, 0xd7, 0x74, 0x7d, 0xcf, 0xf9, 0x7c, 0xe6,
	0x74, 0x3a, 0x11, 0x4c, 0x46, 0x1c, 0x0b, 0xa6, 0x16, 0x36, 0xaf, 0xf8,
	0x75, 0x43, 0xe7, 0x91, 0x28, 0x4d, 0x10, 0x14, 0xdf, 0xbe, 0xa3, 0x21,
	0xb0, 0x8d, 0x3d, 0x26, 0x4e, 0x58, 0xd7, 0x08, 0x3a, 0xc5, 0x77, 0x0d,
	0x20, 0xd8, 0x61, 0xe8, 0xf7, 0xdd, 0xcd, 0xa5, 0x3c, 0xdc, 0xdf, 0x91,
	0xe7, 0x39, 0xb7, 0xeb, 0x8d, 0xaa, 0x2c, 0x79, 0xbb, 0x5c, 0x48, 0xd3,
	0x94, 0xdb, 0xf5, 0xc6, 0x30, 0x0e, 0xa0, 0x4a, 0xd3, 0x76, 0x0c, 0x63,
	0x4f, 0x96, 0xe5, 0x14, 0x45, 0x81, 0x9e, 0xaf, 0xac, 0xae, 0x40, 0x8c,
	0x41, 0x12, 0x4b, 0x52, 0xc4, 0x68, 0xbc, 0x0f, 0x2e, 0xac, 0x41, 0x6c,
	0xbc, 0x4b, 0x13, 0x84, 0x10, 0x0c, 0x44, 0x11, 0x6c, 0x33, 0xc6, 0xa5,
	0x00, 0xd8, 0x34, 0xcd, 0x28, 0xcb, 0x82, 0xbe, 0xef, 0xf9, 0xfc, 0xe5,
	0x0b, 0x59, 0x96, 0x91, 0xa6, 0x29, 0x2f, 0xaf, 0xaf, 0xbc, 0x37, 0xef,
	0x1c, 0x0f, 0x47, 0x8e, 0xa7, 0xfd, 0xca, 0xb3, 0x8c, 0xb7, 0xb7, 0x0b,
	0xce, 0x25, 0xa4, 0xce, 0xd1, 0x75, 0x1d, 0xba, 0xcc, 0xe8, 0x16, 0x90,
	0x24, 0x21, 0x12, 0xc7, 0xe6, 0x61, 0xeb, 0x7b, 0xb6, 0x75, 0x45, 0x50,
	0x48, 0x72, 0x24, 0x3f, 0x21, 0xc6, 0x60, 0x5d, 0x42, 0x94, 0x18, 0x24,
	0x39, 0x31, 0xbd, 0xf5, 0x84, 0xf6, 0x15, 0x5b, 0x55, 0x25, 0xc7, 0xc3,
	0x81, 0xd8, 0x5a, 0xa6, 0x69, 0xcf, 0xeb, 0xe6, 0xbd, 0xa1, 0xaa, 0x2b,
	0x9e, 0x1e, 0x1f, 0x19, 0x86, 0x81, 0xae, 0xef, 0x19, 0xc7, 0x91, 0x38,
	0x4e, 0x38, 0x1c, 0x6a, 0x50, 0x25, 0x71, 0x0e, 0x97, 0x3a, 0x84, 0x47,
	0x92, 0x34, 0x22, 0x6c, 0xe0, 0x57, 0x8f, 0xf1, 0x23, 0x12, 0xc7, 0xc4,
	0x69, 0x86, 0xd8, 0x88, 0xf8, 0x74, 0x24, 0x8c, 0x13, 0xbe, 0x1f, 0x58,
	0xae, 0x2d, 0x1a, 0x36, 0x08, 0x1b, 0x42, 0xd8, 0x25, 0x88, 0xe3, 0x98,
	0xef, 0x2f, 0xaf, 0x34, 0xed, 0x3b, 0x28, 0x3c, 0x3f, 0x3f, 0x43, 0xa6,
	0x7c, 0xf9, 0xfa, 0x95, 0xba, 0x3e, 0xf0, 0xf4, 0xf4, 0x88, 0xaa, 0x12,
	0x45, 0x11, 0x97, 0xcb, 0x05, 0xd5, 0xc0, 0xcf, 0x9f, 0x3e, 0x71, 0xb9,
	0x5e, 0x99, 0xe7, 0x99, 0x30, 0xbe, 0x30, 0x75, 0x31, 0x62, 0x22, 0x34,
	0xec, 0x6b, 0x17, 0x2e, 0xc7, 0x44, 0x96, 0x28, 0x6c, 0x8c, 0x9f, 0xbf,
	0xa0, 0xcb, 0x0c, 0x51, 0x42, 0x94, 0xa6, 0x88, 0x49, 0xd8, 0xd6, 0x80,
	0xc4, 0xf1, 0xee, 0x82, 0xeb, 0xf5, 0xca, 0xf1, 0x78, 0xa4, 0x28, 0x72,
	0x86, 0x61, 0x20, 0x4b, 0x53, 0xde, 0x2e, 0x17, 0xb2, 0x2c, 0xc3, 0xb9,
	0x84, 0x6f, 0xdf, 0x7e, 0xdf, 0xe7, 0x7b, 0x55, 0xf2, 0xe9, 0xd3, 0x27,
	0xe6, 0x79, 0xfe, 0xf1, 0xde, 0xf5, 0x7a, 0xc5, 0x96, 0x25, 0x2a, 0x82,
	0x28, 0x44, 0x79, 0x4d, 0x54, 0x14, 0xa8, 0x0f, 0x4c, 0xdf, 0xbf, 0xb1,
	0xad, 0x2b, 0x44, 0x16, 0xb1, 0x8e, 0xc8, 0xc5, 0x88, 0x11, 0x82, 0x0f,
	0xe8, 0xb6, 0xa2, 0x7e, 0x06, 0x55, 0xec, 0x38, 0x8e, 0x24, 0x89, 0xe3,
	0xe1, 0xfe, 0x0e, 0x55, 0xe5, 0x1f, 0xbf, 0xfe, 0x4a, 0x96, 0x65, 0x1c,
	0xea, 0x7a, 0xcf, 0x86, 0xa1, 0xe3, 0xd3, 0x4f, 0x9f, 0x76, 0xfd, 0x2f,
	0x17, 0x96, 0x65, 0x61, 0x9a, 0x26, 0xc6, 0x69, 0x24, 0x2f, 0x0a, 0xfc,
	0xd0, 0x83, 0x18, 0x8c, 0xcb, 0xd1, 0x05, 0xd6, 0xa5, 0x47, 0x82, 0xdf,
	0x2b, 0x76, 0x39, 0x62, 0x0d, 0x26, 0xda, 0xb7, 0x99, 0xb5, 0xeb, 0x61,
	0x1e, 0x10, 0x1b, 0x61, 0xab, 0x7a, 0x87, 0x30, 0xcb, 0x32, 0x96, 0x65,
	0xe6, 0xd7, 0xff, 0xff, 0x3f, 0xea, 0xaa, 0xe6, 0xf9, 0xf9, 0x99, 0x71,
	0x1c, 0x29, 0xca, 0x02, 0x04, 0x8c, 0x31, 0xbc, 0xbc, 0xbc, 0x50, 0x96,
	0x25, 0x71, 0x1c, 0x53, 0x55, 0x15, 0x5d, 0xd7, 0xe1, 0x9c, 0x23, 0x4d,
	0x53, 0x84, 0x0a, 0x89, 0x62, 0x54, 0xc3, 0x3e, 0x46, 0x35, 0x20, 0x89,
	0x23, 0xae, 0x4b, 0xe2, 0x3c, 0x67, 0x6e, 0x67, 0xd6, 0xbe, 0x41, 0x97,
	0x01, 0x11, 0x01, 0x9b, 0x20, 0x59, 0x8a, 0x38, 0x87, 0x6c, 0x01, 0x7b,
	0x77, 0x77, 0x22, 0x75, 0x29, 0x2f, 0xaf, 0x67, 0xe2, 0x38, 0xa6, 0xc8,
	0x73, 0x04, 0xf8, 0xed, 0xb7, 0xcf, 0x00, 0xdc, 0x9d, 0x4e, 0x3c, 0x3d,
	0x3e, 0xf2, 0xde, 0x34, 0xb4, 0x6d, 0xcb, 0xe5, 0x72, 0xe1, 0xee, 0xee,
	0x8e, 0x24, 0x8e, 0xc9, 0xf3, 0x1c, 0x3b, 0x59, 0x54, 0x37, 0x34, 0x29,
	0x48, 0x9c, 0x61, 0x9d, 0x57, 0xc2, 0x3c, 0x13, 0x9a, 0x1b, 0x5b, 0xfb,
	0x4e, 0x58, 0x37, 0x34, 0x6c, 0x88, 0x4d, 0x90, 0xbc, 0xde, 0x0f, 0x11,
	0x56, 0xb6, 0x6e, 0xb7, 0xbf, 0xed, 0xbb, 0x81, 0xcb, 0xe5, 0x4a, 0x9e,
	0xe7, 0xe4, 0x79, 0xce, 0xdf, 0xfe, 0xfe, 0x77, 0xca, 0xb2, 0xe4, 0x50,
	0xd7, 0xa4, 0x69, 0x4a, 0xdb, 0x75, 0x4c, 0xf3, 0xcc, 0xed, 0xfd, 0xc6,
	0xa1, 0x3e, 0x00, 0x10, 0x5b, 0x4b, 0x9a, 0xa6, 0x5c, 0xae, 0x57, 0x7c,
	0xfb, 0x0e, 0x28, 0x4a, 0xc3, 0x24, 0x06, 0x89, 0x63, 0x4c, 0x1c, 0x43,
	0x9c, 0xb0, 0x8d, 0x13, 0x51, 0x5e, 0x42, 0x92, 0xa1, 0x5b, 0x20, 0x71,
	0x20, 0x59, 0x8a, 0x4e, 0x11, 0xf3, 0xb0, 0xa2, 0xcd, 0x19, 0x6b, 0x22,
	0x43, 0x96, 0xee, 0x9a, 0x9f, 0xdf, 0x2e, 0x3f, 0xf4, 0x2f, 0x8a, 0x02,
	0x45, 0xf1, 0xde, 0x73, 0xbd, 0xdd, 0x38, 0xd6, 0x47, 0x0e, 0xc7, 0x9a,
	0x69, 0x9a, 0x78, 0x6f, 0x1a, 0x6e, 0xef, 0x0d, 0xd3, 0x34, 0x22, 0xf5,
	0x11, 0xc2, 0x86, 0x89, 0x0c, 0x36, 0x89, 0x21, 0x36, 0x6c, 0xd3, 0x42,
	0x98, 0x17, 0xa2, 0xf2, 0x40, 0x14, 0x29, 0xdb, 0xd4, 0x11, 0xe6, 0x99,
	0xa9, 0xf1, 0x1f, 0xeb, 0xf7, 0xba, 0xbb, 0x20, 0x49, 0xb0, 0x97, 0xeb,
	0x85, 0x9f, 0x3f, 0xfd, 0xcc, 0x38, 0x4d, 0x3c, 0x3e, 0xdc, 0x73, 0x7e,
	0x7b, 0xa3, 0xeb, 0x7b, 0x2e, 0xd7, 0x2b, 0xe3, 0x38, 0xee, 0xc1, 0xe4,
	0x52, 0x8e, 0xa7, 0x03, 0xcd, 0x7b, 0x4b, 0x96, 0xa5, 0x3c, 0xff, 0xf2,
	0x0b, 0x6f, 0x97, 0x0b, 0x02, 0xe8, 0xdc, 0x7d, 0x6c, 0xb5, 0xe0, 0x17,
	0x41, 0x62, 0x47, 0x50, 0x21, 0x4a, 0x62, 0x7c, 0x77, 0x65, 0x53, 0x05,
	0x89, 0xd0, 0xc8, 0x22, 0x91, 0x43, 0x5c, 0xb1, 0xcb, 0x20, 0xf2, 0x01,
	0x61, 0x9a, 0xf3, 0xed, 0xdb, 0x37, 0x0e, 0x87, 0x03, 0xde, 0x7b, 0xe2,
	0x38, 0x26, 0xb6, 0x96, 0x3c, 0xcb, 0xb0, 0xd6, 0x52, 0x55, 0x15, 0xeb,
	0xb2, 0xf0, 0xfa, 0x7a, 0xa6, 0xef, 0x7b, 0xf2, 0x39, 0x67, 0x9a, 0x66,
	0xf2, 0x2c, 0x43, 0x44, 0x88, 0x42, 0x06, 0x2a, 0x84, 0x75, 0x25, 0xac,
	0x0b, 0x3a, 0x35, 0x88, 0x80, 0x37, 0x11, 0x51, 0x5e, 0x10, 0xb9, 0x94,
	0xa8, 0x2a, 0xd9, 0x96, 0x0d, 0x3f, 0x4c, 0x68, 0x08, 0x84, 0xf7, 0x17,
	0xd8, 0x8f, 0xcf, 0xbf, 0x00, 0xa5, 0x60, 0x67, 0xe6, 0xf2, 0xa0, 0xa5,
	0x4f, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60,
	0x82
});

/// \brief cubic volume of diffuse lighting data
///
template<size_t size_param>
class volumetric_lighting final {
public:
    static constexpr size_t size = size_param;
    static constexpr size_t size_1d = size * size * size;
    static constexpr size_t size_2d = std::sqrt(size_1d);
    static_assert(is_power_of_two(size));
    static_assert(is_power_of_two(size_1d));
    static_assert(is_power_of_two(size_2d));
    using local_space_component_type = int;
private:
    jfc::cube_array<gdk::color, size> m_Data;

    void add(const local_space_component_type aX, const local_space_component_type aY, const local_space_component_type aZ, 
        const gdk::color &aColor) {
        if (aX < 0 || aX >= size) return;
        if (aY < 0 || aY >= size) return;
        if (aZ < 0 || aZ >= size) return;

        auto &light = m_Data.at(aX, aY, aZ);
        light += aColor;
        light.clamp();
    }

public:
    /// \brief clears the data
    void clear() { m_Data.clear(); }

    // \brief applies a uniform light to the whole volume
    void add_global(const gdk::color &aColor) {
        for (size_t i(0); i < decltype(m_Data)::size_1d; ++i)
            m_Data.data()[i] = aColor;
    }

    /// \brief add a pointlight to the light volume
    void add_point_light(graphics_intvector3_type aLightPosition, const float aSize, const gdk::color &aColor) {
        const gdk::vector3<float> CENTRE(aSize/2.f); 
        const auto HALF(aSize/2.f);

        aLightPosition.x -= HALF;
        aLightPosition.y -= HALF;
        aLightPosition.z -= HALF;

        for (int x(0); x < aSize; ++x) for (int y(0); y < aSize; ++y) for (int z(0); z < aSize; ++z) {
            float distanceFromCentre = CENTRE.distance(gdk::vector3<float>(x,y,z));
            float normalizedHalfDistanceFromCentre = distanceFromCentre / HALF; 
            float intensity = (1.0f / std::sqrt(normalizedHalfDistanceFromCentre)) - 1.0f; 
            intensity = std::clamp(intensity, 0.0f, 1.0f);

            auto color(aColor);
            color.r *= intensity;
            color.g *= intensity;
            color.b *= intensity;

            add(aLightPosition.x + x, aLightPosition.y + y, aLightPosition.z + z, color);   
        }    
    }

    static constexpr size_t CHANNELS_PER_COLOR = 3;

    /// \brief writes lighting data into texture data
    //TODO: should provide a 3d texture alternative, which requires a texture_data::view_3d, etc.
    std::pair<gdk::texture_data::view, std::shared_ptr<std::array<gdk::texture_data::channel_type, size_1d * CHANNELS_PER_COLOR>>> to_2d_texture_data() const {
        std::array<gdk::texture_data::channel_type, size_1d * CHANNELS_PER_COLOR> textureData;

        size_t i(0);
        for (size_t j(0); j < size_1d; ++j) {
            textureData[i + 0] = m_Data.data()[j].r * 255; //Converting normalized floating-point light values
            textureData[i + 1] = m_Data.data()[j].g * 255; //to unsigned int values (so they fit in a single byte) 
            textureData[i + 2] = m_Data.data()[j].b * 255; //([0-1] to [0-255])
            i += CHANNELS_PER_COLOR;
        }

        auto pTextureData = std::make_shared<std::array<gdk::texture_data::channel_type, 
            size_1d * CHANNELS_PER_COLOR>>(std::move(textureData));

        texture_data::view view;
        view.width = size_2d;
        view.height = size_2d;
        view.format = texture::format::rgb;
        view.data = &(pTextureData->front());

        return { view, pTextureData };
    }
};

volumetric_lighting<64> staticLights;
volumetric_lighting<64> lighting;

int main(int argc, char **argv) {
    glfw_window window("volumetric blocks + volumetric lighting");

    const auto pGraphics = webgl1es2_context::make();
    const auto pScene = pGraphics->make_scene();

    auto pCamera = [&]() {
        auto pCamera = pGraphics->make_camera();
        pScene->add(pCamera);
        return pCamera;
    }();

    const auto pTexture = [&]() {
        auto [view, data] = texture_data::make_from_png(PNG);
        auto p = pGraphics->make_texture(view,
            texture::wrap_mode::clamped,
            texture::wrap_mode::clamped);
        return p;
    }();

    texture_data::view view;
    view.width = 0;
    view.height = 0;
    view.format = texture::format::rgba;
    view.data = {};
    auto pLightingTexture = pGraphics->make_texture(view, texture::wrap_mode::clamped, texture::wrap_mode::clamped);

    auto pShader = [&]() {
        const std::string vertexShaderSource(R"V0G0N(
        uniform mat4 _MVP;

        attribute highp vec2 a_UV;
        attribute highp vec2 a_UVOffset;
        attribute highp vec3 a_Position;
        attribute lowp float a_BlockDirection;

        varying highp vec2 v_UVOffset;
        varying highp vec3 v_Position;
        varying lowp float v_BlockDirection;
        varying mediump vec2 v_UV;

        void main () {
            v_UV = a_UV;
            v_UVOffset = a_UVOffset;
            v_BlockDirection = a_BlockDirection;

            // Since we know block vertex position data should only ever be integer values (with respect to its localspace),
            // removing the fractional component will prevent any rounding issues that could create gaps between edges
            //
            vec3 position = floor(a_Position);
            v_Position = position;
            gl_Position = _MVP * vec4(position,1.0);
        }
        )V0G0N");

        const std::string fragmentShaderSource(R"V0G0N(
        uniform lowp float _NormalizedTileSize;
        uniform lowp float _VolumeTextureSize;
        uniform sampler2D _BlockTypeVolumeTexture;
        uniform sampler2D _LightVolumeTexture;
        uniform sampler2D _Texture;

        varying highp vec2 v_UVOffset;
        varying highp vec3 v_Position;
        varying lowp float v_BlockDirection;
        varying mediump vec2 v_UV;

        const float BLOCK_DIRECTION_NORTH = 0.;
        const float BLOCK_DIRECTION_SOUTH = 1.;
        const float BLOCK_DIRECTION_EAST  = 2.;
        const float BLOCK_DIRECTION_WEST  = 3.;
        const float BLOCK_DIRECTION_UP    = 4.;
        const float BLOCK_DIRECTION_DOWN  = 5.;

        void main() {
            lowp vec2 uv = fract(v_UV);
            uv = clamp(uv, 0.1, 0.9); //hiding UV seams
            uv *= _NormalizedTileSize;
            uv += v_UVOffset * _NormalizedTileSize;
            vec4 texel = texture2D(_Texture, uv);

            ivec3 positionInVolumetricData = ivec3(floor(v_Position));

            lowp vec3 blockType = gdk_texture3D(_BlockTypeVolumeTexture, _VolumeTextureSize, vec2(0.5, 0.5), positionInVolumetricData);
            if (blockType == vec3(0,0,0)) {
                //blahblah
            }
            
            lowp vec3 l000 = gdk_texture3D(_LightVolumeTexture, _VolumeTextureSize, vec2(0.5, 0.5), positionInVolumetricData);
            lowp vec3 l100 = gdk_texture3D(_LightVolumeTexture, _VolumeTextureSize, vec2(0.5, 0.5), positionInVolumetricData + ivec3(1,0,0));
            lowp vec3 l010 = gdk_texture3D(_LightVolumeTexture, _VolumeTextureSize, vec2(0.5, 0.5), positionInVolumetricData + ivec3(0,1,0));
            lowp vec3 l110 = gdk_texture3D(_LightVolumeTexture, _VolumeTextureSize, vec2(0.5, 0.5), positionInVolumetricData + ivec3(1,1,0));
            lowp vec3 l001 = gdk_texture3D(_LightVolumeTexture, _VolumeTextureSize, vec2(0.5, 0.5), positionInVolumetricData + ivec3(0,0,1));
            lowp vec3 l101 = gdk_texture3D(_LightVolumeTexture, _VolumeTextureSize, vec2(0.5, 0.5), positionInVolumetricData + ivec3(1,0,1));
            lowp vec3 l011 = gdk_texture3D(_LightVolumeTexture, _VolumeTextureSize, vec2(0.5, 0.5), positionInVolumetricData + ivec3(0,1,1));
            lowp vec3 l111 = gdk_texture3D(_LightVolumeTexture, _VolumeTextureSize, vec2(0.5, 0.5), positionInVolumetricData + ivec3(1,1,1));

            // interpolation has to be done manually since GLES2 does not support 3d textures and i got around that by packing the
            // volumetric data into a 2d texture.  
            // TODO: uniform to choose none/bi/tri? offer 3 separate shaders? I dont really want to go down the rabbit hole of
            // adding preprocessor macros to auto generate different versions of the same shader, it sucks to maintain.
            // TODO: neighbours "wrap" if l000 is on the border, which means you include light values from the other side of the light volume,
            //  which needs to be fixed.
            // Performing trilinear interpolation here:
            vec3 subVoxelPosition = fract(v_Position);
            lowp vec3 lightX0 = mix(l000, l100, subVoxelPosition.x);
            lowp vec3 lightX1 = mix(l010, l110, subVoxelPosition.x);
            lowp vec3 lightY0 = mix(lightX0, lightX1, subVoxelPosition.y);
            lowp vec3 lightX2 = mix(l001, l101, subVoxelPosition.x);
            lowp vec3 lightX3 = mix(l011, l111, subVoxelPosition.x);
            lowp vec3 lightY1 = mix(lightX2, lightX3, subVoxelPosition.y);

            lowp vec3 finalLight = mix(lightY0, lightY1, subVoxelPosition.z); 
            finalLight = l000; //enable this to see lighting with zero interpolation

            gl_FragColor = vec4(texel.rgb * finalLight, 1.0);
        }
        )V0G0N");

        return std::static_pointer_cast<webgl1es2_context>(pGraphics)->make_shader(vertexShaderSource,fragmentShaderSource);
    }();

    std::cout << "Limit: " << webgl1es2_shader_program::MAX_FRAGMENT_SHADER_INSTRUCTIONS() << "\n";
    
    auto pMaterial = [&]() {
        auto pMaterial = pGraphics->make_material(pShader,
            material::render_mode::opaque,
            material::face_culling_mode::back
        );
        pMaterial->setFloat("_NormalizedTileSize", 0.5);
        pMaterial->setFloat("_VolumeTextureSize", 64);
        pMaterial->setTexture("_LightVolumeTexture", pLightingTexture);
        pMaterial->setTexture("_Texture", pTexture);
        pMaterial->setVector2("_UVOffset", {0, 0});
        pMaterial->setVector2("_UVScale", {1, 1});

        return pMaterial;
    }();
 
    auto pSkyboxShader = [&]() {
        const std::string vertexShaderSource(R"V0G0N(
        uniform mat4 _MVP;
        attribute highp vec3 a_Position;
        varying highp vec3 v_Position;

        void main () {
            gl_Position = _MVP * vec4(a_Position,1.0);
            v_Position = a_Position;
        }
        )V0G0N");

        const std::string fragmentShaderSource(R"V0G0N(
        varying highp vec3 v_Position;

        const vec3 spaceColor = vec3(0.8,1.0,1.0);
        const vec3 horizonColor = vec3(0.4, 0.5, 0.8);

        float dist(vec3 a, vec3 b) {
            return sqrt(dot(a, b));
        }

        void main() {
            vec3 color = mix(horizonColor, spaceColor, abs(normalize(v_Position).y)); 
            gl_FragColor.xyz = vec3(color);
        }
        )V0G0N");

        return std::static_pointer_cast<webgl1es2_context>(pGraphics)->make_shader(vertexShaderSource,fragmentShaderSource);
    }();   

    auto pSkyboxMaterial = pGraphics->make_material(pSkyboxShader);

    auto pSkyboxModel(pGraphics->get_cube_model());

    auto pSkyboxEntity = [&]() {
        auto pSkyboxEntity = pGraphics->make_entity(pSkyboxModel, pSkyboxMaterial);
        pScene->add(pSkyboxEntity);
        pSkyboxEntity->set_transform({0, 5, 0}, {}, {30, 30, 30});
        return pSkyboxEntity; 
    }();

    enum class blockType {
        air = 0,
        dirt,
        rock,
        water,
    };
    std::array<blockType, 64*64*64> data{};
    gdk::ext::volume::volumetric_data_view<blockType, 64,64,64> dataView(&data.front());

    struct uvOffsets {
        graphics_vector2_type n;
        graphics_vector2_type s;
        graphics_vector2_type e;
        graphics_vector2_type w;
        graphics_vector2_type u;
        graphics_vector2_type d;
    };
    const auto blockTypeToUVOffset = [](blockType type) -> uvOffsets {
        switch(type) {
            case blockType::dirt:  return { .n={1,0}, .s={1,0}, .e={1,0}, .w={1,0}, .u={0,0}, .d={1,0} };
            case blockType::rock:  return { .n={0,1}, .s={0,1}, .e={0,1}, .w={0,1}, .u={0,1}, .d={0,1} };
            case blockType::water: return { .n={1,1}, .s={1,1}, .e={1,1}, .w={1,1}, .u={1,1}, .d={1,1} };
        }
        throw graphics_exception("unhandled blocktype");
    };

    for (size_t x(0); x < 16; ++x) for (size_t y(0); y < 16; ++y) dataView.at(x,0,y) = blockType::water;
    for (size_t x(1); x < 14; ++x) for (size_t z(1); z < 12; ++z) dataView.at(x,1,z) = blockType::dirt;

    dataView.at(1,7,0) = blockType::dirt;
    dataView.at(1,6,0) = blockType::dirt;
    dataView.at(3,7,0) = blockType::dirt;
    dataView.at(3,6,0) = blockType::dirt;
    dataView.at(0,4,0) = blockType::dirt;
    dataView.at(1,3,0) = blockType::dirt;
    dataView.at(2,3,0) = blockType::dirt;
    dataView.at(3,3,0) = blockType::dirt;
    dataView.at(4,4,0) = blockType::dirt;
    dataView.at(7,7,7) = blockType::dirt;
    dataView.at(8,3,8) = blockType::dirt;

    dataView.at(05,0,15) = blockType::rock;
    dataView.at(05,1,15) = blockType::rock;
    dataView.at(05,2,15) = blockType::rock;
    dataView.at(05,3,15) = blockType::rock;
    dataView.at(06,0,15) = blockType::rock;
    dataView.at(06,1,15) = blockType::rock;
    dataView.at(06,2,15) = blockType::rock;

    const gdk::model_data NORTH_FACE {{
        { "a_Position", { {
            1.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
            0.0f, 0.0f, 1.0f,

            1.0f, 1.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
        }, 3 } },
        { "a_UV", { {
            1, 1,
            0, 1,
            0, 0,

            1, 1,
            0, 0,
            1, 0,
        }, 2 } },
        { "a_UVOffset", { {
            0, 0,
            0, 0,
            0, 0,
            0, 0,
            0, 0,
            0, 0,
        }, 2 } },
        { "a_BlockDirection", { {
            0.0f,
            0.0f,
            0.0f,
            0.0f,
            0.0f,
            0.0f,
        }, 1 } },
    }};

    const gdk::model_data SOUTH_FACE {{ 
        { "a_Position", { {
            1.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,

            1.0f, 1.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
        }, 3 } },
        { "a_UV", { {
            1, 1,
            0, 0,
            0, 1,

            1, 1,
            1, 0,
            0, 0,
        }, 2 } },
        { "a_UVOffset", { {
            0, 0,
            0, 0,
            0, 0,
            0, 0,
            0, 0,
            0, 0,
        }, 2 } },
        { "a_BlockDirection", { {
            1.0f,
            1.0f,
            1.0f,
            1.0f,
            1.0f,
            1.0f,
        }, 1 } },
    }};

    const gdk::model_data EAST_FACE {{
        { "a_Position", { {
            1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f,

            1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
        }, 3 } },
        { "a_UV", { {
            0, 0,
            1, 0,
            1, 1,

            0, 0,
            1, 1,
            0, 1,
        }, 2 } },
        { "a_UVOffset", { {
            0, 0,
            0, 0,
            0, 0,
            0, 0,
            0, 0,
            0, 0,
        }, 2 } },
        { "a_BlockDirection", { {
            2.0f,
            2.0f,
            2.0f,
            2.0f,
            2.0f,
            2.0f,
        }, 1 } },
    }};

    const gdk::model_data WEST_FACE {{
        { "a_Position", { {
            0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 0.0f,

            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
        }, 3 } },
        { "a_UV", { {
            0, 0,
            1, 1,
            1, 0,

            0, 0,
            0, 1,
            1, 1,
        }, 2 } },
        { "a_UVOffset", { {
            0, 0,
            0, 0,
            0, 0,
            0, 0,
            0, 0,
            0, 0,
        }, 2 } },
        { "a_BlockDirection", { {
            3.0f,
            3.0f,
            3.0f,
            3.0f,
            3.0f,
            3.0f,
        }, 1 } },
    }};

    const gdk::model_data TOP_FACE {{
        { "a_Position", { {
            0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 0.0f,

            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
        }, 3 } },
        { "a_UV", { {
            0, 0,
            1, 1,
            1, 0,

            0, 0,
            0, 1,
            1, 1,
        }, 2 } },
        { "a_UVOffset", { {
            0, 0,
            0, 0,
            0, 0,
            0, 0,
            0, 0,
            0, 0,
        }, 2 } },
        { "a_BlockDirection", { {
            4.0f,
            4.0f,
            4.0f,
            4.0f,
            4.0f,
            4.0f,
        }, 1 } },
    }};

    const gdk::model_data BOTTOM_FACE {{ 
        { "a_Position", { {
            0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 1.0f,

            0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
        }, 3 } },
        { "a_UV", { {
            0, 0,
            1, 0,
            1, 1,

            0, 0,
            1, 1,
            0, 1,
        }, 2 } },
        { "a_UVOffset", { {
            0, 0,
            0, 0,
            0, 0,
            0, 0,
            0, 0,
            0, 0,
        }, 2 } },
        { "a_BlockDirection", { {
            5.0f,
            5.0f,
            5.0f,
            5.0f,
            5.0f,
            5.0f,
        }, 1 } },
    }};

    auto blockModelData = gdk::ext::volume::make_per_voxel_model_data<blockType, blockType::air, 64, 64, 64>(&data.front(), [&](
        const size_t x, const size_t y, const size_t z, const blockType &c, const blockType &n, const blockType &s, const blockType &e, 
        const blockType &w, const blockType &u, const blockType &d) {
        model_data voxel;
        auto offsets = blockTypeToUVOffset(c);
        if (n == blockType::air) {  
            auto face = NORTH_FACE;
            face.transform("a_UVOffset", offsets.n); 
            voxel += face;
        }
        if (s == blockType::air) {
            auto face = SOUTH_FACE;
            face.transform("a_UVOffset", offsets.s); 
            voxel += face;
        }
        if (e == blockType::air) {
            auto face = EAST_FACE;
            face.transform("a_UVOffset", offsets.e); 
            voxel += face;
        }
        if (w == blockType::air) {
            auto face = WEST_FACE;
            face.transform("a_UVOffset", offsets.w); 
            voxel += face;
        }
        if (u == blockType::air) { 
            auto face = TOP_FACE;
            face.transform("a_UVOffset", offsets.u); 
            voxel += face;
        }
        if (d == blockType::air) {
            auto face = BOTTOM_FACE;
            face.transform("a_UVOffset", offsets.d); 
            voxel += face;
        }
        voxel.transform("a_Position", {x, y, z});
        return voxel;
    });

    auto blockModelDataGREED = gdk::ext::volume::make_optimized_block_model_data<blockType, blockType::air, 64, 64, 64>(&data.front(), 
        [&](float x, float y, float z, float w, float h) {
            gdk::model_data face = NORTH_FACE;
            face.transform("a_Position", {x, y, z}, {}, {static_cast<graphics_floating_point_type>(w), static_cast<graphics_floating_point_type>(h), 1.0f});
            face.transform("a_UV", {}, {}, {w, h});
            return face;
        },
        [&](float x, float y, float z, float w, float h) {
            gdk::model_data face = SOUTH_FACE;
            face.transform("a_Position", {x, y, z}, {}, {static_cast<float>(w), static_cast<float>(h), 1.0f});
            face.transform("a_UV", {}, {}, {w, h});
            return face;
        },
        [&](float x, float y, float z, float w, float h) {
            gdk::model_data face = EAST_FACE;
            face.transform("a_Position", {x, y, z}, {}, {1.0f, static_cast<float>(w), static_cast<float>(h)});
            face.transform("a_UV", {}, {}, {w,h});
            return face;
        },
        [&](float x, float y, float z, float w, float h) {
            gdk::model_data face = WEST_FACE;
            face.transform("a_Position", {x, y, z}, {}, {1.0f, static_cast<float>(w), static_cast<float>(h)});
            face.transform("a_UV", {}, {}, {w,h});
            return face;
        },
        [&](float x, float y, float z, float w, float h) {
            gdk::model_data face = TOP_FACE;
            face.transform("a_Position", {x, y, z}, {}, {static_cast<float>(w), 1.0f, static_cast<float>(h)});
            face.transform("a_UV", {}, {}, {static_cast<float>(w), static_cast<float>(h)}); 
            return face;
        },
        [&](float x, float y, float z, float w, float h) {
            gdk::model_data face = BOTTOM_FACE;
            face.transform("a_Position", {x, y, z}, {}, {static_cast<float>(w), 1.0f, static_cast<float>(h)});
            face.transform("a_UV", {}, {}, {w,h});
            return face;
        }
    );

    auto pVolumetricModel(pGraphics->make_model());
    pVolumetricModel->upload(model::usage_hint::streaming, blockModelData);
    //pVolumetricModel->upload(model::usage_hint::streaming, blockModelDataGREED);

    auto pVolumetricEntity = [&]() {
        auto pVolumetricEntity = pGraphics->make_entity(pVolumetricModel, pMaterial);
        pScene->add(pVolumetricEntity);
        return pVolumetricEntity;
    }();

    staticLights.add_global({0.6f, 0.6f, 0.6f});
    staticLights.add_point_light({-1, 2, 0}, 20, {0.0, 0.0, 1.0});
    staticLights.add_point_light({14, 2, 14}, 10, {1.0, 0.0, 1.0});
    staticLights.add_point_light({0, 2, 14}, 10, {0.0, 1.0, 0.0});

    game_loop(60, [&](const float time, const float deltaTime) {
        glfwPollEvents();

        lighting = staticLights;
        lighting.add_point_light({8, 1, 8}, std::abs(std::sin(time)) * 10, {1.0, 1.0, 1.0});
        static int x = 8;
        static int z = 8;
        lighting.add_point_light({std::sin(time) * 8 + x, -2, std::cos(time) * 8 + z}, 10, {1.0, 1.0, 1.0});
        auto [lightingTextureView, _] = lighting.to_2d_texture_data();
        pLightingTexture->update_data(lightingTextureView);

        graphics_matrix4x4_type root({0,0,0}, {{0.f, +3.1415f + time*0.5f, 0}});
        graphics_matrix4x4_type chunkMatrix({-8, 0, -8}, {{0, 0, 0}});
        //pVoxelEntity->set_transform(root * chunkMatrix);
        //pVoxelEntity->hide();

        pVolumetricEntity->set_transform(root * chunkMatrix);

        pCamera->set_perspective_projection(90, 0.01, 35, window.getAspectRatio());
        pCamera->set_transform({0, +6, +9}, {{+0.6f, 0, 0}});

        pScene->draw(window.getWindowSize());

        window.swapBuffer(); 

        return window.shouldClose();
    });

    return EXIT_SUCCESS;
}

