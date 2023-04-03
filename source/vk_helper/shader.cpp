#include "shader.hpp"
#include <spirv_reflect.h>
#include <utility>
#include "device.hpp"
#include "initializer.hpp"
#include "logging.hpp"
#include "utils/file_util.hpp"

namespace zen::vkh {
ShaderStage::ShaderStage(std::vector<char> code_, VkShaderModule shader_module_,
                         VkShaderStageFlagBits flag_) {
  code          = std::move(code_);
  shader_module = shader_module_;
  flag          = flag_;
}

ShaderProgram::ShaderProgram(const Device& device, std::string name)
    : m_device(device), m_name(std::move(name)) {}

ShaderProgram& ShaderProgram::add_stage(const std::string& file_name, ShaderType type) {
  auto code = util::read_file_binary_data(file_name);
  VkShaderModuleCreateInfo info{
      .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .codeSize = code.size(),
      .pCode    = reinterpret_cast<const std::uint32_t*>(code.data()),
  };
  VkShaderModule shader_model;
  m_device.create_shader_module(info, &shader_model, m_name);
  m_stages.emplace_back(std::move(code), shader_model, static_cast<VkShaderStageFlagBits>(type));
  return *this;
}

ShaderProgram& ShaderProgram::fill_stage_cis(
    std::vector<VkPipelineShaderStageCreateInfo>& pipeline_stages) {
  for (auto& stage : m_stages) {
    pipeline_stages.push_back(shader_stage_ci(stage.flag, stage.shader_module));
  }
  return *this;
}

struct DescriptorSetLayoutData {
  uint32_t set_number;
  VkDescriptorSetLayoutCreateInfo create_info;
  std::vector<VkDescriptorSetLayoutBinding> bindings;
};

ShaderProgram& ShaderProgram::reflect_layout() {
  std::vector<DescriptorSetLayoutData> set_layouts;
  std::vector<VkPushConstantRange> constant_ranges;
  for (auto& stage : m_stages) {
    SpvReflectShaderModule spv_module;
    SpvReflectResult result = spvReflectCreateShaderModule(stage.code.size() * sizeof(uint32_t),
                                                           stage.code.data(), &spv_module);
    VK_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

    uint32_t count{0};
    result = spvReflectEnumerateDescriptorSets(&spv_module, &count, nullptr);
    VK_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);
    std::vector<SpvReflectDescriptorSet*> reflect_sets(count);
    result = spvReflectEnumerateDescriptorSets(&spv_module, &count, reflect_sets.data());
    VK_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

    for (auto refl_set : reflect_sets) {

      //      const SpvReflectDescriptorSet& refl_set = *set;

      DescriptorSetLayoutData layout = {};

      layout.bindings.resize(refl_set->binding_count);
      for (uint32_t i_binding = 0; i_binding < refl_set->binding_count; ++i_binding) {
        const SpvReflectDescriptorBinding& refl_binding = *(refl_set->bindings[i_binding]);
        VkDescriptorSetLayoutBinding& layout_binding    = layout.bindings[i_binding];

        layout_binding.binding        = refl_binding.binding;
        layout_binding.descriptorType = static_cast<VkDescriptorType>(refl_binding.descriptor_type);

        layout_binding.descriptorCount = 1;
        for (uint32_t i_dim = 0; i_dim < refl_binding.array.dims_count; ++i_dim) {
          layout_binding.descriptorCount *= refl_binding.array.dims[i_dim];
        }
        layout_binding.stageFlags = static_cast<VkShaderStageFlagBits>(spv_module.shader_stage);

        ReflectedBinding reflected{};
        reflected.binding = layout_binding.binding;
        reflected.set     = refl_set->set;
        reflected.type    = layout_binding.descriptorType;

        m_reflected_bindings[refl_binding.name] = reflected;
      }
      layout.set_number               = refl_set->set;
      layout.create_info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      layout.create_info.bindingCount = refl_set->binding_count;
      layout.create_info.pBindings    = layout.bindings.data();

      set_layouts.push_back(layout);
    }

    //push constants
    result = spvReflectEnumeratePushConstantBlocks(&spv_module, &count, nullptr);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    std::vector<SpvReflectBlockVariable*> push_constants(count);
    result = spvReflectEnumeratePushConstantBlocks(&spv_module, &count, push_constants.data());
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    if (count > 0) {
      VkPushConstantRange pcs{};
      pcs.offset     = push_constants[0]->offset;
      pcs.size       = push_constants[0]->size;
      pcs.stageFlags = stage.flag;
      constant_ranges.push_back(pcs);
    }
  }
  std::array<DescriptorSetLayoutData, 4> merged_layouts;

  for (int i = 0; i < 4; i++) {

    DescriptorSetLayoutData& ly = merged_layouts[i];

    ly.set_number = i;

    ly.create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> binds;
    for (auto& layout : set_layouts) {
      if (layout.set_number == i) {
        for (auto& b : layout.bindings) {
          auto it = binds.find(b.binding);
          if (it == binds.end()) {
            binds[b.binding] = b;
            //ly.bindings.push_back(b);
          } else {
            //merge flags
            binds[b.binding].stageFlags |= b.stageFlags;
          }
        }
      }
    }
    for (auto [k, v] : binds) {
      ly.bindings.push_back(v);
    }
    //sort the bindings, for hash purposes
    std::sort(ly.bindings.begin(), ly.bindings.end(),
              [](VkDescriptorSetLayoutBinding& a, VkDescriptorSetLayoutBinding& b) {
                return a.binding < b.binding;
              });

    ly.create_info.bindingCount = (uint32_t)ly.bindings.size();
    ly.create_info.pBindings    = ly.bindings.data();
    ly.create_info.flags        = 0;
    ly.create_info.pNext        = nullptr;

    if (ly.create_info.bindingCount > 0) {
      vkCreateDescriptorSetLayout(m_device.handle(), &ly.create_info, nullptr, &m_ds_layouts[i]);
    } else {
      m_ds_layouts[i] = VK_NULL_HANDLE;
    }
  }

  //we start from just the default empty pipeline layout info
  VkPipelineLayoutCreateInfo mesh_pipeline_layout_info = pipeline_layout_ci();

  mesh_pipeline_layout_info.pPushConstantRanges    = constant_ranges.data();
  mesh_pipeline_layout_info.pushConstantRangeCount = (uint32_t)constant_ranges.size();

  std::array<VkDescriptorSetLayout, 4> compacted_layouts{};
  int s = 0;
  for (int i = 0; i < 4; i++) {
    if (m_ds_layouts[i] != VK_NULL_HANDLE) {
      compacted_layouts[s] = m_ds_layouts[i];
      s++;
    }
  }

  mesh_pipeline_layout_info.setLayoutCount = s;
  mesh_pipeline_layout_info.pSetLayouts    = compacted_layouts.data();

  vkCreatePipelineLayout(m_device.handle(), &mesh_pipeline_layout_info, nullptr,
                         &m_pipeline_layout);
  return *this;
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept : m_device(other.m_device) {
  m_name   = std::move(other.m_name);
  m_stages = std::move(other.m_stages);
}

ShaderProgram::~ShaderProgram() {
  for (auto& stage : m_stages) {
    m_device.destroy_shader_module(stage.shader_module);
  }
  for (auto& ds_layout : m_ds_layouts) {
    vkDestroyDescriptorSetLayout(m_device.handle(), ds_layout, nullptr);
  }
}

std::string VkDescriptorTypeToString(VkDescriptorType descriptorType) {
  switch (descriptorType) {
    case VK_DESCRIPTOR_TYPE_SAMPLER:
      return "VK_DESCRIPTOR_TYPE_SAMPLER";
    case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
      return "VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER";
    case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
      return "VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE";
    case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
      return "VK_DESCRIPTOR_TYPE_STORAGE_IMAGE";
    case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
      return "VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER";
    case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
      return "VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER";
    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
      return "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER";
    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
      return "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER";
    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
      return "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC";
    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
      return "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC";
    case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
      return "VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT";
    case VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK:
      return "VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK";
    case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
      return "VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR";
    case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV:
      return "VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV";
    case VK_DESCRIPTOR_TYPE_MUTABLE_VALVE:
      return "VK_DESCRIPTOR_TYPE_MUTABLE_VALVE";
    case VK_DESCRIPTOR_TYPE_MAX_ENUM:
      return "VK_DESCRIPTOR_TYPE_MAX_ENUM";
    default:
      return "Unknown VkDescriptorType value";
  }
}

void ShaderProgram::show_ds_layout_info() const {
  logger::info("Showing descriptor set layout infos for {}: ", m_name);
  logger::set_list_pattern();
  for (const auto& [name, reflected_binding] : m_reflected_bindings) {
//    std::string type = reflected_binding.type;
    logger::info("name: {}, set: {}, binding: {}, type: {}", name, reflected_binding.set,
                 reflected_binding.binding, VkDescriptorTypeToString(reflected_binding.type));
  }
  logger::set_default_pattern();
}
}  // namespace zen::vkh