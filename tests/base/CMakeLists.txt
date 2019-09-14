
gecko_unit_test(base.cmd_line SOURCES cmdline.cpp LIBS Gecko::Base ARGS "hello" "test_file")
gecko_unit_test(base.any SOURCES any.cpp LIBS Gecko::Base)
