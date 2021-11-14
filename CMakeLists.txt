cmake_minimum_required(VERSION 3.10)

project(execution CXX)
enable_testing()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS Off)
set(DEFAULT_COPTS "-fno-omit-frame-pointer" "-g") # "-fno-builtin-memcpy" "-U_FORTIFY_SOURCE"

set(DEFAULT_LINKOPTS "")
if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64" OR "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "AMD64")
  list(APPEND DEFAULT_COPTS
    #$<$<CONFIG:Debug>:-fsanitize=address,undefined,leak>
    #$<$<CONFIG:Debug>:-fsanitize=thread>
    #$<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Debug>>:-fprofile-instr-generate>
    #$<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Debug>>:-fcoverage-mapping>
    $<$<CXX_COMPILER_ID:GNU>:-ftree-vectorize>
    $<$<CXX_COMPILER_ID:GNU>:-fopt-info-vec-optimized>
    -march=nehalem
  )
  list(APPEND DEFAULT_LINKOPTS
    #$<$<CONFIG:Debug>:-fsanitize=address,undefined,leak>
    #$<$<CONFIG:Debug>:-fsanitize=thread>
    #$<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Debug>>:-fprofile-instr-generate>
    #$<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Debug>>:-fcoverage-mapping>
  )
elseif("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "arm.*|aarch64")
else()
  message(WARNING "Value of CMAKE_SYSTEM_PROCESSOR (${CMAKE_SYSTEM_PROCESSOR}) is unknown")
endif()

add_compile_options(
  ${DEFAULT_COPTS}
)
link_libraries(
  ${DEFAULT_LINKOPTS}
)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_subdirectory(../googletest _build/googletest)

add_library(execution
  bulk.h
  dummy.cpp
  execution.h
  inline_task.h
  intrusive_forward_list.h
  invoke_traits.h
  sync_wait.h
  then.h
  thread_pool.h
)
target_include_directories(execution PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:.>
)
target_link_libraries(execution PUBLIC Threads::Threads)
target_compile_options(execution PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Wformat=2" "-Wconversion")

add_executable(unit_tests
  bulk_tests.cpp
  intrusive_forward_list_tests.cpp
  invoke_traits_tests.cpp
  then_tests.cpp
)
target_link_libraries(unit_tests PRIVATE execution gtest_main)
target_compile_options(unit_tests PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Wformat=2" "-Wconversion")
add_test(unit_tests unit_tests)
