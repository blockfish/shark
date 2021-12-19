project('shark', ['cpp'], version : '0.1.0')

ruleset_json = get_option('ruleset_json')

srcs = []
hdrs = []
incs = []
deps = []

tiny_json = subproject('tiny-json')
deps += tiny_json.get_variable('dep_static')

json_maker = subproject('json-maker')
deps += json_maker.get_variable('dep_static')

python = find_program('python3')

subdir('src')
subdir('inc')

lib = static_library(
  'blockfish',
  srcs + hdrs,
  include_directories : incs,
  dependencies : deps)

dep = declare_dependency(
  link_with : lib,
  include_directories : incs)
  
