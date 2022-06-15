*** Settings ***
Suite Setup                   Setup
Suite Teardown                Teardown
Test Setup                    Reset Emulation
Test Teardown                 Test Teardown
Resource                      ${RENODEKEYWORDS}

*** Variables ***
${UART}                       sysbus.uart0
#${RESULTS_DIRECTORY}         @${CURDIR}/build/robot 
${DURATION}                   39055  
*** Keywords ***
Create Machine
	Execute Command          include @scripts/single-node/sifive_fe310.resc
	Execute Command          sysbus LoadELF @${CURDIR}/build/zephyr/zephyr.elf 
*** Test Cases ***
Zbus Tests
	Create Machine
	${tester}=    Create Terminal Tester    ${UART}
	Execute Command           emulation RunFor "1"
	Wait For Line On Uart         *** Booting Zephyr OS build zephyr-v3.0.0${SPACE*2}***
	Wait For Line On Uart          Benchmark 1 to 1: SYNC transmission and message size (\\d+)    testerId=${tester}    treatAsRegex=true    timeout=2
	Wait For Line On Uart          - Bytes sent = 262144, received = 262144                                                                     
	Wait For Line On Uart          - Average data rate: (\\d+).(\\d+)B/s                          testerId=${tester}    treatAsRegex=true    timeout=2
	Wait For Line On Uart          - Duration: (\\d+)ms                                           testerId=${tester}    treatAsRegex=true    timeout=2
	Wait For Line On Uart          @(\\d+)                                                        testerId=${tester}    treatAsRegex=true    timeout=2
