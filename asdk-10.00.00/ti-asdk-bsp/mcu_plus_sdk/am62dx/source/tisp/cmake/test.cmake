function(TISP_create_test_binary_name myKernelName)
  if(TARGET_PLATFORM STREQUAL "PC")
    set(exeName
        "test_${myKernelName}_${DEVICE}_x86_64"
        PARENT_SCOPE)
  else()
    set(exeName
        "test_${myKernelName}_${DEVICE}"
        PARENT_SCOPE)
  endif()
endfunction()

function(TISP_create_binary_for_kernel_testing)

  if(TARGET_PLATFORM STREQUAL "PC")
    file(GLOB_RECURSE SRC_CPP_FILES "*.cpp")
    file(GLOB_RECURSE SRC_C_FILES "*.c")
    file(GLOB_RECURSE SRC_H_FILES "*.h")
    set_source_files_properties(${SRC_C_FILES} PROPERTIES LANGUAGE CXX)
    list(APPEND SRC_FILES ${SRC_CPP_FILES} ${SRC_C_FILES} ${SRC_H_FILES})
  else()
    file(GLOB_RECURSE SRC_FILES "*.cpp" "*.c" "*.h")
  endif()
  # message("source test files are: ${SRC_FILES}") file(GLOB_RECURSE SRC_FILES
  # "*.cpp" "*.h")
  get_filename_component(myKernelName "${CMAKE_CURRENT_LIST_DIR}" NAME)
  tisp_create_test_binary_name(${myKernelName})

  # message ("Exe name is ${exeName}") message ("                      ")

  # TODO: is this acceptable, hanging exeName; returned from
  # TISP_create_binary_extension()
  add_executable(${exeName} ${SRC_FILES})
  target_include_directories(${exeName} PUBLIC ${CMAKE_SOURCE_DIR}/includes
                                               ${C7X_INCLUDES} ${C7X_UDMA_ROOT})

  tisp_set_bin_extension(${exeName})

  if(AUTO_TEST)
    target_link_libraries(
      ${exeName} PRIVATE TISP TISP_test_common ${FFTLIB_LIBS} ${DSPLIB_LIBS}
                         ${C7x_LIBS} ${C7X_DMA_LIBS})
  else()
    # target_link_libraries( ${exeName} PRIVATE "${myKernelName}_obj"
    # "${extObjNames}" TISP_common TISP_test_common ${C7x_LIBS} ${C7X_DMA_LIBS})

    target_link_libraries(
      ${exeName}
      PRIVATE "${extObjNames}"
              TISP
              # TISP_common
              TISP_test_common
              ${FFTLIB_LIBS}
              ${DSPLIB_LIBS}
              ${C7x_LIBS}
              ${C7X_DMA_LIBS})

    # target_link_libraries(${exeName} PRIVATE "${extObjNames}" ${C7x_LIBS}
    # ${C7X_DMA_LIBS})
  endif()

  if(TARGET_PLATFORM STREQUAL "PC")
    if(CMAKE_HOST_UNIX)
      target_link_options(${exeName} PRIVATE -coverage -lgcov)
    endif()
  endif()

  if(NOT AUTO_TEST)
    tisp_add_coverage(${exeName})
    set(IDAT_DIR ${CMAKE_SOURCE_DIR}/test/TISP_idat_gen/${myKernelName})
    if(EXISTS ${IDAT_DIR})
      tisp_gen_test_cases(${myKernelName})
      add_dependencies(${exeName} gen_test_case_headers)
    endif()
  endif()

  # target_compile_definitions(${exeName} PUBLIC TEST_CASE=${TEST_CASE}
  # TEST_CATEGORY=${TEST_CATEGORY} )

  if(${TEST_CASE})
    target_compile_definitions(${exeName} PUBLIC TEST_CASE=${TEST_CASE})
  endif()

  if(${TEST_CATEGORY})
    target_compile_definitions(${exeName} PUBLIC TEST_CATEGORY=${TEST_CATEGORY})
  endif()

  if(${ALL_TEST_CASES})
    target_compile_definitions(${exeName} PUBLIC ALL_TEST_CASES)
  elseif(${DEMO_CASE})
    target_compile_definitions(${exeName} PUBLIC DEMO_CASE)
  endif()

  if(NOT TARGET_PLATFORM STREQUAL "PC")
    tisp_set_linker_options_c7x(${exeName})
  endif()

  tisp_finish_compilation(${exeName})

endfunction()
