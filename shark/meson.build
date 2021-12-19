project(
  'blockfish-shark',
  ['cpp'],
  version : '0.1.0',
  default_options : ['cpp_std=c++98'],
)

srcs = []
cpp_args = []
link_args = []
deps = []

srcs += files('src/main.cc')

executable(
  'cli',
  srcs,
  cpp_args : cpp_args,
  link_args : link_args,
  dependencies : deps)
  
