


file(GLOB_RECURSE VoiceSDK_TESTS_SOURCES 
  ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp 
  ${CMAKE_CURRENT_LIST_DIR}/src/*.c)
file(GLOB_RECURSE VoiceSDK_TESTS_HEADERS 
  ${CMAKE_CURRENT_LIST_DIR}/src/*.h 
  ${CMAKE_CURRENT_LIST_DIR}/src/*.hpp)

  add_executable(VoiceSDK_tests ${VoiceSDK_TESTS_SOURCES} ${VoiceSDK_TESTS_HEADERS})
  target_link_libraries(VoiceSDK_tests PRIVATE VoiceSDK)
  add_test(
    NAME VoiceSDK_tests 
    COMMAND $<TARGET_FILE:VoiceSDK_tests>
  )
endif()