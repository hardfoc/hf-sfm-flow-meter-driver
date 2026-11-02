# =============================================================================
# hf-sfm-flow-meter-driver — host unit tests
# =============================================================================
# Builds the driver headers against a fake I2C transport that records writes and replays data-sheet frames,
# and runs the tests with CTest. No hardware and no ESP-IDF needed:
#
#   cmake -S . -B build-host -DHF_SFM_BUILD_TESTS=ON
#   cmake --build build-host
#   ctest --test-dir build-host --output-on-failure
# =============================================================================

add_executable(hf_sfm_host_tests
  host/sfm_driver_test.cpp
)

target_link_libraries(hf_sfm_host_tests PRIVATE hf::sfm)
target_compile_features(hf_sfm_host_tests PRIVATE cxx_std_17)
target_compile_options(hf_sfm_host_tests PRIVATE -Wall -Wextra -Wpedantic)

add_test(NAME hf_sfm_host_tests COMMAND hf_sfm_host_tests)

# The code shown on the documentation site (docs/ includes these files with pymdownx.snippets), built so a
# page cannot show code that does not compile against the current headers. Compiled, never linked or run.
add_library(hf_sfm_doc_snippets OBJECT
  docs/quickstart.cpp
  docs/api_tasks.cpp
  docs/ports.cpp
  docs/configuration.cpp
)
target_link_libraries(hf_sfm_doc_snippets PRIVATE hf::sfm)
target_include_directories(hf_sfm_doc_snippets PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/docs)
target_compile_features(hf_sfm_doc_snippets PRIVATE cxx_std_17)
target_compile_options(hf_sfm_doc_snippets PRIVATE -Wall -Wextra -Wpedantic)
