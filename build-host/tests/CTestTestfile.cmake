# CMake generated Testfile for 
# Source directory: /Users/nebex/w/dq3/hf-sfm-flow-meter-driver/tests
# Build directory: /Users/nebex/w/dq3/hf-sfm-flow-meter-driver/build-host/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(hf_sfm_host_tests "/Users/nebex/w/dq3/hf-sfm-flow-meter-driver/build-host/tests/hf_sfm_host_tests")
set_tests_properties(hf_sfm_host_tests PROPERTIES  _BACKTRACE_TRIPLES "/Users/nebex/w/dq3/hf-sfm-flow-meter-driver/tests/CMakeLists.txt;20;add_test;/Users/nebex/w/dq3/hf-sfm-flow-meter-driver/tests/CMakeLists.txt;0;")
add_test(hf_sfm_install_consumer "/opt/homebrew/bin/cmake" "-DBUILD_DIR=/Users/nebex/w/dq3/hf-sfm-flow-meter-driver/build-host" "-DSOURCE_DIR=/Users/nebex/w/dq3/hf-sfm-flow-meter-driver" "-DCXX_COMPILER=/usr/bin/c++" "-P" "/Users/nebex/w/dq3/hf-sfm-flow-meter-driver/tests/install/run_install_test.cmake")
set_tests_properties(hf_sfm_install_consumer PROPERTIES  _BACKTRACE_TRIPLES "/Users/nebex/w/dq3/hf-sfm-flow-meter-driver/tests/CMakeLists.txt;36;add_test;/Users/nebex/w/dq3/hf-sfm-flow-meter-driver/tests/CMakeLists.txt;0;")
