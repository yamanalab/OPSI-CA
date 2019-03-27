file(GLOB sources *.cpp)

set(name querier)
add_executable(${name} ${sources})

target_link_libraries(${name} opsica_querier ${COMMON_LIBS})
