#pragma once

#include <wvn/graphics/renderer_backend.h>
#include <vulkan/vulkan.h>
#include <wvn/container/vector.h>

namespace wvn::gfx
{
	struct LogicalDeviceData
	{
		VkDevice device;

		LogicalDeviceData()
			: device(VK_NULL_HANDLE)
		{
		}
	};

	struct PhysicalDeviceData
	{
		VkPhysicalDevice device;
		VkPhysicalDeviceProperties properties;
		VkPhysicalDeviceFeatures features;

		PhysicalDeviceData()
			: device(VK_NULL_HANDLE)
			, features()
			, properties()
		{
		}
	};

	class VulkanBackend : public RendererBackend
	{
	public:
		VulkanBackend();
		~VulkanBackend() override;

		RendererProperties properties() override;

		void debug_tick() override;

	private:
		void enumerate_physical_devices();
		void create_logical_device();

		u32 assign_physical_device_usability(VkPhysicalDevice device, VkPhysicalDeviceProperties properties, VkPhysicalDeviceFeatures features);

		VkInstance m_instance;

		LogicalDeviceData m_logical_data;
		PhysicalDeviceData m_physical_data;

#if WVN_DEBUG
		VkDebugUtilsMessengerEXT m_debug_messenger;
#endif
	};
}
