#ifndef VK_UTIL_H_
#define VK_UTIL_H_

#include <vulkan/vulkan.h>

#include <wvn/common.h>

#include <wvn/graphics/renderer_backend.h>
#include <wvn/graphics/texture.h>
#include <wvn/graphics/shader.h>
#include <wvn/graphics/gpu_buffer.h>
#include <wvn/graphics/blend.h>

#include <wvn/container/vector.h>
#include <wvn/container/optional.h>
#include <wvn/container/array.h>

namespace wvn::gfx
{
	class VulkanBackend;

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		Vector<VkSurfaceFormatKHR> surface_formats;
		Vector<VkPresentModeKHR> present_modes;
	};

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
            const u32 g = graphics_family.value();
            const u32 p = present_family.value();
            const u32 c = compute_family.value();
            const u32 t = transfer_family.value();

            return (
				(g != p) && (g != t) && (t != p) && (t != c) && (g != c) && (p != c)
            );
        }

        const Array<u32, 4> package() const
        {
            return {
				graphics_family.value_or(-1),
				present_family.value_or(-1),
				compute_family.value_or(-1),
				transfer_family.value_or(-1)
            };
        }
    };

	namespace vkutil
	{
		static constexpr u32 FRAMES_IN_FLIGHT = 3;

		static const char* VALIDATION_LAYERS[] = {
			"VK_LAYER_KHRONOS_validation" // idk what the #define macro name for this is
		};

		static const char* DEVICE_EXTENSIONS[] = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			"VK_KHR_portability_subset" // same here lol
		};

		VkSurfaceFormatKHR choose_swap_surface_format(const Vector<VkSurfaceFormatKHR>& available_surface_formats);
		VkPresentModeKHR choose_swap_present_mode(const Vector<VkPresentModeKHR>& available_present_modes);
		VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);

		u32 find_memory_type(VkPhysicalDevice physical_device, u32 filter, VkMemoryPropertyFlags properties);

		VkFormat find_supported_format(VkPhysicalDevice device, const Vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		VkFormat find_depth_format(VkPhysicalDevice device);

		bool has_stencil_component(VkFormat format);

		VkCommandBuffer begin_single_time_commands(VkCommandPool cmd_pool, VkDevice device);
		void end_single_time_commands(VkCommandPool cmd_pool, VkCommandBuffer cmd_buf, VkDevice device, VkQueue graphics);
		void end_single_time_graphics_commands(VulkanBackend* backend, VkCommandBuffer cmd_buf);

		VkDeviceSize get_ubo_size(u64 object_size, VkPhysicalDeviceProperties properties);
		u32 calc_ubo_aligned_offset(u64 offset, VkPhysicalDeviceProperties properties);

		SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice physical_device, VkSurfaceKHR surface);
		QueueFamilyIdx find_queue_families(VkPhysicalDevice physical_device, VkSurfaceKHR surface);
		bool check_device_extension_support(VkPhysicalDevice physical_device);
		u32 assign_physical_device_usability(VkSurfaceKHR surface, VkPhysicalDevice physical_device, VkPhysicalDeviceProperties properties, VkPhysicalDeviceFeatures features, bool* essentials_complete);

		TextureFormat get_wvn_texture_format(VkFormat fmt);

		VkFormat get_vk_texture_format(TextureFormat fmt);
		VkImageTiling get_vk_texture_tile(TextureTiling tiling);
		VkFilter get_vk_filter(TextureFilter filter);
		VkSamplerAddressMode get_vk_address_mode(TextureWrap wrap);
		VkShaderStageFlagBits get_vk_shader_flag_bits(ShaderProgramType type);
		VkImageViewType get_vk_image_view_type(TextureType type);
		VkImageType get_vk_image_type(TextureType type);
		VkBlendOp get_vk_blend_op(BlendOp op);
		VkBlendFactor get_vk_blend_factor(BlendFactor factor);
		VkCompareOp get_vk_compare_op(CompareOp op);
		VkLogicOp get_vk_logic_op(LogicOp op);
		VkBorderColor get_vk_border_colour(TextureBorderColour border_colour);
		VkCullModeFlagBits get_vk_cull_mode(CullMode cull);
	}
}
#endif // VK_UTIL_H_
