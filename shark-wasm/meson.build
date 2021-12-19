project('shark-wasm', ['cpp'], version : '1.0.0')

srcs = files('main.cc')
deps = []
cpp_args = ['-fno-exceptions']
link_args = ['-fno-exceptions']
link_deps = []

link_args += [
  '-s', 'ENVIRONMENT=worker',
  '-s', 'STANDALONE_WASM',
  '-s', 'FILESYSTEM=0',
  '-s', 'ASSERTIONS=0',
  '--no-entry',
]

subdir('rts')

executable(
  'webworker',
  srcs,
  dependencies : deps,
  link_args : link_args,
  link_depends : link_deps,
)
