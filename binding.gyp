{
	'targets': [
	{
		'target_name':'sptrader',
			'sources': [
				'sptrader.cc','SpTraderLogic.hpp'
			],
			"include_dirs": [
				"include"
			],
			'libraries': ['~/sptrader_tool/libapiwrapper.so'],

			"cflags_ok2": [ "-ggdb","-c","-O3","-std=c++0x","-fexceptions","-frtti"],
			"cflags_test1": ['-fexceptions', '-Wall', '-D_FILE_OFFSET_BITS=64','-D_LARGEFILE_SOURCE', '-O2'],
			"cflags_ok3!": ['-fno-exceptions'],
			"cflags_cc_debug": [ "-D__LINUX__=1","-DBOOST_MODE=1","-DBOOST_UTILS=1","-D_DEBUG=1"],
			"cflags!_cc_debug": ['-fno-exceptions'],

			"cflags_cc": ["-fexceptions","-D__LINUX__=1","-DBOOST_MODE=1","-DBOOST_UTILS=1","-D_DEBUG=0"]
	}
	]
}
