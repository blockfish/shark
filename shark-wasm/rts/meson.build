# iitalics

rts_pre = custom_target(
  'rts-pre',
  output : 'rts_pre.js',
  input : files('pre.js'),
  command : ['cp', '@INPUT@', '@OUTPUT@'],
)
link_args += ['--pre-js', rts_pre.full_path()]
link_deps += rts_pre

rts_library = custom_target(
  'rts-library',
  output : 'rts_library.js',
  input : files('library.js'),
  command : ['cp', '@INPUT@', '@OUTPUT@'],
)
link_args += ['--js-library', rts_library.full_path()]
link_deps += rts_library

