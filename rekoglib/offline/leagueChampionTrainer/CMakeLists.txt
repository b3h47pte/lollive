cmake_minimum_required(VERSION 3.3)
project("League Champion Trainer")

include_directories("${PROJECT_SOURCE_DIR}/../../")

# Eigen Headers
include_directories("/usr/include/eigen3/")

# Boost Headers
set(Boost_USE_MULTITHREADED     ON)
set(Boost_USE_STATIC_LIBS       OFF)
find_package(Boost 1.58.0 EXACT REQUIRED COMPONENTS log filesystem thread system)
include_directories(${Boost_INCLUDE_DIRS})
add_definitions("-DBOOST_ALL_DYN_LINK")

# OpenCV Headers
find_package(OpenCV REQUIRED)

# DLib
set(DLIB_SOURCE "/usr/include/dlib/all/source.cpp")

set(LCT_CXX_FLAGS "-std=c++14 -Wall")
set(LCT_CXX_FLAGS_EIGEN "-Wno-deprecated-declarations")
add_definitions(${LCT_CXX_FLAGS} ${LCT_CXX_FLAGS_EIGEN})

add_executable(leagueChampionTrainer main.cpp ${DLIB_SOURCE})

# Additional Libraries
target_link_libraries(leagueChampionTrainer pthread X11)

# Boost Link
target_link_libraries(leagueChampionTrainer ${Boost_LIBRARIES})
target_link_libraries(leagueChampionTrainer "boost_log_setup")

# OpenCV Link
target_link_libraries(leagueChampionTrainer ${OpenCV_LIBS})
