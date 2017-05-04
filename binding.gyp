{
	'targets': [{
		"target_name":"SpTrader" ,'sources': [ 'Addon.cc','AddonLogic.hpp' ]
			,"include_dirs": [ "include" ]
			,'libraries': ['-L.','-lapiwrapper']

			,'xcode_settings': {
				'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
					'OTHER_CFLAGS': [ '-fexceptions', '-frtti', ],
			}

		,"cflags": [ "-g3", "-ggdb", "-O0" ]
		,"cflags_dev": [ "-ggdb","-c","-O3","-std=c++0x","-fexceptions","-frtti","-D_DEBUG=1"]
		,"cflags_prod": [ "-c","-O3","-std=c++0x","-fexceptions","-frtti","-DBOOST_MODE=1","-DBOOST_UTILS=1","-D_DEBUG=0"]
			,"cflags_cc": ["-g3","-O0","-ggdb","-fexceptions","-frtti","-D__LINUX__=1","-D_DEBUG=1"]
	}]
}
