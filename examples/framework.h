#ifndef   _FRAMEWORK_H_
#define   _FRAMEWORK_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <purrr/purrr.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

typedef struct {
  purrr_window_callbacks_t *callbacks;
  purrr_window_t *window;
  purrr_image_t **swapchain_images;
  purrr_image_t *color_images[2];
  purrr_format_t swapchain_format;
  purrr_renderer_t *renderer;
  purrr_sample_count_t sample_count;
  purrr_pipeline_descriptor_t *pipeline_descriptor;
  purrr_render_target_t *render_targets[2];
  purrr_render_target_t *current_render_target;
} renderer_t;

bool create_renderer(renderer_t *renderer, purrr_window_options_t options, int width, int height, const char *name, bool vsync);
void renderer_begin(renderer_t *renderer);
void renderer_end(renderer_t *renderer);
void free_renderer(renderer_t *renderer);

#ifdef FRAMEWORK_IMPLEMENTATION

bool create_swapchain(renderer_t *renderer) {
  uint32_t swapchain_width = 0, swapchain_height = 0;
  purrr_window_get_size(renderer->window, &swapchain_width, &swapchain_height);

  for (uint32_t i = 0; i < 2; ++i) {
    purrr_image_info_t info = {
      .width = swapchain_width,
      .height = swapchain_height,
      .format = renderer->swapchain_format,
      .sample_count = renderer->sample_count,
    };
    renderer->color_images[i] = purrr_image_create(&info, renderer->renderer);
    assert(renderer->color_images[i]);
  }

  purrr_pipeline_descriptor_attachment_info_t color_attachment = {
    .format = renderer->swapchain_format,
    .load = false,
    .store = true,
    .present_src = !renderer->sample_count,
    .sample_count = renderer->sample_count,
  };

  purrr_pipeline_descriptor_attachment_info_t resolve_attachment = {
    .format = renderer->swapchain_format,
    .load = false,
    .store = true,
    .present_src = true,
  };

  purrr_pipeline_descriptor_info_t pipeline_descriptor_info = {
    .color_attachments = &color_attachment,
    .resolve_attachments = &resolve_attachment,
    .color_attachment_count = 1,
  };

  renderer->pipeline_descriptor = purrr_pipeline_descriptor_create(&pipeline_descriptor_info, renderer->renderer);
  if (!renderer->pipeline_descriptor) return false;

  purrr_render_target_info_t render_target_info = {
    .pipeline_descriptor = renderer->pipeline_descriptor,
    .width = swapchain_width,
    .height = swapchain_height,
  };

  for (uint32_t i = 0; i < 2; ++i) {
    purrr_image_t *images[2] = {
      renderer->color_images[i],
      renderer->swapchain_images[i],
    };
    render_target_info.images = images;

    renderer->render_targets[i] = purrr_render_target_create(&render_target_info, renderer->renderer);
    if (!renderer->render_targets[i]) return false;
  }

  return true;
}

void cleanup_swapchain(renderer_t *renderer) {
  if (renderer->pipeline_descriptor) purrr_pipeline_descriptor_destroy(renderer->pipeline_descriptor);
  renderer->pipeline_descriptor = NULL;

  for (size_t i = 0; i < 2; ++i) {
    if (renderer->color_images[i]) purrr_image_destroy(renderer->color_images[i]);
    renderer->color_images[i] = NULL;
    if (renderer->render_targets[i]) purrr_render_target_destroy(renderer->render_targets[i]);
    renderer->render_targets[i] = NULL;
  }
}

void resize_renderer(purrr_renderer_t *renderer) {
  void *user_ptr = purrr_renderer_get_user_pointer(renderer);
  if (!user_ptr) return;
  renderer_t *renderer_ptr = (renderer_t*)user_ptr;
  cleanup_swapchain(renderer_ptr);
  create_swapchain(renderer_ptr);
}

bool create_renderer(renderer_t *renderer, purrr_window_options_t options, int width, int height, const char *name, bool vsync) {
  assert(renderer);

  purrr_window_info_t window_info = {
    .options = options,
    .api = PURRR_API_VULKAN,
    .title = name,
    .width = width,
    .height = height,
    .x = PURRR_WINDOW_POS_CENTER,
    .y = PURRR_WINDOW_POS_CENTER,
    .callbacks_ptr = &renderer->callbacks,
  };

  renderer->window = purrr_window_create(&window_info);
  if (!renderer->window) return false;

  purrr_renderer_info_t renderer_info = {
    .window = renderer->window,
    .vsync = true,
    .image_count = 2,
    .swapchain_format = &renderer->swapchain_format,
    .swapchain_images = &renderer->swapchain_images,
  };

  renderer->renderer = purrr_renderer_create(&renderer_info);
  if (!renderer->renderer) return false;

  purrr_sample_count_t *sample_counts = NULL;
  uint32_t sample_count_count = purrr_renderer_get_sample_counts(renderer->renderer, &sample_counts);
  assert(sample_count_count);
  renderer->sample_count = sample_counts[sample_count_count-1];

  purrr_renderer_set_user_pointer(renderer->renderer, renderer);
  purrr_renderer_set_resize_callback(renderer->renderer, &resize_renderer);

  if (!create_swapchain(renderer)) return false;

  return true;
}

void renderer_begin(renderer_t *renderer) {
  uint32_t image_index = 0;
  purrr_renderer_begin_frame(renderer->renderer, &image_index);
  renderer->current_render_target = renderer->render_targets[image_index];
}

void renderer_end(renderer_t *renderer) {
  purrr_renderer_end_frame(renderer->renderer);
  purrr_poll_events();
}

void free_renderer(renderer_t *renderer) {
  cleanup_swapchain(renderer);
  purrr_renderer_destroy(renderer->renderer);
  purrr_window_destroy(renderer->window);
}

#endif // FRAMEWORK_IMPLEMENTATION

#endif // _FRAMEWORK_H_