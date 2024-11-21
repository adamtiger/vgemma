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

	Relu relu(ctx, x, y);
	relu.execute();

	print_tensor(ctx, x);
	print_tensor(ctx, y);


	Tensor x2 = crt_tensor(ctx, DataType::FLOAT32, { 2, 3 });
	std::vector<f32> x2_data = { -8.f, 4.f, -1.f, 2.f, -9.f, 5.f };
	copy_to_gpu(ctx, x2, x2_data);

	Tensor y2 = crt_tensor(ctx, DataType::FLOAT32, { 2, 3 });

	Relu relu2(ctx, x2, y2);
	relu2.execute();

	print_tensor(ctx, x2);
	print_tensor(ctx, y2);

	return 0;
}
