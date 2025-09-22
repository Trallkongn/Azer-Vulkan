#pragma once 
#include <vulkan/vulkan.h>
#include "Azer/Renderer/Buffer.h"

namespace Azer {

  class VulkanVertexBuffer : public VertexBuffer
  {
  public:
    VulkanVertexBuffer(void* vertices, uint32_t size);
    ~VulkanVertexBuffer();

    virtual void Bind() const override;
		virtual void UnBind() const override;

		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

  private:
    BufferLayout m_Layout;
    VkVertexInputBindingDescription m_BindDescription;
  };

  class VulkanIndexBuffer : public IndexBuffer
  {
  public:
    VulkanIndexBuffer(void* indices, uint32_t count);
    ~VulkanIndexBuffer();

    virtual void Bind() const override;
		virtual void UnBind() const override;

		virtual uint32_t GetCount() const override;
  };
}