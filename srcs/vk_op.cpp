#include "vk_op.hpp"


Operator::Operator(Context& ctx) : ctx(ctx)
{
}

Operator::~Operator()
{
	vkDestroyShaderModule(ctx.ldevice, shader, nullptr);
	vkFreeDescriptorSets(ctx.ldevice, descr_pool, 1, &descriptor_set);
	vkDestroyDescriptorSetLayout(ctx.ldevice, descr_set_layout, nullptr);
	vkDestroyDescriptorPool(ctx.ldevice, descr_pool, nullptr);
	
	vkDestroyPipelineLayout(ctx.ldevice, pipeline_layout, nullptr);
	vkDestroyPipeline(ctx.ldevice, compute_pipeline, nullptr);

	vkFreeCommandBuffers(ctx.ldevice, cmd_pool, 1, &cmd_buffer);
	vkDestroyCommandPool(ctx.ldevice, cmd_pool, nullptr);
	vkDestroyFence(ctx.ldevice, fence, nullptr);
}


void Operator::execute()
{
	CHECK(vkQueueSubmit(queue, 1, &submit_info, fence));
	CHECK(vkWaitForFences(ctx.ldevice, 1, &fence, VK_TRUE, 100000000));  // nanoseconds
}


void Operator::crt_fence()
{
	VkFenceCreateInfo fence_crti = {};
	fence_crti.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	CHECK(vkCreateFence(ctx.ldevice, &fence_crti, nullptr, &fence));
}

void Operator::crt_submit_info()
{
	submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &cmd_buffer;
}

void Operator::crt_cmd_buffer()
{
	// allocate command buffer
	VkCommandPoolCreateInfo cmd_pool_crti = {};
	cmd_pool_crti.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmd_pool_crti.queueFamilyIndex = ctx.queue_fam_idx;
	CHECK(vkCreateCommandPool(ctx.ldevice, &cmd_pool_crti, nullptr, &cmd_pool));


	VkCommandBufferAllocateInfo cmd_buffer_aloci = {};
	cmd_buffer_aloci.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmd_buffer_aloci.commandPool = cmd_pool;
	cmd_buffer_aloci.commandBufferCount = 1;
	cmd_buffer_aloci.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	CHECK(vkAllocateCommandBuffers(ctx.ldevice, &cmd_buffer_aloci, &cmd_buffer));


	// record command buffer
	VkCommandBufferBeginInfo cmd_buf_begin_info = {};
	cmd_buf_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	CHECK(vkBeginCommandBuffer(cmd_buffer, &cmd_buf_begin_info));

	/* record the relevant parts */

	vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline_layout, 0, 1, &descriptor_set, 0, nullptr);

	vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, compute_pipeline);

	bind_push_constants();

	u32 gx, gy, gz;
	calc_group_sizes(gx, gy, gz);
	vkCmdDispatch(cmd_buffer, gx, gy, gz);

	CHECK(vkEndCommandBuffer(cmd_buffer));
}

void Operator::crt_descriptor_set(const std::vector<Tensor>& tensors)
{
	// create bindings
	bindings.reserve(tensors.size());
	for (u32 bix = 0; bix < tensors.size(); ++bix)
	{
		VkDescriptorSetLayoutBinding b = {};
		b.binding = bix;
		b.descriptorCount = 1;
		b.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		b.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
		bindings.push_back(b);
	}

	// create descriptor set layout
	VkDescriptorSetLayoutCreateInfo descr_set_crti = {};
	descr_set_crti.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descr_set_crti.bindingCount = bindings.size();
	descr_set_crti.pBindings = bindings.data();
	CHECK(vkCreateDescriptorSetLayout(ctx.ldevice, &descr_set_crti, nullptr, &descr_set_layout));

	// allocate descriptor set
	VkDescriptorPoolSize pool_size = {};
	pool_size.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	pool_size.descriptorCount = tensors.size();

	VkDescriptorPoolCreateInfo descr_pool_crti = {};
	descr_pool_crti.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descr_pool_crti.maxSets = 1;   // TODO: what does this mean?
	descr_pool_crti.poolSizeCount = 1;
	descr_pool_crti.pPoolSizes = &pool_size;  // TODO: how does this relate to the layout?
	CHECK(vkCreateDescriptorPool(ctx.ldevice, &descr_pool_crti, nullptr, &descr_pool));

	VkDescriptorSetAllocateInfo descr_set_alloci = {};
	descr_set_alloci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descr_set_alloci.descriptorSetCount = 1;
	descr_set_alloci.pSetLayouts = &descr_set_layout;
	descr_set_alloci.descriptorPool = descr_pool;

	CHECK(vkAllocateDescriptorSets(ctx.ldevice, &descr_set_alloci, &descriptor_set));

	// connect the tensor buffer to the descriptor set
	wrt_descr_sets.reserve(tensors.size());
	for (u32 ix = 0; ix < tensors.size(); ++ix)
	{
		VkDescriptorBufferInfo buf_info = {};
		buf_info.buffer = tensors[ix].buffer;
		buf_info.offset = 0;
		buf_info.range = VK_WHOLE_SIZE;

		buf_infos.push_back(buf_info);  // TODO: without this, memory disappears in the below part

		VkWriteDescriptorSet wrt_descr_set = {};
		wrt_descr_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		wrt_descr_set.dstBinding = ix;
		wrt_descr_set.dstSet = descriptor_set;
		wrt_descr_set.descriptorCount = 1;
		wrt_descr_set.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		wrt_descr_set.pBufferInfo = &buf_infos[ix];

		wrt_descr_sets.push_back(wrt_descr_set);
	}

	vkUpdateDescriptorSets(ctx.ldevice, wrt_descr_sets.size(), wrt_descr_sets.data(), 0, nullptr);
}

void Operator::crt_compute_pipeline()
{
	// create pipeline layout
	VkPipelineLayoutCreateInfo pipeline_layout_crti = {};
	pipeline_layout_crti.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_crti.setLayoutCount = 1;
	pipeline_layout_crti.pSetLayouts = &descr_set_layout;
	pipeline_layout_crti.pushConstantRangeCount = push_const_ranges.size();
	pipeline_layout_crti.pPushConstantRanges = push_const_ranges.data();

	CHECK(vkCreatePipelineLayout(ctx.ldevice, &pipeline_layout_crti, nullptr, &pipeline_layout));


	// load shader from file
	std::ifstream shader_code_file(shader_path, std::ios::ate | std::ios::binary);
	int code_size = shader_code_file.tellg();
	shader_code_file.seekg(0);
	shader_code.resize(code_size);
	shader_code_file.read(shader_code.data(), code_size);

	VkShaderModuleCreateInfo shader_crti = {};
	shader_crti.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shader_crti.codeSize = code_size;
	shader_crti.pCode = reinterpret_cast<u32*>(shader_code.data());

	CHECK(vkCreateShaderModule(ctx.ldevice, &shader_crti, nullptr, &shader));


	// comp pipeline build
	VkPipelineShaderStageCreateInfo pipeline_shader_stage_crti = {};
	pipeline_shader_stage_crti.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipeline_shader_stage_crti.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	pipeline_shader_stage_crti.pName = "main";  // TODO: can be a constant string
	pipeline_shader_stage_crti.module = shader;

	VkComputePipelineCreateInfo comp_pipeline_crti = {};
	comp_pipeline_crti.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	comp_pipeline_crti.layout = pipeline_layout;
	comp_pipeline_crti.stage = pipeline_shader_stage_crti;

	CHECK(vkCreateComputePipelines(ctx.ldevice, VK_NULL_HANDLE, 1, &comp_pipeline_crti, nullptr, &compute_pipeline));
}

void Operator::init_op()
{
	crt_descriptor_set(list_tensors());

	push_const_ranges = crt_push_constants();
	crt_compute_pipeline();

	crt_cmd_buffer();

	vkGetDeviceQueue(ctx.ldevice, ctx.queue_fam_idx, 0, &queue);

	crt_fence();
	crt_submit_info();
}
