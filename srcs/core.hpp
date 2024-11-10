#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include <vector>
#include <functional>

#include <vulkan/vulkan.h>


// type defs.
using f64 = double;
using f32 = float;
using i64 = long long;
using i32 = int;
using i16 = short int;
using i8 = char;
using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8 = unsigned char;

static_assert(sizeof(f64) == 8, "float64 needs to be 8 bytes");
static_assert(sizeof(f32) == 4, "float32 needs to be 4 bytes");
static_assert(sizeof(i64) == 8, "int64 needs to be 8 bytes");
static_assert(sizeof(i32) == 4, "int32 needs to be 4 bytes");
static_assert(sizeof(i16) == 2, "int16 needs to be 2 bytes");
static_assert(sizeof(i8) == 1, "int8 needs to be 1 byte");
static_assert(sizeof(u64) == 8, "uint64 needs to be 8 bytes");
static_assert(sizeof(u32) == 4, "uint32 needs to be 4 bytes");
static_assert(sizeof(u16) == 2, "uint16 needs to be 2 bytes");
static_assert(sizeof(u8) == 1, "uint8 needs to be 1 byte");


enum class DataType
{
	FLOAT64,
	FLOAT32,
	INT64,
	INT32,
	INT16,
	INT8,
	UINT64,
	UINT32,
	UINT16,
	UINT8
};


static u64 get_datatype_size(const DataType& dtype)
{
	static u64 sizes[] = { 8, 4, 8, 4, 2, 1, 8, 4, 2, 1 };
	return sizes[static_cast<int>(dtype)];
}


// check the vulkan result
static void CHECK(const VkResult result)
{
	if (result != VK_SUCCESS)
	{
		std::cout << "Vulkan error with code: " << result << "\n";
		std::cout << "Exiting ...";
		exit(0);
	}
}


static void EMIT_NOT_FOUND_ERROR(const bool found, const std::string& cause)
{
	if (!found)
	{
		std::cout << "Not found: " << cause << "\n";
		std::cout << "Exiting ...";
		exit(0);
	}
}
