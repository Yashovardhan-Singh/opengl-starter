inc_dir = [include_directories('.')]

glad_lib = static_library(
  'glad',
  'glad/glad.c',
  include_directories: inc_dir
)

glad_dep = declare_dependency(
  link_with: glad_lib,
  include_directories: inc_dir
)
