#include <backend/graphics/vulkan/vk_buffer.h>
#include <backend/graphics/vulkan/vk_util.h>
#include <backend/graphics/vulkan/vk_backend.h>
#include <wvn/devenv/log_mgr.h>
#include <wvn/util/types.h>
#include <backend/graphics/vulkan/vk_texture.h>

using namespace wvn;
using namespace wvn::gfx;

VulkanBuffer::VulkanBuffer()
	: m_device(VK_NULL_HANDLE)
	, m_physical_device(VK_NULL_HANDLE)
	, m_buffer(VK_NULL_HANDLE)
	, m_memory(VK_NULL_HANDLE)
	, m_size()
	, m_usage()
	, m_properties()
{
}

VulkanBuffer::~VulkanBuffer()
{
    clean_up();
}

void VulkanBuffer::create(VkDevice device, VkPhysicalDevice physical_device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
	this->m_device = device;
	this->m_physical_device = physical_device;
	this->m_size = size;
	this->m_usage = usage;
	this->m_properties = properties;

	VkBufferCreateInfo buffer_create_info = {};
	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.size = size;
	buffer_create_info.usage = usage;
	buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (VkResult result = vkCreateBuffer(m_device, &buffer_create_info, nullptr, &m_buffer); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton()->print("[VULKAN] Result: %d", result);
		WVN_ERROR("[VULKAN|DEBUG] Failed to create buffer.");
	}

	VkMemoryRequirements memory_requirements = {};
	vkGetBufferMemoryRequirements(m_device, m_buffer, &memory_requirements);

	VkMemoryAllocateInfo memory_allocate_info = {};
	memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_allocate_info.allocationSize = memory_requirements.size;
	memory_allocate_info.memoryTypeIndex = vkutil::find_memory_type(m_physical_device, memory_requirements.memoryTypeBits, properties);

	if (VkResult result = vkAllocateMemory(m_device, &memory_allocate_info, nullptr, &m_memory); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton()->print("[VULKAN] Result: %d", result);
		WVN_ERROR("[VULKAN|DEBUG] Failed to realloc memory for buffer.");
	}

	vkBindBufferMemory(m_device, m_buffer, m_memory, 0);
}
void VulkanBuffer::clean_up()
{
    if (m_buffer == VK_NULL_HANDLE &&
        m_memory == VK_NULL_HANDLE)
    {
        return;
    }

	vkDestroyBuffer(m_device, m_buffer, nullptr);
	vkFreeMemory(m_device, m_memory, nullptr);

    m_buffer = VK_NULL_HANDLE;
    m_memory = VK_NULL_HANDLE;
}

void VulkanBuffer::send_data(const void* data)
{
	void* mapped_address = nullptr;
	vkMapMemory(m_device, m_memory, 0, m_size, 0, &mapped_address);
	mem::copy(mapped_address, data, m_size);
	vkUnmapMemory(m_device, m_memory);
}

void VulkanBuffer::copy_to(const VulkanBuffer& other, VkCommandPool cmd_pool, VkQueue graphics)
{
	VkCommandBuffer cmd_buf = vkutil::begin_single_time_commands(cmd_pool, m_device);
	{
		VkBufferCopy region = {};
		region.srcOffset = 0;
		region.dstOffset = 0;
		region.size = m_size;

		vkCmdCopyBuffer(
			cmd_buf,
			m_buffer,
			other.m_buffer,
			1,
			&region
		);
	}
	vkutil::end_single_time_commands(cmd_pool, cmd_buf, m_device, graphics);
}

void VulkanBuffer::copy_to_texture(const VulkanTexture& texture, VkCommandPool cmd_pool, VkQueue graphics)
{
	VkCommandBuffer cmd_buf = vkutil::begin_single_time_commands(cmd_pool, m_device);
	{
		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { texture.width(), texture.height(), 1 };

		vkCmdCopyBufferToImage(
			cmd_buf,
			m_buffer,
			texture.image(),
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);
	}
	vkutil::end_single_time_commands(cmd_pool, cmd_buf, m_device, graphics);
}

VkDevice VulkanBuffer::device() const { return m_device; }
VkPhysicalDevice VulkanBuffer::physical_device() const { return m_physical_device; }
VkBuffer VulkanBuffer::buffer() const { return m_buffer; }
VkDeviceMemory VulkanBuffer::memory() const { return m_memory; }
VkDeviceSize VulkanBuffer::size() const { return m_size; }
VkBufferUsageFlags VulkanBuffer::usage() const { return m_usage; }
VkMemoryPropertyFlags VulkanBuffer::properties() const { return m_properties; }
