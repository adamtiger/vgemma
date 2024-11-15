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

	VkDescriptorSetLayout descr_set_layout;
	VkDescriptorPool descr_pool;
	VkDescriptorSet descriptor_set;
	VkPipelineLayout pipeline_layout;
	VkPipeline compute_pipeline;

	VkCommandPool cmd_pool;
	VkCommandBuffer cmd_buffer;

	VkSubmitInfo submit_info;

	VkQueue queue;
	VkFence fence;

	void crt_fence();
	void crt_submit_info();
	void crt_cmd_buffer();

	void crt_descriptor_set(const std::vector<Tensor>& tensors);
};


