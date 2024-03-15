#pragma once

#include "kve_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace kve {

    class KveDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(KveDevice& kveDevice) : kveDevice{ kveDevice } {}

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<KveDescriptorSetLayout> build() const;

        private:
            KveDevice& kveDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        KveDescriptorSetLayout(
            KveDevice& kveDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~KveDescriptorSetLayout();
        KveDescriptorSetLayout(const KveDescriptorSetLayout&) = delete;
        KveDescriptorSetLayout& operator=(const KveDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        KveDevice& kveDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class KveDescriptorWriter;
    };

    class KveDescriptorPool {
    public:
        class Builder {
        public:
            Builder(KveDevice& kveDevice) : kveDevice{ kveDevice } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<KveDescriptorPool> build() const;

        private:
            KveDevice& kveDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        KveDescriptorPool(
            KveDevice& kveDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~KveDescriptorPool();
        KveDescriptorPool(const KveDescriptorPool&) = delete;
        KveDescriptorPool& operator=(const KveDescriptorPool&) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        KveDevice& kveDevice;
        VkDescriptorPool descriptorPool;

        friend class KveDescriptorWriter;
    };

    class KveDescriptorWriter {
    public:
        KveDescriptorWriter(KveDescriptorSetLayout& setLayout, KveDescriptorPool& pool);

        KveDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        KveDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        KveDescriptorSetLayout& setLayout;
        KveDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

}  // namespace Kve