# Vulkan setup on different devices

## Setup on rasberry pi 5 (pi os)

The vulkan driver is available by default.

But the vulkan library and glslc have to be installed:

```
sudo apt-get install libvulkan-dev libvulkan1
sudo apt-get install glslang-tools glslang-dev glslc
```

## References

- [data structures in the shader](https://docs.vulkan.org/guide/latest/mapping_data_to_shaders.html)

