#pragma once

#include "se_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace se 
{

    class SeDescriptorSetLayout 
    {
    public:
        class Builder 
        {
        public:
            Builder(SeDevice& seDevice) : seDevice{ seDevice } {}

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<SeDescriptorSetLayout> build() const;

        private:
            SeDevice& seDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        SeDescriptorSetLayout(
            SeDevice& seDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~SeDescriptorSetLayout();
        SeDescriptorSetLayout(const SeDescriptorSetLayout&) = delete;
        SeDescriptorSetLayout& operator=(const SeDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        SeDevice& seDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class SeDescriptorWriter;
    };

    class SeDescriptorPool 
    {
    public:
        class Builder 
        {
        public:
            Builder(SeDevice& seDevice) : seDevice{ seDevice } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<SeDescriptorPool> build() const;

        private:
            SeDevice& seDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        SeDescriptorPool(
            SeDevice& seDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~SeDescriptorPool();
        SeDescriptorPool(const SeDescriptorPool&) = delete;
        SeDescriptorPool& operator=(const SeDescriptorPool&) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        SeDevice& seDevice;
        VkDescriptorPool descriptorPool;

        friend class SeDescriptorWriter;
    };

    class SeDescriptorWriter 
    {
    public:
        SeDescriptorWriter(SeDescriptorSetLayout& setLayout, SeDescriptorPool& pool);

        SeDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        SeDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        SeDescriptorSetLayout& setLayout;
        SeDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

}  