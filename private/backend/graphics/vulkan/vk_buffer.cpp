#include <backend/graphics/vulkan/vk_buffer.h>
#include <backend/graphics/vulkan/vk_texture.h>
#include <backend/graphics/vulkan/vk_backend.h>
#include <backend/graphics/vulkan/vk_util.h>
#include <wvn/devenv/log_mgr.h>
#include <wvn/common.h>

using namespace wvn;
using namespace wvn::gfx;

VulkanBuffer::VulkanBuffer(GPUBufferUsage usage)
	: GPUBuffer(usage)
	, m_backend(nullptr)
	, m_buffer(VK_NULL_HANDLE)
	, m_memory(VK_NULL_HANDLE)
	, m_usage(usage)
	, m_properties()
{
}

VulkanBuffer::~VulkanBuffer()
{
    clean_up();
}

void VulkanBuffer::create(VulkanBackend* backend, VkMemoryPropertyFlags properties, u64 size)
{
	this->m_backend = backend;
	this->m_properties = properties;

	VkBufferCreateInfo buffer_create_info = {};
	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.size = size;
	buffer_create_info.usage = m_usage;
	buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (VkResult result = vkCreateBuffer(m_backend->device, &buffer_create_info, nullptr, &m_buffer); result != VK_SUCCESS) {
		WVN_ERROR("[VULKAN:BUFFER|DEBUG] Failed to create buffer: %d", result);
	}

	VkMemoryRequirements memory_requirements = {};
	vkGetBufferMemoryRequirements(m_backend->device, m_buffer, &memory_requirements);

	VkMemoryAllocateInfo memory_allocate_info = {};
	memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_allocate_info.allocationSize = memory_requirements.size;
	memory_allocate_info.memoryTypeIndex = vkutil::find_memory_type(m_backend->physical_data.device, memory_requirements.memoryTypeBits, properties);

	if (VkResult result = vkAllocateMemory(m_backend->device, &memory_allocate_info, nullptr, &m_memory); result != VK_SUCCESS) {
		WVN_ERROR("[VULKAN:BUFFER|DEBUG] Failed to reallocate memory for buffer: %d", result);
	}

	vkBindBufferMemory(m_backend->device, m_buffer, m_memory, 0);
}
void VulkanBuffer::clean_up()
{
    if (m_buffer == VK_NULL_HANDLE &&
        m_memory == VK_NULL_HANDLE)
    {
        return;
    }

	vkDestroyBuffer(m_backend->device, m_buffer, nullptr);
	vkFreeMemory(m_backend->device, m_memory, nullptr);

    m_buffer = VK_NULL_HANDLE;
    m_memory = VK_NULL_HANDLE;
}

void VulkanBuffer::read_data(const void* src, u64 length, u64 offset)
{
	void* dst = nullptr;
	vkMapMemory(m_backend->device, m_memory, offset, length, 0, &dst);
	mem::copy(dst, src, length);
	vkUnmapMemory(m_backend->device, m_memory);
}

void VulkanBuffer::write_data(void* dst, u64 length, u64 offset)
{
	void* src = nullptr;
	vkMapMemory(m_backend->device, m_memory, offset, length, 0, &src);
	mem::copy(dst, src, length);
	vkUnmapMemory(m_backend->device, m_memory);
}

void VulkanBuffer::write_to(const GPUBuffer* other, u64 length, u64 src_offset, u64 dst_offset)
{
	VkCommandBuffer cmd_buf = vkutil::begin_single_time_commands(m_backend->current_frame().command_pool, m_backend->device);
	{
		VkBufferCopy region = {};
		region.srcOffset = src_offset;
		region.dstOffset = dst_offset;
		region.size = length;

		vkCmdCopyBuffer(
			cmd_buf,
			m_buffer,
			static_cast<const VulkanBuffer*>(other)->m_buffer,
			1,
			&region
		);
	}
	vkutil::end_single_time_graphics_commands(m_backend, cmd_buf);
}

void VulkanBuffer::write_to_tex(const Texture* texture, u64 size, u64 offset, u32 base_array_layer)
{
	const VulkanTexture* vktex = (const VulkanTexture*)texture;

	VkCommandBuffer cmd_buf = vkutil::begin_single_time_commands(m_backend->current_frame().command_pool, m_backend->device);
	{
		VkBufferImageCopy region = {};
		region.bufferOffset = offset;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = base_array_layer;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { texture->width(), texture->height(), 1 };

		vkCmdCopyBufferToImage(
			cmd_buf,
			m_buffer,
			static_cast<const VulkanTexture*>(texture)->image(),
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);
	}
	vkutil::end_single_time_graphics_commands(m_backend, cmd_buf);
}

VkBuffer VulkanBuffer::buffer() const { return m_buffer; }
VkDeviceMemory VulkanBuffer::memory() const { return m_memory; }
GPUBufferUsage VulkanBuffer::usage() const { return m_usage; }
VkMemoryPropertyFlags VulkanBuffer::properties() const { return m_properties; }
