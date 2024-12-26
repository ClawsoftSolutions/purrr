#ifndef   PURRR_H_
#define   PURRR_H_

#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

typedef enum {
  PURRR_API_VULKAN = 0,
  COUNT_PURRR_APIS
} purrr_api_t;

typedef void *handle_t;

#define PURRR_WINDOW_POS_CENTER UINT32_MAX
#define PURRR_WINDOW_SIZE_DONT_MIND -1
#define PURRR_WINDOW_SIZE_MAX INT32_MAX

// Structures

typedef struct purrr_window_s purrr_window_t;
typedef struct purrr_cursor_s purrr_cursor_t;
typedef struct purrr_renderer_s purrr_renderer_t;
typedef struct purrr_sampler_s purrr_sampler_t;
typedef struct purrr_texture_s purrr_texture_t;
typedef struct purrr_pipeline_descriptor_s purrr_pipeline_descriptor_t;
typedef struct purrr_render_target_s purrr_render_target_t;
typedef struct purrr_shader_s purrr_shader_t;
typedef struct purrr_pipeline_s purrr_pipeline_t;
typedef struct purrr_buffer_s purrr_buffer_t;

// Enums

typedef enum {
  PURRR_STANDARD_CURSOR_ARROW     = 0x00036001,
  PURRR_STANDARD_CURSOR_IBEAM     = 0x00036002,
  PURRR_STANDARD_CURSOR_CROSSHAIR = 0x00036003,
  PURRR_STANDARD_CURSOR_HAND      = 0x00036004,
  PURRR_STANDARD_CURSOR_HRESIZE   = 0x00036005,
  PURRR_STANDARD_CURSOR_VRESIZE   = 0x00036006,
  PURRR_STANDARD_CURSOR_RESIZE    = 0x00036009,

  PURRR_STANDARD_CURSOR_RESIZE_NWSE = 0x00036007,
  PURRR_STANDARD_CURSOR_RESIZE_NESW = 0x00036008,
} purrr_standard_cursor_t;

typedef enum {
  PURRR_FORMAT_UNDEFINED = 0,

  // Color formats
  PURRR_FORMAT_GRAYSCALE,
  PURRR_FORMAT_GRAY_ALPHA,
  PURRR_FORMAT_RGBA8U,
  PURRR_FORMAT_RGBA8RGB,
  PURRR_FORMAT_BGRA8U,
  PURRR_FORMAT_BGRA8RGB,
  PURRR_FORMAT_RGBA16F,
  PURRR_FORMAT_RG32F,
  PURRR_FORMAT_RGB32F,
  PURRR_FORMAT_RGBA32F,
  PURRR_FORMAT_RGBA64F,

  // Depth formats
  PURRR_FORMAT_DEPTH,

  COUNT_PURRR_FORMATS
} purrr_format_t;

typedef enum {
  PURRR_SAMPLER_FILTER_NEAREST = 0,
  PURRR_SAMPLER_FILTER_LINEAR,
  COUNT_PURRR_SAMPLER_FILTERS
} purrr_sampler_filter_t;

typedef enum {
  PURRR_SAMPLER_ADDRESS_MODE_REPEAT = 0,
  PURRR_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
  PURRR_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
  PURRR_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE,
  PURRR_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
  COUNT_PURRR_SAMPLER_ADDRESS_MODES
} purrr_sampler_address_mode_t;

typedef enum {
  PURRR_DESCRIPTOR_TYPE_TEXTURE = 0,
  PURRR_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
  PURRR_DESCRIPTOR_TYPE_STORAGE_BUFFER,
  COUNT_PURRR_DESCRIPTOR_TYPES
} purrr_descriptor_type_t;

typedef enum {
  PURRR_SHADER_TYPE_VERTEX = 0,
  PURRR_SHADER_TYPE_FRAGMENT,
  PURRR_SHADER_TYPE_COMPUTE,
  COUNT_PURRR_SHADER_TYPES
} purrr_shader_type_t;

typedef enum {
  PURRR_BUFFER_TYPE_UNIFORM = 0,
  PURRR_BUFFER_TYPE_STORAGE,
  PURRR_BUFFER_TYPE_VERTEX,
  PURRR_BUFFER_TYPE_INDEX,
  COUNT_PURRR_BUFFER_TYPES
} purrr_buffer_type_t;

// Callbacks

typedef void (*purrr_key_callback_t)(purrr_window_t* window, int key, int scancode, int action, int mods);
typedef void (*purrr_mouse_button_callback_t)(purrr_window_t* window, int button, int action, int mods);
typedef void (*purrr_cursor_position_callback_t)(purrr_window_t* window, double xpos, double ypos);
typedef void (*purrr_window_size_callback_t)(purrr_window_t* window, int width, int height);
typedef bool (*purrr_window_close_callback_t)(purrr_window_t* window);

typedef struct purrr_window_callbacks_s {
  purrr_key_callback_t key;
  purrr_mouse_button_callback_t mouse_button;
  purrr_cursor_position_callback_t cursor_position;
  purrr_window_size_callback_t window_size;
  purrr_window_close_callback_t window_close;
} purrr_window_callbacks_t;

// Catdev ik you will changes this later on
// You're right :3

// Infos

typedef struct {
  uint32_t x, y;
  int32_t width, height;
  const char *title;
  bool fullscreen;
  purrr_api_t api;

  purrr_window_callbacks_t **callbacks_ptr;
} purrr_window_info_t;

typedef struct {
  uint8_t *pixels;
  int width;
  int height;
} purrr_window_icon_info_t;

typedef struct {
  uint8_t *pixels;
  int width;
  int height;
  int xhot;
  int yhot;
} purrr_cursor_info_t;

typedef struct {
  purrr_sampler_filter_t mag_filter;
  purrr_sampler_filter_t min_filter;
  purrr_sampler_address_mode_t address_mode_u;
  purrr_sampler_address_mode_t address_mode_v;
  purrr_sampler_address_mode_t address_mode_w;
} purrr_sampler_info_t;

typedef struct {
  purrr_sampler_t *sampler;
  uint32_t width, height;
  purrr_format_t format;
} purrr_texture_info_t;

typedef struct {
  purrr_format_t format;
  bool load;
  bool store;
  purrr_sampler_t *sampler;
} purrr_pipeline_descriptor_attachment_info_t;

typedef struct {
  purrr_pipeline_descriptor_attachment_info_t *color_attachments;
  uint32_t color_attachment_count;
  purrr_pipeline_descriptor_attachment_info_t *depth_attachment;
} purrr_pipeline_descriptor_info_t;

typedef struct {
  purrr_pipeline_descriptor_t *pipeline_descriptor;
  uint32_t width, height;
} purrr_render_target_info_t;

typedef struct {
  purrr_format_t format;
  uint32_t size;
  uint32_t offset;
} purrr_vertex_info_t;

typedef struct {
  purrr_vertex_info_t *vertex_infos;
  uint32_t vertex_info_count;
} purrr_mesh_binding_info_t;

typedef struct {
  uint32_t offset;
  uint32_t size;
} purrr_pipeline_push_constant_t;

// If filename is not null load from file, else load from buffer.
typedef struct {
  purrr_shader_type_t type;
  const char *filename;
  char *buffer;
  size_t buffer_size;
} purrr_shader_info_t;

typedef struct {
  purrr_shader_t **shaders;
  uint32_t shader_count;

  purrr_mesh_binding_info_t mesh_info;

  purrr_pipeline_descriptor_t *pipeline_descriptor;
  // bool depth;
  purrr_descriptor_type_t *descriptor_slots;
  uint32_t descriptor_slot_count;

  purrr_pipeline_push_constant_t *push_constants;
  uint32_t push_constant_count;
} purrr_pipeline_info_t;

typedef struct {
  purrr_buffer_type_t type;
  uint32_t size;
} purrr_buffer_info_t;

typedef struct {
  purrr_window_t *window;
  bool vsync;

  // Can be null I think
  purrr_format_t *swapchain_format;
  purrr_pipeline_descriptor_t **swapchain_pipeline_descriptor;
  purrr_render_target_t **swapchain_render_targets[2];
  purrr_render_target_t **swapchain_render_target;
} purrr_renderer_info_t;

// Functions

purrr_window_t *purrr_window_create(purrr_window_info_t *info);
void purrr_window_destroy(purrr_window_t *window);
bool purrr_window_should_close(purrr_window_t *window);
void purrr_window_get_size(purrr_window_t *window, uint32_t *width, uint32_t *height);
void purrr_window_set_icons(purrr_window_t *window, purrr_window_icon_info_t *big, purrr_window_icon_info_t *small);
void purrr_window_set_cursor(purrr_window_t *window, purrr_cursor_t *cursor);
void purrr_window_set_user_ptr(purrr_window_t *window, void *user_ptr);
void *purrr_window_get_user_ptr(purrr_window_t *window);

purrr_cursor_t *purrr_cursor_create_standard(purrr_standard_cursor_t type);
purrr_cursor_t *purrr_cursor_create(purrr_cursor_info_t *info);
void purrr_cursor_destroy(purrr_cursor_t *cursor);

purrr_sampler_t *purrr_sampler_create(purrr_sampler_info_t *info, purrr_renderer_t *renderer);
void purrr_sampler_destroy(purrr_sampler_t *sampler);

purrr_texture_t *purrr_texture_create(purrr_texture_info_t *info, purrr_renderer_t *renderer);
void purrr_texture_destroy(purrr_texture_t *texture);
bool purrr_texture_load(purrr_texture_t *dst, uint8_t *src, uint32_t src_width, uint32_t src_height);
bool purrr_texture_copy(purrr_texture_t *dst, purrr_texture_t *src, uint32_t src_width, uint32_t src_height);

purrr_pipeline_descriptor_t *purrr_pipeline_descriptor_create(purrr_pipeline_descriptor_info_t *info, purrr_renderer_t *renderer);
void purrr_pipeline_descriptor_destroy(purrr_pipeline_descriptor_t *pipeline_descriptor);

purrr_render_target_t *purrr_render_target_create(purrr_render_target_info_t *info, purrr_renderer_t *renderer);
purrr_texture_t *purrr_render_target_get_texture(purrr_render_target_t *render_target, uint32_t texture_index);
void purrr_render_target_destroy(purrr_render_target_t *render_target);

purrr_shader_t *purrr_shader_create(purrr_shader_info_t *info, purrr_renderer_t *renderer);
void purrr_shader_destroy(purrr_shader_t *shader);

purrr_pipeline_t *purrr_pipeline_create(purrr_pipeline_info_t *info, purrr_renderer_t *renderer);
void purrr_pipeline_destroy(purrr_pipeline_t *pipeline);

purrr_buffer_t *purrr_buffer_create(purrr_buffer_info_t *info, purrr_renderer_t *renderer);
void purrr_buffer_destroy(purrr_buffer_t *buffer);
bool purrr_buffer_copy(purrr_buffer_t *buffer, void *data, uint32_t size, uint32_t offset);
bool purrr_buffer_map(purrr_buffer_t *buffer, void **data);
void purrr_buffer_unmap(purrr_buffer_t *buffer);

// Callbacks

typedef void (*purrr_renderer_resize_cb)(purrr_renderer_t *);

purrr_renderer_t *purrr_renderer_create(purrr_renderer_info_t *info);
void purrr_renderer_destroy(purrr_renderer_t *renderer);

void *purrr_renderer_get_user_pointer(purrr_renderer_t *renderer);
void  purrr_renderer_set_user_pointer(purrr_renderer_t *renderer, void *ptr);

void purrr_renderer_set_resize_callback(purrr_renderer_t *renderer, purrr_renderer_resize_cb cb);

void purrr_renderer_begin_frame(purrr_renderer_t *renderer);
void purrr_renderer_begin_render_target(purrr_renderer_t *renderer, purrr_render_target_t *render_target);
void purrr_renderer_bind_pipeline(purrr_renderer_t *renderer, purrr_pipeline_t *pipeline);
void purrr_renderer_bind_texture(purrr_renderer_t *renderer, purrr_texture_t *texture, uint32_t slot_index);
void purrr_renderer_bind_buffer(purrr_renderer_t *renderer, purrr_buffer_t *buffer, uint32_t slot_index);
void purrr_renderer_push_constant(purrr_renderer_t *renderer, uint32_t offset, uint32_t size, const void *value);

void purrr_renderer_draw(purrr_renderer_t *renderer, uint32_t instance_count, uint32_t first_instance, uint32_t vertex_count, uint32_t first_vertex);
void purrr_renderer_draw_indexed(purrr_renderer_t *renderer, uint32_t instance_count, uint32_t first_instance, uint32_t index_count, uint32_t first_index, int32_t vertex_offset);

void purrr_renderer_end_render_target(purrr_renderer_t *renderer);
void purrr_renderer_end_frame(purrr_renderer_t *renderer);
void purrr_renderer_wait(purrr_renderer_t *renderer);

void purrr_poll_events();

#include "input.h"

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // PURRR_H_

/*
 * TODO( #1): Add support for other graphics apis
 * TODO( #2): If device doesn't support api selected
              by user in purrr_window_info_t, choose
              other one instead of returning an error
 * TODO( #3): Let user select monitor for window
 * TODO( #4): Change function names to
              purrr_<operation>_<name> (for example:
              purrr_create_renderer)
 * TODO( #5): Add a macro for stripping `purrr`
              prefix
 * TODO( #6): Maybe change name of pipeline
              descriptor?
 * TODO( #7): Better vertex input info.
 */