project(
  'blockfish-shark',
  ['cpp'],
  version : '0.1.0',
  default_options : ['cpp_std=c++17'],
)

ruleset_json = get_option('blockfish_ruleset_json')

srcs = []
hdrs = []
incs = []
cpp_args = []
link_args = []
deps = []

tiny_json = subproject('tiny-json')
deps += tiny_json.get_variable('dep_static')

json_maker = subproject('json-maker')
deps += json_maker.get_variable('dep_static')

python = find_program('python3')

subdir('src')
subdir('inc')

executable(
  'cli',
  srcs + hdrs,
  cpp_args : cpp_args,
  include_directories : incs,
  link_args : link_args,
  dependencies : deps)
  
