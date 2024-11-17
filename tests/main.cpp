#include "vk_context.hpp"
#include "vk_op_unary.hpp"


int main()
{
	std::cout << "VGemma implementation" << std::endl;

	Context ctx = create_context();
	std::cout << "context was created \n";


	Tensor x = crt_tensor(ctx, DataType::FLOAT32, {2, 3});
	std::vector<f32> x_data = { 3.f, -4.f, 1.f, -3.f, 0.f, -7.f };
	copy_to_gpu(ctx, x, x_data);

	Tensor y = crt_tensor(ctx, DataType::FLOAT32, { 2, 3 });
	std::vector<f32> y_data = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	copy_to_gpu(ctx, y, y_data);

	Relu relu(ctx, x, y);
	relu.execute();

	print_tensor(ctx, x);
	print_tensor(ctx, y);

	return 0;
}
