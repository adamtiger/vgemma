#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include <vector>
#include <functional>

#include <vulkan/vulkan.h>

// type defs.
using u32 = uint32_t;


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
