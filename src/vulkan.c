#include "internal.h"

#include <stdio.h>
#include <assert.h>

#define min(a, b) (a<b?a:b)
#define max(a, b) (a>b?a:b)
#define clamp(x, upper, lower) (min(upper, max(x, lower)))

VkShaderStageFlagBits vk_shader_stage(purrr_shader_type_t type) {
  switch (type) {
  case PURRR_SHADER_TYPE_VERTEX:   return VK_SHADER_STAGE_VERTEX_BIT;
  case PURRR_SHADER_TYPE_FRAGMENT: return VK_SHADER_STAGE_FRAGMENT_BIT;
  case PURRR_SHADER_TYPE_COMPUTE:  return VK_SHADER_STAGE_COMPUTE_BIT;
  case COUNT_PURRR_SHADER_TYPES:
  default: {
    assert(0 && "Unreachable");
    return 0;
  }
  }
}

VkFilter vk_filter(purrr_sampler_filter_t filter) {
  switch (filter) {
  case PURRR_SAMPLER_FILTER_NEAREST: return VK_FILTER_NEAREST;
  case PURRR_SAMPLER_FILTER_LINEAR:  return VK_FILTER_LINEAR;
  case COUNT_PURRR_SAMPLER_FILTERS:
  default: {
    assert(0 && "Unreachable");
    return 0;
  }
  }
}

VkSamplerAddressMode vk_sampler_address_mode(purrr_sampler_address_mode_t address_mode) {
  switch (address_mode) {
  case PURRR_SAMPLER_ADDRESS_MODE_REPEAT:               return VK_SAMPLER_ADDRESS_MODE_REPEAT;
  case PURRR_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT:      return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
  case PURRR_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE:        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  case PURRR_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE: return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
  case PURRR_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER:      return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
  case COUNT_PURRR_SAMPLER_ADDRESS_MODES:
  default: {
    assert(0 && "Unreachable");
    return 0;
  } break;
  }
}

VkDescriptorType vk_descriptor_type(purrr_buffer_type_t type) {
  switch (type) {
  case PURRR_BUFFER_TYPE_UNIFORM: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  case PURRR_BUFFER_TYPE_STORAGE: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  case COUNT_PURRR_BUFFER_TYPES:
  default: {
    assert(0 && "Unreachable");
    return 0;
  }
  }
}

// Data structs

typedef struct {
  VkSampler sampler;
} _purrr_sampler_data_t;

typedef struct {
  VkImage image;
  VkDeviceMemory image_memory;
  VkImageView image_view;
} _purrr_image_data_t;

typedef struct {
  VkDescriptorSet descriptor_set;
} _purrr_texture_data_t;

typedef struct {
  VkRenderPass render_pass;
} _purrr_pipeline_descriptor_data_t;

typedef struct {
  VkShaderModule shader_module;
} _purrr_shader_data_t;

typedef struct {
  VkPipeline pipeline;
  VkPipelineLayout pipeline_layout;
} _purrr_pipeline_data_t;

typedef struct {
  VkFramebuffer framebuffer;
} _purrr_render_target_data_t;

typedef struct {
  VkBuffer vertex_buffer;
  VkDeviceMemory vertex_buffer_memory;
  VkBuffer index_buffer;
  VkDeviceMemory index_buffer_memory;
} _purrr_mesh_data_t;

typedef struct {
  VkBuffer buffer;
  VkDeviceMemory buffer_memory;
  VkDescriptorSet set;
} _purrr_buffer_data_t;

typedef struct {
  VkInstance instance;
  VkSurfaceKHR surface;
  VkPhysicalDevice gpu;
  uint32_t graphics_family;
  uint32_t present_family;
  VkDevice device;
  VkQueue graphics_queue;
  VkQueue present_queue;

  VkCommandPool command_pool;

  // Swapchain
  VkSurfaceFormatKHR swapchain_format;
  VkPresentModeKHR swapchain_present_mode;
  VkExtent2D swapchain_extent;
  VkSwapchainKHR swapchain;
  VkImage *swapchain_images;
  VkImageView *swapchain_image_views;

  uint32_t image_index;
  uint32_t frame_index;
  VkCommandBuffer *render_cmd_bufs;
  VkCommandBuffer active_cmd_buf;

  VkSemaphore *image_semaphores;
  VkSemaphore *render_semaphores;
  VkFence *flight_fences;

  _purrr_render_target_t *active_render_target;
  _purrr_pipeline_t *active_pipeline;

  VkDescriptorPool descriptor_pool;
  VkDescriptorSetLayout texture_descriptor_set_layout;
  VkDescriptorSetLayout uniform_descriptor_set_layout;
  VkDescriptorSetLayout storage_descriptor_set_layout;

  VkSampler sampler;
} _purrr_renderer_data_t;



VkFormat vk_format(_purrr_renderer_data_t *data, purrr_format_t format) {
  switch (format) {
  case PURRR_FORMAT_UNDEFINED:  return VK_FORMAT_UNDEFINED;
  case PURRR_FORMAT_GRAYSCALE:  return VK_FORMAT_R8_UNORM;
  case PURRR_FORMAT_GRAY_ALPHA: return VK_FORMAT_R8G8_UNORM;
  case PURRR_FORMAT_RGBA8U:     return VK_FORMAT_R8G8B8A8_UNORM;
  case PURRR_FORMAT_RGBA8RGB:   return VK_FORMAT_R8G8B8A8_SRGB;
  case PURRR_FORMAT_BGRA8U:     return VK_FORMAT_B8G8R8A8_UNORM;
  case PURRR_FORMAT_BGRA8RGB:   return VK_FORMAT_B8G8R8A8_SRGB;
  case PURRR_FORMAT_RGBA16F:    return VK_FORMAT_R16G16B16A16_SFLOAT;
  case PURRR_FORMAT_RG32F:      return VK_FORMAT_R32G32_SFLOAT;
  case PURRR_FORMAT_RGB32F:     return VK_FORMAT_R32G32B32_SFLOAT;
  case PURRR_FORMAT_RGBA32F:    return VK_FORMAT_R32G32B32A32_SFLOAT;
  case PURRR_FORMAT_RGBA64F:    return VK_FORMAT_R64G64B64A64_SFLOAT;

  case PURRR_FORMAT_DEPTH: {
    VkFormat candidates[] = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
    for (uint32_t i = 0; i < sizeof(candidates)/sizeof(candidates[0]); ++i) {
      VkFormat format = candidates[i];
      VkFormatProperties props = {0};
      vkGetPhysicalDeviceFormatProperties(data->gpu, format, &props);

      if ((props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) return format;
    }

    return VK_FORMAT_UNDEFINED;
  } break;

  case COUNT_PURRR_FORMATS:
  default: {
    assert(0 && "Unreachable");
    return VK_FORMAT_UNDEFINED;
  }
  }
}

VkDeviceSize format_size(purrr_format_t format) {
  switch (format) {
  case PURRR_FORMAT_UNDEFINED:  return 0;
  case PURRR_FORMAT_GRAYSCALE:  return 1;
  case PURRR_FORMAT_GRAY_ALPHA: return 2;
  case PURRR_FORMAT_RGBA8U:     return 4;
  case PURRR_FORMAT_RGBA8RGB:   return 4;
  case PURRR_FORMAT_BGRA8U:     return 4;
  case PURRR_FORMAT_BGRA8RGB:   return 4;
  case PURRR_FORMAT_RGBA16F:    return 4;
  case PURRR_FORMAT_RG32F:      return 2;
  case PURRR_FORMAT_RGB32F:     return 3;
  case PURRR_FORMAT_RGBA32F:    return 4;
  case PURRR_FORMAT_RGBA64F:    return 4;
  case PURRR_FORMAT_DEPTH:      return 0; // idc

  case COUNT_PURRR_FORMATS:
  default: {
    assert(0 && "Unreachable");
    return VK_FORMAT_UNDEFINED;
  }
  }
}

purrr_format_t purrr_format(VkFormat format) {
  switch (format) {
  case VK_FORMAT_UNDEFINED:           return PURRR_FORMAT_UNDEFINED;
  case VK_FORMAT_R8G8B8A8_UNORM:      return PURRR_FORMAT_RGBA8U;
  case VK_FORMAT_R8G8B8A8_SRGB:       return PURRR_FORMAT_RGBA8RGB;
  case VK_FORMAT_B8G8R8A8_UNORM:      return PURRR_FORMAT_BGRA8U;
  case VK_FORMAT_B8G8R8A8_SRGB:       return PURRR_FORMAT_BGRA8RGB;
  case VK_FORMAT_R16G16B16A16_SFLOAT: return PURRR_FORMAT_RGBA16F;
  case VK_FORMAT_R32G32B32A32_SFLOAT: return PURRR_FORMAT_RGBA32F;
  case VK_FORMAT_R64G64B64A64_SFLOAT: return PURRR_FORMAT_RGBA64F;

  case VK_FORMAT_D32_SFLOAT:
  case VK_FORMAT_D32_SFLOAT_S8_UINT:
  case VK_FORMAT_D24_UNORM_S8_UINT:
    return PURRR_FORMAT_DEPTH;

  default: {
    assert(0 && "Unreachable");
    return PURRR_FORMAT_UNDEFINED;
  }
  }
}



bool _purrr_renderer_vulkan_find_queue_families(VkSurfaceKHR surface, VkPhysicalDevice device, uint32_t *g_out, uint32_t *p_out) {
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, VK_NULL_HANDLE);
  VkQueueFamilyProperties *queueFamilies = (VkQueueFamilyProperties*)malloc(sizeof(*queueFamilies)*queueFamilyCount);
  assert(queueFamilies);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

  uint32_t g = UINT32_MAX;
  uint32_t p = UINT32_MAX;

  for (uint32_t i = 0; i < queueFamilyCount; ++i) {
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) g = i;

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
    if (presentSupport) p = i;

    if (g != UINT32_MAX && p != UINT32_MAX) break;
  }

  if (g_out) *g_out = g;
  if (p_out) *p_out = p;

  free(queueFamilies);

  return g != UINT32_MAX
      && p != UINT32_MAX;
}

typedef struct {
  VkSurfaceCapabilitiesKHR capabilities;
  uint32_t format_count;
  VkSurfaceFormatKHR *formats;
  uint32_t present_mode_count;
  VkPresentModeKHR *present_modes;
} _purrr_renderer_vulkan_swapchain_details_t;

static _purrr_renderer_vulkan_swapchain_details_t _purrr_renderer_vulkan_request_swapchain_details(VkSurfaceKHR surface, VkPhysicalDevice device) {
  _purrr_renderer_vulkan_swapchain_details_t details = {0};

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

  { // Formats
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details.format_count, VK_NULL_HANDLE);
    if (details.format_count > 0) {
      details.formats = (VkSurfaceFormatKHR*)malloc(sizeof(*details.formats)*details.format_count);
      assert(details.formats);
      vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details.format_count, details.formats);
    }
  }

  { // Present modes
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &details.present_mode_count, VK_NULL_HANDLE);
    if (details.present_mode_count > 0) {
      details.present_modes = (VkPresentModeKHR*)malloc(sizeof(*details.present_modes)*details.present_mode_count);
      assert(details.present_modes);
      vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &details.present_mode_count, details.present_modes);
    }
  }

  return details;
}

// Utils

VkCommandBuffer _purrr_vulkan_begin_single_time(_purrr_renderer_data_t *data) {
  VkCommandBufferAllocateInfo allocInfo = {0};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = data->command_pool;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer command_buf;
  vkAllocateCommandBuffers(data->device, &allocInfo, &command_buf);

  VkCommandBufferBeginInfo beginInfo = {0};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(command_buf, &beginInfo);

  return command_buf;
}

void _purrr_vulkan_end_single_time(_purrr_renderer_data_t *data, VkCommandBuffer command_buf) {
  vkEndCommandBuffer(command_buf);

  VkSubmitInfo submit_info = {0};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &command_buf;

  vkQueueSubmit(data->graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
  vkQueueWaitIdle(data->graphics_queue);

  vkFreeCommandBuffers(data->device, data->command_pool, 1, &command_buf);
}

void _purrr_vulkan_transition_image_layout(_purrr_renderer_data_t *data, VkImage image,
                                           VkImageLayout old_layout, VkImageLayout new_layout,
                                           VkAccessFlags src_access, VkAccessFlags dst_access,
                                           VkPipelineStageFlagBits src_stage, VkPipelineStageFlagBits dst_stage) {
  VkCommandBuffer command_buf = _purrr_vulkan_begin_single_time(data);

  VkImageMemoryBarrier barrier = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    .oldLayout = old_layout,
    .newLayout = new_layout,
    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .image = image,
    .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    .subresourceRange.baseMipLevel = 0,
    .subresourceRange.levelCount = 1,
    .subresourceRange.baseArrayLayer = 0,
    .subresourceRange.layerCount = 1,
    .srcAccessMask = src_access,
    .dstAccessMask = dst_access,
  };
  if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;// | VK_IMAGE_ASPECT_STENCIL_BIT;

  vkCmdPipelineBarrier(
    command_buf,
    src_stage, dst_stage,
    0,
    0, VK_NULL_HANDLE,
    0, VK_NULL_HANDLE,
    1, &barrier
  );

  _purrr_vulkan_end_single_time(data, command_buf);
}

void _purrr_renderer_vulkan_copy_buffer(_purrr_renderer_data_t *data, VkBuffer src, VkBuffer dst, VkDeviceSize size, VkDeviceSize offset) {
  VkCommandBuffer cmd_buf = _purrr_vulkan_begin_single_time(data);

  VkBufferCopy copy_region = {
    .size = size,
    .srcOffset = 0,
    .dstOffset = offset,
  };
  vkCmdCopyBuffer(cmd_buf, src, dst, 1, &copy_region);

  _purrr_vulkan_end_single_time(data, cmd_buf);
}

void _purrr_renderer_vulkan_copy_buffer_to_image(_purrr_renderer_data_t *data, VkBuffer src, VkImage dst, uint32_t width, uint32_t height) {
  VkCommandBuffer cmd_buf = _purrr_vulkan_begin_single_time(data);

  VkBufferImageCopy region = {
    .imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    .imageSubresource.mipLevel = 0,
    .imageSubresource.baseArrayLayer = 0,
    .imageSubresource.layerCount = 1,
    .imageExtent = {
      width,
      height,
      1
    },
  };
  vkCmdCopyBufferToImage(cmd_buf, src, dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

  _purrr_vulkan_end_single_time(data, cmd_buf);
}

uint32_t _purrr_renderer_vulkan_find_memory_type(_purrr_renderer_data_t *data, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memProperties = {0};
  vkGetPhysicalDeviceMemoryProperties(data->gpu, &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
      return i;
  }

  return UINT32_MAX;
}

bool _purrr_renderer_vulkan_create_buffer(_purrr_renderer_data_t *data, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *buffer_memory) {
  VkBufferCreateInfo buffer_info = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    .size = size,
    .usage = usage,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
  };

  if (vkCreateBuffer(data->device, &buffer_info, VK_NULL_HANDLE, buffer) != VK_SUCCESS) return false;

  VkMemoryRequirements memRequirements = {0};
  vkGetBufferMemoryRequirements(data->device, *buffer, &memRequirements);

  VkMemoryAllocateInfo alloc_info = {
    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
    .allocationSize = memRequirements.size,
    .memoryTypeIndex = _purrr_renderer_vulkan_find_memory_type(data, memRequirements.memoryTypeBits, properties),
  };

  if (vkAllocateMemory(data->device, &alloc_info, VK_NULL_HANDLE, buffer_memory) != VK_SUCCESS) return false;

  vkBindBufferMemory(data->device, *buffer, *buffer_memory, 0);

  return true;
}

// sampler

bool _purrr_sampler_vulkan_init(_purrr_sampler_t *sampler) {
  if (!sampler || !sampler->renderer || !sampler->renderer->initialized) return false;
  _purrr_sampler_data_t *data = (_purrr_sampler_data_t*)malloc(sizeof(*data));
  assert(data);
  memset(data, 0, sizeof(*data));

  _purrr_renderer_data_t *renderer_data = (_purrr_renderer_data_t*)sampler->renderer->data_ptr;

  {
    VkSamplerCreateInfo sampler_info = {
      .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
      .magFilter = vk_filter(sampler->info.mag_filter),
      .minFilter = vk_filter(sampler->info.min_filter),
      .addressModeU = vk_sampler_address_mode(sampler->info.address_mode_u),
      .addressModeV = vk_sampler_address_mode(sampler->info.address_mode_v),
      .addressModeW = vk_sampler_address_mode(sampler->info.address_mode_w),
      .anisotropyEnable = VK_FALSE, // TODO:
      .maxAnisotropy = 1.0f, // TODO:
      .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
      .unnormalizedCoordinates = VK_FALSE,
      .compareEnable = VK_FALSE,
      .compareOp = VK_COMPARE_OP_ALWAYS,
      .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
      .mipLodBias = 0.0f,
      .minLod = 0.0f,
      .maxLod = 0.0f,
    };

    if (vkCreateSampler(renderer_data->device, &sampler_info, VK_NULL_HANDLE, &data->sampler) != VK_SUCCESS) goto error;
  }

  sampler->initialized = true;
  sampler->data_ptr = data;

  return true;
error:
  free(data);
  return false;
}

void _purrr_sampler_vulkan_cleanup(_purrr_sampler_t *sampler) {
  if (!sampler || !sampler->initialized) return;
  assert(sampler->renderer);
  _purrr_sampler_data_t *data = (_purrr_sampler_data_t*)sampler->data_ptr;
  _purrr_renderer_data_t *renderer_data = (_purrr_renderer_data_t*)sampler->renderer->data_ptr;
  assert(data && renderer_data);
  vkDestroySampler(renderer_data->device, data->sampler, VK_NULL_HANDLE);
}

// image

bool _purrr_image_vulkan_init(_purrr_image_t *image) {
  if (!image || !image->renderer || !image->renderer->initialized) return false;
  _purrr_image_data_t *data = (_purrr_image_data_t*)malloc(sizeof(*data));
  assert(data);
  memset(data, 0, sizeof(*data));

  _purrr_renderer_data_t *renderer_data = (_purrr_renderer_data_t*)image->renderer->data_ptr;
  assert(renderer_data);

  VkFormat format = vk_format(renderer_data, image->info.format);
  VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  VkImageAspectFlags aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT;

  bool depth = (image->info.format==PURRR_FORMAT_DEPTH);
  if (depth) {
    usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    aspect_flags = VK_IMAGE_ASPECT_DEPTH_BIT;
  }

  {
    VkImageCreateInfo create_info = {
      VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO, VK_NULL_HANDLE, 0,
      VK_IMAGE_TYPE_2D,
      format,
      (VkExtent3D){
        .width = image->info.width,
        .height = image->info.height,
        .depth = 1,
      },
      1, // TODO: Implement, I forgot what it was an am too lazy to check :p
      1,
      (VkSampleCountFlagBits)1<<image->info.sample_count,
      VK_IMAGE_TILING_OPTIMAL, // TODO: Add an option for tiling (if needed)
      (VkImageUsageFlags)(usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT),
      VK_SHARING_MODE_EXCLUSIVE,
      0, NULL,
      VK_IMAGE_LAYOUT_UNDEFINED,
    };

    if (vkCreateImage(renderer_data->device, &create_info, VK_NULL_HANDLE, &data->image) != VK_SUCCESS) goto error;
  }

  {
    VkMemoryRequirements memRequirements = {0};
    vkGetImageMemoryRequirements(renderer_data->device, data->image, &memRequirements);

    VkMemoryAllocateInfo alloc_info = {0};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = memRequirements.size;
    alloc_info.memoryTypeIndex = _purrr_renderer_vulkan_find_memory_type(renderer_data, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (alloc_info.memoryTypeIndex == UINT32_MAX) return false;

    if (vkAllocateMemory(renderer_data->device, &alloc_info, VK_NULL_HANDLE, &data->image_memory) != VK_SUCCESS) goto error;

    vkBindImageMemory(renderer_data->device, data->image, data->image_memory, 0);
  }

  {
    VkImageViewCreateInfo create_info = {
      VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, VK_NULL_HANDLE, 0,
      data->image,
      VK_IMAGE_VIEW_TYPE_2D,
      format,
      (VkComponentMapping){0},
      (VkImageSubresourceRange){
        aspect_flags,
        0,
        1,
        0,
        1,
      },
    };

    if (vkCreateImageView(renderer_data->device, &create_info, VK_NULL_HANDLE, &data->image_view) != VK_SUCCESS) goto error;
  }

  // if (depth) {
  //   _purrr_vulkan_transition_image_layout(renderer_data, data->image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
  //                                         0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
  //                                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT);
  // }

  image->initialized = true;
  image->data_ptr = data;

  return true;
error:
  free(data);
  return false;
}

void _purrr_image_vulkan_cleanup(_purrr_image_t *image) {
  if (!image || !image->initialized) return;
  assert(image->renderer);
  _purrr_image_data_t *data = (_purrr_image_data_t*)image->data_ptr;
  _purrr_renderer_data_t *renderer_data = (_purrr_renderer_data_t*)image->renderer->data_ptr;
  assert(data && renderer_data);
  vkDestroyImageView(renderer_data->device, data->image_view, VK_NULL_HANDLE);
  vkFreeMemory(renderer_data->device, data->image_memory, VK_NULL_HANDLE);
  vkDestroyImage(renderer_data->device, data->image, VK_NULL_HANDLE);
}

bool _purrr_image_vulkan_load(_purrr_image_t *dst, uint8_t *src, uint32_t src_width, uint32_t src_height) {
  if (!dst || !dst->initialized || !dst->renderer || !dst->renderer->initialized) return false;
  _purrr_image_data_t *data = (_purrr_image_data_t*)dst->data_ptr;
  _purrr_renderer_data_t *renderer_data = (_purrr_renderer_data_t*)dst->renderer->data_ptr;
  assert(data && renderer_data);
  if (dst->info.width < src_width || dst->info.height < src_height) return false;

  VkDeviceSize size = src_width*src_height*format_size(dst->info.format);

  VkBuffer staging_buffer;
  VkDeviceMemory staging_buffer_memory;
  if (!_purrr_renderer_vulkan_create_buffer(renderer_data, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &staging_buffer, &staging_buffer_memory)) return false;

  void* buffer_data;
  vkMapMemory(renderer_data->device, staging_buffer_memory, 0, size, 0, &buffer_data);
    memcpy(buffer_data, src, (size_t)size);
  vkUnmapMemory(renderer_data->device, staging_buffer_memory);

  _purrr_vulkan_transition_image_layout(renderer_data, data->image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
  _purrr_renderer_vulkan_copy_buffer_to_image(renderer_data, staging_buffer, data->image, src_width, src_height);
  _purrr_vulkan_transition_image_layout(renderer_data, data->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

  vkDestroyBuffer(renderer_data->device, staging_buffer, VK_NULL_HANDLE);
  vkFreeMemory(renderer_data->device, staging_buffer_memory, VK_NULL_HANDLE);

  return true;
}

bool _purrr_image_vulkan_copy(_purrr_image_t *dst, _purrr_image_t *src, uint32_t src_width, uint32_t src_height) {
  (void)dst;
  (void)src;
  (void)src_width;
  (void)src_height;
  return true;
}

// texture

bool _purrr_texture_vulkan_init(_purrr_texture_t *texture) {
  if (!texture || !texture->renderer || !texture->renderer->initialized) return false;
  _purrr_texture_data_t *data = (_purrr_texture_data_t*)malloc(sizeof(*data));
  assert(data);
  memset(data, 0, sizeof(*data));

  _purrr_image_data_t *image_data = (_purrr_image_data_t*)((_purrr_image_t*)texture->info.image)->data_ptr;

  _purrr_renderer_data_t *renderer_data = (_purrr_renderer_data_t*)texture->renderer->data_ptr;
  _purrr_sampler_data_t *sampler_data = (_purrr_sampler_data_t*)((_purrr_sampler_t*)texture->info.sampler)->data_ptr;
  assert(renderer_data && sampler_data);

  {
    VkDescriptorSetAllocateInfo alloc_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
      .descriptorPool = renderer_data->descriptor_pool,
      .descriptorSetCount = 1,
      .pSetLayouts = &renderer_data->texture_descriptor_set_layout,
    };

    if (vkAllocateDescriptorSets(renderer_data->device, &alloc_info, &data->descriptor_set) != VK_SUCCESS) return false;
  }

  {
    VkDescriptorImageInfo texture_info = {
      .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
      .imageView = image_data->image_view,
      .sampler = sampler_data->sampler,
    };

    VkWriteDescriptorSet descriptor_write = {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .dstSet = data->descriptor_set,
      .dstBinding = 0,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .descriptorCount = 1,
      .pImageInfo = &texture_info,
    };

    vkUpdateDescriptorSets(renderer_data->device, (uint32_t)1, &descriptor_write, 0, VK_NULL_HANDLE);
  }

  texture->initialized = true;
  texture->data_ptr = data;

  return true;
error:
  free(data);
  return false;
}

void _purrr_texture_vulkan_cleanup(_purrr_texture_t *texture) {
  if (!texture || !texture->initialized) return;
  assert(texture->renderer);
  _purrr_texture_data_t *data = (_purrr_texture_data_t*)texture->data_ptr;
  _purrr_renderer_data_t *renderer_data = (_purrr_renderer_data_t*)texture->renderer->data_ptr;
  assert(data && renderer_data);

}

// pipeline descriptor

bool _purrr_pipeline_descriptor_vulkan_init(_purrr_pipeline_descriptor_t *pipeline_descriptor) {
  if (!pipeline_descriptor || !pipeline_descriptor->renderer || !pipeline_descriptor->renderer->initialized) return false;

  _purrr_pipeline_descriptor_data_t *data = (_purrr_pipeline_descriptor_data_t*)malloc(sizeof(*data));
  _purrr_renderer_data_t *renderer_data = (_purrr_renderer_data_t*)pipeline_descriptor->renderer->data_ptr;
  assert(data && renderer_data);

  {
    uint8_t resolve_multiplier = pipeline_descriptor->info.resolve_attachments?2:1;
    uint32_t attachment_count = (pipeline_descriptor->info.color_attachment_count*resolve_multiplier)+(pipeline_descriptor->info.depth_attachment?1:0);
    VkAttachmentDescription *attachments = (VkAttachmentDescription*)malloc(sizeof(*attachments) * attachment_count);
    assert(attachments);
    VkAttachmentReference *references = (VkAttachmentReference*)malloc(sizeof(*references) * pipeline_descriptor->info.color_attachment_count * resolve_multiplier);
    assert(references);

    bool depth = false;
    VkAttachmentReference depth_reference = {0};

    uint32_t i = 0;
    for (; i < pipeline_descriptor->info.color_attachment_count; ++i) {
      purrr_pipeline_descriptor_attachment_info_t attachment_info = pipeline_descriptor->info.color_attachments[i];
      attachments[i] = (VkAttachmentDescription){
        0,
        vk_format(renderer_data, attachment_info.format),
        (VkSampleCountFlagBits)1<<attachment_info.sample_count,
        (attachment_info.load?VK_ATTACHMENT_LOAD_OP_LOAD:VK_ATTACHMENT_LOAD_OP_CLEAR),
        (attachment_info.store?VK_ATTACHMENT_STORE_OP_STORE:VK_ATTACHMENT_STORE_OP_DONT_CARE),
        VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED,
        (attachment_info.present_src?VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:((resolve_multiplier>1)?VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)),
      };

      references[i] = (VkAttachmentReference){
        i, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
      };
    }

    if (pipeline_descriptor->info.resolve_attachments) {
      for (uint32_t j = 0; j < pipeline_descriptor->info.color_attachment_count; ++j, ++i) {
        purrr_pipeline_descriptor_attachment_info_t attachment_info = pipeline_descriptor->info.resolve_attachments[j];
        attachments[i] = (VkAttachmentDescription){
          0,
          vk_format(renderer_data, attachment_info.format),
          (VkSampleCountFlagBits)1<<attachment_info.sample_count,
          (attachment_info.load?VK_ATTACHMENT_LOAD_OP_LOAD:VK_ATTACHMENT_LOAD_OP_CLEAR),
          (attachment_info.store?VK_ATTACHMENT_STORE_OP_STORE:VK_ATTACHMENT_STORE_OP_DONT_CARE),
          VK_ATTACHMENT_LOAD_OP_DONT_CARE,
          VK_ATTACHMENT_STORE_OP_DONT_CARE,
          VK_IMAGE_LAYOUT_UNDEFINED,
          (attachment_info.present_src?VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL),
        };

        references[i] = (VkAttachmentReference){
          i, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        };
      }
    }

    if (pipeline_descriptor->info.depth_attachment) {
      depth_reference.attachment = pipeline_descriptor->info.color_attachment_count;
      depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

      purrr_pipeline_descriptor_attachment_info_t attachment_info = *pipeline_descriptor->info.depth_attachment;
      attachments[depth_reference.attachment] = (VkAttachmentDescription){
        0,
        vk_format(renderer_data, attachment_info.format),
        (VkSampleCountFlagBits)1<<attachment_info.sample_count,
        (attachment_info.load?VK_ATTACHMENT_LOAD_OP_LOAD:VK_ATTACHMENT_LOAD_OP_CLEAR),
        (attachment_info.store?VK_ATTACHMENT_STORE_OP_STORE:VK_ATTACHMENT_STORE_OP_DONT_CARE),
        VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
      };
      depth = true;
    }

    VkSubpassDescription subpass = {
      .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
      .colorAttachmentCount = pipeline_descriptor->info.color_attachment_count,
      .pColorAttachments = references,
      .pResolveAttachments = references+pipeline_descriptor->info.color_attachment_count,
      .pDepthStencilAttachment = (depth?&depth_reference:NULL),
    };

    VkSubpassDependency dependency = {
      .srcSubpass = VK_SUBPASS_EXTERNAL,
      .dstSubpass = 0,
      .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      .srcAccessMask = 0,
      .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    };

    if (depth) {
      dependency.srcStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
      dependency.dstStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
      dependency.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }

    VkRenderPassCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      .attachmentCount = attachment_count,
      .pAttachments = attachments,
      .subpassCount = 1,
      .pSubpasses = &subpass,
      .dependencyCount = 1,
      .pDependencies = &dependency,
    };

    if (vkCreateRenderPass(renderer_data->device, &create_info, VK_NULL_HANDLE, &data->render_pass) != VK_SUCCESS) goto error;
  }

  pipeline_descriptor->data_ptr = data;
  pipeline_descriptor->initialized = true;

  return true;
error:
  free(data);
  return false;
}

void _purrr_pipeline_descriptor_vulkan_cleanup(_purrr_pipeline_descriptor_t *pipeline_descriptor) {
  _purrr_pipeline_descriptor_data_t *data = (_purrr_pipeline_descriptor_data_t*)pipeline_descriptor->data_ptr;
  _purrr_renderer_data_t *renderer_data = (_purrr_renderer_data_t*)pipeline_descriptor->renderer->data_ptr;
  if (!data || !renderer_data) return;
  if (pipeline_descriptor->initialized) {
    vkDestroyRenderPass(renderer_data->device, data->render_pass, VK_NULL_HANDLE);
  }
  free(data);
  pipeline_descriptor->initialized = false;
}

// shader

bool _purrr_shader_vulkan_init(_purrr_shader_t *shader) {
  if (!shader || !shader->renderer || !shader->renderer->initialized) return false;

  _purrr_shader_data_t *data = (_purrr_shader_data_t*)malloc(sizeof(*data));
  _purrr_renderer_data_t *renderer_data = (_purrr_renderer_data_t*)shader->renderer->data_ptr;
  assert(data && renderer_data);
  memset(data, 0, sizeof(*data));

  if (shader->info.filename) {
    if (shader->info.buffer) free(shader->info.buffer);
    FILE *fd = fopen(shader->info.filename, "rb");
    if (!fd) return VK_NULL_HANDLE;
    fseek(fd, 0, SEEK_END);
    shader->info.buffer_size = (size_t)ftell(fd);
    fseek(fd, 0, SEEK_SET);
    shader->info.buffer = malloc(shader->info.buffer_size);
    assert(shader->info.buffer);
    if ((shader->info.buffer_size % 4) != 0 || !shader->info.buffer ||
        (fread(shader->info.buffer, shader->info.buffer_size, 1, fd) != 1)) {
      fclose(fd);
      free(shader->info.buffer);
      return false;
    }
    fclose(fd);
  }

  VkShaderModuleCreateInfo create_info = {
    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    .codeSize = shader->info.buffer_size,
    .pCode = (const uint32_t*)shader->info.buffer,
  };

  VkResult result = vkCreateShaderModule(renderer_data->device, &create_info, VK_NULL_HANDLE, &data->shader_module);
  if (result != VK_SUCCESS) {
    assert(false);
  }

  if (shader->info.filename) free(shader->info.buffer);

  shader->type = shader->info.type;

  shader->initialized = true;
  shader->data_ptr = data;

  return true;
}

void _purrr_shader_vulkan_cleanup(_purrr_shader_t *shader) {
  _purrr_shader_data_t *data = (_purrr_shader_data_t*)shader->data_ptr;
  _purrr_renderer_data_t *renderer_data = (_purrr_renderer_data_t*)shader->renderer->data_ptr;
  if (!data || !renderer_data) return;
  if (shader->initialized) vkDestroyShaderModule(renderer_data->device, data->shader_module, VK_NULL_HANDLE);
  free(data);
  shader->initialized = false;
}

// pipeline

bool _purrr_pipeline_vulkan_init(_purrr_pipeline_t *pipeline) {
  if (!pipeline || !pipeline->renderer || !pipeline->renderer->initialized) return false;

  _purrr_pipeline_data_t *data = (_purrr_pipeline_data_t*)malloc(sizeof(*data));
  _purrr_renderer_data_t *renderer_data = (_purrr_renderer_data_t*)pipeline->renderer->data_ptr;
  _purrr_pipeline_descriptor_data_t *pipeline_descriptor_data = (_purrr_pipeline_descriptor_data_t*)((_purrr_pipeline_descriptor_t*)pipeline->info.pipeline_descriptor)->data_ptr;
  assert(data && renderer_data && pipeline_descriptor_data);
  memset(data, 0, sizeof(*data));

  VkPipelineShaderStageCreateInfo *stage_infos = (VkPipelineShaderStageCreateInfo*)malloc(sizeof(*stage_infos)*pipeline->info.shader_count);
  assert(stage_infos);
  for (uint32_t i = 0; i < pipeline->info.shader_count; ++i) {
    _purrr_shader_t *shader = (_purrr_shader_t*)pipeline->info.shaders[i];
    assert(shader->initialized);

    stage_infos[i] = (VkPipelineShaderStageCreateInfo){
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = vk_shader_stage(shader->type),
      .module = ((_purrr_shader_data_t*)shader->data_ptr)->shader_module,
      .pName = "main",
    };
  }

  VkDynamicState dynamic_states[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

  VkPipelineDynamicStateCreateInfo dynamic_state = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
    .dynamicStateCount = 2,
    .pDynamicStates = dynamic_states,
  };

  uint32_t vertex_attrib_count = pipeline->info.mesh_info.vertex_info_count;
  VkPipelineVertexInputStateCreateInfo vertex_input_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    .vertexBindingDescriptionCount = 0,
    .pVertexBindingDescriptions = VK_NULL_HANDLE,
    .vertexAttributeDescriptionCount = vertex_attrib_count,
    .pVertexAttributeDescriptions = VK_NULL_HANDLE,
  };

  VkVertexInputAttributeDescription *vertex_attributes = (vertex_attrib_count>0?(VkVertexInputAttributeDescription*)malloc(sizeof(*vertex_attributes)*vertex_attrib_count):VK_NULL_HANDLE);
  assert(vertex_attributes);
  uint32_t vertex_size = 0;
  for (uint32_t i = 0; i < vertex_attrib_count; ++i) {
    purrr_vertex_info_t info = pipeline->info.mesh_info.vertex_infos[i];
    vertex_size += info.size;
    vertex_attributes[i].location = i;
    vertex_attributes[i].binding = 0;
    vertex_attributes[i].format = vk_format(renderer_data, info.format);
    vertex_attributes[i].offset = info.offset;
  }

  VkVertexInputBindingDescription binding_description = {
    .binding = 0,
    .stride = vertex_size,
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
  };

  if (vertex_attrib_count > 0) {
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = &binding_description;
    vertex_input_info.pVertexAttributeDescriptions = vertex_attributes;
    assert(vertex_input_info.pVertexAttributeDescriptions);
  }

  VkPipelineInputAssemblyStateCreateInfo input_assembly = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
    .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    .primitiveRestartEnable = VK_FALSE,
  };

  VkPipelineViewportStateCreateInfo viewport_state = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    .viewportCount = 1,
    .scissorCount = 1,
  };

  VkPipelineRasterizationStateCreateInfo rasterizer = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    .depthClampEnable = VK_FALSE,
    .rasterizerDiscardEnable = VK_FALSE,
    .polygonMode = VK_POLYGON_MODE_FILL,
    .lineWidth = 1.0f,
    .cullMode = VK_CULL_MODE_BACK_BIT,
    .frontFace = VK_FRONT_FACE_CLOCKWISE,
    .depthBiasEnable = VK_FALSE,
  };

  VkPipelineMultisampleStateCreateInfo multisampling = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    .sampleShadingEnable = VK_FALSE,
    .rasterizationSamples = (VkSampleCountFlagBits)1<<pipeline->info.sample_count,
  };

  VkPipelineColorBlendAttachmentState color_blend_attachment = {
    .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    .blendEnable = VK_TRUE,
    .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
    .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    .colorBlendOp = VK_BLEND_OP_ADD,
    .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
    .alphaBlendOp = VK_BLEND_OP_ADD,
  };

  VkPipelineColorBlendStateCreateInfo color_blending = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    .logicOpEnable = VK_FALSE,
    .attachmentCount = 1,
    .pAttachments = &color_blend_attachment,
  };

  VkPipelineDepthStencilStateCreateInfo depth_stencil = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
    .depthTestEnable = VK_TRUE,
    .depthWriteEnable = VK_TRUE,
    .depthCompareOp = VK_COMPARE_OP_LESS,
    .depthBoundsTestEnable = VK_FALSE,
    .minDepthBounds = 0.0f,
    .maxDepthBounds = 1.0f,
    .stencilTestEnable = VK_FALSE,
  };

  VkDescriptorSetLayout *layouts = (VkDescriptorSetLayout*)malloc(sizeof(*layouts)*pipeline->info.descriptor_slot_count);
  assert(layouts);
  for (uint32_t i = 0; i < pipeline->info.descriptor_slot_count; ++i) {
    VkDescriptorSetLayout layout = VK_NULL_HANDLE;
    switch (pipeline->info.descriptor_slots[i]) {
    case PURRR_DESCRIPTOR_TYPE_TEXTURE:
      layout = renderer_data->texture_descriptor_set_layout;
      break;
    case PURRR_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
      layout = renderer_data->uniform_descriptor_set_layout;
      break;
    case PURRR_DESCRIPTOR_TYPE_STORAGE_BUFFER:
      layout = renderer_data->storage_descriptor_set_layout;
      break;
    case COUNT_PURRR_DESCRIPTOR_TYPES: {
      assert(0 && "Unreachable");
      return false;
    }
    }
    layouts[i] = layout;
  }

  VkPushConstantRange *pc_ranges = (VkPushConstantRange*)malloc(sizeof(*pc_ranges)*pipeline->info.push_constant_count);
  assert(pc_ranges);
  for (size_t i = 0; i < pipeline->info.push_constant_count; ++i) {
    pc_ranges[i] = (VkPushConstantRange){
      .stageFlags = VK_SHADER_STAGE_ALL,
      .offset = pipeline->info.push_constants[i].offset,
      .size = pipeline->info.push_constants[i].size,
    };
  }

  VkPipelineLayoutCreateInfo pipeline_layout_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    .pSetLayouts = layouts,
    .setLayoutCount = pipeline->info.descriptor_slot_count,
    .pPushConstantRanges = pc_ranges,
    .pushConstantRangeCount = pipeline->info.push_constant_count,
  };

  if (vkCreatePipelineLayout(renderer_data->device, &pipeline_layout_info, VK_NULL_HANDLE, &data->pipeline_layout) != VK_SUCCESS) return false;

  free(pc_ranges);
  free(layouts);

  VkGraphicsPipelineCreateInfo pipeline_info = {
    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    .stageCount = pipeline->info.shader_count,
    .pStages = stage_infos,
    .pVertexInputState = &vertex_input_info,
    .pInputAssemblyState = &input_assembly,
    .pViewportState = &viewport_state,
    .pRasterizationState = &rasterizer,
    .pMultisampleState = &multisampling,
    .pColorBlendState = &color_blending,
    .pDynamicState = &dynamic_state,
    .layout = data->pipeline_layout,
    .renderPass = pipeline_descriptor_data->render_pass,
    .subpass = 0,
  };
  if (((_purrr_pipeline_descriptor_t*)pipeline->info.pipeline_descriptor)->info.depth_attachment) pipeline_info.pDepthStencilState = &depth_stencil;

  if (vkCreateGraphicsPipelines(renderer_data->device, VK_NULL_HANDLE, 1, &pipeline_info, VK_NULL_HANDLE, &data->pipeline) != VK_SUCCESS) return false;

  free(stage_infos);

  pipeline->data_ptr = data;
  pipeline->initialized = true;

  return true;
}

void _purrr_pipeline_vulkan_cleanup(_purrr_pipeline_t *pipeline) {
  _purrr_pipeline_data_t *data = (_purrr_pipeline_data_t*)pipeline->data_ptr;
  _purrr_renderer_data_t *renderer_data = (_purrr_renderer_data_t*)pipeline->renderer->data_ptr;
  if (!data || !renderer_data) return;
  if (pipeline->initialized) {
    vkDestroyPipeline(renderer_data->device, data->pipeline, VK_NULL_HANDLE);
    vkDestroyPipelineLayout(renderer_data->device, data->pipeline_layout, VK_NULL_HANDLE);
  }
  free(data);
  pipeline->initialized = false;
}

// render target

bool _purrr_render_target_vulkan_init(_purrr_render_target_t *render_target) {
  if (!render_target || !render_target->renderer || !render_target->renderer->initialized) return false;
  _purrr_renderer_data_t *renderer_data = (_purrr_renderer_data_t*)render_target->renderer->data_ptr;
  _purrr_render_target_data_t *data = (_purrr_render_target_data_t*)malloc(sizeof(*data));
  assert(data && renderer_data);
  memset(data, 0, sizeof(*data));

  _purrr_pipeline_descriptor_t *pipeline_descriptor = (_purrr_pipeline_descriptor_t*)render_target->descriptor;
  if (!pipeline_descriptor || !pipeline_descriptor->initialized || pipeline_descriptor->info.color_attachment_count == 0 || !pipeline_descriptor->info.color_attachments) return false;
  _purrr_pipeline_descriptor_data_t *pipeline_descriptor_data = (_purrr_pipeline_descriptor_data_t*)pipeline_descriptor->data_ptr;
  assert(pipeline_descriptor_data);

  render_target->image_count = pipeline_descriptor->info.color_attachment_count*(pipeline_descriptor->info.resolve_attachments?2:1)+(pipeline_descriptor->info.depth_attachment?1:0);

  VkImageView *views = (VkImageView*)malloc(sizeof(*views)*render_target->image_count);
  assert(views);

  if (render_target->info.images) {
    for (uint32_t i = 0; i < render_target->image_count; ++i) {
      purrr_image_t *image = render_target->info.images[i];
      assert(image);
      views[i] = ((_purrr_image_data_t*)(((_purrr_image_t*)image)->data_ptr))->image_view;
    }
  } else {
    render_target->images = (_purrr_image_t**)malloc(sizeof(render_target->images)*render_target->image_count);
    assert(render_target->images);

    uint32_t i = 0;
    for (; i < pipeline_descriptor->info.color_attachment_count; ++i) {
      purrr_pipeline_descriptor_attachment_info_t attachment_info = pipeline_descriptor->info.color_attachments[i];
      purrr_image_info_t info = {
        .width = render_target->width,
        .height = render_target->height,
        .format = attachment_info.format,
        .sample_count = attachment_info.sample_count,
      };

      _purrr_image_t *image = (_purrr_image_t*)purrr_image_create(&info, (purrr_renderer_t*)render_target->renderer);
      if (!image) return false;
      render_target->images[i] = image;
      views[i] = ((_purrr_image_data_t*)image->data_ptr)->image_view;
    }
    if (pipeline_descriptor->info.resolve_attachments) {
      for (uint32_t j = 0; j < pipeline_descriptor->info.color_attachment_count; ++j, ++i) {
        purrr_pipeline_descriptor_attachment_info_t attachment_info = pipeline_descriptor->info.resolve_attachments[i];
        purrr_image_info_t info = {
          .width = render_target->width,
          .height = render_target->height,
          .format = attachment_info.format,
        };

        _purrr_image_t *image = (_purrr_image_t*)purrr_image_create(&info, (purrr_renderer_t*)render_target->renderer);
        if (!image) return false;
        render_target->images[i] = image;
        views[i] = ((_purrr_image_data_t*)image->data_ptr)->image_view;
      }
    }
    if (i < render_target->image_count) { // depth I think
      purrr_pipeline_descriptor_attachment_info_t attachment_info = *pipeline_descriptor->info.depth_attachment;
      purrr_image_info_t info = {
        .width = render_target->width,
        .height = render_target->height,
        .format = attachment_info.format,
      };

      _purrr_image_t *image = (_purrr_image_t*)purrr_image_create(&info, (purrr_renderer_t*)render_target->renderer);
      if (!image) return false;
      render_target->images[i] = image;
      views[i++] = ((_purrr_image_data_t*)image->data_ptr)->image_view;
    }
    assert(i == render_target->image_count);
  }

  {
    VkFramebufferCreateInfo framebuffer_info = {
      .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .renderPass = pipeline_descriptor_data->render_pass,
      .attachmentCount = render_target->image_count,
      .pAttachments = views,
      .width = render_target->width,
      .height = render_target->height,
      .layers = 1,
    };

    if (vkCreateFramebuffer(renderer_data->device, &framebuffer_info, VK_NULL_HANDLE, &data->framebuffer) != VK_SUCCESS) return false;
  }

  render_target->data_ptr = data;

  return true;
}

void _purrr_render_target_vulkan_cleanup(_purrr_render_target_t *render_target) {
  _purrr_render_target_data_t *data = (_purrr_render_target_data_t*)render_target->data_ptr;
  _purrr_renderer_data_t *renderer_data = (_purrr_renderer_data_t*)render_target->renderer->data_ptr;
  if (!data || !renderer_data) return;
  if (render_target->initialized) {
    vkDestroyFramebuffer(renderer_data->device, data->framebuffer, VK_NULL_HANDLE);
    for (uint32_t i = 0; i < render_target->image_count && render_target->images; ++i)
      _purrr_image_free(render_target->images[i]);
  }
  free(data);
  render_target->initialized = false;
  render_target->image_count = 0;
}

_purrr_image_t *_purrr_render_target_vulkan_get_image(_purrr_render_target_t *render_target, uint32_t image_index) {
  if (!render_target || !render_target->initialized ||
      !render_target->images || image_index >= render_target->image_count)
    return NULL;
  return render_target->images[image_index];
}

// buffer

bool _purrr_buffer_vulkan_init(_purrr_buffer_t *buffer) {
  if (!buffer) return false;
  _purrr_renderer_data_t *renderer_data = (_purrr_renderer_data_t*)buffer->renderer->data_ptr;
  _purrr_buffer_data_t *data = (_purrr_buffer_data_t*)malloc(sizeof(*data));
  assert(data && renderer_data);
  memset(data, 0, sizeof(*data));

  VkBufferUsageFlagBits usage = 0;
  VkDescriptorSetLayout layout = VK_NULL_HANDLE;
  switch (buffer->info.type) {
  case PURRR_BUFFER_TYPE_UNIFORM:
    usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    layout = renderer_data->uniform_descriptor_set_layout;
    break;
  case PURRR_BUFFER_TYPE_STORAGE:
    usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    layout = renderer_data->storage_descriptor_set_layout;
    break;
  case PURRR_BUFFER_TYPE_VERTEX:
    usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    break;
  case PURRR_BUFFER_TYPE_INDEX:
    usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    break;
  case COUNT_PURRR_BUFFER_TYPES:
  default: {
    assert(0 && "Unreachable");
    return false;
  }
  }

  purrr_buffer_info_t info = buffer->info;
  if (!_purrr_renderer_vulkan_create_buffer(renderer_data, info.size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &data->buffer, &data->buffer_memory)) return false;

  if (!layout) goto defer;

  VkDescriptorSetAllocateInfo alloc_info = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
    .descriptorPool = renderer_data->descriptor_pool,
    .descriptorSetCount = 1,
    .pSetLayouts = &layout,
  };

  if (vkAllocateDescriptorSets(renderer_data->device, &alloc_info, &data->set) != VK_SUCCESS) return false;

  VkDescriptorBufferInfo buffer_info = {
    .buffer = data->buffer,
    .offset = 0,
    .range = buffer->info.size,
  };

  VkWriteDescriptorSet descriptor_write = {
    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    .dstSet = data->set,
    .dstBinding = 0,
    .dstArrayElement = 0,
    .descriptorType = vk_descriptor_type(buffer->info.type),
    .descriptorCount = 1,
    .pBufferInfo = &buffer_info,
  };

  vkUpdateDescriptorSets(renderer_data->device, (uint32_t)1, &descriptor_write, 0, VK_NULL_HANDLE);

defer:
  buffer->data_ptr = data;

  return true;
}

void _purrr_buffer_vulkan_cleanup(_purrr_buffer_t *buffer) {
  if (!buffer) return;
  _purrr_buffer_data_t *data = (_purrr_buffer_data_t*)buffer->data_ptr;
  _purrr_renderer_data_t *renderer_data = (_purrr_renderer_data_t*)buffer->renderer->data_ptr;
  if (!data || !renderer_data) return;
  if (buffer->initialized) {
    vkDestroyBuffer(renderer_data->device, data->buffer, VK_NULL_HANDLE);
    vkFreeMemory(renderer_data->device, data->buffer_memory, VK_NULL_HANDLE);
  }
  free(data);
  buffer->initialized = false;
}

bool _purrr_buffer_vulkan_copy(_purrr_buffer_t *buffer, void *in_data, uint32_t size, uint32_t offset) {
  if (!buffer || !buffer->initialized || !in_data) return false;
  assert(size <= buffer->info.size);
  _purrr_buffer_data_t *data = (_purrr_buffer_data_t*)buffer->data_ptr;
  _purrr_renderer_data_t *renderer_data = (_purrr_renderer_data_t*)buffer->renderer->data_ptr;
  if (!data || !renderer_data) return false;

  VkBuffer staging_buffer;
  VkDeviceMemory staging_buffer_memory;
  if (!_purrr_renderer_vulkan_create_buffer(renderer_data, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &staging_buffer, &staging_buffer_memory)) return false;

  void* buffer_data;
  vkMapMemory(renderer_data->device, staging_buffer_memory, 0, size, 0, &buffer_data);
    memcpy(buffer_data, in_data, (size_t)size);
  vkUnmapMemory(renderer_data->device, staging_buffer_memory);

  _purrr_renderer_vulkan_copy_buffer(renderer_data, staging_buffer, data->buffer, size, offset);

  vkDestroyBuffer(renderer_data->device, staging_buffer, VK_NULL_HANDLE);
  vkFreeMemory(renderer_data->device, staging_buffer_memory, VK_NULL_HANDLE);
  return true;
}

bool _purrr_buffer_vulkan_map(_purrr_buffer_t *buffer, void **out_data) {
  if (!buffer || !buffer->initialized || !out_data) return false;
  _purrr_buffer_data_t *data = (_purrr_buffer_data_t*)buffer->data_ptr;
  _purrr_renderer_data_t *renderer_data = (_purrr_renderer_data_t*)buffer->renderer->data_ptr;
  if (!data || !renderer_data) return false;

  return vkMapMemory(renderer_data->device, data->buffer_memory, 0, buffer->info.size, 0, out_data) == VK_SUCCESS;
}

bool _purrr_buffer_vulkan_unmap(_purrr_buffer_t *buffer) {
  if (!buffer || !buffer->initialized) return false;
  _purrr_buffer_data_t *data = (_purrr_buffer_data_t*)buffer->data_ptr;
  _purrr_renderer_data_t *renderer_data = (_purrr_renderer_data_t*)buffer->renderer->data_ptr;
  if (!data || !renderer_data) return false;

  vkUnmapMemory(renderer_data->device, data->buffer_memory);

  return true;
}

// renderer

typedef struct {
  const char **items;
  size_t capacity;
  size_t count;
} strs_t;

uint32_t _purrr_renderer_vulkan_rate_device(VkSurfaceKHR surface, VkPhysicalDevice device) {
  _purrr_renderer_vulkan_swapchain_details_t details = _purrr_renderer_vulkan_request_swapchain_details(surface, device);
  if (details.format_count == 0 ||
      details.present_mode_count == 0 ||
      details.capabilities.maxImageCount < 2) return 0;
  return 1;
}

bool _purrr_renderer_create_swapchain(_purrr_renderer_t *renderer) {
  _purrr_renderer_data_t *data = (_purrr_renderer_data_t*)renderer->data_ptr;

  {
    _purrr_renderer_vulkan_swapchain_details_t details = _purrr_renderer_vulkan_request_swapchain_details(data->surface, data->gpu);

    if (details.format_count == 0 ||
        details.present_mode_count == 0)
      return false;
    data->swapchain_format = details.formats[0];
    for (uint32_t i = 0; i < details.format_count; ++i) {
      VkSurfaceFormatKHR format = details.formats[i];
      if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        data->swapchain_format = format;
        break;
      }
    }

    data->swapchain_present_mode = VK_PRESENT_MODE_FIFO_KHR;
    VkPresentModeKHR best_mode = (renderer->info.vsync?VK_PRESENT_MODE_MAILBOX_KHR:VK_PRESENT_MODE_IMMEDIATE_KHR);
    for (uint32_t i = 0; i < details.present_mode_count; ++i) {
      if (details.present_modes[i] == best_mode) {
        data->swapchain_present_mode = best_mode;
        break;
      }
    }

    if (details.capabilities.currentExtent.width != UINT32_MAX) data->swapchain_extent = details.capabilities.currentExtent;
    else {
      int width, height;
      glfwGetFramebufferSize(((_purrr_window_t*)renderer->info.window)->window, &width, &height);

      data->swapchain_extent = (VkExtent2D){
        clamp((uint32_t)width, details.capabilities.minImageExtent.width, details.capabilities.maxImageExtent.width),
        clamp((uint32_t)height, details.capabilities.minImageExtent.height, details.capabilities.maxImageExtent.height)
      };
    }

    VkSwapchainCreateInfoKHR createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = data->surface;
    createInfo.minImageCount = renderer->info.image_count;
    createInfo.imageFormat = data->swapchain_format.format;
    createInfo.imageColorSpace = data->swapchain_format.colorSpace;
    createInfo.imageExtent = data->swapchain_extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    uint32_t queue_family_indices[] = {data->graphics_family, data->present_family};

    if (data->graphics_family != data->present_family) {
      createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      createInfo.queueFamilyIndexCount = 2;
      createInfo.pQueueFamilyIndices = queue_family_indices;
    } else createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

    createInfo.preTransform = details.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = data->swapchain_present_mode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(data->device, &createInfo, VK_NULL_HANDLE, &data->swapchain) != VK_SUCCESS) return false;

    if (!data->swapchain_images) {
      data->swapchain_images = (VkImage*)malloc(sizeof(*data->swapchain_images) * renderer->info.image_count);
      assert(data->swapchain_images);
    }
    vkGetSwapchainImagesKHR(data->device, data->swapchain, &renderer->info.image_count, data->swapchain_images);
  }

  {
    VkImageViewCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = data->swapchain_format.format;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (!data->swapchain_image_views) {
      data->swapchain_image_views = (VkImageView*)malloc(sizeof(*data->swapchain_image_views) * renderer->info.image_count);
      assert(data->swapchain_image_views);
    }
    for (uint8_t i = 0; i < renderer->info.image_count; i++) {
      _purrr_vulkan_transition_image_layout(data, data->swapchain_images[i], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 0, VK_ACCESS_MEMORY_READ_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
      createInfo.image = data->swapchain_images[i];
      if (vkCreateImageView(data->device, &createInfo, VK_NULL_HANDLE, &data->swapchain_image_views[i]) != VK_SUCCESS) return false;
    }
  }

  if (renderer->info.swapchain_format) *renderer->info.swapchain_format = purrr_format(data->swapchain_format.format);

  if (renderer->info.swapchain_images) {
    *renderer->info.swapchain_images = (purrr_image_t**)malloc(sizeof(**renderer->info.swapchain_images) * renderer->info.image_count);
    assert(*renderer->info.swapchain_images);
    purrr_image_t **ptr = *renderer->info.swapchain_images;
    for (uint32_t i = 0; i < renderer->info.image_count; ++i) {
      _purrr_image_data_t *internal_image_data = (_purrr_image_data_t*)malloc(sizeof(*internal_image_data));
      assert(internal_image_data);
      internal_image_data->image = data->swapchain_images[i];
      internal_image_data->image_view = data->swapchain_image_views[i];

      _purrr_image_t *internal_image = (_purrr_image_t*)malloc(sizeof(_purrr_image_t));
      assert(internal_image);
      *internal_image = (_purrr_image_t){
        .initialized = true,
        .renderer = renderer,
        .info = (purrr_image_info_t){
          .width = data->swapchain_extent.width,
          .height = data->swapchain_extent.height,
          .format = PURRR_FORMAT_UNDEFINED,
        },
        .cleanup = _purrr_image_vulkan_cleanup,
        .copy = _purrr_image_vulkan_copy,
        .data_ptr = internal_image_data,
      };

      *(ptr++) = (purrr_image_t*)internal_image;
    }
  }

  return true;
}

void _purrr_renderer_cleanup_swapchain(_purrr_renderer_t *renderer) {
  assert(renderer);

  _purrr_renderer_data_t *data = (_purrr_renderer_data_t*)renderer->data_ptr;
  assert(data);
  vkDestroySwapchainKHR(data->device, data->swapchain, VK_NULL_HANDLE);

  for (uint8_t i = 0; i < renderer->info.image_count; ++i)
    vkDestroyImageView(data->device, data->swapchain_image_views[i], VK_NULL_HANDLE);
}

bool _purrr_renderer_recreate_swapchain(_purrr_renderer_t *renderer) {
  assert(renderer && renderer->initialized && renderer->data_ptr);

  int width = 0, height = 0;
  _purrr_window_t *window = (_purrr_window_t*)renderer->info.window;
  glfwGetFramebufferSize(window->window, &width, &height);
  while (width == 0 || height == 0) {
    glfwGetFramebufferSize(window->window, &width, &height);
    if (purrr_window_should_close(renderer->info.window)) return false;
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(((_purrr_renderer_data_t*)renderer->data_ptr)->device);

  _purrr_renderer_cleanup_swapchain(renderer);
  if (!_purrr_renderer_create_swapchain(renderer)) return false;

  if (renderer->callbacks.resize) renderer->callbacks.resize((purrr_renderer_t*)renderer);

  return true;
}



bool _purrr_renderer_vulkan_init(_purrr_renderer_t *renderer) {
  if (!renderer || !glfwVulkanSupported()) return false;
  _purrr_renderer_data_t *data = (_purrr_renderer_data_t*)malloc(sizeof(*data));
  assert(data);
  memset(data, 0, sizeof(*data));

  {
    strs_t extensions = {0};
    strs_t layers = {0};

    { // Load GLFW extensions
      uint32_t count = 0;
      const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&count);
      extensions.count = count;
      extensions.items = (const char **)malloc(sizeof(*extensions.items)*(extensions.capacity = (count + 1)));
      assert(extensions.items);
      memcpy(extensions.items, glfw_extensions, sizeof(*glfw_extensions)*count);
    }

    #ifdef PURRR_DEBUG
      extensions.items[extensions.count++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
      layers.items = malloc(sizeof(*layers.items)*8);
      assert(layers.items);
      layers.items[layers.count++] = "VK_LAYER_KHRONOS_validation";
    #endif // PURRR_DEBUG

    VkInstanceCreateInfo createInfo = {
      VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, VK_NULL_HANDLE, 0,
      VK_NULL_HANDLE,
      (uint32_t)layers.count, layers.items,
      (uint32_t)extensions.count, extensions.items
    };

    VkResult result = vkCreateInstance(&createInfo, VK_NULL_HANDLE, &data->instance);
    if (extensions.count > 0) free(extensions.items);
    if (layers.count > 0) free(layers.items);

    if (result != VK_SUCCESS) goto error;
  }

  {
    if (glfwCreateWindowSurface(data->instance, ((_purrr_window_t*)renderer->info.window)->window, VK_NULL_HANDLE, &data->surface) != VK_SUCCESS) goto error;
  }

  const char *device_extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
  {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(data->instance, &deviceCount, VK_NULL_HANDLE);
    if (deviceCount == 0) return false;
    VkPhysicalDevice *devices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice)*deviceCount);
    assert(devices);
    vkEnumeratePhysicalDevices(data->instance, &deviceCount, devices);

    uint32_t best_score = 0;
    while (deviceCount-- > 0) {
      VkPhysicalDevice device = devices[deviceCount];
      uint32_t score = _purrr_renderer_vulkan_rate_device(data->surface, device);
      if (score < best_score) continue;
      best_score = score;
      data->gpu = device;
    }

    if (best_score == 0) goto error;

    if (!_purrr_renderer_vulkan_find_queue_families(data->surface, data->gpu, &data->graphics_family, &data->present_family)) goto error;
  }

  {
    uint32_t unique_count = 1;
    uint32_t uniques[] = { data->graphics_family, data->present_family };
    if (data->graphics_family != data->present_family) ++unique_count;

    VkDeviceQueueCreateInfo queueCreateInfos[2] = {0};

    float queuePriority = 1.0f;
    for (uint32_t i = 0; i < unique_count; ++i) {
      queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queueCreateInfos[i].queueFamilyIndex = uniques[i];
      queueCreateInfos[i].queueCount = 1;
      queueCreateInfos[i].pQueuePriorities = &queuePriority;
    }

    VkPhysicalDeviceFeatures deviceFeatures = {0};
    VkDeviceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos;
    createInfo.queueCreateInfoCount = unique_count;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = sizeof(device_extensions)/sizeof(device_extensions[0]);
    createInfo.ppEnabledExtensionNames = device_extensions;
    #ifdef PURRR_DEBUG
    createInfo.enabledLayerCount = 1;
    createInfo.ppEnabledLayerNames = (const char *[]){ "VK_LAYER_KHRONOS_validation", };
    #else // PURRR_DEBUG
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = NULL;
    #endif // PURRR_DEBUG

    if (vkCreateDevice(data->gpu, &createInfo, VK_NULL_HANDLE, &data->device) != VK_SUCCESS) goto error;

    vkGetDeviceQueue(data->device, data->graphics_family, 0, &data->graphics_queue);
    vkGetDeviceQueue(data->device, data->present_family, 0, &data->present_queue);
  }

  {
    VkCommandPoolCreateInfo pool_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = data->graphics_family,
    };

    if (vkCreateCommandPool(data->device, &pool_info, VK_NULL_HANDLE, &data->command_pool) != VK_SUCCESS) return false;
  }

  data->frame_index = 0;
  data->image_index = 0;
  renderer->data_ptr = data;

  if (!_purrr_renderer_create_swapchain(renderer)) goto error;

  {
    VkCommandBufferAllocateInfo alloc_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool = data->command_pool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = renderer->info.image_count,
    };

    data->render_cmd_bufs = (VkCommandBuffer*)malloc(sizeof(*data->render_cmd_bufs) * renderer->info.image_count);
    assert(data->render_cmd_bufs);
    if (vkAllocateCommandBuffers(data->device, &alloc_info, data->render_cmd_bufs) != VK_SUCCESS) return false;
  }

  {
    VkSemaphoreCreateInfo semaphore_info = {
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };

    VkFenceCreateInfo fence_info = {
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    data->image_semaphores  = (VkSemaphore*)malloc(sizeof(*data->image_semaphores)*renderer->info.image_count);
    data->render_semaphores = (VkSemaphore*)malloc(sizeof(*data->render_semaphores)*renderer->info.image_count);
    data->flight_fences     = (VkFence*)malloc(sizeof(*data->flight_fences)*renderer->info.image_count);
    assert(data->image_semaphores && data->render_semaphores && data->flight_fences);

    for (uint8_t i = 0; i < renderer->info.image_count; ++i) {
        if (vkCreateSemaphore(data->device, &semaphore_info, VK_NULL_HANDLE, &data->image_semaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(data->device, &semaphore_info, VK_NULL_HANDLE, &data->render_semaphores[i]) != VK_SUCCESS ||
            vkCreateFence(data->device, &fence_info, VK_NULL_HANDLE, &data->flight_fences[i]) != VK_SUCCESS)
          return false;
    }
  }

  {
    VkDescriptorPoolSize pool_sizes[] = {
      (VkDescriptorPoolSize){
        .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 2048, // TODO: Customize?
      },
    };

    VkDescriptorPoolCreateInfo pool_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
      .poolSizeCount = sizeof(pool_sizes)/sizeof(pool_sizes[0]),
      .pPoolSizes = pool_sizes,
      .maxSets = 2048,
    };

    if (vkCreateDescriptorPool(data->device, &pool_info, VK_NULL_HANDLE, &data->descriptor_pool) != VK_SUCCESS) return false;
  }

  {
    VkDescriptorSetLayoutBinding binding = {
      .binding = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_ALL,
    };

    VkDescriptorSetLayoutCreateInfo layout_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .bindingCount = 1,
      .pBindings = &binding,
    };

    if (vkCreateDescriptorSetLayout(data->device, &layout_info, VK_NULL_HANDLE, &data->texture_descriptor_set_layout) != VK_SUCCESS) return false;
  }

  {
    VkDescriptorSetLayoutBinding binding = {
      .binding = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_ALL,
    };

    VkDescriptorSetLayoutCreateInfo layout_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .bindingCount = 1,
      .pBindings = &binding,
    };

    if (vkCreateDescriptorSetLayout(data->device, &layout_info, VK_NULL_HANDLE, &data->uniform_descriptor_set_layout) != VK_SUCCESS) return false;
  }

  {
    VkDescriptorSetLayoutBinding binding = {
      .binding = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_ALL,
    };

    VkDescriptorSetLayoutCreateInfo layout_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .bindingCount = 1,
      .pBindings = &binding,
    };

    if (vkCreateDescriptorSetLayout(data->device, &layout_info, VK_NULL_HANDLE, &data->storage_descriptor_set_layout) != VK_SUCCESS) return false;
  }

  renderer->initialized = true;

  return true;
error:
  free(data);
  renderer->data_ptr = NULL;
  return false;
}

void _purrr_renderer_vulkan_cleanup(_purrr_renderer_t *renderer) {
  _purrr_renderer_data_t *data = (_purrr_renderer_data_t*)renderer->data_ptr;
  if (!data) return;
  if (renderer->initialized) {
    for (uint8_t i = 0; i < 2; ++i) {
      vkDestroySemaphore(data->device, data->render_semaphores[i], VK_NULL_HANDLE);
      vkDestroySemaphore(data->device, data->image_semaphores[i], VK_NULL_HANDLE);
      vkDestroyFence(data->device, data->flight_fences[i], VK_NULL_HANDLE);
    }

    vkDestroyDescriptorSetLayout(data->device, data->texture_descriptor_set_layout, VK_NULL_HANDLE);
    vkDestroyDescriptorSetLayout(data->device, data->uniform_descriptor_set_layout, VK_NULL_HANDLE);
    vkDestroyDescriptorSetLayout(data->device, data->storage_descriptor_set_layout, VK_NULL_HANDLE);
    vkDestroyDescriptorPool(data->device, data->descriptor_pool, VK_NULL_HANDLE);

    vkDestroyCommandPool(data->device, data->command_pool, VK_NULL_HANDLE);

    _purrr_renderer_cleanup_swapchain(renderer);
    vkDestroyDevice(data->device, VK_NULL_HANDLE);
    vkDestroySurfaceKHR(data->instance, data->surface, VK_NULL_HANDLE);
    vkDestroyInstance(data->instance, VK_NULL_HANDLE);
  }
  free(data);
}

uint32_t _purrr_renderer_vulkan_get_sample_counts(_purrr_renderer_t *renderer, purrr_sample_count_t **array) {
  _purrr_renderer_data_t *data = (_purrr_renderer_data_t*)renderer->data_ptr;
  assert(renderer->initialized && data);

  VkPhysicalDeviceProperties properties = {0};
  vkGetPhysicalDeviceProperties(data->gpu, &properties);

  VkSampleCountFlags counts = properties.limits.framebufferColorSampleCounts & properties.limits.framebufferDepthSampleCounts;
  VkSampleCountFlags save = counts;

  uint32_t n = 0;
  purrr_sample_count_t *sample_counts = NULL;
  if (array) {
    while (save > 0) {
      if (save & 1) ++n;
      save >>= 1;
    }

    sample_counts = malloc(sizeof(*sample_counts) * n);
    assert(sample_counts);
    n = 0;
  }

  uint32_t i = 0;
  while (counts > 0) {
    if (counts & 1) {
      if (array) sample_counts[i] = (purrr_sample_count_t)n;
      ++i;
    }
    counts >>= 1;
    ++n;
  }

  if (array) *array = sample_counts;

  return i;
}

bool _purrr_renderer_vulkan_begin_frame(_purrr_renderer_t *renderer, uint32_t *image_index) {
  _purrr_renderer_data_t *data = (_purrr_renderer_data_t*)renderer->data_ptr;
  assert(renderer->initialized && data);

  vkWaitForFences(data->device, 1, &data->flight_fences[data->frame_index], VK_TRUE, UINT64_MAX);
  VkResult result = vkAcquireNextImageKHR(data->device, data->swapchain, UINT64_MAX, data->image_semaphores[data->frame_index], VK_NULL_HANDLE, &data->image_index);
  if (image_index) *image_index = data->image_index;
  if (result == VK_ERROR_OUT_OF_DATE_KHR) return _purrr_renderer_recreate_swapchain(renderer) && _purrr_renderer_vulkan_begin_frame(renderer, image_index);
  else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) return false;

  vkResetFences(data->device, 1, &data->flight_fences[data->frame_index]);

  data->active_cmd_buf = data->render_cmd_bufs[data->frame_index];

  vkResetCommandBuffer(data->active_cmd_buf, 0);

  VkCommandBufferBeginInfo begin_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
  };

  if (vkBeginCommandBuffer(data->active_cmd_buf, &begin_info) != VK_SUCCESS) return false;

  return true;
}

bool _purrr_renderer_vulkan_begin_render_target(_purrr_renderer_t *renderer, _purrr_render_target_t *render_target) {
  if (!renderer || !render_target || !renderer->initialized || !render_target->initialized) return false;

  _purrr_renderer_data_t *data = (_purrr_renderer_data_t*)renderer->data_ptr;
  _purrr_pipeline_descriptor_data_t *pipeline_descriptor_data = (_purrr_pipeline_descriptor_data_t*)render_target->descriptor->data_ptr;
  _purrr_render_target_data_t *render_target_data = (_purrr_render_target_data_t*)render_target->data_ptr;

  VkRenderPass render_pass = pipeline_descriptor_data->render_pass;
  VkFramebuffer framebuffer = render_target_data->framebuffer;

  uint32_t color_count = render_target->descriptor->info.color_attachment_count;
  uint32_t clear_value_count = color_count*(render_target->descriptor->info.resolve_attachments?2:1)+(render_target->descriptor->info.depth_attachment?1:0);
  VkClearValue *clear_values = malloc(sizeof(*clear_values)*clear_value_count);
  assert(clear_values);

  VkClearColorValue clear_color = {
    .float32 = { 0.0f, 0.0f, 0.0f, 1.0f } // TODO: Let user decide
  };
  VkClearDepthStencilValue clear_depth = { 1.0f, 0 };

  uint32_t i = 0;
  for (; i < color_count; ++i) clear_values[i] = (VkClearValue){ clear_color };
  if (i < clear_value_count) clear_values[i] = (VkClearValue){ .depthStencil = clear_depth };

  VkRect2D area = (VkRect2D){
    .offset = (VkOffset2D){0},
    .extent = (VkExtent2D){ render_target->width, render_target->height },
  };

  VkRenderPassBeginInfo begin_info = {
    VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO, VK_NULL_HANDLE,
    render_pass,
    framebuffer,
    area,
    clear_value_count,
    clear_values,
  };

  vkCmdBeginRenderPass(data->active_cmd_buf, &begin_info, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport = {
    .x = 0.0f,
    .y = 0.0f,
    .width = (float)render_target->width,
    .height = (float)render_target->height,
    .minDepth = 0.0f,
    .maxDepth = 1.0f,
  };
  vkCmdSetViewport(data->active_cmd_buf, 0, 1, &viewport);

  vkCmdSetScissor(data->active_cmd_buf, 0, 1, &area);
  data->active_render_target = render_target;

  free(clear_values);

  return true;
}

bool _purrr_renderer_vulkan_bind_pipeline(_purrr_renderer_t *renderer, _purrr_pipeline_t *pipeline) {
  if (!renderer || !renderer->initialized || !pipeline || !pipeline->initialized) return false;
  _purrr_renderer_data_t *data = (_purrr_renderer_data_t*)renderer->data_ptr;
  _purrr_pipeline_data_t *pipeline_data = (_purrr_pipeline_data_t*)pipeline->data_ptr;
  assert(data && pipeline_data);
  if (!data->active_cmd_buf || !data->active_render_target) return false;

  vkCmdBindPipeline(data->active_cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_data->pipeline);

  data->active_pipeline = pipeline;

  return true;
}

bool _purrr_renderer_vulkan_bind_texture(_purrr_renderer_t *renderer, _purrr_texture_t *texture, uint32_t slot_index) {
  if (!renderer || !renderer->initialized || !texture || !texture->initialized) return false;
  _purrr_renderer_data_t *data = (_purrr_renderer_data_t*)renderer->data_ptr;
  _purrr_texture_data_t *texture_data = (_purrr_texture_data_t*)texture->data_ptr;
  assert(data && texture_data);
  if (!data->active_cmd_buf || !data->active_render_target || !data->active_render_target->initialized || !data->active_pipeline || !data->active_pipeline->initialized || slot_index >= data->active_pipeline->info.descriptor_slot_count) return false;
  _purrr_pipeline_data_t *pipeline_data = (_purrr_pipeline_data_t*)data->active_pipeline->data_ptr;
  assert(pipeline_data);

  vkCmdBindDescriptorSets(data->active_cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_data->pipeline_layout, slot_index, 1, &texture_data->descriptor_set, 0, NULL);

  return true;
}

bool _purrr_renderer_vulkan_bind_buffer(_purrr_renderer_t *renderer, _purrr_buffer_t *buffer, uint32_t slot_index) {
  if (!renderer || !renderer->initialized || !buffer || !buffer->initialized) return false;
  _purrr_renderer_data_t *data = (_purrr_renderer_data_t*)renderer->data_ptr;
  _purrr_buffer_data_t *buffer_data = (_purrr_buffer_data_t*)buffer->data_ptr;
  assert(data && buffer_data);
  assert(buffer->info.type < COUNT_PURRR_BUFFER_TYPES);

  if (!data->active_cmd_buf || !data->active_render_target || !data->active_render_target->initialized || !data->active_pipeline || !data->active_pipeline->initialized) return false;

  switch (buffer->info.type) {
  case PURRR_BUFFER_TYPE_UNIFORM:
  case PURRR_BUFFER_TYPE_STORAGE: {
    if (slot_index >= data->active_pipeline->info.descriptor_slot_count) return false;
    _purrr_pipeline_data_t *pipeline_data = (_purrr_pipeline_data_t*)data->active_pipeline->data_ptr;
    assert(pipeline_data);

    vkCmdBindDescriptorSets(data->active_cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_data->pipeline_layout, slot_index, 1, &buffer_data->set, 0, NULL);
  } break;
  case PURRR_BUFFER_TYPE_VERTEX: {
    if (slot_index != 0) return false;
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(data->active_cmd_buf, slot_index, 1, &buffer_data->buffer, &offset);
  } break;
  case PURRR_BUFFER_TYPE_INDEX: {
    vkCmdBindIndexBuffer(data->active_cmd_buf, buffer_data->buffer, 0, VK_INDEX_TYPE_UINT32);
  } break;
  }

  return true;
}

bool _purrr_renderer_vulkan_push_constant(_purrr_renderer_t *renderer, uint32_t offset, uint32_t size, const void *value) {
  if (!renderer || !renderer->initialized || !value || !size) return false;
  _purrr_renderer_data_t *data = (_purrr_renderer_data_t*)renderer->data_ptr;
  assert(data);
  if (!data->active_cmd_buf || !data->active_render_target || !data->active_pipeline || !data->active_pipeline->initialized) return false;

  _purrr_pipeline_data_t *pipeline_data = (_purrr_pipeline_data_t*)data->active_pipeline->data_ptr;
  assert(pipeline_data);

  vkCmdPushConstants(data->active_cmd_buf, pipeline_data->pipeline_layout, VK_SHADER_STAGE_ALL, offset, size, value);

  return true;
}

bool _purrr_renderer_vulkan_draw(_purrr_renderer_t *renderer, uint32_t instance_count, uint32_t first_instance, uint32_t vertex_count, uint32_t first_vertex) {
  if (!renderer || !renderer->initialized) return false;
  _purrr_renderer_data_t *data = (_purrr_renderer_data_t*)renderer->data_ptr;
  assert(data);
  if (!data->active_cmd_buf || !data->active_render_target || !data->active_pipeline || !data->active_pipeline->initialized) return false;
  vkCmdDraw(data->active_cmd_buf, vertex_count, instance_count, first_vertex, first_instance);
  return true;
}

bool _purrr_renderer_vulkan_draw_indexed(_purrr_renderer_t *renderer, uint32_t instance_count, uint32_t first_instance, uint32_t index_count, uint32_t first_index, int32_t vertex_offset) {
  if (!renderer || !renderer->initialized) return false;
  _purrr_renderer_data_t *data = (_purrr_renderer_data_t*)renderer->data_ptr;
  assert(data);
  if (!data->active_cmd_buf || !data->active_render_target || !data->active_pipeline || !data->active_pipeline->initialized) return false;
  vkCmdDrawIndexed(data->active_cmd_buf, index_count, instance_count, first_index, first_instance, vertex_offset);
  return true;
}

bool _purrr_renderer_vulkan_end_render_target(_purrr_renderer_t *renderer) {
  if (!renderer || !renderer->initialized) return false;
  _purrr_renderer_data_t *data = (_purrr_renderer_data_t*)renderer->data_ptr;
  assert(data);
  if (!data->active_cmd_buf || !data->active_render_target) return false;
  data->active_render_target = NULL;

  vkCmdEndRenderPass(data->active_cmd_buf);

  return true;
}

bool _purrr_renderer_vulkan_end_frame(_purrr_renderer_t *renderer) {
  if (!renderer || !renderer->initialized) return false;
  _purrr_renderer_data_t *data = (_purrr_renderer_data_t*)renderer->data_ptr;
  assert(data);
  if (!data->active_cmd_buf) return false;

  assert(!data->active_render_target);

  if (vkEndCommandBuffer(data->active_cmd_buf) != VK_SUCCESS) return false;

  {
    VkSemaphore wait_semaphores[] = { data->image_semaphores[data->frame_index] };
    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSemaphore signal_semaphores[] = {data->render_semaphores[data->frame_index]};

    VkSubmitInfo submit_info = {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = wait_semaphores,
      .pWaitDstStageMask = wait_stages,
      .commandBufferCount = 1,
      .pCommandBuffers = &data->active_cmd_buf,
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = signal_semaphores,
    };

    if (vkQueueSubmit(data->graphics_queue, 1, &submit_info, data->flight_fences[data->frame_index]) != VK_SUCCESS) return false;

    VkPresentInfoKHR present_info = {
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = signal_semaphores,
      .swapchainCount = 1,
      .pSwapchains = &data->swapchain,
      .pImageIndices = &data->image_index,
    };

    VkResult result = vkQueuePresentKHR(data->present_queue, &present_info);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) _purrr_renderer_recreate_swapchain(renderer);
    else if (result != VK_SUCCESS) return false;
  }

  data->active_cmd_buf = NULL;
  data->active_pipeline = NULL;
  data->frame_index = (data->frame_index+1)%2;

  return true;
}

bool _purrr_renderer_vulkan_wait(_purrr_renderer_t *renderer) {
  if (!renderer || !renderer->initialized) return false;
  _purrr_renderer_data_t *data = (_purrr_renderer_data_t*)renderer->data_ptr;
  assert(data);
  vkDeviceWaitIdle(data->device);
  return true;
}

// TODO: Ensure snake_case everywhere