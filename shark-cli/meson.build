project('shark-cli', ['cpp'], version : '1.0.0')

srcs = files('main.cc')
deps = [dependency('boost')]

shark = subproject('shark')
deps += shark.get_variable('dep')

executable('cli', srcs, dependencies : deps)
  
