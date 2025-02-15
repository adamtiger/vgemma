#pragma once

#include "vk_op.hpp"


struct Elementwise : public Operator
{
	explicit Elementwise(Context& ctx, const std::string& shader_path, const Tensor& x, Tensor& y);

	Tensor x;
	Tensor y;

protected:

	virtual std::vector<Tensor> list_tensors();
	virtual void calc_group_sizes(u32& gx, u32& gy, u32& gz);
	virtual std::vector<VkPushConstantRange> crt_push_constants();
	virtual void bind_push_constants();

	i32 push_const_data[1];  // contains the length of the tensor
};


struct Relu : public Elementwise
{
	explicit Relu(Context& ctx, const Tensor& x, Tensor& y);
};
