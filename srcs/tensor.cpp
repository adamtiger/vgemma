#include "tensor.hpp"


Tensor crt_tensor(
	const Context& ctx, 
	const DataType dtype, 
	const std::vector<u64>& shape)
{
	Tensor t = {};
	t.dtype = dtype;
	t.shape = shape;
	t.stride = calc_default_stride(shape);

	u64 buffer_size = calc_tensor_size(t);


	// create the buffer
	VkBufferCreateInfo buffer_crti = {};
	buffer_crti.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_crti.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	buffer_crti.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	buffer_crti.size = buffer_size;

	vkCreateBuffer(ctx.ldevice, &buffer_crti, nullptr, &t.buffer);


	// create the device memory for the buffer
	VkPhysicalDeviceMemoryProperties physical_device_props;
	vkGetPhysicalDeviceMemoryProperties(ctx.phys_device, &physical_device_props);

	u32 selected_mem_type_idx = 0;
	{
		bool found = false;
		for (u32 ix = 0; ix < physical_device_props.memoryTypeCount; ++ix)
		{
			auto memory_type = physical_device_props.memoryTypes[ix];

			bool host_visible = (memory_type.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

			if (host_visible)
			{
				selected_mem_type_idx = ix;
				found = true;
				break;
			}
		}

		EMIT_NOT_FOUND_ERROR(found, "device memory type");
	}


	VkMemoryAllocateInfo mem_alloci = {};
	mem_alloci.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	mem_alloci.allocationSize = buffer_size;
	mem_alloci.memoryTypeIndex = selected_mem_type_idx;

	vkAllocateMemory(ctx.ldevice, &mem_alloci, nullptr, &t.memory);


	// bind the buffer to the device memory
	vkBindBufferMemory(ctx.ldevice, t.buffer, t.memory, 0);

	return t;
}


void destroy_tensor(Tensor& t)
{

}


void copy_to_gpu(const Context& ctx, Tensor& t, const std::vector<f32>& hdata)
{
	f32* gpu_data;
	vkMapMemory(ctx.ldevice, t.memory, 0, VK_WHOLE_SIZE, 0, (void**)&gpu_data);

	memcpy(gpu_data, hdata.data(), sizeof(f32) * hdata.size());

	vkUnmapMemory(ctx.ldevice, t.memory);
}


void copy_to_gpu(const Context& ctx, Tensor& t, const std::vector<i64>& hdata)
{
	i64* gpu_data;
	vkMapMemory(ctx.ldevice, t.memory, 0, VK_WHOLE_SIZE, 0, (void**)&gpu_data);

	memcpy(gpu_data, hdata.data(), sizeof(i64) * hdata.size());

	vkUnmapMemory(ctx.ldevice, t.memory);
}


void copy_to_cpu(const Context& ctx, const Tensor& t, std::vector<f32>& hdata)
{

}


void copy_to_cpu(const Context& ctx, const Tensor& t, std::vector<i64>& hdata)
{

}


u64 calc_tensor_size(const Tensor& t)
{
	return 0;
}


u64 count_tensor_elements(const Tensor& t)
{
	return 0;
}


std::vector<u64> calc_default_stride(const std::vector<u64>& shape)
{
	return {};
}
