#include <backend/graphics/vulkan/vk_backbuffer.h>
#include <backend/graphics/vulkan/vk_util.h>
#include <backend/graphics/vulkan/vk_backend.h>
#include <wvn/system/system_backend.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

VulkanBackbuffer::VulkanBackbuffer(VulkanBackend* backend)
	: m_backend(backend)
	, m_render_pass_builder()
    , m_swap_chain(VK_NULL_HANDLE)
    , m_swap_chain_images()
    , m_swap_chain_image_views()
    , m_surface(VK_NULL_HANDLE)
    , m_curr_swap_chain_image_idx(0)
	, m_colour(backend)
	, m_depth(backend)
	, m_render_finished_semaphores()
	, m_image_available_semaphores()
{
	m_type = RENDER_TARGET_TYPE_BACKBUFFER;
}

VulkanBackbuffer::~VulkanBackbuffer()
{
	clean_up();
}

void VulkanBackbuffer::create()
{
	create_swap_chain();
	acquire_next_image();
}

void VulkanBackbuffer::create_surface()
{
    if (bool result = Root::get_singleton()->system_backend()->vk_create_surface(m_backend->vulkan_instance, &m_surface); !result) {
        wvn_ERROR("[VULKAN:BACKBUFFER|DEBUG] Failed to create surface: %d", result);
    }
}

void VulkanBackbuffer::create_colour_resources()
{
	m_colour.init_size(m_width, m_height);
	m_colour.init_metadata(vkutil::get_wvn_texture_format(m_backend->swap_chain_image_format), TEX_TILE_OPTIMAL, TEX_TYPE_2D);
	m_colour.init_sample_count(m_backend->msaa_samples);
	m_colour.init_transient(true);

	m_colour.create_internal_resources();

	m_render_pass_builder.create_colour_attachment(0,
		m_backend->swap_chain_image_format,
		m_backend->msaa_samples,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		false
	);

	m_render_pass_builder.create_colour_attachment(2,
		m_backend->swap_chain_image_format,
		VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		true
	);

	dev::LogMgr::get_singleton()->print("[VULKAN:BACKBUFFER] Created colour resources!");
}

void VulkanBackbuffer::create_depth_resources()
{
    VkFormat format = vkutil::find_depth_format(m_backend->physical_data.device);

	m_depth.init_size(m_width, m_height);
	m_depth.init_metadata(vkutil::get_wvn_texture_format(format), TEX_TILE_OPTIMAL, TEX_TYPE_2D);
	m_depth.init_sample_count(m_backend->msaa_samples);

	m_depth.create_internal_resources();

    m_depth.transition_layout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	m_render_pass_builder.create_depth_attachment(1, m_backend->msaa_samples);

    dev::LogMgr::get_singleton()->print("[VULKAN:BACKBUFFER] Created depth resources!");
}

void VulkanBackbuffer::clean_up()
{
	m_render_pass_builder.clean_up();

	if (m_surface != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(m_backend->vulkan_instance, m_surface, nullptr);
		m_surface = VK_NULL_HANDLE;
	}
}

void VulkanBackbuffer::clean_up_textures()
{
	m_depth.clean_up();
	m_colour.clean_up();
}

void VulkanBackbuffer::clean_up_swap_chain()
{
	clean_up_textures();

	m_render_pass_builder.clean_up_fbos();

	for (int i = 0; i < vkutil::FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(m_backend->device, m_render_finished_semaphores[i], nullptr);
		vkDestroySemaphore(m_backend->device, m_image_available_semaphores[i], nullptr);
	}

	m_render_finished_semaphores.clear();
	m_image_available_semaphores.clear();

    for (auto& view : m_swap_chain_image_views) {
        vkDestroyImageView(m_backend->device, view, nullptr);
    }

    vkDestroySwapchainKHR(m_backend->device, m_swap_chain, nullptr);
}

VulkanRenderPassBuilder* VulkanBackbuffer::get_render_pass_builder()
{
	return &m_render_pass_builder;
}

void VulkanBackbuffer::acquire_next_image()
{
    if (VkResult result = vkAcquireNextImageKHR(m_backend->device, m_swap_chain, UINT64_MAX, get_image_available_semaphore(), VK_NULL_HANDLE, &m_curr_swap_chain_image_idx); result == VK_ERROR_OUT_OF_DATE_KHR) {
        rebuild_swap_chain();
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        wvn_ERROR("[VULKAN:BACKBUFFER|DEBUG] Failed to acquire next image in swap chain: %d", result);
    }
}

void VulkanBackbuffer::swap_buffers()
{
    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &get_render_finished_semaphore();
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &m_swap_chain;
    present_info.pImageIndices = &m_curr_swap_chain_image_idx;
    present_info.pResults = nullptr;

    if (VkResult result = vkQueuePresentKHR(m_backend->queues.present, &present_info); result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		rebuild_swap_chain();
	} else if (result != VK_SUCCESS) {
        wvn_ERROR("[VULKAN:BACKBUFFER|DEBUG] Failed to present swap chain image: %d", result);
    }
}

const Texture* VulkanBackbuffer::get_attachment(int idx) const
{
	return &m_colour;
}

const Texture* VulkanBackbuffer::get_depth_attachment() const
{
	return &m_depth;
}

void VulkanBackbuffer::create_swap_chain()
{
    SwapChainSupportDetails details = vkutil::query_swap_chain_support(m_backend->physical_data.device, m_surface);

    auto surf_fmt  = vkutil::choose_swap_surface_format(details.surface_formats);
    auto pres_mode = vkutil::choose_swap_present_mode(details.present_modes);
    auto extent    = vkutil::choose_swap_extent(details.capabilities);

    u32 img_count = details.capabilities.minImageCount + 1;

    if (details.capabilities.maxImageCount > 0 && img_count > details.capabilities.maxImageCount) {
        img_count = details.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = m_surface;
    create_info.minImageCount = img_count;
    create_info.imageFormat = surf_fmt.format;
    create_info.imageColorSpace = surf_fmt.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

//    u32 idxs[2] = { phys_idx.graphics_family.value(), phys_idx.present_family.value() };

//    if (phys_idx.all_unique())
//    {
//        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
//        create_info.queueFamilyIndexCount = 2;
//        create_info.pQueueFamilyIndices = idxs;
//    }
//    else
//    {
	create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	create_info.queueFamilyIndexCount = 0;
	create_info.pQueueFamilyIndices = nullptr;
//    }

    create_info.preTransform = details.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = pres_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;

    if (VkResult result = vkCreateSwapchainKHR(m_backend->device, &create_info, nullptr, &m_swap_chain); result != VK_SUCCESS) {
        wvn_ERROR("[VULKAN:BACKBUFFER|DEBUG] Failed to create swap chain: %d", result);
    }

    vkGetSwapchainImagesKHR(m_backend->device, m_swap_chain, &img_count, nullptr);

    if (!img_count) {
        wvn_ERROR("[VULKAN:BACKBUFFER|DEBUG] Failed to find any images in swap chain!");
    }

    m_swap_chain_images.resize(img_count);

    VkImage images[img_count];
    vkGetSwapchainImagesKHR(m_backend->device, m_swap_chain, &img_count, images);

    for (int i = 0; i < img_count; i++) {
        m_swap_chain_images[i] = images[i];
    }

    m_backend->swap_chain_image_format = surf_fmt.format;

	create_swap_chain_sync_objects();
	create_swap_chain_image_views();

	VkExtent2D ext = vkutil::choose_swap_extent(details.capabilities);

	m_width = ext.width;
	m_height = ext.height;

	m_render_pass_builder.make_backbuffer(this);

	ClearValues values(2);
	Colour::black().premultiplied().export_to_float(values[0].color.float32);
	values[1].depthStencil = { 1.0f, 0 };
	m_render_pass_builder.set_clear_colours(values);

	m_render_pass_builder.set_dimensions(m_width, m_height);
	m_render_pass_builder.init(m_backend);

	create_depth_resources();
	create_colour_resources();

	VkImageView attachments[] = {
		m_colour.image_view(),
		m_depth.image_view(),
		m_swap_chain_image_views[0]
	};

	m_render_pass_builder.create_render_pass(
		m_swap_chain_image_views.size(),
		wvn_ARRAY_LENGTH(attachments), attachments,
		m_swap_chain_image_views.data(), 2
	);

    dev::LogMgr::get_singleton()->print("[VULKAN:BACKBUFFER] Created the swap chain!");
}

void VulkanBackbuffer::create_swap_chain_sync_objects()
{
	VkSemaphoreCreateInfo semaphore_create_info = {};
	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	for (u32 i = 0; i < vkutil::FRAMES_IN_FLIGHT; i++)
	{
		if (VkResult result = vkCreateSemaphore(m_backend->device, &semaphore_create_info, nullptr, &m_image_available_semaphores[i]); result != VK_SUCCESS) {
			wvn_ERROR("[VULKAN:BACKBUFFER|DEBUG] Failed to create image available semaphore: %d", result);
		}

		if (VkResult result = vkCreateSemaphore(m_backend->device, &semaphore_create_info, nullptr, &m_render_finished_semaphores[i]); result != VK_SUCCESS) {
			wvn_ERROR("[VULKAN:BACKBUFFER|DEBUG] Failed to create render finished semaphore: %d", result);
		}
	}

	dev::LogMgr::get_singleton()->print("[VULKAN:BACKBUFFER] Created sync objects!");
}

void VulkanBackbuffer::create_swap_chain_image_views()
{
    m_swap_chain_image_views.resize(m_swap_chain_images.size());

    for (u64 i = 0; i < m_swap_chain_images.size(); i++)
    {
        VkImageViewCreateInfo view_info = {};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = m_swap_chain_images[i];
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = m_backend->swap_chain_image_format;

        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.layerCount = 1;

        view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        if (VkResult result = vkCreateImageView(m_backend->device, &view_info, nullptr, &m_swap_chain_image_views[i]); result != VK_SUCCESS) {
            wvn_ERROR("[VULKAN:BACKBUFFER|DEBUG] Failed to create texture image view: %d", result);
        }
    }

    dev::LogMgr::get_singleton()->print("[VULKAN:BACKBUFFER] Created swap chain image views!");
}

void VulkanBackbuffer::on_window_resize(int width, int height)
{
	if (width == 0 || height == 0) {
		return;
	}

	if (m_width == width && m_height == height) {
		return;
	}

	m_width = width;
	m_height = height;

	rebuild_swap_chain();
	acquire_next_image();
}

void VulkanBackbuffer::rebuild_swap_chain()
{
	m_backend->sync_stall();

	clean_up_swap_chain();
	create_swap_chain();

	VkImageView attachments[] = {
		m_colour.image_view(),
		m_depth.image_view(),
		m_swap_chain_image_views[0]
	};

	m_render_pass_builder.set_dimensions(m_width, m_height);

	m_render_pass_builder.create_render_pass(
		m_swap_chain_image_views.size(),
		wvn_ARRAY_LENGTH(attachments), attachments,
		m_swap_chain_image_views.data(), 2
	);
}

void VulkanBackbuffer::set_clear_colour(const Colour& colour)
{
	VkClearValue value = {};
	colour.premultiplied().export_to_float(value.color.float32);
	m_render_pass_builder.set_clear_colour(0, value);
}

void VulkanBackbuffer::set_depth_stencil_clear(float depth, u32 stencil)
{
	VkClearValue value = {};
	value.depthStencil = { depth, stencil };
	m_render_pass_builder.set_clear_colour(1, value);
}

VkSurfaceKHR VulkanBackbuffer::get_surface() const
{
	return m_surface;
}

int VulkanBackbuffer::get_current_texture_idx() const
{
	return m_curr_swap_chain_image_idx;
}

int VulkanBackbuffer::get_msaa() const
{
	return m_backend->msaa_samples;
}

const VkSemaphore& VulkanBackbuffer::get_render_finished_semaphore() const
{
	return m_render_finished_semaphores[m_backend->get_current_frame_idx()];
}

const VkSemaphore& VulkanBackbuffer::get_image_available_semaphore() const
{
	return m_image_available_semaphores[m_backend->get_current_frame_idx()];
}
