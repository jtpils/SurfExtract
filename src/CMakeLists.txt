# Main cmake file 
cmake_minimum_required(VERSION 2.6)




file(GLOB GLUtils_SRC
    "*.h"
    "*.cpp"
)

source_group(src FILES ${GLUtils_SRC})



add_compile_definitions(_WITH_CAMERA)
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_definitions(-DWITH_EXPORT)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${OutputDir})
set(CMAKE_DEBUG_POSTFIX d)


link_directories(${CMAKE_SOURCE_DIR}/dep/lib)

# Create an executable
add_executable(${PROJECT_NAME}  ${GLUtils_SRC} )

set(GLSupport
	debug GLUtilsd.lib
	optimized GLUtils.lib
)




# Add libraries
target_link_libraries(${PROJECT_NAME}   ${GLEW_LIBRARIES} ${GLEW_LIBRARY} ${GLFW3_LIBRARY} ${OPENGL_LIBRARIES}  ${OPENCV_LIBRARIES}  ${GLSupport} )


