#include "azpch.h"
#include <GLFW/glfw3.h>
#include "Renderer.h"
#include "GraphicsContext.h"

#include "Platform/Vulkan/VulkanContext.h"

namespace Azer {

	Scope<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			return nullptr;
		case RendererAPI::API::OpenGL:
			//return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
			return nullptr;
		case RendererAPI::API::Vulkan:
			return CreateScope<VulkanContext>(static_cast<GLFWwindow*>(window));
		}

		AZ_CORE_ASSERT(false, "UnKnow RendererAPI!");
		return nullptr;
	}
}