#include <backend/graphics/vulkan/vk_buffer.h>
#include <wvn/devenv/log_mgr.h>
#include <backend/graphics/vulkan/vk_util.h>
#include <wvn/util/types.h>

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
		dev::LogMgr::get_singleton().print("[VULKAN] Result: %d", result);
		WVN_ERROR("[VULKAN:DEBUG] Failed to create buffer.");
	}

	VkMemoryRequirements memory_requirements = {};
	vkGetBufferMemoryRequirements(m_device, m_buffer, &memory_requirements);

	VkMemoryAllocateInfo memory_allocate_info = {};
	memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_allocate_info.allocationSize = memory_requirements.size;
	memory_allocate_info.memoryTypeIndex = vkutil::find_memory_type(m_physical_device, memory_requirements.memoryTypeBits, properties);

	if (VkResult result = vkAllocateMemory(m_device, &memory_allocate_info, nullptr, &m_memory); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton().print("[VULKAN] Result: %d", result);
		WVN_ERROR("[VULKAN:DEBUG] Failed to allocate memory for buffer.");
	}

	vkBindBufferMemory(m_device, m_buffer, m_memory, 0);
}

void VulkanBuffer::clean_up()
{
	vkDestroyBuffer(m_device, m_buffer, nullptr);
	vkFreeMemory(m_device, m_memory, nullptr);
}

void VulkanBuffer::send_data(const void* data)
{
	void* mapped_data = nullptr;
	vkMapMemory(m_device, m_memory, 0, m_size, 0, &mapped_data);
	mem::copy(mapped_data, data, m_size);
	vkUnmapMemory(m_device, m_memory);
}

void VulkanBuffer::copy_to(const wvn::gfx::VulkanBuffer& other, VkCommandPool cmd_pool, VkQueue graphics_queue)
{
	VkCommandBufferAllocateInfo allocate_info = {};
	allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocate_info.commandPool = cmd_pool;
	allocate_info.commandBufferCount = 1;

	VkCommandBuffer command_buffer = {};

	if (VkResult result = vkAllocateCommandBuffers(m_device, &allocate_info, &command_buffer); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton().print("[VULKAN] Result: %d", result);
		WVN_ERROR("[VULKAN:DEBUG] Failed to allocate command buffers when copying buffer.");
	}

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (VkResult result = vkBeginCommandBuffer(command_buffer, &begin_info); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton().print("[VULKAN] Result: %d", result);
		WVN_ERROR("[VULKAN:DEBUG] Failed to begin command buffer when copying buffer.");
	}

	VkBufferCopy region = {};
	region.srcOffset = 0;
	region.dstOffset = 0;
	region.size = m_size;

	vkCmdCopyBuffer(command_buffer, m_buffer, other.m_buffer, 1, &region);

	vkEndCommandBuffer(command_buffer);

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;

	vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphics_queue);

	vkFreeCommandBuffers(m_device, cmd_pool, 1, &command_buffer);
}

VkDevice VulkanBuffer::device() const
{
	return m_device;
}

VkPhysicalDevice VulkanBuffer::physical_device() const
{
	return m_physical_device;
}

VkBuffer VulkanBuffer::buffer() const
{
	return m_buffer;
}

VkDeviceMemory VulkanBuffer::memory() const
{
	return m_memory;
}

VkDeviceSize VulkanBuffer::size() const
{
	return m_size;
}

VkBufferUsageFlags VulkanBuffer::usage() const
{
	return m_usage;
}

VkMemoryPropertyFlags VulkanBuffer::properties() const
{
	return m_properties;
}
