cmake_minimum_required(VERSION 3.16.3)
project(SICSA C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fexec-charset=GBK")
add_executable(SICSA main.c test/testConsole.c test/testConsole.h global.c global.h "onboardControlServices/dataInjection.c" "onboardControlServices/dataInjection.h" onboardControlServices/commandParsing.c onboardControlServices/commandParsing.h onboardControlServices/eventTable.c onboardControlServices/eventTable.h onboardControlServices/macroInstruction.c onboardControlServices/macroInstruction.h onboardControlServices/onlineMonitoring.c onboardControlServices/onlineMonitoring.h util.c util.h main.h windowsUtil.c windowsUtil.h test/testMacro.c test/testMacro.h)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fexec-charset=GBK")
