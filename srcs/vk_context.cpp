#include "vk_context.hpp"

void crt_instance(Context& ctx)
{
	ctx.app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	ctx.app_info.apiVersion = VK_API_VERSION_1_3;
	ctx.app_info.pApplicationName = "VGemma";
	ctx.app_info.applicationVersion = 1;

	VkInstanceCreateInfo instance_crtinf = {};
	instance_crtinf.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_crtinf.pApplicationInfo = &ctx.app_info;

	CHECK(vkCreateInstance(&instance_crtinf, nullptr, &ctx.instance));
}


void crt_physical_device(Context& ctx)
{
	// gather physical devices
	u32 num_phys_dev = 0;
	CHECK(vkEnumeratePhysicalDevices(ctx.instance, &num_phys_dev, nullptr));

	std::vector<VkPhysicalDevice> phys_devices(num_phys_dev);
	CHECK(vkEnumeratePhysicalDevices(ctx.instance, &num_phys_dev, phys_devices.data()));

	// select the right device
	bool found = false;
	for (auto& pd : phys_devices)
	{
		VkPhysicalDeviceProperties pd_props;
		vkGetPhysicalDeviceProperties(pd, &pd_props);

		bool version = pd_props.apiVersion >= VK_API_VERSION_1_3;
		bool discrete = (pd_props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
		bool integrated = (pd_props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);

		if (version && (discrete || integrated))
		{
			ctx.phys_device = pd;  // set physical device
			found = true;
			break;
		}
	}
	
	// report if not found
	EMIT_NOT_FOUND_ERROR(found, "physical device");
}


void crt_logical_device(Context& ctx)
{
	// gather queue family properties
	u32 num_queue_families = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(ctx.phys_device, &num_queue_families, nullptr);

	std::vector<VkQueueFamilyProperties> queue_fam_props(num_queue_families);
	vkGetPhysicalDeviceQueueFamilyProperties(ctx.phys_device, &num_queue_families, queue_fam_props.data());

	// select the queue family (for compute pipeline)
	bool found = false;
	for (u32 qidx = 0; qidx < num_queue_families; ++qidx)
	{
		auto& qp = queue_fam_props[qidx];

		bool enough_qs = (qp.queueCount >= 1);  // guaranteed, but later can change
		bool compute_req = (qp.queueFlags & (VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT));

		if (enough_qs && compute_req)
		{
			ctx.queue_fam_idx = qidx;
			found = true;
			break;
		}
	}

	EMIT_NOT_FOUND_ERROR(found, "queue family");

	// create the logical device
	VkDeviceQueueCreateInfo queue_crti = {};
	queue_crti.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_crti.queueCount = 1;
	queue_crti.queueFamilyIndex = ctx.queue_fam_idx;

	VkDeviceCreateInfo ldevice_crti = {};
	ldevice_crti.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	ldevice_crti.queueCreateInfoCount = 1;
	ldevice_crti.pQueueCreateInfos = &queue_crti;

	CHECK(vkCreateDevice(ctx.phys_device, &ldevice_crti, nullptr, &ctx.ldevice));
}


VkQueue get_device_queue(const Context& ctx)
{
	VkQueue queue;
	vkGetDeviceQueue(ctx.ldevice, ctx.queue_fam_idx, 0, &queue);
	return queue;
}


std::string stringify_physical_device_props(const VkPhysicalDevice& pd)
{
	// physical device prop. query
	VkPhysicalDeviceProperties pd_props;
	vkGetPhysicalDeviceProperties(pd, &pd_props);


	// functions to get complex properties
	auto _get_device_type_name = [=](const VkPhysicalDeviceType& dtype) -> std::string
		{
			return (
				dtype == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? "DISCRETE" : (
					dtype == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ? "INTEGRATED" : "OTHER")
			);
		};

	auto _get_api_version_str = [=](const u32 version)
		{
			std::stringstream ss;
			ss << VK_API_VERSION_MAJOR(pd_props.apiVersion) << "." << VK_API_VERSION_MINOR(pd_props.apiVersion);
			return ss.str();
		};

	// building the string
	std::stringstream ss;
	ss << "  Physical device properties: \n";
	ss << "    Device name:                   " << pd_props.deviceName << "\n";
	ss << "    Device type:                   " << _get_device_type_name(pd_props.deviceType) << "\n";
	ss << "    Api version:                   " << _get_api_version_str(pd_props.apiVersion) << "\n";
	
	ss << "\n";

	ss << "   maxComputeSharedMemorySize:     " << pd_props.limits.maxComputeSharedMemorySize << "\n";

	return ss.str();
}

