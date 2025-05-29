project(
	'paradis-exp',
	'c',
	default_options: {
		'c_std': 'gnu2x',
	},
	meson_version: '>=1.2.0'
)

pd_inc = include_directories(['include'])

both_libraries(
	'paradis',
	['src/paradis.c'],
	install: false,
	include_directories: [pd_inc],
)

test_0_exe = executable(
	'test0',
	['src/test.c'],
	install: false,
	include_directories: [pd_inc],
	override_options: {
		'buildtype': 'debug',
	},
	c_args: ['-DDEBUG']
)

test(
	'simple_radix_0',
	test_0_exe,
)