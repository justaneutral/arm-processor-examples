# add_subdirectory(common)

file(
  GLOB
  COMMON_CPP_FILES
  "*.cpp"
  # "common/*.cpp"
  "FFTLIB/*.cpp"
  "DSPLIB/*.cpp"
  "superNode/*.cpp"
  "misc/*.cpp")
file(
  GLOB
  COMMON_H_FILES
  "*.hpp"
  "common/*.hpp"
  "FFTLIB/*.hpp"
  "DSPLIB/*.hpp"
  "misc/*.hpp"
  "superNode/*.hpp"
  "${CMAKE_SOURCE_DIR}/includes/*.hpp")

list(APPEND COMMON_FILES ${COMMON_CPP_FILES} ${COMMON_H_FILES})

add_library(TISP STATIC ${COMMON_FILES})

tisp_set_lib_extension(TISP)

target_include_directories(TISP PUBLIC ${CMAKE_SOURCE_DIR}/includes
                                       ${C7X_INCLUDES} ${DMA_UTILS_INCLUDE_DIR})

message("C7X_INCLUDES: ${C7X_INCLUDES}")
