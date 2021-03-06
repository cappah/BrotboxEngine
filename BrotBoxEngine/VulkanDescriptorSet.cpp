#include "stdafx.h"
#include "BBE/VulkanDescriptorSet.h"
#include "BBE/VulkanDescriptorPool.h"
#include "BBE/VulkanDescriptorSetLayout.h"
#include "BBE/VulkanDevice.h"
#include "BBE/VulkanDescriptorSet.h"
#include "BBE/VulkanBuffer.h"
#include "BBE/Image.h"

void bbe::INTERNAL::vulkan::VulkanDescriptorSet::addUniformBuffer(const VulkanBuffer & buffer, VkDeviceSize offset, uint32_t binding)
{
	if (m_descriptorSet != VK_NULL_HANDLE)
	{
		throw AlreadyCreatedException();
	}

	VkDescriptorBufferInfo dbi = {};
	dbi.buffer = buffer.getBuffer();
	dbi.offset = offset;
	dbi.range = buffer.getSize();

	m_descriptorBufferInfos.add(AdvancedDescriptorBufferInfo(dbi, binding));
}

void bbe::INTERNAL::vulkan::VulkanDescriptorSet::addCombinedImageSampler(const Image& image, uint32_t binding)
{
	VkDescriptorImageInfo dii = {};
	dii.sampler = image.getSampler();
	dii.imageView = image.getImageView();
	dii.imageLayout = image.getImageLayout();
	

	m_descriptorImageInfos.add(AdvancedDescriptorImageInfo(dii, binding));
}

void bbe::INTERNAL::vulkan::VulkanDescriptorSet::create(const VulkanDevice &device, const VulkanDescriptorPool & descriptorPool, const VulkanDescriptorSetLayout & setLayout)
{
	VkDescriptorSetAllocateInfo dsai = {};
	dsai.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	dsai.pNext = nullptr;
	dsai.descriptorPool = descriptorPool.getDescriptorPool();
	dsai.descriptorSetCount = 1;
	VkDescriptorSetLayout dsl = setLayout.getDescriptorSetLayout();
	dsai.pSetLayouts = &dsl;

	vkAllocateDescriptorSets(device.getDevice(), &dsai, &m_descriptorSet);



	List<VkWriteDescriptorSet> writeDescriptorSets;
	for (int i = 0; i < m_descriptorBufferInfos.getLength(); i++)
	{
		VkWriteDescriptorSet wds = {};
		wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		wds.pNext = nullptr;
		wds.dstSet = m_descriptorSet;
		wds.dstBinding = m_descriptorBufferInfos[i].m_binding;
		wds.dstArrayElement = 0;
		wds.descriptorCount = 1;
		wds.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		wds.pImageInfo = nullptr;
		wds.pBufferInfo = &(m_descriptorBufferInfos[i].m_descriptorBufferInfo);
		wds.pTexelBufferView = nullptr;

		writeDescriptorSets.add(wds);
	}

	for (int i = 0; i < m_descriptorImageInfos.getLength(); i++)
	{
		VkWriteDescriptorSet wds = {};
		wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		wds.pNext = nullptr;
		wds.dstSet = m_descriptorSet;
		wds.dstBinding = m_descriptorImageInfos[i].m_binding;
		wds.dstArrayElement = 0;
		wds.descriptorCount = 1;
		wds.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		wds.pImageInfo = &(m_descriptorImageInfos[i].m_descriptorImageInfo);
		wds.pBufferInfo = nullptr;
		wds.pTexelBufferView = nullptr;

		writeDescriptorSets.add(wds);
	}

	vkUpdateDescriptorSets(device.getDevice(), writeDescriptorSets.getLength(), writeDescriptorSets.getRaw(), 0, nullptr);
}

VkDescriptorSet bbe::INTERNAL::vulkan::VulkanDescriptorSet::getDescriptorSet() const
{
	return m_descriptorSet;
}

const VkDescriptorSet * bbe::INTERNAL::vulkan::VulkanDescriptorSet::getPDescriptorSet() const
{
	return &m_descriptorSet;
}

bbe::INTERNAL::vulkan::AdvancedDescriptorBufferInfo::AdvancedDescriptorBufferInfo(VkDescriptorBufferInfo descriptorBufferInfo, uint32_t binding)
{
	m_descriptorBufferInfo = descriptorBufferInfo;
	m_binding = binding;
}

bbe::INTERNAL::vulkan::AdvancedDescriptorImageInfo::AdvancedDescriptorImageInfo(VkDescriptorImageInfo descriptorImageInfo, uint32_t binding)
{
	m_descriptorImageInfo = descriptorImageInfo;
	m_binding = binding;
}
