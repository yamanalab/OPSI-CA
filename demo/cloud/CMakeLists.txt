file(GLOB sources *.cpp)

set(name cloud)
add_executable(${name} ${sources})

target_link_libraries(${name} opsica_cloud ${COMMON_LIBS})
