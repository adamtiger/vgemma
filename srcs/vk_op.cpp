#include "vk_op.hpp"


Operator::Operator(Context& ctx) : ctx(ctx)
{
	vkGetDeviceQueue(ctx.ldevice, ctx.queue_fam_idx, 0, &queue);

	crt_fence();
	crt_submit_info();
}

Operator::~Operator()
{
	vkFreeCommandBuffers(ctx.ldevice, cmd_pool, 1, &cmd_buffer);
	vkDestroyCommandPool(ctx.ldevice, cmd_pool, nullptr);
	vkDestroyFence(ctx.ldevice, fence, nullptr);
}


void Operator::execute()
{
	CHECK(vkQueueSubmit(queue, 1, &submit_info, fence));
}


void Operator::crt_fence()
{
	VkFenceCreateInfo fence_crti = {};
	fence_crti.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	CHECK(vkCreateFence(ctx.ldevice, &fence_crti, nullptr, &fence));
}

void Operator::crt_submit_info()
{
	VkSubmitInfo submit_info = {};
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

	/* TODO: record the relevant parts */

	vkCmdDispatch(cmd_buffer, 1, 1, 1);  // should be calculated

	CHECK(vkEndCommandBuffer(cmd_buffer));
}

