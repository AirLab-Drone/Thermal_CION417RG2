cmake_minimum_required(VERSION 3.8)

Project(small_thermal)


# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()


find_package(OpenCV REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(image_transport REQUIRED)





include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${rclcpp_INCLUDE_DIRS}) 
include_directories(${std_msgs_INCLUDE_DIRS})  
include_directories(${sensor_msgs_INCLUDE_DIRS})  
include_directories(${cv_bridge_INCLUDE_DIRS})
include_directories(${image_transport_INCLUDE_DIRS})



include_directories(include)
link_directories(lib) # old sdk lib
link_directories(lib/dynamic)
link_directories(lib/static)


# /* ------------------------------ In ros2 node ------------------------------ */
# add_executable(test_thermal_node src/test_thermal_node.cpp)
# target_link_libraries(test_thermal_node 
#     # "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
#     # "$<INSTALL_INTERFACE:include>"
#     ${OpenCV_LIBS}
#     ${rclcpp_LIBRARIES}
#     ${std_msgs_LIBRARIES}
#     ${sensor_msgs_LIBRARIES}
#     # ${cv_bridge_LIBRARIES}  # 這樣不行
#     cv_bridge::cv_bridge      # 這樣會成功==
#     image_transport::image_transport
#     GuideUSBCamera -ldl
#     pthread
#     m
#     udev
# )


# /* ---------------------------------- 可顯示畫面 --------------------------------- */
# add_executable(test_thermal src/test_small_thermal.cpp)
# target_link_libraries(test_thermal 
#     ${OpenCV_LIBS}
#     GuideUSBCamera -ldl
#     pthread
#     m
#     udev
# )



# /* -------------------------------- 官方給的Demo -------------------------------- */
# add_executable(Demo src/Demo.c)
# target_link_libraries(Demo 
#     ${OpenCV_LIBS}
#     GuideUSBCamera -ldl
#     pthread
#     m
# )


# /* ------------------------------ 測試新的SDK Demo ------------------------------ */
add_executable(test_new_SDK_demo src/test_new_SDK_demo.cpp)
target_link_libraries(test_new_SDK_demo 
    ${OpenCV_LIBS}
    # GuideUSBCamera -ldl   # old SDK
    pthread
    m
    libGuideMT.a
    libGuideMT.so
)