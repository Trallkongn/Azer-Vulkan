#include "azpch.h"
#include "Buffer.h"
#include "Azer/Renderer/Renderer.h"

#include "Platform/Vulkan/VulkanBuffer.h"

namespace Azer {

  Ref<VertexBuffer> VertexBuffer::Create(void* vertices, uint32_t size)
  {
    switch (Renderer::GetAPI())
    {
    case RendererAPI::API::None :
      return nullptr;
    case RendererAPI::API::OpenGL :
      return nullptr;
    case RendererAPI::API::Vulkan:
      return CreateScope<VulkanVertexBuffer>(vertices, size);
    }

    AZ_CORE_ASSERT(false, "UnKnow RendererAPI!");
    return nullptr;
  }

  Ref<IndexBuffer> IndexBuffer::Create(void* indices, uint32_t count)
  {
    switch (Renderer::GetAPI())
    {
    case RendererAPI::API::None:
      return nullptr;
    case RendererAPI::API::OpenGL:
      return nullptr;
    case RendererAPI::API::Vulkan:
      return CreateScope<VulkanIndexBuffer>(indices, count);
    }

    AZ_CORE_ASSERT(false, "UnKnow RendererAPI!");
    return nullptr;
  }
}