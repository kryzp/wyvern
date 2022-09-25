#pragma once

#include <wvn/graphics/renderer_backend.h>
#include <vulkan/vulkan.h>
#include <wvn/container/vector.h>
#include <wvn/container/optional.h>

namespace wvn::gfx
{
	class VulkanBackend : public RendererBackend
	{
		struct QueueFamilyIdx
		{
			Optional<u32> graphics_family;
			Optional<u32> present_family;
			Optional<u32> compute_family;
			Optional<u32> transfer_family;

			constexpr bool is_complete() const
			{
				return (
					graphics_family &&
					present_family &&
					compute_family &&
					transfer_family
				);
			}

			Vector<u32> package() const
			{
				return {
					graphics_family.value(),
					present_family.value(),
					compute_family.value(),
					transfer_family.value()
				};
			}
		};

		struct QueueData
		{
			VkQueue graphics_queue;
			VkQueue present_queue;
			VkQueue compute_queue;
			VkQueue transfer_queue;
		};

		struct LogicalDeviceData
		{
			VkDevice device;
		};

		struct PhysicalDeviceData
		{
			VkPhysicalDevice device;
			VkPhysicalDeviceProperties properties;
			VkPhysicalDeviceFeatures features;
		};

	public:
		VulkanBackend();
		~VulkanBackend() override;

		RendererProperties properties() override;

		void debug_tick() override;

	private:
		void enumerate_physical_devices();
		void create_logical_device();

		u32 assign_physical_device_usability(VkPhysicalDevice device, VkPhysicalDeviceProperties properties, VkPhysicalDeviceFeatures features);
		QueueFamilyIdx find_queue_families(VkPhysicalDevice device);

		VkInstance m_instance;
		VkSurfaceKHR m_surface;

		QueueData m_queues;
		LogicalDeviceData m_logical_data;
		PhysicalDeviceData m_physical_data;

#if WVN_DEBUG
		VkDebugUtilsMessengerEXT m_debug_messenger;
#endif
	};
}
