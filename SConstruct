env = Environment(CC = 'gcc', 
		CXX = 'g++',
		CPPFLAGS = '-std=c++11 -Wall -Wno-unknown-pragmas',
    CPPPATH = ['/usr/include/gstreamer-1.0/', 
             '/usr/include/glib-2.0',
            '/usr/lib64/glib-2.0/include',
            '/usr/include/libxml2',
            '/usr/local/include',
            '/usr/include'],
    LIBS = ['dl', 'gstreamer-1.0', 'gobject-2.0', 'glib-2.0',
            'opencv_core', 'opencv_highgui', 'opencv_imgproc', 'opencv_imgcodecs',
            'tesseract'],
    LIBPATH = ['/usr/lib64', '/usr/local/lib'])

cjson = env.Object(Glob("cjson/*.c"))
civet = env.Object(Glob("civetweb/*.cpp"))
civetc = env.Object(Glob("civetweb/*.c"))
inih = env.Object(Glob("inih/*.cpp"))
inihc = env.Object(Glob("inih/*.c"))

env.Program("lolllive", Glob("*.cpp") + cjson + civet + inih + civetc + inihc)
