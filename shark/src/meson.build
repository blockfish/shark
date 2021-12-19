# iitalics

srcs += files(
  'heuristic.cc',
  'matrix.cc',
  'move.cc',
  'queue.cc',
  'tbp/bot.cc',
  'tbp/msg.cc',
  'think.cc',
)

srcs += custom_target(
  'ruleset-data',
  output : 'ruleset_data.cc',
  input : files('./gen-ruleset-data.py', ruleset_json),
  command : [
    python,
    '@INPUT0@',
    '@INPUT1@',
    '@OUTPUT@',
  ]
)
