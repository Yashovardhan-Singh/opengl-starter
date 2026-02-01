project('opengl-workshop', 'c')

cc = meson.get_compiler('c')

glfw_dep = dependency('glfw3', required: true)
libm = cc.find_library('m')

inc_dir = []
subdir('vendor')
subdir('src')
