#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "Azer/Renderer/GraphicsContext.h"
#include "VulkanData.h"
#include "VulkanVertex.h"
#include "VulkanPipeline.h"

namespace Azer {

    const std::vector<Vertex2D> vertices = {
        {{-0.5f,-0.5f},{1.0f,0.0f,0.0f}},
        {{0.5f,-0.5f},{0.0f,1.0f,0.0f}},
        {{0.0f,0.5f},{0.0f,0.0f,1.0f}}
    };
	class VulkanContext : public GraphicsContext
	{
	public:
        VulkanContext(GLFWwindow* windowHandle);

        void Init() override;
        void SwapBuffers() override;

        static VulkanData& GetVulkanData() { return *s_VulkanData; }
	private:
        GLFWwindow* m_WindowHandle;
        static VulkanData* s_VulkanData;
        // Temp Vulkan buffers
        VkBuffer m_VertexBuffer;
        VkDeviceMemory m_VertexBufferMemory;

        // temp
        Ref<VulkanPipeline> m_GraphicPipeline;

        void DrawFrame();

        // Vulkan Staff
        void CreateInstance();
        void CreateSurface();
        void PickPhysicalDevice();
        void CreateLogicalDevice();
        void CreateSwapChain();
        void CreateImageViews();
        void CreateRenderPass();
        void CreateGraphicsPipeline();
        void CreateVertexBuffer();
        void CreateFramebuffers();
        void CreateCommandPool();
        void CreateCommandbuffers();
        void CreateSyncObjects();

        void ReCreateSwapChain();

        // Tools
        void CreateBuffer(VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties,
            VkBuffer& buffer,
            VkDeviceMemory& bufferMemory);
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    };
}