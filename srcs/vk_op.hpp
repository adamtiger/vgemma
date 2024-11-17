#pragma once

#include "tensor.hpp"


struct Operator
{
	std::vector<Tensor> inputs;
	std::vector<Tensor> outputs;

	explicit Operator(Context& ctx);
	~Operator();

	void execute();

protected:

	Context& ctx;

	std::vector<VkDescriptorSetLayoutBinding> bindings;
	std::vector<VkDescriptorBufferInfo> buf_infos;
	std::vector<VkWriteDescriptorSet> wrt_descr_sets;
	VkDescriptorSetLayout descr_set_layout;
	VkDescriptorPool descr_pool;
	VkDescriptorSet descriptor_set;
	VkPipelineLayout pipeline_layout;
	VkPipeline compute_pipeline;
	std::vector<VkPushConstantRange> push_const_ranges;
	
	std::string shader_path;
	VkShaderModule shader;
	std::vector<char> shader_code;

	VkCommandPool cmd_pool;
	VkCommandBuffer cmd_buffer;

	VkSubmitInfo submit_info;

	VkQueue queue;
	VkFence fence;

	void crt_fence();
	void crt_submit_info();
	void crt_cmd_buffer();

	void crt_descriptor_set(const std::vector<Tensor>& tensors);
	void crt_compute_pipeline();

	void init_op();

	virtual std::vector<Tensor> list_tensors() = 0;
	virtual void calc_group_sizes(u32& gx, u32& gy, u32& gz) = 0;
	virtual std::vector<VkPushConstantRange> crt_push_constants() = 0;
	virtual void bind_push_constants() = 0;
};

