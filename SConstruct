import copy 
import subprocess

def ParseCPPFlags(env, flagString):
  env.Append(CPPPATH = flagString.split())

def ParseLibFlags(env, flagString):
  flagString = flagString.split()
  for lib in flagString:
    if lib[1] == 'L':
      env.Append(LIBPATH = [lib[2:]])
    elif lib[1] == 'l':
      env.Append(LIBS = [lib[2:]])

env = Environment(CC = 'gcc', 
		CXX = 'g++',
		CXXFLAGS = ['-std=c++14'],
    CPPFLAGS = ['-Wall', '-Wno-unknown-pragmas', '-Wno-format-security'])

env.Append(LIBS = ['dl', 'pthread'])

# OpenCV Parameters
env.Append(LIBS = ['opencv_core', 'opencv_highgui', 'opencv_imgproc', 'opencv_imgcodecs', 'opencv_features2d', 'opencv_flann'])

# Poco Parameters
env.Append(LIBS = ['PocoNet', 'PocoFoundation'])

# Tesseract Parameters 
env.Append(LIBS = ['tesseract', 'lept'])

# Crypto++ Paramters
env.Append(CXXFLAGS = ['-I/usr/include/cryptopp'])
env.Append(LIBS = ['cryptopp'])

# CURL Paramters
curlCFlags = subprocess.check_output(["curl-config", "--cflags"])
ParseCPPFlags(env, curlCFlags)

curlLibFlags = subprocess.check_output(["curl-config", "--libs"])
ParseLibFlags(env, curlLibFlags)

# GStreamer Parameters
gstreamerCFlags = subprocess.check_output(["pkg-config", "--cflags", "gstreamer-1.0"])
ParseCPPFlags(env, gstreamerCFlags)

gstreamerLFlags = subprocess.check_output(["pkg-config", "--libs", "gstreamer-1.0"])
ParseLibFlags(env, gstreamerLFlags)

# LibSVM Parameters (TODO: Remove)
env.Append(LIBS = ['svm'])

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
