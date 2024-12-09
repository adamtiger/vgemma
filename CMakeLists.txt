cmake_minimum_required(VERSION 3.25)

project(VGemma CXX)
message("Start generating!")

option(BUILD_WITH_ASSERT "Whether to assert when needed" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Vulkan REQUIRED)

IF (BUILD_WITH_ASSERT)
    add_compile_definitions(AC_WITH_ASSERT)
	message("Compile with asserts")
ENDIF(BUILD_WITH_ASSERT)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(VGEMMA_ROOT ${CMAKE_CURRENT_LIST_DIR})


# shaders
set(SHADER_DIR ${VGEMMA_ROOT}/shaders)

set (SHADER_FILES
    ${SHADER_DIR}/vk_op_relu_f32.comp
)

source_group("shaders" FILES ${SHADER_FILES})

# srcs

set(SRCS_DIR ${VGEMMA_ROOT}/srcs)

set (SRCS_SRCS
	${SRCS_DIR}/tensor.cpp
    ${SRCS_DIR}/vk_context.cpp
    ${SRCS_DIR}/vk_op.cpp
    ${SRCS_DIR}/vk_op_unary.cpp
)

set (SRCS_HEADERS
    ${SRCS_DIR}/core.hpp
	${SRCS_DIR}/tensor.hpp
    ${SRCS_DIR}/vk_context.hpp
    ${SRCS_DIR}/vk_op.hpp
    ${SRCS_DIR}/vk_op_unary.hpp
)

source_group("srcs" FILES ${SRCS_HEADERS} ${SRCS_SRCS})

# tests

set(TEST_DIR ${VGEMMA_ROOT}/tests)

set (TEST_SRCS
    ${TEST_DIR}/main.cpp
)

set (TEST_HEADERS
)

source_group("tests" FILES ${TEST_HEADERS} ${TEST_SRCS})


# set all sources
set (SOURCES
    ${SRCS_SRCS}
    ${TEST_SRCS}
)

set (HEADERS
    ${SRCS_HEADERS}
    ${TEST_HEADERS}
)

if (MSVC OR CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
endif()

add_executable(vgemma ${SOURCES} ${HEADERS} ${SHADER_FILES})
target_include_directories(
    vgemma PUBLIC ${Vulkan_INCLUDE_DIR}
	${SRCS_DIR} ${TEST_DIR} 
)

if (MSVC)
    target_link_options(vgemma PUBLIC "/NODEFAULTLIB:libcmt.lib")
endif()

target_link_libraries(vgemma PUBLIC ${Vulkan_LIBRARIES})

message("Finished")
