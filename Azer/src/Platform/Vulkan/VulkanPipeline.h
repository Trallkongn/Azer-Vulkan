#pragma once
#include <vulkan/vulkan.h>
#include "VulkanData.h"

namespace Azer {

  class VulkanPipeline
  {
  public:
    VulkanPipeline(VulkanData& vDa);
    ~VulkanPipeline();

    VkPipeline GetPipeline() const { return m_Pipeline; }
    VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }
  private:
    VkPipeline m_Pipeline{};
    VkPipelineLayout m_PipelineLayout{};
  };
}