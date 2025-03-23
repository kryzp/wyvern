#include <backend/graphics/vulkan/vk_render_pass_builder.h>
#include <backend/graphics/vulkan/vk_backend.h>
#include <backend/graphics/vulkan/vk_util.h>
#include <backend/graphics/vulkan/vk_backbuffer.h>

#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

VulkanRenderPassBuilder::VulkanRenderPassBuilder()
	: m_backend()
	, m_clear_colours()
	, m_render_pass(VK_NULL_HANDLE)
	, m_attachment_descriptions()
	, m_colour_attachments{}
	, m_colour_attachment_resolves{}
	, m_depth_attachment_ref()
	, m_colour_attachment_count(0)
	, m_attachment_count(0)
	, m_width(0)
	, m_height(0)
	, m_framebuffers()
	, m_backbuffer(nullptr)
{
}

VulkanRenderPassBuilder::~VulkanRenderPassBuilder()
{
	clean_up();
}

void VulkanRenderPassBuilder::init(VulkanBackend* backend)
{
	this->m_backend = backend;
}

void VulkanRenderPassBuilder::create_colour_attachment(int idx, VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp load_op, VkImageLayout final_layout, bool is_resolve)
{
	m_attachment_descriptions[idx].format = format;
	m_attachment_descriptions[idx].samples = samples;
	m_attachment_descriptions[idx].loadOp = load_op;
	m_attachment_descriptions[idx].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	m_attachment_descriptions[idx].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // todo: we dont use the stencil buffer YET
	m_attachment_descriptions[idx].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	m_attachment_descriptions[idx].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	m_attachment_descriptions[idx].finalLayout = final_layout;

	VkAttachmentReference swap_chain_colour_attachment_ref = {};
	swap_chain_colour_attachment_ref.attachment = idx;
	swap_chain_colour_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	if (!is_resolve) {
		m_colour_attachments.push_back(swap_chain_colour_attachment_ref);
		m_colour_attachment_count++;
	} else {
		m_colour_attachment_resolves.push_back(swap_chain_colour_attachment_ref);
	}

	m_attachment_count++;
}

void VulkanRenderPassBuilder::create_depth_attachment(int idx, VkSampleCountFlagBits samples)
{
	m_attachment_descriptions[idx].format = vkutil::find_depth_format(m_backend->physical_data.device);
	m_attachment_descriptions[idx].samples = samples;
	m_attachment_descriptions[idx].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	m_attachment_descriptions[idx].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	m_attachment_descriptions[idx].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	m_attachment_descriptions[idx].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	m_attachment_descriptions[idx].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	if (m_colour_attachment_count == 0) { // want to read the depth stencil buffer instead
		m_attachment_descriptions[idx].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		m_attachment_descriptions[idx].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	} else {
		m_attachment_descriptions[idx].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}

	m_depth_attachment_ref.attachment = idx;
	m_depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	m_attachment_count++;
}

void VulkanRenderPassBuilder::create_render_pass(u64 n_framebuffers, u64 attachment_count, VkImageView* attachments, VkImageView* backbuffer_attachments, u64 backbuffer_attachment_idx)
{
	destroy_render_pass();
	destroy_fbos();

	VkSubpassDescription swap_chain_sub_pass = {};
	swap_chain_sub_pass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	swap_chain_sub_pass.pColorAttachments = m_colour_attachments.data();
	swap_chain_sub_pass.pResolveAttachments = m_colour_attachment_resolves.data();
	swap_chain_sub_pass.colorAttachmentCount = m_colour_attachment_count;
	swap_chain_sub_pass.pDepthStencilAttachment = &m_depth_attachment_ref;

	VkSubpassDependency sub_pass_dependency = {};
	sub_pass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	sub_pass_dependency.dstSubpass = 0;
	sub_pass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	sub_pass_dependency.srcAccessMask = 0;
	sub_pass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	sub_pass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo render_pass_create_info = {};
	render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_create_info.pAttachments = m_attachment_descriptions.data();
	render_pass_create_info.attachmentCount = m_attachment_count;
	render_pass_create_info.pSubpasses = &swap_chain_sub_pass;
	render_pass_create_info.subpassCount = 1;
	render_pass_create_info.pDependencies = &sub_pass_dependency;
	render_pass_create_info.dependencyCount = 1;

	if (VkResult result = vkCreateRenderPass(m_backend->device, &render_pass_create_info, nullptr, &m_render_pass); result != VK_SUCCESS) {
		wvn_ERROR("[VULKAN:RENDERPASSBUILDER|DEBUG] Failed to create render pass: %d", result);
	}

	m_framebuffers.resize(n_framebuffers);

	for (u64 n = 0; n < n_framebuffers; n++)
    {
		if (backbuffer_attachments != nullptr) {
			attachments[backbuffer_attachment_idx] = backbuffer_attachments[n];
		}

		VkFramebufferCreateInfo framebuffer_create_info = {};
		framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_create_info.renderPass = m_render_pass;
		framebuffer_create_info.pAttachments = attachments;
		framebuffer_create_info.attachmentCount = attachment_count;
		framebuffer_create_info.width = m_width;
		framebuffer_create_info.height = m_height;
		framebuffer_create_info.layers = 1;

		if (VkResult result = vkCreateFramebuffer(m_backend->device, &framebuffer_create_info, nullptr, &m_framebuffers[n]); result != VK_SUCCESS) {
			wvn_ERROR("[VULKAN:RENDERPASSBUILDER|DEBUG] Failed to create framebuffer for render pass: %d", result);
		}
	}

	dev::LogMgr::get_singleton()->print("[VULKAN:RENDERPASSBUILDER] Created render pass!");
}

void VulkanRenderPassBuilder::clean_up()
{
	destroy_render_pass();
	destroy_fbos();

	m_width = 0;
	m_height = 0;
}

void VulkanRenderPassBuilder::destroy_render_pass()
{
	if (m_render_pass != VK_NULL_HANDLE)
	{
		vkDestroyRenderPass(m_backend->device, m_render_pass, nullptr);
		m_render_pass = VK_NULL_HANDLE;
	}
}

void VulkanRenderPassBuilder::clean_up_fbos()
{
	destroy_fbos();

	m_framebuffers.clear();
	m_attachment_descriptions.clear();
	m_colour_attachments.clear();
	m_colour_attachment_resolves.clear();
	m_depth_attachment_ref = {};
	m_colour_attachment_count = 0;
	m_attachment_count = 0;
}

void VulkanRenderPassBuilder::destroy_fbos()
{
	for (auto& fbo : m_framebuffers)
	{
		if (fbo != VK_NULL_HANDLE)
		{
			vkDestroyFramebuffer(m_backend->device, fbo, nullptr);
			fbo = VK_NULL_HANDLE;
		}
	}
}

VkRenderPassBeginInfo VulkanRenderPassBuilder::build_begin_info() const
{
	u32 framebuffer_index = 0;

	if (m_backbuffer) {
		framebuffer_index = m_backbuffer->get_current_texture_idx();
	}

	VkRenderPassBeginInfo render_pass_begin_info = {};

	render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_begin_info.renderPass = m_render_pass;
	render_pass_begin_info.framebuffer = m_framebuffers[framebuffer_index % m_framebuffers.size()];
	render_pass_begin_info.renderArea.offset = { 0, 0 };
	render_pass_begin_info.renderArea.extent = { m_width, m_height };
	render_pass_begin_info.pClearValues = m_clear_colours.data();
	render_pass_begin_info.clearValueCount = m_clear_colours.size();

	return render_pass_begin_info;
}

void VulkanRenderPassBuilder::make_backbuffer(VulkanBackbuffer* backbuffer)
{
	m_backbuffer = backbuffer;
}

void VulkanRenderPassBuilder::set_clear_colour(int idx, VkClearValue value)
{
	m_clear_colours[idx] = value;
}

void VulkanRenderPassBuilder::set_clear_colours(const ClearValues& values)
{
	m_clear_colours = values;
}

VkRenderPass VulkanRenderPassBuilder::get_render_pass()
{
	return m_render_pass;
}

void VulkanRenderPassBuilder::set_dimensions(u32 width, u32 height)
{
	m_width = width;
	m_height = height;
}

int VulkanRenderPassBuilder::get_colour_attachment_count() const { return m_colour_attachment_count; }
int VulkanRenderPassBuilder::get_attachment_count() const { return m_attachment_count; }

u32 VulkanRenderPassBuilder::get_width() const { return m_width; }
u32 VulkanRenderPassBuilder::get_height() const { return m_height; }
