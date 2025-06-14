
function(add_git_version_definition target)
    execute_process(COMMAND git rev-parse --short HEAD
        OUTPUT_VARIABLE GIT_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)
    if("${GIT_VERSION}" STREQUAL "")
        set(GIT_VERSION "unknown")
    endif()
    target_compile_definitions(${target} PRIVATE GIT_VERSION="${GIT_VERSION}")
endfunction()

function(init_git_submodule submodule_path)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${submodule_path}/.git")
        message(STATUS "Initializing Git submodules...")

        execute_process(COMMAND git submodule update --init --recursive ${submodule_path}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE result)
        if(result)
            message(FATAL_ERROR "Failed to initialize Git submodules. Please check your Git configuration.")
        endif()
    endif()
endfunction()

