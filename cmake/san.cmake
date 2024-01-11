if(ASAN)
  print("Address Sanitizer is enabled")
  include(${CMAKE_CURRENT_LIST_DIR}/san/clang-14_cxx20_asan.cmake)
elseif(LSAN)
  print("Leak Sanitizer is enabled")
  include(${CMAKE_CURRENT_LIST_DIR}/san/clang-14_cxx20_lsan.cmake)
elseif(MSAN)
  print("Memory Sanitizer is enabled")
  include(${CMAKE_CURRENT_LIST_DIR}/san/clang-14_cxx20_msan.cmake)
elseif(TSAN)
  print("Thread Sanitizer is enabled")
  include(${CMAKE_CURRENT_LIST_DIR}/san/clang-14_cxx20_tsan.cmake)
elseif(UBSAN)
  print("Undefined Behavior Sanitizer is enabled")
  include(${CMAKE_CURRENT_LIST_DIR}/san/clang-14_cxx20_ubsan.cmake)
endif()