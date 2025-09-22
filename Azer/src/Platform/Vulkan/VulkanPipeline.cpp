#include "azpch.h"
#include "VulkanPipeline.h"
#include "VulkanVertex.h"

#include <fstream>
#include <filesystem>
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

  VulkanPipeline::VulkanPipeline(VulkanData &vDa)
  {
    auto vertShaderCode = readFile("vert.spv");
		auto fragShaderCode = readFile("frag.spv");

		VkShaderModuleCreateInfo vertCreateInfo{};
		vertCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		vertCreateInfo.codeSize = vertShaderCode.size();
		vertCreateInfo.pCode = reinterpret_cast<const uint32_t*>(vertShaderCode.data());

		VkShaderModule vertShaderModule;
		if (vkCreateShaderModule(vDa.device, &vertCreateInfo, nullptr, &vertShaderModule) != VK_SUCCESS)
			throw std::runtime_error("Failed to create vertex shader module!");

		VkShaderModuleCreateInfo fragCreateInfo{};
		fragCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		fragCreateInfo.codeSize = fragShaderCode.size();
		fragCreateInfo.pCode = reinterpret_cast<const uint32_t*>(fragShaderCode.data());

		VkShaderModule fragShaderModule;
		if (vkCreateShaderModule(vDa.device, &fragCreateInfo, nullptr, &fragShaderModule) != VK_SUCCESS)
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

		// Vertex Input
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;

		VkVertexInputBindingDescription bd = VulkanVertex2D::GetBindDesc2D();
		vertexInputInfo.pVertexBindingDescriptions = &bd;
		vertexInputInfo.vertexAttributeDescriptionCount = 2;
		VkVertexInputAttributeDescription* ad = VulkanVertex2D::GetAtrriDesc2D().data();
		vertexInputInfo.pVertexAttributeDescriptions = ad;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)vDa.swapChainExtent.width;
		viewport.height = (float)vDa.swapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { 0,0 };
		scissor.extent = vDa.swapChainExtent;

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

		if (vkCreatePipelineLayout(vDa.device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
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
		pipelineInfo.layout = m_PipelineLayout;
		pipelineInfo.renderPass = vDa.renderPass;
		pipelineInfo.subpass = 0;

		if (vkCreateGraphicsPipelines(vDa.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS)
			throw std::runtime_error("Failed to create graphic pipeline!");

		vkDestroyShaderModule(vDa.device, vertShaderModule, nullptr);
		vkDestroyShaderModule(vDa.device, fragShaderModule, nullptr);
  }

  VulkanPipeline::~VulkanPipeline()
  {

  }
}