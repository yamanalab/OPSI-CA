file(GLOB sources *.cpp)

set(name dataownerA)
add_executable(${name} ${sources})

target_link_libraries(${name} opsica_dataowner ${COMMON_LIBS})
