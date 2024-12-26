#ifndef   PURRR_INTERNAL_H_
#define   PURRR_INTERNAL_H_

#include "purrr/purrr.h"

#include <vulkan/vulkan.h>
#include <glfw/glfw3.h>

#include <stdlib.h>
#include <string.h>

typedef struct {
  purrr_api_t api;
  GLFWwindow *window;
  GLFWimage cursors[2];
  purrr_window_callbacks_t callbacks;
  void *user_ptr;
} _purrr_window_t;

typedef struct {
  GLFWcursor *cursor;
  uint8_t *pixels;
} _purrr_cursor_t;



typedef struct _purrr_sampler_s _purrr_sampler_t;
typedef bool (*_purrr_sampler_init_t)(_purrr_sampler_t *);
typedef void (*_purrr_sampler_cleanup_t)(_purrr_sampler_t *);

typedef struct _purrr_texture_s _purrr_texture_t;
typedef bool (*_purrr_texture_init_t)(_purrr_texture_t *);
typedef void (*_purrr_texture_cleanup_t)(_purrr_texture_t *);
typedef bool (*_purrr_texture_load_t)(_purrr_texture_t *, uint8_t *, uint32_t, uint32_t);
typedef bool (*_purrr_texture_copy_t)(_purrr_texture_t *, _purrr_texture_t *, uint32_t, uint32_t);

typedef struct _purrr_pipeline_descriptor_s _purrr_pipeline_descriptor_t;
typedef bool (*_purrr_pipeline_descriptor_init_t)(_purrr_pipeline_descriptor_t *);
typedef void (*_purrr_pipeline_descriptor_cleanup_t)(_purrr_pipeline_descriptor_t *);

typedef struct _purrr_shader_s _purrr_shader_t;
typedef bool (*_purrr_shader_init_t)(_purrr_shader_t *);
typedef void (*_purrr_shader_cleanup_t)(_purrr_shader_t *);

typedef struct _purrr_pipeline_s _purrr_pipeline_t;
typedef bool (*_purrr_pipeline_init_t)(_purrr_pipeline_t *);
typedef void (*_purrr_pipeline_cleanup_t)(_purrr_pipeline_t *);

typedef struct _purrr_render_target_s _purrr_render_target_t;
typedef bool (*_purrr_render_target_init_t)(_purrr_render_target_t *);
typedef void (*_purrr_render_target_cleanup_t)(_purrr_render_target_t *);
typedef _purrr_texture_t *(*_purrr_render_target_get_texture_t)(_purrr_render_target_t *, uint32_t);

typedef struct _purrr_buffer_s _purrr_buffer_t;
typedef bool (*_purrr_buffer_init_t)(_purrr_buffer_t *);
typedef void (*_purrr_buffer_cleanup_t)(_purrr_buffer_t *);
typedef bool (*_purrr_buffer_copy_t)(_purrr_buffer_t *, void *, uint32_t, uint32_t);
typedef bool (*_purrr_buffer_map_t)(_purrr_buffer_t *, void **);
typedef bool (*_purrr_buffer_unmap_t)(_purrr_buffer_t *);

typedef struct _purrr_renderer_s _purrr_renderer_t;
typedef bool (*_purrr_renderer_init_t)(_purrr_renderer_t *);
typedef void (*_purrr_renderer_cleanup_t)(_purrr_renderer_t *);
typedef bool (*_purrr_renderer_begin_frame_t)(_purrr_renderer_t *);
typedef bool (*_purrr_renderer_begin_render_target_t)(_purrr_renderer_t *, _purrr_render_target_t *);
typedef bool (*_purrr_renderer_bind_pipeline_t)(_purrr_renderer_t *, _purrr_pipeline_t *);
typedef bool (*_purrr_renderer_bind_texture_t)(_purrr_renderer_t *, _purrr_texture_t *, uint32_t);
typedef bool (*_purrr_renderer_bind_buffer_t)(_purrr_renderer_t *, _purrr_buffer_t *, uint32_t);
typedef bool (*_purrr_renderer_push_constant_t)(_purrr_renderer_t *, uint32_t, uint32_t, const void *);
typedef bool (*_purrr_renderer_draw_t)(_purrr_renderer_t *, uint32_t, uint32_t, uint32_t, uint32_t);
typedef bool (*_purrr_renderer_draw_indexed_t)(_purrr_renderer_t *, uint32_t, uint32_t, uint32_t, uint32_t, int32_t);
typedef bool (*_purrr_renderer_end_render_target_t)(_purrr_renderer_t *);
typedef bool (*_purrr_renderer_end_frame_t)(_purrr_renderer_t *);
typedef bool (*_purrr_renderer_wait_t)(_purrr_renderer_t *);

// sampler

struct _purrr_sampler_s {
  bool initialized;
  _purrr_renderer_t *renderer;
  purrr_sampler_info_t info;

  _purrr_sampler_init_t init;
  _purrr_sampler_cleanup_t cleanup;

  void *data_ptr;
};

void _purrr_sampler_free(_purrr_sampler_t *sampler);

bool _purrr_sampler_vulkan_init(_purrr_sampler_t *sampler);
void _purrr_sampler_vulkan_cleanup(_purrr_sampler_t *sampler);

// texture

struct _purrr_texture_s {
  bool initialized;
  _purrr_renderer_t *renderer;
  purrr_texture_info_t info;

  uint32_t width, height; // You never can trust user

  _purrr_texture_init_t init;
  _purrr_texture_cleanup_t cleanup;
  _purrr_texture_load_t load;
  _purrr_texture_copy_t copy;

  void *data_ptr;
};

void _purrr_texture_free(_purrr_texture_t *texture);

bool _purrr_texture_vulkan_init(_purrr_texture_t *texture);
void _purrr_texture_vulkan_cleanup(_purrr_texture_t *texture);
bool _purrr_texture_vulkan_load(_purrr_texture_t *dst, uint8_t *src, uint32_t src_width, uint32_t src_height);
bool _purrr_texture_vulkan_copy(_purrr_texture_t *dst, _purrr_texture_t *src, uint32_t src_width, uint32_t src_height);

// pipeline descriptor (render pass)

struct _purrr_pipeline_descriptor_s {
  bool initialized;
  _purrr_renderer_t *renderer;
  purrr_pipeline_descriptor_info_t info;

  _purrr_pipeline_descriptor_init_t init;
  _purrr_pipeline_descriptor_cleanup_t cleanup;

  void *data_ptr;
};

void _purrr_pipeline_descriptor_free(_purrr_pipeline_descriptor_t *pipeline_descriptor);

bool _purrr_pipeline_descriptor_vulkan_init(_purrr_pipeline_descriptor_t *pipeline_descriptor);
void _purrr_pipeline_descriptor_vulkan_cleanup(_purrr_pipeline_descriptor_t *pipeline_descriptor);

// shader

struct _purrr_shader_s {
  bool initialized;
  _purrr_renderer_t *renderer;
  purrr_shader_info_t info;

  purrr_shader_type_t type;

  _purrr_shader_init_t init;
  _purrr_shader_cleanup_t cleanup;

  void *data_ptr;
};

void _purrr_shader_free(_purrr_shader_t *shader);

bool _purrr_shader_vulkan_init(_purrr_shader_t *shader);
void _purrr_shader_vulkan_cleanup(_purrr_shader_t *shader);

// pipeline

struct _purrr_pipeline_s {
  bool initialized;
  _purrr_renderer_t *renderer;
  purrr_pipeline_info_t info;

  _purrr_pipeline_init_t init;
  _purrr_pipeline_cleanup_t cleanup;

  void *data_ptr;
};

void _purrr_pipeline_free(_purrr_pipeline_t *pipeline);

bool _purrr_pipeline_vulkan_init(_purrr_pipeline_t *pipeline);
void _purrr_pipeline_vulkan_cleanup(_purrr_pipeline_t *pipeline);

// render target (frame buffer)

struct _purrr_render_target_s {
  bool initialized;
  _purrr_renderer_t *renderer;

  _purrr_texture_t **textures;
  uint32_t texture_count;

  uint32_t width, height;
  _purrr_pipeline_descriptor_t *descriptor;

  _purrr_render_target_init_t init;
  _purrr_render_target_cleanup_t cleanup;
  _purrr_render_target_get_texture_t get_texture;

  void *data_ptr;
};

void _purrr_render_target_free(_purrr_render_target_t *render_target);

bool _purrr_render_target_vulkan_init(_purrr_render_target_t *render_target);
void _purrr_render_target_vulkan_cleanup(_purrr_render_target_t *render_target);
_purrr_texture_t *_purrr_render_target_vulkan_get_texture(_purrr_render_target_t *render_target, uint32_t texture_index);

// buffer

struct _purrr_buffer_s {
  bool initialized;
  _purrr_renderer_t *renderer;
  purrr_buffer_info_t info;

  _purrr_buffer_init_t init;
  _purrr_buffer_cleanup_t cleanup;
  _purrr_buffer_copy_t copy;
  _purrr_buffer_map_t map;
  _purrr_buffer_unmap_t unmap;

  void *data_ptr;
};

void _purrr_buffer_free(_purrr_buffer_t *buffer);

bool _purrr_buffer_vulkan_init(_purrr_buffer_t *buffer);
void _purrr_buffer_vulkan_cleanup(_purrr_buffer_t *buffer);
bool _purrr_buffer_vulkan_copy(_purrr_buffer_t *buffer, void *data, uint32_t size, uint32_t offset);
bool _purrr_buffer_vulkan_map(_purrr_buffer_t *buffer, void **data);
bool _purrr_buffer_vulkan_unmap(_purrr_buffer_t *buffer);

// renderer

struct _purrr_renderer_s {
  bool initialized;
  purrr_renderer_info_t info;
  purrr_api_t api;

  _purrr_renderer_init_t init;
  _purrr_renderer_cleanup_t cleanup;
  _purrr_renderer_begin_frame_t begin_frame;
  _purrr_renderer_begin_render_target_t begin_render_target;
  _purrr_renderer_bind_pipeline_t bind_pipeline;
  _purrr_renderer_bind_texture_t bind_texture;
  _purrr_renderer_bind_buffer_t bind_buffer;
  _purrr_renderer_push_constant_t push_constant;
  _purrr_renderer_draw_t draw;
  _purrr_renderer_draw_indexed_t draw_indexed;
  _purrr_renderer_end_render_target_t end_render_target;
  _purrr_renderer_end_frame_t end_frame;
  _purrr_renderer_wait_t wait;

  struct {
    purrr_renderer_resize_cb resize;
  } callbacks;

  void *user_ptr;
  void *data_ptr;
};

void _purrr_renderer_free(_purrr_renderer_t *renderer);

bool _purrr_renderer_vulkan_init(_purrr_renderer_t *renderer);
void _purrr_renderer_vulkan_cleanup(_purrr_renderer_t *renderer);
bool _purrr_renderer_vulkan_begin_frame(_purrr_renderer_t *renderer);
bool _purrr_renderer_vulkan_begin_render_target(_purrr_renderer_t *renderer, _purrr_render_target_t *render_target);
bool _purrr_renderer_vulkan_bind_pipeline(_purrr_renderer_t *renderer, _purrr_pipeline_t *pipeline);
bool _purrr_renderer_vulkan_bind_texture(_purrr_renderer_t *renderer, _purrr_texture_t *texture, uint32_t slot_index);
bool _purrr_renderer_vulkan_bind_buffer(_purrr_renderer_t *renderer, _purrr_buffer_t *buffer, uint32_t slot_index);
bool _purrr_renderer_vulkan_push_constant(_purrr_renderer_t *renderer, uint32_t offset, uint32_t size, const void *value);
bool _purrr_renderer_vulkan_draw(_purrr_renderer_t *renderer, uint32_t instance_count, uint32_t first_instance, uint32_t vertex_count, uint32_t first_vertex);
bool _purrr_renderer_vulkan_draw_indexed(_purrr_renderer_t *renderer, uint32_t instance_count, uint32_t first_instance, uint32_t index_count, uint32_t first_index, int32_t vertex_offset);
bool _purrr_renderer_vulkan_end_render_target(_purrr_renderer_t *renderer);
bool _purrr_renderer_vulkan_end_frame(_purrr_renderer_t *renderer);
bool _purrr_renderer_vulkan_wait(_purrr_renderer_t *renderer);

#endif // PURRR_INTERNAL_H_