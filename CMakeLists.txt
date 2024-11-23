cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 17)

project(VoxelEngine)

if(CMAKE_HOST_APPLE)
    set(PRJ_SHADERS_COMPILER    "${CMAKE_CURRENT_SOURCE_DIR}/build/lib/bgfx/cmake/bgfx/shaderc")

elseif(CMAKE_HOST_WIN32)
    set(PRJ_SHADERS_COMPILER    "${CMAKE_CURRENT_SOURCE_DIR}/build/lib/bgfx/cmake/bgfx/shaderc.exe")

else()
    message(FATAL_ERROR "unknown host")
endif()

if (WIN32)
    set(PRJ_SHADERS_PLATFORM windows)
    set(PRJ_SHADERS_ARGS_VS -p vs_4_0 -O 3)
    set(PRJ_SHADERS_ARGS_FS -p ps_4_0 -O 3)
endif (WIN32)
if (APPLE)
    set(PRJ_SHADERS_PLATFORM osx)
    set(PRJ_SHADERS_ARGS_FS -p metal)
    set(PRJ_SHADERS_ARGS_VS -p metal)
endif (APPLE)



macro(compileShaders SRCS)
    foreach(FILE ${SRCS})
        message(${FILE})
        get_filename_component(ARRAY_NAME "${FILE}" NAME_WE)

        if    ("${FILE}" MATCHES ".*\\.sc")
            continue()
        endif()
        
        if    ("${FILE}" MATCHES ".*\\.fs")
           set(SHADER_TYPE "fragment")
           set(SHADER_ARGS ${PRJ_SHADERS_ARGS_FS})
           set(ARRAY_NAME "${ARRAY_NAME}_fs")
        elseif("${FILE}" MATCHES ".*\\.vs")
            set(SHADER_TYPE "vertex")
            set(SHADER_ARGS ${PRJ_SHADERS_ARGS_VS})
            set(ARRAY_NAME "${ARRAY_NAME}_vs")
        elseif("${FILE}" MATCHES ".*\\.cs")
            set(SHADER_TYPE "compute")
            set(ARRAY_NAME "${ARRAY_NAME}_cs")
        endif()

        list(APPEND gen_src ${FILE}.h)

        add_custom_command(
            OUTPUT ${FILE}.h
            COMMAND ${PRJ_SHADERS_COMPILER}
            -i ${CMAKE_CURRENT_SOURCE_DIR}/lib/bgfx/bgfx/src --type ${SHADER_TYPE} --platform ${PRJ_SHADERS_PLATFORM} ${SHADER_ARGS}
            -f ${FILE} -o ${FILE}.h --bin2c ${ARRAY_NAME}
            DEPENDS ${FILE} ${PRJ_SHADERS_COMPILER}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    endforeach()
endmacro()


file (GLOB ALL_SHADERS
    ${PROJECT_SOURCE_DIR}/res/shaders/*/*.sc
    ${PROJECT_SOURCE_DIR}/res/shaders/*/*.vs
    ${PROJECT_SOURCE_DIR}/res/shaders/*/*.fs
    ${PROJECT_SOURCE_DIR}/res/shaders/*/*.cs
)

compileShaders("${ALL_SHADERS}")

add_custom_target(compileShaders ALL
    DEPENDS ${gen_src}
    COMMAND ${CMAKE_COMMAND} -E echo "Recompiling shaders..."
)

add_subdirectory(lib/bgfx)
add_subdirectory(lib/glfw)

add_executable(game src/main.cpp ${gen_src})
target_link_libraries(game PUBLIC bx bgfx bimg glfw)