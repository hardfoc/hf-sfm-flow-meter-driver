# Installs the driver from BUILD_DIR into a scratch prefix, then configures, builds and runs a consumer that
# uses find_package(hf_sfm). Run by CTest (test hf_sfm_install_consumer).
set(prefix "${BUILD_DIR}/install-test/prefix")
set(consumer_build "${BUILD_DIR}/install-test/consumer")
file(REMOVE_RECURSE "${BUILD_DIR}/install-test")

execute_process(COMMAND "${CMAKE_COMMAND}" --install "${BUILD_DIR}" --prefix "${prefix}" RESULT_VARIABLE rc)
if(NOT rc EQUAL 0)
  message(FATAL_ERROR "cmake --install failed (${rc})")
endif()
foreach(f include/hf_sfm/sfm.hpp include/hf_sfm/sfm_driver.hpp include/hf_sfm/sfm_version.h
          lib/cmake/hf_sfm/hf_sfmConfig.cmake lib/cmake/hf_sfm/hf_sfmTargets.cmake)
  if(NOT EXISTS "${prefix}/${f}")
    message(FATAL_ERROR "not installed: ${f}")
  endif()
endforeach()

execute_process(COMMAND "${CMAKE_COMMAND}" -S "${SOURCE_DIR}/tests/install/consumer" -B "${consumer_build}"
                        "-DCMAKE_PREFIX_PATH=${prefix}" "-DCMAKE_CXX_COMPILER=${CXX_COMPILER}"
                RESULT_VARIABLE rc)
if(NOT rc EQUAL 0)
  message(FATAL_ERROR "consumer configure failed (${rc})")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" --build "${consumer_build}" RESULT_VARIABLE rc)
if(NOT rc EQUAL 0)
  message(FATAL_ERROR "consumer build failed (${rc})")
endif()
execute_process(COMMAND "${consumer_build}/consumer" RESULT_VARIABLE rc)
if(NOT rc EQUAL 0)
  message(FATAL_ERROR "consumer run failed (${rc})")
endif()
