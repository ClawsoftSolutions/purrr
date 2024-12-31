#include "internal.h"

#define FREE_FUNC(type, name) \
  void _purrr_##name##_free(type *name) { \
    if (!name || !name->initialized) return; \
    if (name->cleanup) name->cleanup(name); \
    free(name); \
  }

FREE_FUNC(_purrr_sampler_t, sampler)
FREE_FUNC(_purrr_image_t, image)
FREE_FUNC(_purrr_texture_t, texture)
FREE_FUNC(_purrr_pipeline_descriptor_t, pipeline_descriptor)
FREE_FUNC(_purrr_shader_t, shader)
FREE_FUNC(_purrr_pipeline_t, pipeline)
FREE_FUNC(_purrr_render_target_t, render_target)
FREE_FUNC(_purrr_buffer_t, buffer)
FREE_FUNC(_purrr_renderer_t, renderer)