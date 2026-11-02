# =============================================================================
# hf-sfm-flow-meter-driver — Root CMakeLists.txt
# =============================================================================
# Hardware-agnostic header-only I2C driver for Sensirion SF06-family gas mass
# flow meters (SFM4300 data sheet §4 command set; also SFM3003/3013/3019/3119).
# Integrate via cmake/hf_sfm_build_settings.cmake or the ESP-IDF component
# under examples/esp32/components/hf_sfm.
# =============================================================================
cmake_minimum_required(VERSION 3.16)

project(hf_sfm
  VERSION 1.0.0
  LANGUAGES CXX
)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/hf_sfm_build_settings.cmake)

include(GNUInstallDirs)

add_library(${HF_SFM_TARGET_NAME} INTERFACE)
foreach(dir IN LISTS HF_SFM_PUBLIC_INCLUDE_DIRS)
  target_include_directories(${HF_SFM_TARGET_NAME} INTERFACE $<BUILD_INTERFACE:${dir}>)
endforeach()
target_include_directories(${HF_SFM_TARGET_NAME}
    INTERFACE $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/hf_sfm>)
target_compile_features(${HF_SFM_TARGET_NAME} INTERFACE cxx_std_17)

add_library(hf::sfm ALIAS ${HF_SFM_TARGET_NAME})

option(HF_SFM_ENABLE_WARNINGS "Enable compiler warnings for consumers" OFF)
if(HF_SFM_ENABLE_WARNINGS)
  target_compile_options(${HF_SFM_TARGET_NAME}
      INTERFACE -Wall -Wextra -Wpedantic)
endif()

option(HF_SFM_BUILD_TESTS "Build the host unit tests in tests/ (no hardware)" OFF)
if(HF_SFM_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/hf_sfmConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/hf_sfmConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/hf_sfmConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/hf_sfm
)

# Install: headers (with the generated sfm_version.h) under include/hf_sfm, the hf::sfm target and the package
# files, so find_package(hf_sfm) works from an install prefix.
set_target_properties(${HF_SFM_TARGET_NAME} PROPERTIES EXPORT_NAME sfm)
install(TARGETS ${HF_SFM_TARGET_NAME} EXPORT hf_sfmTargets)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/inc/"
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/hf_sfm
  FILES_MATCHING PATTERN "*.hpp")
install(FILES "${HF_SFM_VERSION_HEADER}" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/hf_sfm)
install(EXPORT hf_sfmTargets
  NAMESPACE hf::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/hf_sfm)
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/hf_sfmConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/hf_sfmConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/hf_sfm)
