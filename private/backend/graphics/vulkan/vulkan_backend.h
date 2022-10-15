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

			constexpr bool all_unique() const
			{
				// todo: store this??
				const u32 g = graphics_family.value();
				const u32 p = present_family.value();
				const u32 c = compute_family.value();
				const u32 t = transfer_family.value();

				return (
					(g != p && g != c && g != t) &&
					(p != g && p != c && p != t) &&
					(c != g && c != p && c != t) &&
					(t != g && t != c && t != p)
				);
			}

			const Vector<u32> package() const
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

		struct SwapChainSupportDetails
		{
			VkSurfaceCapabilitiesKHR capabilities;
			Vector<VkSurfaceFormatKHR> surface_formats;
			Vector<VkPresentModeKHR> present_modes;
		};

	public:
		VulkanBackend();
		~VulkanBackend() override;

		RendererProperties properties() override;

		void debug_tick() override;

	private:
		void enumerate_physical_devices();
		void create_logical_device(const QueueFamilyIdx& phys_idx);
		void create_swap_chain(const QueueFamilyIdx& phys_idx);

		u32 assign_physical_device_usability(VkPhysicalDevice device, VkPhysicalDeviceProperties properties, VkPhysicalDeviceFeatures features, bool* essentials_completed);
		QueueFamilyIdx find_queue_families(VkPhysicalDevice device);
		bool check_device_extension_support(VkPhysicalDevice device);
		SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device);
		VkSurfaceFormatKHR choose_swap_surface_format(const Vector<VkSurfaceFormatKHR>& available_surface_formats);
		VkPresentModeKHR choose_swap_present_mode(const Vector<VkPresentModeKHR>& available_present_modes);
		VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);

		VkInstance m_instance;
		VkSurfaceKHR m_surface;

		VkSwapchainKHR m_swap_chain;
		Vector<VkImage> m_swap_chain_images;
		VkFormat m_swap_chain_image_format;
		VkExtent2D m_swap_chain_extent;

		QueueData m_queues;
		LogicalDeviceData m_logical_data;
		PhysicalDeviceData m_physical_data;

#if WVN_DEBUG
		VkDebugUtilsMessengerEXT m_debug_messenger;
#endif
	};
}
