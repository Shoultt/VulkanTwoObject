#pragma once
#include "Source/VulkanMonotony.h"
#include "Models/modelReader.h"

modelReader chaika;
modelReader monster;

class initVK
{
public:
	void InitVulkan()
	{
		instance_create();
		physDevice_enum();
		device_create();
		surface_create();
		swapchain_create();
		DescriptorSetUboLayout_create();
		graphPipelineLayout_create();
		graphPipilines_create();
		cmdPool_create();
		load_and_create_resources();
		depthBuffering_create();
		dstPool_create();
		dstSets_allocate();
		cmdBuffers_allocate();
		synchrone_create();
	}
	void load_and_create_resources()
	{
		createAllocator();
		indexBuffers_create();
		vertexBuffers_create();
		uniformBuffers_create();
	}
private:
	void instance_create()
	{
		const VkApplicationInfo appInfo{ .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Platformer",
		.applicationVersion = VK_MAKE_VERSION(0, 0, 1),
		.pEngineName = "Elf",
		.engineVersion = VK_MAKE_VERSION(0, 0, 1),
		.apiVersion = VK_API_VERSION_1_3 };

		const VkInstanceCreateInfo instanceCI{ .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = static_cast<uint32_t>(validLayer.size()),
		.ppEnabledLayerNames = validLayer.data(),
		.enabledExtensionCount = static_cast<uint32_t>(ExtsIns.size()),
		.ppEnabledExtensionNames = ExtsIns.data() };

		vkCreateInstance(&instanceCI, nullptr, &instance);
	}

	void physDevice_enum()
	{
		uint32_t physDeviceCount = 1;
		std::vector<VkPhysicalDevice> physDevices(physDeviceCount);
		vkEnumeratePhysicalDevices(instance, &physDeviceCount, physDevices.data());
		physDevice = physDevices[0];
	}

	void device_create()
	{
		float priorQueue = { 1.0f };
		const VkDeviceQueueCreateInfo dqci{ .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = 0,
		.queueCount = 1,
		.pQueuePriorities = &priorQueue };
		
		VkPhysicalDeviceFeatures features{ .samplerAnisotropy = VK_TRUE };

		const VkPhysicalDeviceDynamicRenderingFeatures dynamicRenFeatures{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES, .dynamicRendering = VK_TRUE };
		const VkDeviceCreateInfo dci{ .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = &dynamicRenFeatures,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &dqci,
		.enabledExtensionCount = static_cast<uint32_t>(ExtDev.size()),
		.ppEnabledExtensionNames = ExtDev.data(),
		.pEnabledFeatures = &features };

		vkCreateDevice(physDevice, &dci, nullptr, &device);

		vkGetDeviceQueue(device, 0, 0, &queue);
	}

	void surface_create()
	{
		glfwCreateWindowSurface(instance, window, nullptr, &surface);
		uint32_t count = 1;
		VkSurfaceFormatKHR surfaceFormat;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &count, &surfaceFormat);
		rendering_format = surfaceFormat.format;
	}

	void swapchain_create()
	{
		const VkSwapchainCreateInfoKHR swapCI{ .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = surface,
		.minImageCount = 2,
		.imageFormat = rendering_format,
		.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
		.imageExtent = {Wwidth, Wheight},
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR };

		vkCreateSwapchainKHR(device, &swapCI, nullptr, &swapchain);

		uint32_t swapchainImageCount;
		vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr);
		swapImages.resize(swapchainImageCount);
		vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapImages.data());

		swapImageViews.resize(swapchainImageCount);

		for (int i = 0; i < swapchainImageCount; i++)
		{
			swapImageViews[i] = createImageViews(swapImages[i], rendering_format, VK_IMAGE_ASPECT_COLOR_BIT);
		}
	}

	void DescriptorSetUboLayout_create()
	{
		const VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{
		.binding = 0,
		.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.descriptorCount = 1,
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT };

		const VkDescriptorSetLayoutCreateInfo descriptSetLayoutCI{ .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount = 1,
		.pBindings = &descriptorSetLayoutBinding };
		vkCreateDescriptorSetLayout(device, &descriptSetLayoutCI, nullptr, &descriptSetLayout);
	}

	void graphPipelineLayout_create()
	{
		VkPipelineLayoutCreateInfo pipeLayoutCI{ .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 1,
		.pSetLayouts = &descriptSetLayout };
		vkCreatePipelineLayout(device, &pipeLayoutCI, nullptr, &pipeLayout);
	}

	void graphPipilines_create()
	{
		createShaderModeles(vsCode, &vsModule);
		createShaderModeles(fsCode, &fsModule);

		const VkVertexInputBindingDescription vertexBindingDsn{
		.binding = 0,
		.stride = sizeof(float) * 3,
		.inputRate = VK_VERTEX_INPUT_RATE_VERTEX };

		const VkVertexInputAttributeDescription vertexAttributeDsn{
		.location = 0,
		.binding = 0,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = 0 };

		const VkPipelineVertexInputStateCreateInfo pipeVertexInputStCI{ .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = 1,
		.pVertexBindingDescriptions = &vertexBindingDsn,
		.vertexAttributeDescriptionCount = 1,
		.pVertexAttributeDescriptions = &vertexAttributeDsn };

		VkGraphicsPipelineCreateInfo pipeCI{
		.pVertexInputState = &pipeVertexInputStCI };
		graphPipeline_create(vsModule, fsModule, pipeCI, graphPipeline0, true);

		createShaderModeles(polyVSCode, &polyVSmodule);
		createShaderModeles(polyFSCode, &polyFSmodule);

		const VkVertexInputBindingDescription vertexBindingDsn1{
		.binding = 1,
		.stride = sizeof(float) * 3,
		.inputRate = VK_VERTEX_INPUT_RATE_VERTEX };

		const VkVertexInputAttributeDescription vertexAttributeDsn1{
		.location = 1,
		.binding = 1,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = 0 };
		const VkVertexInputAttributeDescription vertexAttributeDsns1[] = { vertexAttributeDsn1 };

		const VkPipelineVertexInputStateCreateInfo pipeVertexInputStCI1{ .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = 1,
		.pVertexBindingDescriptions = &vertexBindingDsn1,
		.vertexAttributeDescriptionCount = 1,
		.pVertexAttributeDescriptions = &vertexAttributeDsn1 };

		VkGraphicsPipelineCreateInfo pipeCI1{
		.pVertexInputState = &pipeVertexInputStCI1 };
		graphPipeline_create(polyVSmodule, polyFSmodule, pipeCI1, graphPipeline1, false);
	}
	
	void cmdPool_create()
	{
		const VkCommandPoolCreateInfo cmdPoolCI{ .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = 0 };

		vkCreateCommandPool(device, &cmdPoolCI, nullptr, &cmdPool);
	}

	void depthBuffering_create()
	{
		createDepthImage();
		createDepthImageView();
		createDepthImageLayout();
	}

	void indexBuffers_create()
	{
		chaika.load("Models/chaika.glb");
		monster.load("Models/monster.glb");
		createBuffer(indexBuffer, chaika.getIndexSize(), chaika.getIndexData(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indexBufferAlloc);
		createBuffer(indexBuffer1, monster.getIndexSize(), monster.getIndexData(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indexBufferAlloc1);
	}

	void vertexBuffers_create()
	{
		createBuffer(vertexBuffer, chaika.getVertexSize(), chaika.getVertexData(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexBufferAlloc);
		chaika.destroyModel();
		createBuffer(vertexBuffer1, monster.getVertexSize(), monster.getVertexData(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexBufferAlloc1);
		monster.destroyModel();
	}

	void uniformBuffers_create()
	{
		VkDeviceSize uniformBufferSize = sizeof(MVP);
		createUnibuffer(uniformBuffer, uniformBufferSize, uniformBufferAlloc, uniformBufferAllocInfo);
	}

	void dstPool_create()
	{
		const VkDescriptorPoolSize dstPoolSizes{
		.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.descriptorCount = 1 };

		const VkDescriptorPoolCreateInfo descriptPoolCI{ .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.maxSets = 1,
		.poolSizeCount = 1,
		.pPoolSizes = &dstPoolSizes };

		vkCreateDescriptorPool(device, &descriptPoolCI, nullptr, &descriptPool);
	}

	void dstSets_allocate()
	{
		const VkDescriptorSetAllocateInfo descriptSetAI{ .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool = descriptPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &descriptSetLayout };
		vkAllocateDescriptorSets(device, &descriptSetAI, &descriptorSet);

		const VkDescriptorBufferInfo descripBI{
		.buffer = uniformBuffer,
		.offset = 0,
		.range = sizeof(MVP) };

		const VkWriteDescriptorSet descriptSetWriteUniformBuffer{ .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = descriptorSet,
		.dstBinding = 0,
		.descriptorCount = 1,
		.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.pBufferInfo = &descripBI };

		vkUpdateDescriptorSets(device, 1, &descriptSetWriteUniformBuffer, 0, nullptr);
	}

	void cmdBuffers_allocate()
	{
		const VkCommandBufferAllocateInfo cmdBufferAI{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = cmdPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1 };

		vkAllocateCommandBuffers(device, &cmdBufferAI, &cmdBuffer);
	}

	void synchrone_create()
	{
		VkSemaphoreCreateInfo sci{ .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		vkCreateSemaphore(device, &sci, nullptr, &waitSemaphore);
		vkCreateSemaphore(device, &sci, nullptr, &signalSemaphore);

		VkFenceCreateInfo fci{ .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .flags = VK_FENCE_CREATE_SIGNALED_BIT };
		vkCreateFence(device, &fci, nullptr, &fence);
	}
};