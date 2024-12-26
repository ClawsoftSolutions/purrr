#include "internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void _purrr_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  void *user_ptr = glfwGetWindowUserPointer(window);
  _purrr_window_t *internal = (_purrr_window_t*)user_ptr;
  if (internal->callbacks.key) internal->callbacks.key((purrr_window_t*)internal, key, scancode, action, mods);
}

void _purrr_mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
  void *user_ptr = glfwGetWindowUserPointer(window);
  _purrr_window_t *internal = (_purrr_window_t*)user_ptr;
  if (internal->callbacks.mouse_button) internal->callbacks.mouse_button((purrr_window_t*)internal, button, action, mods);
}

void _purrr_cursor_pos_callback(GLFWwindow *window, double xpos, double ypos) {
  void *user_ptr = glfwGetWindowUserPointer(window);
  _purrr_window_t *internal = (_purrr_window_t*)user_ptr;
  if (internal->callbacks.cursor_position) internal->callbacks.cursor_position((purrr_window_t*)internal, xpos, ypos);
}

void _purrr_window_size_callback(GLFWwindow *window, int width, int height) {
  void *user_ptr = glfwGetWindowUserPointer(window);
  _purrr_window_t *internal = (_purrr_window_t*)user_ptr;
  if (internal->callbacks.window_size) internal->callbacks.window_size((purrr_window_t*)internal, width, height);
}

void _purrr_window_close_callback(GLFWwindow *window) {
  void *user_ptr = glfwGetWindowUserPointer(window);
  _purrr_window_t *internal = (_purrr_window_t*)user_ptr;
  if (internal->callbacks.window_close) {
    glfwSetWindowShouldClose(window, internal->callbacks.window_close((purrr_window_t*)internal));
  }
}

purrr_window_t *purrr_window_create(purrr_window_info_t *info) {
  if (!info || info->api >= COUNT_PURRR_APIS) return NULL;
  if (!info->title) info->title = "purrr window";

  if (glfwInit() != GLFW_TRUE) return NULL;
  // #2
  if (info->api == PURRR_API_VULKAN && !glfwVulkanSupported()) return NULL;

  GLFWmonitor *monitor = NULL;
  const GLFWvidmode *mode = NULL;
  if (info->fullscreen) {
    // #3
    monitor = glfwGetPrimaryMonitor();
    mode = glfwGetVideoMode(monitor);
    if (info->width  == PURRR_WINDOW_SIZE_DONT_MIND) info->width  = mode->width;
    if (info->height == PURRR_WINDOW_SIZE_DONT_MIND) info->height = mode->height;
    info->x = 0;
    info->y = 0;
  } else {
    mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    if (info->width == PURRR_WINDOW_SIZE_DONT_MIND)  info->width  = mode->width/2;
    else if (info->width == PURRR_WINDOW_SIZE_MAX)   info->width  = mode->width;
    if (info->height == PURRR_WINDOW_SIZE_DONT_MIND) info->height = mode->height/2;
    else if (info->height == PURRR_WINDOW_SIZE_MAX)  info->height = mode->height;
    if (info->x == PURRR_WINDOW_POS_CENTER) info->x = (mode->width/2)-(info->width/2);
    if (info->y == PURRR_WINDOW_POS_CENTER) info->y = (mode->height/2)-(info->height/2);
  }

  _purrr_window_t *internal = (_purrr_window_t*)malloc(sizeof(*internal));
  if (!internal) return NULL;
  memset(internal, 0, sizeof(*internal));
  internal->api = info->api;

  switch (internal->api) {
  case PURRR_API_VULKAN: {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  } break;
  case COUNT_PURRR_APIS:
  default: {
    assert(0 && "purrr_window_create: Unreachable");
  } break;
  }

  if (info->callbacks_ptr) *info->callbacks_ptr = &internal->callbacks;

  internal->window = glfwCreateWindow(info->width, info->height, info->title, monitor, NULL);
  if (!internal->window) {
    purrr_window_destroy((purrr_window_t*)internal);
    return NULL;
  }

  glfwMakeContextCurrent(internal->window);
  glfwSetWindowUserPointer(internal->window, internal);

  glfwSetKeyCallback(internal->window, &_purrr_key_callback);
  glfwSetMouseButtonCallback(internal->window, &_purrr_mouse_button_callback);
  glfwSetCursorPosCallback(internal->window, &_purrr_cursor_pos_callback);

  glfwSetWindowSizeCallback(internal->window, &_purrr_window_size_callback);
  glfwSetWindowCloseCallback(internal->window, &_purrr_window_close_callback);

  glfwSetWindowMonitor(internal->window, monitor, info->x, info->y, info->width, info->height, mode->refreshRate);

  return (purrr_window_t*)internal;
}

void purrr_window_destroy(purrr_window_t *window) {
  _purrr_window_t *internal = (_purrr_window_t*)window;
  assert(internal);
  if (internal->window) glfwDestroyWindow(internal->window);
  free(internal);
}

bool purrr_window_should_close(purrr_window_t *window) {
  return glfwWindowShouldClose(((_purrr_window_t*)window)->window);
}

void purrr_window_get_size(purrr_window_t *window, uint32_t *out_width, uint32_t *out_height) {
  _purrr_window_t *internal = (_purrr_window_t*)window;
  assert(internal && internal->window);
  int width = 0, height = 0;
  glfwGetWindowSize(internal->window, &width, &height);
  if (out_width) *out_width = (uint32_t)width;
  if (out_height) *out_height = (uint32_t)height;
}

// I don't like this but it is what it is
void purrr_window_set_icons(purrr_window_t *window, purrr_window_icon_info_t *big, purrr_window_icon_info_t *small) {
  _purrr_window_t *internal = (_purrr_window_t*)window;
  assert(internal && internal->window);

  if (small) assert(big);

  uint32_t count = (big != NULL) + (small != NULL);

  static GLFWimage s_images[2] = {0};

  GLFWimage *images = NULL;
  if (count > 0) images = s_images;

  if (big) {
    images[0] = (GLFWimage){
      .pixels = big->pixels,
      .width  = big->width,
      .height = big->height,
    };
  }

  if (small) {
    images[1] = (GLFWimage){
      .pixels = small->pixels,
      .width  = small->width,
      .height = small->height,
    };
  }

  glfwSetWindowIcon(internal->window, count, images);
}

void purrr_window_set_cursor(purrr_window_t *window, purrr_cursor_t *cursor) {
  _purrr_window_t *internal = (_purrr_window_t*)window;
  assert(internal && internal->window);

  _purrr_cursor_t *internal_cursor = (_purrr_cursor_t*)cursor;

  GLFWcursor *cursor_handle = NULL;
  if (internal_cursor) cursor_handle = internal_cursor->cursor;

  glfwSetCursor(internal->window, cursor_handle);
}

void purrr_window_set_user_ptr(purrr_window_t *window, void *user_ptr) {
  _purrr_window_t *internal = (_purrr_window_t*)window;
  assert(internal && internal->window);
  internal->user_ptr = user_ptr;
}

void *purrr_window_get_user_ptr(purrr_window_t *window) {
  _purrr_window_t *internal = (_purrr_window_t*)window;
  assert(internal && internal->window);
  return internal->user_ptr;
}

// cursor

purrr_cursor_t *purrr_cursor_create_standard(purrr_standard_cursor_t type) {
  _purrr_cursor_t *internal = (_purrr_cursor_t*)malloc(sizeof(*internal));
  if (!internal) return NULL;
  memset(internal, 0, sizeof(*internal));
  internal->cursor = glfwCreateStandardCursor((int)type);

  if (!internal->cursor) {
    free(internal);
    return NULL;
  }

  return (purrr_cursor_t*)internal;
}

purrr_cursor_t *purrr_cursor_create(purrr_cursor_info_t *info) {
  assert(info && info->pixels);

  _purrr_cursor_t *internal = (_purrr_cursor_t*)malloc(sizeof(*internal));
  if (!internal) return NULL;
  memset(internal, 0, sizeof(*internal));

  size_t pixels_size = info->width*info->height*4;

  internal->pixels = (uint8_t*)malloc(pixels_size);
  assert(internal->pixels);
  memcpy(internal->pixels, info->pixels, pixels_size);

  GLFWimage image = {
    .pixels = internal->pixels,
    .width  = info->width,
    .height = info->height,
  };

  internal->cursor = glfwCreateCursor(&image, info->xhot, info->yhot);

  return (purrr_cursor_t*)internal;
}

void purrr_cursor_destroy(purrr_cursor_t *cursor) {
  _purrr_cursor_t *internal = (_purrr_cursor_t*)cursor;
  if (!internal) return;
  if (internal->cursor) glfwDestroyCursor(internal->cursor);
  if (internal->pixels) free(internal->pixels);
}

// sampler

purrr_sampler_t *purrr_sampler_create(purrr_sampler_info_t *info, purrr_renderer_t *renderer) {
  if (!info ||
      info->mag_filter >= COUNT_PURRR_SAMPLER_FILTERS || info->min_filter >= COUNT_PURRR_SAMPLER_FILTERS ||
      info->address_mode_u >= COUNT_PURRR_SAMPLER_ADDRESS_MODES || info->address_mode_v >= COUNT_PURRR_SAMPLER_ADDRESS_MODES || info->address_mode_w >= COUNT_PURRR_SAMPLER_ADDRESS_MODES ||
      !renderer) return NULL;

  _purrr_sampler_t *internal = (_purrr_sampler_t*)malloc(sizeof(*internal));
  if (!internal) return NULL;
  memset(internal, 0, sizeof(*internal));
  internal->info = *info;
  internal->renderer = (_purrr_renderer_t*)renderer;

  switch (((_purrr_renderer_t*)renderer)->api) {
  case PURRR_API_VULKAN: {
    internal->init = _purrr_sampler_vulkan_init;
    internal->cleanup = _purrr_sampler_vulkan_cleanup;
  } break;
  default: {
    assert(0 && "Unreachable");
    return NULL;
  }
  }

  if (!internal->init(internal)) {
    _purrr_sampler_free(internal);
    return NULL;
  }

  return (purrr_sampler_t*)internal;
}

void purrr_sampler_destroy(purrr_sampler_t *sampler) {
  if (sampler) _purrr_sampler_free((_purrr_sampler_t*)sampler);
}

// texture

purrr_texture_t *purrr_texture_create(purrr_texture_info_t *info, purrr_renderer_t *renderer) {
  if (!info || !info->sampler || info->format >= COUNT_PURRR_FORMATS || info->format == PURRR_FORMAT_UNDEFINED || !renderer) return NULL;

  _purrr_texture_t *internal = (_purrr_texture_t*)malloc(sizeof(*internal));
  if (!internal) return NULL;
  memset(internal, 0, sizeof(*internal));
  internal->info = *info;
  internal->renderer = (_purrr_renderer_t*)renderer;

  switch (((_purrr_renderer_t*)renderer)->api) {
  case PURRR_API_VULKAN: {
    internal->init = _purrr_texture_vulkan_init;
    internal->cleanup = _purrr_texture_vulkan_cleanup;
    internal->load = _purrr_texture_vulkan_load;
    internal->copy = _purrr_texture_vulkan_copy;
  } break;
  default: {
    assert(0 && "Unreachable");
    return NULL;
  }
  }

  if (!internal->init(internal)) {
    _purrr_texture_free(internal);
    return NULL;
  }

  return (purrr_texture_t*)internal;
}

void purrr_texture_destroy(purrr_texture_t *texture) {
  if (texture) _purrr_texture_free((_purrr_texture_t*)texture);
}

bool purrr_texture_load(purrr_texture_t *dst, uint8_t *src, uint32_t src_width, uint32_t src_height) {
  _purrr_texture_t *internal = (_purrr_texture_t*)dst;
  assert(internal && src && internal->load);
  return internal->load(internal, src, src_width, src_height);
}

bool purrr_texture_copy(purrr_texture_t *dst, purrr_texture_t *src, uint32_t src_width, uint32_t src_height) {
  _purrr_texture_t *internal = (_purrr_texture_t*)dst;
  assert(internal && src && internal->copy);
  return internal->copy(internal, (_purrr_texture_t*)src, src_width, src_height);
}

// pipeline descriptor

purrr_pipeline_descriptor_t *purrr_pipeline_descriptor_create(purrr_pipeline_descriptor_info_t *info, purrr_renderer_t *renderer) {
  if (!info || info->color_attachment_count == 0 || !info->color_attachments || !renderer) return NULL;

  _purrr_pipeline_descriptor_t *internal = (_purrr_pipeline_descriptor_t*)malloc(sizeof(*internal));
  if (!internal) return NULL;
  memset(internal, 0, sizeof(*internal));
  internal->info = *info;
  internal->renderer = (_purrr_renderer_t*)renderer;

  switch (((_purrr_renderer_t*)renderer)->api) {
  case PURRR_API_VULKAN: {
    internal->init = _purrr_pipeline_descriptor_vulkan_init;
    internal->cleanup = _purrr_pipeline_descriptor_vulkan_cleanup;
  } break;
  default: {
    assert(0 && "Unreachable");
    return NULL;
  }
  }

  if (!internal->init(internal)) {
    _purrr_pipeline_descriptor_free(internal);
    return NULL;
  }

  internal->initialized = true;

  return (purrr_pipeline_descriptor_t*)internal;
}

void purrr_pipeline_descriptor_destroy(purrr_pipeline_descriptor_t *pipeline_descriptor) {
  if (pipeline_descriptor) _purrr_pipeline_descriptor_free((_purrr_pipeline_descriptor_t*)pipeline_descriptor);
}

// shader

purrr_shader_t *purrr_shader_create(purrr_shader_info_t *info, purrr_renderer_t *renderer) {
  if (!info || info->type >= COUNT_PURRR_SHADER_TYPES || (!info->filename && !info->buffer)) return NULL;

  _purrr_shader_t *internal = (_purrr_shader_t*)malloc(sizeof(*internal));
  if (!internal) return NULL;
  memset(internal, 0, sizeof(*internal));
  internal->info = *info;
  internal->renderer = (_purrr_renderer_t*)renderer;

  switch (((_purrr_renderer_t*)renderer)->api) {
  case PURRR_API_VULKAN: {
    internal->init = _purrr_shader_vulkan_init;
    internal->cleanup = _purrr_shader_vulkan_cleanup;
  } break;
  default: {
    assert(0 && "Unreachable");
    return NULL;
  }
  }

  if (!internal->init(internal)) {
    _purrr_shader_free(internal);
    return NULL;
  }

  internal->initialized = true;

  return (purrr_shader_t*)internal;
}

void purrr_shader_destroy(purrr_shader_t *shader) {

}

// pipeline

purrr_pipeline_t *purrr_pipeline_create(purrr_pipeline_info_t *info, purrr_renderer_t *renderer) {
  if (!info || !renderer ||
      !info->pipeline_descriptor ||
      (info->descriptor_slot_count > 0 && !info->descriptor_slots) ||
      (info->shader_count > 0 && !info->shaders))
    return NULL;

  _purrr_pipeline_t *internal = (_purrr_pipeline_t*)malloc(sizeof(*internal));
  if (!internal) return NULL;
  memset(internal, 0, sizeof(*internal));
  internal->info = *info;
  internal->renderer = (_purrr_renderer_t*)renderer;

  switch (((_purrr_renderer_t*)renderer)->api) {
  case PURRR_API_VULKAN: {
    internal->init = _purrr_pipeline_vulkan_init;
    internal->cleanup = _purrr_pipeline_vulkan_cleanup;
  } break;
  default: {
    assert(0 && "Unreachable");
    return NULL;
  }
  }

  if (!internal->init(internal)) {
    _purrr_pipeline_free(internal);
    return NULL;
  }

  internal->initialized = true;

  return (purrr_pipeline_t*)internal;
}

void purrr_pipeline_destroy(purrr_pipeline_t *pipeline) {
  if (pipeline) _purrr_pipeline_free((_purrr_pipeline_t*)pipeline);
}

// render target

purrr_render_target_t *purrr_render_target_create(purrr_render_target_info_t *info, purrr_renderer_t *renderer) {
  if (!info || !info->pipeline_descriptor ||
      !info->width || !info->height ||
      !info->pipeline_descriptor || !((_purrr_pipeline_descriptor_t*)info->pipeline_descriptor)->initialized ||
      !renderer) return NULL;

  _purrr_render_target_t *internal = (_purrr_render_target_t*)malloc(sizeof(*internal));
  if (!internal) return NULL;
  memset(internal, 0, sizeof(*internal));
  internal->descriptor = (_purrr_pipeline_descriptor_t*)info->pipeline_descriptor;
  internal->width = info->width;
  internal->height = info->height;
  internal->renderer = (_purrr_renderer_t*)renderer;

  switch (((_purrr_renderer_t*)renderer)->api) {
  case PURRR_API_VULKAN: {
    internal->init = _purrr_render_target_vulkan_init;
    internal->cleanup = _purrr_render_target_vulkan_cleanup;
    internal->get_texture = _purrr_render_target_vulkan_get_texture;
  } break;
  default: {
    assert(0 && "Unreachable");
    return NULL;
  }
  }

  if (!internal->init(internal)) {
    _purrr_render_target_free(internal);
    return NULL;
  }

  internal->initialized = true;

  return (purrr_render_target_t*)internal;
}

purrr_texture_t *purrr_render_target_get_texture(purrr_render_target_t *render_target, uint32_t texture_index) {
  _purrr_render_target_t *internal = (_purrr_render_target_t*)render_target;
  assert(internal && internal->get_texture);
  _purrr_texture_t *texture = internal->get_texture(internal, texture_index);
  return (purrr_texture_t*)texture;
}

void purrr_render_target_destroy(purrr_render_target_t *render_target) {
  if (render_target) _purrr_render_target_free((_purrr_render_target_t*)render_target);
}

// buffer

purrr_buffer_t *purrr_buffer_create(purrr_buffer_info_t *info, purrr_renderer_t *renderer) {
  if (!info || info->type >= COUNT_PURRR_BUFFER_TYPES) return NULL;

  _purrr_buffer_t *internal = (_purrr_buffer_t*)malloc(sizeof(*internal));
  if (!internal) return NULL;
  memset(internal, 0, sizeof(*internal));
  internal->info = *info;
  internal->renderer = (_purrr_renderer_t*)renderer;

  switch (internal->renderer->api) {
  case PURRR_API_VULKAN: {
    internal->init = _purrr_buffer_vulkan_init;
    internal->cleanup = _purrr_buffer_vulkan_cleanup;
    internal->copy = _purrr_buffer_vulkan_copy;
    internal->map = _purrr_buffer_vulkan_map;
    internal->unmap = _purrr_buffer_vulkan_unmap;
  } break;
  case COUNT_PURRR_APIS:
  default: {
    assert(0 && "Unreachable");
    return NULL;
  }
  }

  if (!internal->init(internal)) {
    _purrr_buffer_free(internal);
    return NULL;
  }

  internal->initialized = true;

  return (purrr_buffer_t*)internal;
}

void purrr_buffer_destroy(purrr_buffer_t *buffer) {
  if (buffer) _purrr_buffer_free((_purrr_buffer_t*)buffer);
}

bool purrr_buffer_copy(purrr_buffer_t *buffer, void *data, uint32_t size, uint32_t offset) {
  _purrr_buffer_t *internal = (_purrr_buffer_t*)buffer;
  assert(internal && internal->copy);
  return internal->copy(internal, data, size, offset);
}

bool purrr_buffer_map(purrr_buffer_t *buffer, void **data) {
  _purrr_buffer_t *internal = (_purrr_buffer_t*)buffer;
  assert(internal && internal->map);
  return internal->map(internal, data);
}

void purrr_buffer_unmap(purrr_buffer_t *buffer) {
  _purrr_buffer_t *internal = (_purrr_buffer_t*)buffer;
  assert(internal && internal->unmap);
  internal->unmap(internal);
}

// renderer

purrr_renderer_t *purrr_renderer_create(purrr_renderer_info_t *info) {
  if (!info || !info->window) return NULL;

  _purrr_renderer_t *internal = (_purrr_renderer_t*)malloc(sizeof(*internal));
  if (!internal) return NULL;
  memset(internal, 0, sizeof(*internal));
  internal->info = *info;
  internal->api = ((_purrr_window_t*)info->window)->api;

  switch (internal->api) {
  case PURRR_API_VULKAN: {
    internal->init = _purrr_renderer_vulkan_init;
    internal->cleanup = _purrr_renderer_vulkan_cleanup;
    internal->begin_frame = _purrr_renderer_vulkan_begin_frame;
    internal->begin_render_target = _purrr_renderer_vulkan_begin_render_target;
    internal->bind_pipeline = _purrr_renderer_vulkan_bind_pipeline;
    internal->bind_texture = _purrr_renderer_vulkan_bind_texture;
    internal->bind_buffer = _purrr_renderer_vulkan_bind_buffer;
    internal->push_constant = _purrr_renderer_vulkan_push_constant;
    internal->draw = _purrr_renderer_vulkan_draw;
    internal->draw_indexed = _purrr_renderer_vulkan_draw_indexed;
    internal->end_render_target = _purrr_renderer_vulkan_end_render_target;
    internal->end_frame = _purrr_renderer_vulkan_end_frame;
    internal->wait = _purrr_renderer_vulkan_wait;
  } break;
  case COUNT_PURRR_APIS:
  default: {
    assert(0 && "Unreachable");
    return NULL;
  }
  }

  if (!internal->init(internal)) {
    _purrr_renderer_free(internal);
    return NULL;
  }

  internal->initialized = true;

  return (purrr_renderer_t*)internal;
}

void purrr_renderer_destroy(purrr_renderer_t *renderer) {
  if (renderer) _purrr_renderer_free((_purrr_renderer_t*)renderer);
}

void *purrr_renderer_get_user_pointer(purrr_renderer_t *renderer) {
  _purrr_renderer_t *internal = (_purrr_renderer_t*)renderer;
  assert(internal);
  return internal->user_ptr;
}

void purrr_renderer_set_user_pointer(purrr_renderer_t *renderer, void *ptr) {
  _purrr_renderer_t *internal = (_purrr_renderer_t*)renderer;
  assert(internal);
  internal->user_ptr = ptr;
}

void purrr_renderer_set_resize_callback(purrr_renderer_t *renderer, purrr_renderer_resize_cb cb) {
  _purrr_renderer_t *internal = (_purrr_renderer_t*)renderer;
  assert(internal);
  internal->callbacks.resize = cb;
}

void purrr_renderer_begin_frame(purrr_renderer_t *renderer) {
  _purrr_renderer_t *internal = (_purrr_renderer_t*)renderer;
  assert(internal && internal->begin_frame);
  assert(internal->begin_frame(internal));
}

void purrr_renderer_begin_render_target(purrr_renderer_t *renderer, purrr_render_target_t *render_target) {
  _purrr_renderer_t *internal = (_purrr_renderer_t*)renderer;
  assert(internal && internal->begin_render_target && render_target);
  assert(internal->begin_render_target(internal, (_purrr_render_target_t*)render_target));
}

void purrr_renderer_bind_pipeline(purrr_renderer_t *renderer, purrr_pipeline_t *pipeline) {
  _purrr_renderer_t *internal = (_purrr_renderer_t*)renderer;
  assert(internal && internal->bind_pipeline && pipeline);
  assert(internal->bind_pipeline(internal, (_purrr_pipeline_t*)pipeline));
}

void purrr_renderer_bind_texture(purrr_renderer_t *renderer, purrr_texture_t *texture, uint32_t slot_index) {
  _purrr_renderer_t *internal = (_purrr_renderer_t*)renderer;
  assert(internal && internal->bind_texture && texture);
  assert(internal->bind_texture(internal, (_purrr_texture_t*)texture, slot_index));
}

void purrr_renderer_bind_buffer(purrr_renderer_t *renderer, purrr_buffer_t *buffer, uint32_t slot_index) {
  _purrr_renderer_t *internal = (_purrr_renderer_t*)renderer;
  assert(internal && internal->bind_buffer && buffer);
  assert(internal->bind_buffer(internal, (_purrr_buffer_t*)buffer, slot_index));
}

void purrr_renderer_push_constant(purrr_renderer_t *renderer, uint32_t offset, uint32_t size, const void *value) {
  _purrr_renderer_t *internal = (_purrr_renderer_t*)renderer;
  assert(internal && internal->push_constant && value && size);
  assert(internal->push_constant(internal, offset, size, value));
}

void purrr_renderer_draw(purrr_renderer_t *renderer, uint32_t instance_count, uint32_t first_instance, uint32_t vertex_count, uint32_t first_vertex) {
  _purrr_renderer_t *internal = (_purrr_renderer_t*)renderer;
  assert(internal && internal->draw);
  assert(internal->draw(internal, instance_count, first_instance, vertex_count, first_vertex));
}

void purrr_renderer_draw_indexed(purrr_renderer_t *renderer, uint32_t instance_count, uint32_t first_instance, uint32_t index_count, uint32_t first_index, int32_t vertex_offset) {
  _purrr_renderer_t *internal = (_purrr_renderer_t*)renderer;
  assert(internal && internal->draw_indexed);
  assert(internal->draw_indexed(internal, instance_count, first_instance, index_count, first_index, vertex_offset));
}

void purrr_renderer_end_render_target(purrr_renderer_t *renderer) {
  _purrr_renderer_t *internal = (_purrr_renderer_t*)renderer;
  assert(internal && internal->end_render_target);
  assert(internal->end_render_target(internal));
}

void purrr_renderer_end_frame(purrr_renderer_t *renderer) {
  _purrr_renderer_t *internal = (_purrr_renderer_t*)renderer;
  assert(internal && internal->end_frame);
  assert(internal->end_frame(internal));
}

void purrr_renderer_wait(purrr_renderer_t *renderer) {
  _purrr_renderer_t *internal = (_purrr_renderer_t*)renderer;
  assert(internal && internal->wait);
  internal->wait(internal);
}

void purrr_poll_events() {
  glfwPollEvents();
}

// Input:

bool purrr_window_is_key_down(purrr_window_t *window, purrr_key_t key) {
  _purrr_window_t *internal_window = (_purrr_window_t*)window;
  assert(internal_window);
  return glfwGetKey(internal_window->window, key) == GLFW_PRESS;
}

bool purrr_window_is_key_up(purrr_window_t *window, purrr_key_t key) {
  _purrr_window_t *internal_window = (_purrr_window_t*)window;
  assert(internal_window);
  return glfwGetKey(internal_window->window, key) == GLFW_RELEASE;
}

bool purrr_window_is_mouse_button_down(purrr_window_t *window, purrr_mouse_button_t button) {
  _purrr_window_t *internal_window = (_purrr_window_t*)window;
  assert(internal_window);
  return glfwGetMouseButton(internal_window->window, button) == GLFW_PRESS;
}

bool purrr_window_is_mouse_button_up(purrr_window_t *window, purrr_mouse_button_t button) {
  _purrr_window_t *internal_window = (_purrr_window_t*)window;
  assert(internal_window);
  return glfwGetMouseButton(internal_window->window, button) == GLFW_RELEASE;
}

void purrr_window_get_cursor_pos(purrr_window_t *window, double *xpos, double *ypos) {
  _purrr_window_t *internal_window = (_purrr_window_t*)window;
  assert(internal_window);
  glfwGetCursorPos(internal_window->window, xpos, ypos);
}

void purrr_window_set_cursor_mode(purrr_window_t *window, purrr_cursor_mode_t mode) {
  _purrr_window_t *internal_window = (_purrr_window_t*)window;
  assert(internal_window);
  int glfw_mode = 0;
  switch (mode) {
  case PURRR_CURSOR_MODE_NORMAL:
    glfw_mode = GLFW_CURSOR_NORMAL;
    break;
  case PURRR_CURSOR_MODE_HIDDEN:
    glfw_mode = GLFW_CURSOR_HIDDEN;
    break;
  case PURRR_CURSOR_MODE_DISABLED:
    glfw_mode = GLFW_CURSOR_DISABLED;
    break;
  default: assert(0 && "Unreachable");
  }
  glfwSetInputMode(internal_window->window, GLFW_CURSOR, glfw_mode);
}