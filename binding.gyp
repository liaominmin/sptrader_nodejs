{
	'targets': [{
		"target_name":"SpTrader" ,'sources': [ 'SpTrader.cc','SpTraderLogic.hpp' ]
			,"include_dirs": [ "include" ]
			,'libraries': ['-L.','-lapiwrapper']

			,'xcode_settings': {
				'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
					'OTHER_CFLAGS': [ '-fexceptions', '-frtti', ],
			}

		,"cflags_dev": [ "-ggdb","-c","-O3","-std=c++0x","-fexceptions","-frtti","-D_DEBUG=1"]
		,"cflags_prod": [ "-c","-O3","-std=c++0x","-fexceptions","-frtti","-DBOOST_MODE=1","-DBOOST_UTILS=1","-D_DEBUG=0"]
			,"cflags_cc": ["-g","-O0","-ggdb","-fexceptions","-D__LINUX__=1","-D_DEBUG=1"]
	}]
}
