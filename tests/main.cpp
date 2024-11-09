#include "vk_context.hpp"


int main()
{
	std::cout << "VGemma implementation" << std::endl;

	Context ctx;

	crt_instance(ctx);
	crt_physical_device(ctx);
	crt_logical_device(ctx);

	std::cout << stringify_physical_device_props(ctx.phys_device) << std::endl;

	return 0;
}
