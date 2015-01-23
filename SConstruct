import copy 
env = Environment(CC = 'gcc', 
		CXX = 'g++',
		CXXFLAGS = ['-std=c++11'],
    CPPFLAGS = ['-Wall', '-Wno-unknown-pragmas', '-Wno-deprecated-register'])

print env['PLATFORM']
if env['PLATFORM'] == 'darwin':
  env.Append(CPPPATH = ['/Library/Frameworks/GStreamer.framework/Versions/1.0/Headers'])
  env.Append(LIBS = ['dl', 'gstreamer-1.0', 'gobject-2.0', 'glib-2.0',
            'opencv_core', 'opencv_highgui', 'opencv_imgproc', 'opencv_imgcodecs',
            'tesseract'])
  env.Append(LIBPATH = ['/Library/Frameworks/GStreamer.framework/Versions/1.0/lib'])
else:
  env.Append(CPPPATH = ['/usr/include/gstreamer-1.0/', 
             '/usr/include/glib-2.0',
            '/usr/lib64/glib-2.0/include',
            '/usr/include/libxml2',
            '/usr/local/include',
            '/usr/include'])
  env.Append(LIBS = ['dl', 'gstreamer-1.0', 'gobject-2.0', 'glib-2.0',
            'opencv_core', 'opencv_highgui', 'opencv_imgproc', 'opencv_imgcodecs',
            'tesseract'])
  env.Append(LIBPATH = ['/usr/lib64', '/usr/local/lib'])

envDebug = env.Clone()

# Environment specific parameters
env.Append(CPPFLAGS = ['-O3'])
envDebug.Append(CPPFLAGS = ['-g', '-O0'])

doDebug = ARGUMENTS.get('debug',0)

if not int(doDebug):
  env.VariantDir('build/release', '.', duplicate=0)
  cjson = env.Object(Glob("build/release/cjson/*.c"))
  civet = env.Object(Glob("build/release/civetweb/*.cpp"))
  civetc = env.Object(Glob("build/release/civetweb/*.c"))
  inih = env.Object(Glob("build/release/inih/*.cpp"))
  inihc = env.Object(Glob("build/release/inih/*.c"))
  main = env.Object(Glob("build/release/*.cpp"))
  release = env.Program("build/release/lollive", main + cjson + civet + inih + civetc + inihc)
else:
  envDebug.VariantDir("build/debug", '.', duplicate=0)
  cjson = envDebug.Object(Glob("build/debug/cjson/*.c"))
  civet = envDebug.Object(Glob("build/debug/civetweb/*.cpp"))
  civetc = envDebug.Object(Glob("build/debug/civetweb/*.c"))
  inih = envDebug.Object(Glob("build/debug/inih/*.cpp"))
  inihc = envDebug.Object(Glob("build/debug/inih/*.c"))
  main = envDebug.Object(Glob("build/debug/*.cpp"))
  debug = envDebug.Program("build/debug/lollive_debug", main + cjson + civet + inih + civetc + inihc)
