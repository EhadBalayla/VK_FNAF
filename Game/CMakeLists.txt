#finding the packages
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

#making sure to get the include directories in the Vulkan SDK
if(NOT Vulkan_INCLUDE_DIRS)
    set(Vulkan_INCLUDE_DIRS "$ENV{VULKAN_SDK}/Include")
endif()
message(STATUS "Vulkan include: ${Vulkan_INCLUDE_DIRS}")

file(GLOB_RECURSE SRC_FILES
    src/*.c
    src/*.h
    src/*.cpp
)

add_executable(Game ${SRC_FILES})

target_include_directories(Game PRIVATE
    ${Vulkan_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty
)
target_link_libraries(Game PRIVATE m
    Vulkan::Vulkan
    glfw
)

set_target_properties(Game PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Compiled
	RUNTIME_OUTPUT_NAME "VK_FNAF"
)