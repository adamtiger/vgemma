#pragma once

#include "core.hpp"

// Vulkan context storing the most relevant 
// data to use other vulkan modules.
struct Context
{
	VkApplicationInfo app_info;
	VkInstance instance;
	VkPhysicalDevice phys_device;
	VkDevice ldevice;  // logical device
	u32 queue_fam_idx;
};

Context create_context();

void destroy_context(Context& ctx);

// Defines app info and vulkan instance.
// (version: >1.3)
void crt_instance(Context& ctx);


// Selects an appropriate device.
// Device with compute pipeline support.
void crt_physical_device(Context& ctx);


// Creates a logical device.
// Assumes the physical device is already
// created.
void crt_logical_device(Context& ctx);


/* helper functions for getting informations */

// Get the corresponding queue
VkQueue get_device_queue(const Context& ctx);


// Print the chosen device properties
std::string stringify_physical_device_props(const VkPhysicalDevice& pd);

