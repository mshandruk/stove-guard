add_executable(stove_guard_video_demo main.cpp)
target_link_libraries(stove_guard_video_demo PRIVATE apps_common)

function(copy_runtime_dependencies target dependency)
    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:${dependency}>
            $<TARGET_FILE_DIR:${target}>
    )
endfunction()

copy_runtime_dependencies(
        stove_guard_video_demo
        onnxruntime::onnxruntime
)