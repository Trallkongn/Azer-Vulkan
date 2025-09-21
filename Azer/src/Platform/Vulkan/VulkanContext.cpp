#define GLFW_INCLUDE_VULKAN
#include "azpch.h"
#include "VulkanContext.h"
#include <fstream>
#include <filesystem>
#include <set>
#include <stdexcept>

namespace Azer {

	static std::vector<char> readFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		if (!file.is_open()) {
			std::cerr << "Current Path: " << std::filesystem::current_path() << std::endl;
			throw std::runtime_error("Failed to open file: " + filename);
		}
		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();
		return buffer;
	}

	VulkanData* VulkanContext::s_VulkanData = nullptr;

	VulkanContext::VulkanContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		AZ_CORE_ASSERT(windowHandle, "Handle is null!");
		s_VulkanData = new VulkanData();
	}

	void VulkanContext::Init()
	{
		CreateInstance();
		CreateSurface();
		PickPhysicalDevice();
		CreateLogicalDevice();
		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		CreateGraphicsPipeline();
		CreateFramebuffers();
		CreateCommandPool();
		CreateVertexBuffer();
		CreateCommandbuffers();
		CreateSyncObjects();
	}

	void VulkanContext::SwapBuffers()
	{
		DrawFrame();
		// ReCreateSwapChain();
	}

  void VulkanContext::DrawFrame()
  {
		uint32_t imageIndex;
		VkResult resANI = vkAcquireNextImageKHR(s_VulkanData->device, s_VulkanData->swapChain, UINT64_MAX,
			s_VulkanData->imageAvailableSemaphore, VK_NULL_HANDLE,
			&imageIndex);

		if (resANI == VK_ERROR_OUT_OF_DATE_KHR || resANI == VK_SUBOPTIMAL_KHR)
			ReCreateSwapChain();

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { s_VulkanData->imageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &s_VulkanData->commandBuffers[imageIndex];

		VkSemaphore signalSemaphores[] = { s_VulkanData->renderFinishedSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(s_VulkanData->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { s_VulkanData->swapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		VkResult resQP = vkQueuePresentKHR(s_VulkanData->presentQueue, &presentInfo);
		if (resQP == VK_ERROR_OUT_OF_DATE_KHR || resQP == VK_SUBOPTIMAL_KHR)
			ReCreateSwapChain();

		vkQueueWaitIdle(s_VulkanData->presentQueue);
  }

  // Vulkan
  void VulkanContext::CreateInstance()
	{
		// App Info
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Vulkan Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		// Vulkan Instance
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		// GLFW extensions
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;

		// disable valid layer
		createInfo.enabledLayerCount = 0;

		// Create instance
		if (vkCreateInstance(&createInfo, nullptr, &s_VulkanData->instance) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create instance!");
		}
	}

	void VulkanContext::CreateSurface()
	{
		if (glfwCreateWindowSurface(s_VulkanData->instance, m_WindowHandle, nullptr, &s_VulkanData->surface) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create window surface!");
		}
	}

	void VulkanContext::PickPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(s_VulkanData->instance, &deviceCount, nullptr);
		if (deviceCount == 0) {
			throw std::runtime_error("Failed to find GPUs with Vulkan support!");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(s_VulkanData->instance, &deviceCount, devices.data());

		for (const auto& device : devices) {
			if (CheckDeviceExtensionSupport(device)) {
				s_VulkanData->physicalDevice = device;
				break;
			}
		}

		if (s_VulkanData->physicalDevice == VK_NULL_HANDLE) {
			throw std::runtime_error("Failed to find a GPU with VK_KHR_swapchain support!");
		}
	}

	void VulkanContext::CreateLogicalDevice()
	{
		uint32_t queueFamilyIndex = 0;
		float queuePriority = 1.0f;
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.queueCreateInfoCount = 1;

		const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		createInfo.enabledLayerCount = 0;

		if (vkCreateDevice(s_VulkanData->physicalDevice, &createInfo, nullptr, &s_VulkanData->device) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create logical device!");
		}

		vkGetDeviceQueue(s_VulkanData->device, queueFamilyIndex, 0, &s_VulkanData->graphicsQueue);
		vkGetDeviceQueue(s_VulkanData->device, queueFamilyIndex, 0, &s_VulkanData->presentQueue);
	}

	void VulkanContext::CreateSwapChain()
	{
		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(s_VulkanData->physicalDevice, s_VulkanData->surface, &capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(s_VulkanData->physicalDevice, s_VulkanData->surface, &formatCount, nullptr);
		std::vector<VkSurfaceFormatKHR> formats(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(s_VulkanData->physicalDevice, s_VulkanData->surface, &formatCount, formats.data());
		VkSurfaceFormatKHR surfaceFormat = formats[0];

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(s_VulkanData->physicalDevice, s_VulkanData->surface, &presentModeCount, nullptr);
		std::vector<VkPresentModeKHR> presentModes(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(s_VulkanData->physicalDevice, s_VulkanData->surface, &presentModeCount, presentModes.data());
		VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

		// extent
		VkExtent2D extent = capabilities.currentExtent;
		if (extent.width == UINT32_MAX) {
			extent = { 1280, 720 };
		}

		// image count
		uint32_t imageCount = capabilities.minImageCount + 1;
		if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
			imageCount = capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = s_VulkanData->surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.preTransform = capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(s_VulkanData->device, &createInfo, nullptr, &s_VulkanData->swapChain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}

		vkGetSwapchainImagesKHR(s_VulkanData->device, s_VulkanData->swapChain, &imageCount, nullptr);
		s_VulkanData->swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(s_VulkanData->device, s_VulkanData->swapChain, &imageCount, s_VulkanData->swapChainImages.data());

		s_VulkanData->swapChainImageFormat = surfaceFormat.format;
		s_VulkanData->swapChainExtent = extent;
	}

	void VulkanContext::CreateImageViews()
	{
		s_VulkanData->swapChainImageViews.resize(s_VulkanData->swapChainImages.size());
		for (size_t i = 0; i < s_VulkanData->swapChainImages.size(); i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = s_VulkanData->swapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = s_VulkanData->swapChainImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(s_VulkanData->device, &createInfo, nullptr, &s_VulkanData->swapChainImageViews[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create image views!");
			}
		}
	}

	void VulkanContext::CreateRenderPass()
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = s_VulkanData->swapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;

		if (vkCreateRenderPass(s_VulkanData->device, &renderPassInfo, nullptr, &s_VulkanData->renderPass) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create render pass!");
		}
	}

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;                  // ���㻺�����󶨺�
		bindingDescription.stride = sizeof(Vertex);      // ÿ��������ֽڴ�С
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // ÿ������ʹ��һ��
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

		// pos -> location = 0
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT; // vec2
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		// color -> location = 1
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT; // vec3
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}

	void VulkanContext::CreateGraphicsPipeline()
	{
		auto vertShaderCode = readFile("vert.spv");
		auto fragShaderCode = readFile("frag.spv");

		VkShaderModuleCreateInfo vertCreateInfo{};
		vertCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		vertCreateInfo.codeSize = vertShaderCode.size();
		vertCreateInfo.pCode = reinterpret_cast<const uint32_t*>(vertShaderCode.data());

		VkShaderModule vertShaderModule;
		if (vkCreateShaderModule(s_VulkanData->device, &vertCreateInfo, nullptr, &vertShaderModule) != VK_SUCCESS)
			throw std::runtime_error("Failed to create vertex shader module!");

		VkShaderModuleCreateInfo fragCreateInfo{};
		fragCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		fragCreateInfo.codeSize = fragShaderCode.size();
		fragCreateInfo.pCode = reinterpret_cast<const uint32_t*>(fragShaderCode.data());

		VkShaderModule fragShaderModule;
		if (vkCreateShaderModule(s_VulkanData->device, &fragCreateInfo, nullptr, &fragShaderModule) != VK_SUCCESS)
			throw std::runtime_error("Failed to create fragment shader module!");

		VkPipelineShaderStageCreateInfo vertStageInfo{};
		vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertStageInfo.module = vertShaderModule;
		vertStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragStageInfo{};
		fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragStageInfo.module = fragShaderModule;
		fragStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertStageInfo, fragStageInfo };

		// Vertex Input (Bind no buffer)
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;

		VkVertexInputBindingDescription bd = getBindingDescription();
		vertexInputInfo.pVertexBindingDescriptions = &bd;
		vertexInputInfo.vertexAttributeDescriptionCount = 2;
		VkVertexInputAttributeDescription* ad = getAttributeDescriptions().data();
		vertexInputInfo.pVertexAttributeDescriptions = ad;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)s_VulkanData->swapChainExtent.width;
		viewport.height = (float)s_VulkanData->swapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { 0,0 };
		scissor.extent = s_VulkanData->swapChainExtent;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasClamp = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		if (vkCreatePipelineLayout(s_VulkanData->device, &pipelineLayoutInfo, nullptr, &s_VulkanData->pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("Failed to create pipeline layout!");

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.layout = s_VulkanData->pipelineLayout;
		pipelineInfo.renderPass = s_VulkanData->renderPass;
		pipelineInfo.subpass = 0;

		if (vkCreateGraphicsPipelines(s_VulkanData->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &s_VulkanData->graphicsPipeline) != VK_SUCCESS)
			throw std::runtime_error("Failed to create graphic pipeline!");

		vkDestroyShaderModule(s_VulkanData->device, vertShaderModule, nullptr);
		vkDestroyShaderModule(s_VulkanData->device, fragShaderModule, nullptr);
	}

	void VulkanContext::CreateVertexBuffer()
	{
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		CreateBuffer(bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			m_VertexBuffer,
			m_VertexBufferMemory);

		void* data;
		vkMapMemory(s_VulkanData->device, m_VertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(s_VulkanData->device, m_VertexBufferMemory);
	}

	void VulkanContext::CreateFramebuffers()
	{
		s_VulkanData->swapChainFramebuffers.resize(s_VulkanData->swapChainImageViews.size());

		for (size_t i = 0; i < s_VulkanData->swapChainImageViews.size(); i++)
		{
			VkImageView attachments[] = { s_VulkanData->swapChainImageViews[i] };

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = s_VulkanData->renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = s_VulkanData->swapChainExtent.width;
			framebufferInfo.height = s_VulkanData->swapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(s_VulkanData->device, &framebufferInfo, nullptr, &s_VulkanData->swapChainFramebuffers[i]) != VK_SUCCESS)
				throw std::runtime_error("Failed to create framebuffer!");
		}
	}

	void VulkanContext::CreateCommandPool()
	{
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = 0;

		if (vkCreateCommandPool(s_VulkanData->device, &poolInfo, nullptr, &s_VulkanData->commandPool) != VK_SUCCESS)
			throw std::runtime_error("Failed to create command pool!");
	}

	void VulkanContext::CreateCommandbuffers()
	{
		s_VulkanData->commandBuffers.resize(s_VulkanData->swapChainFramebuffers.size());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = s_VulkanData->commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)s_VulkanData->commandBuffers.size();

		if (vkAllocateCommandBuffers(s_VulkanData->device, &allocInfo, s_VulkanData->commandBuffers.data()) != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate commandbuffers!");

		for (size_t i = 0; i < s_VulkanData->commandBuffers.size(); i++)
		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(s_VulkanData->commandBuffers[i], &beginInfo) != VK_SUCCESS)
				throw std::runtime_error("Failed to begin recording command buffer!");

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = s_VulkanData->renderPass;
			renderPassInfo.framebuffer = s_VulkanData->swapChainFramebuffers[i];
			renderPassInfo.renderArea.offset = { 0,0 };
			renderPassInfo.renderArea.extent = s_VulkanData->swapChainExtent;

			VkClearValue clearColor = { {{0.0f,0.0f,0.0f,1.0f}} };
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			vkCmdBeginRenderPass(s_VulkanData->commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdBindPipeline(s_VulkanData->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, s_VulkanData->graphicsPipeline);

			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(s_VulkanData->commandBuffers[i], 0, 1, &m_VertexBuffer, offsets);
			vkCmdDraw(s_VulkanData->commandBuffers[i], 3, 1, 0, 0);
			vkCmdEndRenderPass(s_VulkanData->commandBuffers[i]);

			if (vkEndCommandBuffer(s_VulkanData->commandBuffers[i]) != VK_SUCCESS)
				throw std::runtime_error("Failed to record command buffer!");
		}
	}

	void VulkanContext::CreateSyncObjects()
	{
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		if (vkCreateSemaphore(s_VulkanData->device, &semaphoreInfo, nullptr, &s_VulkanData->imageAvailableSemaphore) != VK_SUCCESS ||
			vkCreateSemaphore(s_VulkanData->device, &semaphoreInfo, nullptr, &s_VulkanData->renderFinishedSemaphore) != VK_SUCCESS) {
			throw std::runtime_error("failed to create semaphores!");
		}
	}

	void VulkanContext::ReCreateSwapChain()
	{
		vkDeviceWaitIdle(s_VulkanData->device);

		if (s_VulkanData->swapChain != VK_NULL_HANDLE)
			vkDestroySwapchainKHR(s_VulkanData->device, s_VulkanData->swapChain, nullptr);
		for (auto& framebuffer : s_VulkanData->swapChainFramebuffers)
			vkDestroyFramebuffer(s_VulkanData->device, framebuffer, nullptr);
		for (auto& imageView : s_VulkanData->swapChainImageViews)
			vkDestroyImageView(s_VulkanData->device, imageView, nullptr);
		vkDestroyPipeline(s_VulkanData->device, s_VulkanData->graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(s_VulkanData->device, s_VulkanData->pipelineLayout, nullptr);
		vkDestroyRenderPass(s_VulkanData->device, s_VulkanData->renderPass, nullptr);
		vkDestroyCommandPool(s_VulkanData->device, s_VulkanData->commandPool, nullptr);

		int width = 0, height = 0;
		glfwGetFramebufferSize(m_WindowHandle, &width, &height);
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(m_WindowHandle, &width, &height);
			glfwWaitEvents();
		}

		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		CreateGraphicsPipeline();
		CreateFramebuffers();
		CreateCommandPool();
		CreateCommandbuffers();
	}

	void VulkanContext::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(s_VulkanData->device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(s_VulkanData->device, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(
			memRequirements.memoryTypeBits,
			properties
		);

		if (vkAllocateMemory(s_VulkanData->device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate buffer memory!");
		}

		vkBindBufferMemory(s_VulkanData->device, buffer, bufferMemory, 0);
	}

	uint32_t VulkanContext::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(s_VulkanData->physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) &&
				(memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	bool VulkanContext::CheckDeviceExtensionSupport(VkPhysicalDevice device)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		// ����Ҫ����չ�б�
		std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}
}

