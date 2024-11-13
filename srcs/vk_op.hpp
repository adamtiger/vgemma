#pragma once

#include "tensor.hpp"


struct Operator
{
	std::vector<Tensor> inputs;
	std::vector<Tensor> outputs;

	explicit Operator(Context& ctx);
	~Operator();

	void execute();

private:

	Context& ctx;

	VkCommandPool cmd_pool;
	VkCommandBuffer cmd_buffer;

	VkSubmitInfo submit_info;

	VkQueue queue;
	VkFence fence;

	void crt_fence();
	void crt_submit_info();
	void crt_cmd_buffer();
};


