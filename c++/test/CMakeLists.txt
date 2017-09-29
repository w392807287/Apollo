file(GLOB SRC *.h *cc)
file(GLOB FRAMEWORK framework/*.cpp framework/*.h)

include_directories(framework)

add_executable(Apollo_Test ${SRC} ${FRAMEWORK})
target_link_libraries(Apollo_Test ${OPENAL_LIBS})

add_custom_command(TARGET Apollo_Test PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/media/stereo.wav ${Apollo_BINARY_DIR}/test/media/stereo.wav)