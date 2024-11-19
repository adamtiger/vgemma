#include "vk_op_unary.hpp"


Elementwise::Elementwise(
	Context& ctx, 
	const std::string& shader_path, 
	const Tensor& x, 
	Tensor& y
    ) : Operator(ctx),  
	    x(x), y(y)
{
	inputs.push_back(x);
	outputs.push_back(y);

	this->shader_path = shader_path;

	init_op();
}

std::vector<Tensor> Elementwise::list_tensors()
{
	return { x, y };
}

void Elementwise::calc_group_sizes(u32& gx, u32& gy, u32& gz)
{
	gy = 1;
	gz = 1;

	gx = count_tensor_elements(x) / 32 + 1;  // TODO: 32 should be a constant 
}

std::vector<VkPushConstantRange> Elementwise::crt_push_constants()
{
	VkPushConstantRange pr = {};
	pr.offset = 0;
	pr.size = 4;  // one 32 bit integer 
	pr.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	return { pr };
}

void Elementwise::bind_push_constants()
{
	push_const_data[0] = count_tensor_elements(x);

	vkCmdPushConstants(
		cmd_buffer,
		pipeline_layout,
		VK_SHADER_STAGE_COMPUTE_BIT,
		0, 4,
		push_const_data
	);
}


Relu::Relu(Context& ctx, const Tensor& x, Tensor& y
    ) : Elementwise(ctx, "C:\\Data\\AI\\projects\\vgemma\\artifacts\\shaders\\vk_op_relu_f32.comp.spv", x, y)
{
}

