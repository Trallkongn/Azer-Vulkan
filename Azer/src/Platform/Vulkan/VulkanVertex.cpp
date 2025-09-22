#include "azpch.h"
#include "VulkanVertex.h"

namespace Azer {

  VkVertexInputBindingDescription VulkanVertex2D::GetBindDesc2D()
  {
    VkVertexInputBindingDescription desc{};
    desc.binding = 0;
    desc.stride = sizeof(Vertex2D);
    desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return desc;
  }

  std::array<VkVertexInputAttributeDescription, 2> VulkanVertex2D::GetAtrriDesc2D()
  {
    std::array<VkVertexInputAttributeDescription,2> attri{};
    attri[0].binding = 0;
    attri[0].location = 0;
    attri[0].format = VK_FORMAT_R32G32_SFLOAT;
    attri[0].offset = offsetof(Vertex2D,pos);

    attri[1].binding = 0;
    attri[1].location = 1;
    attri[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attri[1].offset = offsetof(Vertex2D,color);

    return attri;
  }
}