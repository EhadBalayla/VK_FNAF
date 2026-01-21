#finding the packages
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Freetype REQUIRED)

file(GLOB_RECURSE SRC_FILES
    src/*.c
    src/*.h
    src/*.cpp
)

add_executable(Game ${SRC_FILES})

target_include_directories(Game PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty
)
target_link_libraries(Game PRIVATE
    Vulkan::Vulkan
    glfw
    Freetype::Freetype
)

set_target_properties(Game PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Compiled
	RUNTIME_OUTPUT_NAME "VK_FNAF"
)
if(MSVC)
    set_target_properties(Game PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "$<1:${CMAKE_CURRENT_SOURCE_DIR}/Compiled>"
    )
endif()