{
	'targets': [{
		"target_name":"SpTrader" ,'sources': [ 'SpTrader.cc','SpTraderLogic.hpp' ]
			,"include_dirs": [ "include" ]
			,'libraries': ['-L.','-lapiwrapper']

			,'xcode_settings': {
				'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
					'OTHER_CFLAGS': [ '-fexceptions', '-frtti', ],
			}

		,"cflags_test": [ "-ggdb","-c","-O3","-std=c++0x","-fexceptions","-frtti"]
			,"cflags_cc": ["-fexceptions","-D__LINUX__=1","-DBOOST_MODE=1","-DBOOST_UTILS=1","-D_DEBUG=0"]
	}]
}
