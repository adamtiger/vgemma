#pragma once

#include "core.hpp"
#include "vk_context.hpp"


struct Tensor
{
	u64 dim;
	DataType dtype;
	VkBuffer buffer;
	VkDeviceMemory memory;

	std::vector<u64> shape;
	std::vector<u64> stride;
};


Tensor crt_tensor(const Context& ctx, const DataType dtype, const std::vector<u64>& shape);


void destroy_tensor(const Context& ctx, Tensor& t);


void copy_to_gpu(const Context& ctx, Tensor& t, const std::vector<f32>& hdata);


void copy_to_gpu(const Context& ctx, Tensor& t, const std::vector<i64>& hdata);


void copy_to_cpu(const Context& ctx, const Tensor& t, std::vector<f32>& hdata);


void copy_to_cpu(const Context& ctx, const Tensor& t, std::vector<i64>& hdata);


u64 calc_tensor_size(const Tensor& t);


u64 count_tensor_elements(const Tensor& t);


std::vector<u64> calc_default_stride(const std::vector<u64>& shape);
