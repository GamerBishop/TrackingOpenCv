cmake_minimum_required(VERSION 2.8)

project( trackerVideo )

find_package( OpenCV REQUIRED )

add_executable( trackerVideo trackerVideo.cpp )

target_link_libraries( trackerVideo ${OpenCV_LIBS} )
