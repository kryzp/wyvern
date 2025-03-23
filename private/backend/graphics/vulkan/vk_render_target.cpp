#include <backend/graphics/vulkan/vk_render_target.h>
#include <backend/graphics/vulkan/vk_backend.h>
#include <backend/graphics/vulkan/vk_util.h>

using namespace wvn;
using namespace wvn::gfx;

VulkanRenderTarget::VulkanRenderTarget(VulkanBackend* backend)
	: RenderTarget()
	, m_backend(backend)
	, m_render_pass_builder()
	, m_depth(backend)
{
	m_type = RENDER_TARGET_TYPE_TEXTURE;
}

VulkanRenderTarget::VulkanRenderTarget(VulkanBackend* backend, u32 width, u32 height)
	: RenderTarget(width, height)
	, m_backend(backend)
	, m_render_pass_builder()
	, m_depth(backend)
{
	m_type = RENDER_TARGET_TYPE_TEXTURE;
}

VulkanRenderTarget::~VulkanRenderTarget()
{
	clean_up();
}

void VulkanRenderTarget::clean_up()
{
	m_render_pass_builder.clean_up();
	m_depth.clean_up();

	// todo: should this part of cleaning-up be moved upwards into the engine top-layer away from vulkan?
	for (Texture* texture : m_attachments)
	{
		if (!texture) {
			continue;
		}

		if (texture->get_parent() == this) {
			delete texture;
		}
	}

	m_attachments.clear();
}

void VulkanRenderTarget::create()
{
	m_render_pass_builder.init(m_backend);
	m_render_pass_builder.set_dimensions(m_width, m_height);

	create_depth_resources(1);

	ClearValues values(2);
	m_render_pass_builder.set_clear_colours(values);

	set_clear_colour(Colour::black());
	set_depth_stencil_clear(1.0f, 0);

	VkImageView attachments[] = { // todo: this will bite me in the ass in the future: what if i do deferred rendering?? how will i add multiple targets here??!?!?
		m_attachments[0]->image_view(),
		m_depth.image_view()
	};

	m_render_pass_builder.create_render_pass(
		1,
		wvn_ARRAY_LENGTH(attachments), attachments,
		nullptr, 0
	);
}

void VulkanRenderTarget::create_only_depth()
{
	m_render_pass_builder.init(m_backend);
	m_render_pass_builder.set_dimensions(m_width, m_height);

	create_depth_resources(0);

	ClearValues values(1);
	m_render_pass_builder.set_clear_colours(values);

	set_depth_stencil_clear(1.0f, 0);

	VkImageView attachments[] = {
		m_depth.image_view()
	};

	m_render_pass_builder.create_render_pass(
		1,
		wvn_ARRAY_LENGTH(attachments), attachments,
		nullptr, 0
	);
}

void VulkanRenderTarget::create_depth_resources(int idx)
{
	VkFormat format = vkutil::find_depth_format(m_backend->physical_data.device);

	m_depth.init_size(m_width, m_height);
	m_depth.init_metadata(vkutil::get_wvn_texture_format(format), TEX_TILE_OPTIMAL, TEX_TYPE_2D);
	m_depth.init_sample_count(VK_SAMPLE_COUNT_1_BIT);

	m_depth.create_internal_resources();

	m_depth.transition_layout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	m_depth.set_parent(this);

	m_render_pass_builder.create_depth_attachment(idx, VK_SAMPLE_COUNT_1_BIT);
}

VulkanRenderPassBuilder* VulkanRenderTarget::get_render_pass_builder()
{
	return &m_render_pass_builder;
}

void VulkanRenderTarget::set_clear_colour(const Colour& colour) // todo: doesnt support attachments!
{
	wvn_ASSERT(m_render_pass_builder.get_colour_attachment_count() > 0, "[VULKAN:RENDERTARGET] Must have at least one colour attachment!");

	VkClearValue value = {};
	colour.premultiplied().export_to_float(value.color.float32);
	m_render_pass_builder.set_clear_colour(0, value);
}

void VulkanRenderTarget::set_depth_stencil_clear(float depth, u32 stencil)
{
	VkClearValue value = {};
	value.depthStencil = { depth, stencil };

	if (m_render_pass_builder.get_colour_attachment_count() == 0) { // depth only
		m_render_pass_builder.set_clear_colour(0, value);
	} else {
		m_render_pass_builder.set_clear_colour(1, value);
	}
}

const Texture* VulkanRenderTarget::get_attachment(int idx) const
{
	return m_attachments[idx];
}

const Texture* VulkanRenderTarget::get_depth_attachment() const
{
	return &m_depth;
}

void VulkanRenderTarget::set_attachment(int idx, VulkanTexture* tex)
{
    auto meta = tex->meta_data();

	m_attachments[idx] = tex;

    m_render_pass_builder.create_colour_attachment(
		idx,
		vkutil::get_vk_texture_format(meta.format),
		VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		false
	);
}

int VulkanRenderTarget::get_msaa() const
{
	return VK_SAMPLE_COUNT_1_BIT;
}
