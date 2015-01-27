import copy 
import subprocess

env = Environment(CC = 'gcc', 
		CXX = 'g++',
		CXXFLAGS = ['-std=c++11'],
    CPPFLAGS = ['-Wall', '-Wno-unknown-pragmas', '-Wno-deprecated-register', '-Wno-format-security'])

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

# CURL Paramters
curlCFlags = subprocess.check_output(["curl-config", "--cflags"])
env.Append(CPPPATH = curlCFlags.split())

curlLibFlags = subprocess.check_output(["curl-config", "--libs"])
curlLibFlags = curlLibFlags.split()
for lib in curlLibFlags:
  if lib[1] == 'L':
    env.Append(LIBPATH = [lib[2:]])
  elif lib[1] == 'l':
    env.Append(LIBS = [lib[2:]])

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
