find_path(ONNXRUNTIME_INCLUDE_DIR
        onnxruntime_cxx_api.h
        PATHS ${ONNXRUNTIME_ROOT}
        PATH_SUFFIXES include
)

find_library(ONNXRUNTIME_LIBRARY
        NAMES onnxruntime
        PATHS ${ONNXRUNTIME_ROOT}
        PATH_SUFFIXES lib
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
        OnnxRuntime
        REQUIRED_VARS
        ONNXRUNTIME_LIBRARY
        ONNXRUNTIME_INCLUDE_DIR
)

add_library(onnxruntime::onnxruntime SHARED IMPORTED)

if(WIN32)
    set_target_properties(onnxruntime::onnxruntime PROPERTIES
            IMPORTED_IMPLIB "${ONNXRUNTIME_ROOT}/lib/onnxruntime.lib"
            IMPORTED_LOCATION "${ONNXRUNTIME_ROOT}/lib/onnxruntime.dll"
    )
else()
    set_target_properties(onnxruntime::onnxruntime PROPERTIES
            IMPORTED_LOCATION "${ONNXRUNTIME_LIBRARY}"
    )
endif()

set_target_properties(onnxruntime::onnxruntime PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES
        "${ONNXRUNTIME_INCLUDE_DIR}"
)