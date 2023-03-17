#include <backend/graphics/vulkan/vk_texture.h>
#include <backend/graphics/vulkan/vk_util.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

VulkanTexture::VulkanTexture()
	: m_image()
	, m_view()
	, m_sampler()
	, m_width(0)
	, m_height(0)
	, m_format()
	, m_tiling()
{
}

VulkanTexture::~VulkanTexture()
{
	clean_up();
}

void VulkanTexture::init(VkDevice device, VkPhysicalDevice physical_device, VkPhysicalDeviceProperties properties)
{
	m_device = device;
	m_physical_device = physical_device;
	m_properties = properties;
}

void VulkanTexture::clean_up()
{
	m_image.clean_up(m_device);
	m_view.clean_up();
	m_sampler.clean_up();
}

void VulkanTexture::create(const Image& image)
{
	create(image.width(), image.height(), TEX_FMT_R8G8B8A8_SRGB, TEX_TILE_OPTIMAL);
}

void VulkanTexture::create(u32 width, u32 height, TextureFormat format, TextureTiling tiling)
{
	create(width, height, format, tiling, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
}

void VulkanTexture::create(u32 width, u32 height, TextureFormat format, TextureTiling tiling, VkImageUsageFlags usage, VkImageAspectFlags aspect_flags)
{
	this->m_width = width;
	this->m_height = height;

	this->m_format = format;
	this->m_tiling = tiling;

	VkFormat vkfmt = vkutil::get_vk_texture_format(format);
	VkImageTiling vktile = vkutil::get_vk_texture_tile(tiling);

	m_image.create(
		m_device, m_physical_device,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		width, height,
		vkfmt, vktile, usage
	);

	m_view.create(m_device, m_image.image(), vkfmt, aspect_flags);

	m_sampler.create(m_device, m_properties, TextureSampler(TEX_FILTER_LINEAR, TEX_WRAP_CLAMP, TEX_WRAP_CLAMP, TEX_WRAP_CLAMP));
}

VulkanImage& VulkanTexture::image() { return m_image; }
const VulkanImage& VulkanTexture::image() const { return m_image; }
VulkanImageView& VulkanTexture::view() { return m_view; }
const VulkanImageView& VulkanTexture::view() const { return m_view; }
int VulkanTexture::width() const { return m_width; }
int VulkanTexture::height() const { return m_height; }
VulkanTextureSampler& VulkanTexture::sampler() { return m_sampler; }
const VulkanTextureSampler& VulkanTexture::sampler() const { return m_sampler; }

TextureMetaData VulkanTexture::meta_data() const
{
	TextureMetaData data = {};
	data.format = m_format;
	data.tiling = m_tiling;

	return data;
}
