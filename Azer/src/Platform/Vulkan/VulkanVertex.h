#pragma once
#include <vulkan/vulkan.h>
#include <array>

namespace Azer {

  class VulkanVertex2D
  {
  public:
    static VkVertexInputBindingDescription GetBindDesc2D();
    static std::array<VkVertexInputAttributeDescription,2> GetAtrriDesc2D();
  };
}