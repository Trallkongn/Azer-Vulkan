#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "Azer/Renderer/GraphicsContext.h"

namespace Azer {

    struct Vertex
    {
        float pos[2];
        float color[3];
    };

    const std::vector<Vertex> vertices = {
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
	private:
        GLFWwindow* m_WindowHandle;

        struct VulkanData
        {

        };

        VkInstance instance;
        VkSurfaceKHR surface;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device;
        VkQueue graphicsQueue;
        VkQueue presentQueue;
        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        std::vector<VkImageView> swapChainImageViews;
        VkRenderPass renderPass;
        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;
        std::vector<VkFramebuffer> swapChainFramebuffers;
        VkCommandPool commandPool;
        std::vector<VkCommandBuffer> commandBuffers;
        VkSemaphore imageAvailableSemaphore;
        VkSemaphore renderFinishedSemaphore;


        // Vulkan buffers
        VkBuffer m_VertexBuffer;
        VkDeviceMemory m_VertexBufferMemory;

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