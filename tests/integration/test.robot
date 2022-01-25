

*** Settings ***
Suite Setup                   Setup
Suite Teardown                Teardown
Test Setup                    Reset Emulation
Test Teardown                 Test Teardown
Resource                      ${RENODEKEYWORDS}

*** Variables ***
${UART}                       sysbus.uart0
${URI}                        @/home/rodrigo/Cloud/Projects/zephyrproject/zbus/tests/integration/build/zephyr

${PLATFORM}=     SEPARATOR=
...  """                                                 ${\n}
...  using "platforms/cpus/nrf52840.repl"                ${\n}
...  """

*** Keywords ***
Create Machine
	Execute Command          mach create
	Execute Command          machine LoadPlatformDescriptionFromString ${PLATFORM}
	Execute Command          sysbus LoadELF ${URI}/zephyr.elf

*** Test Cases ***
Should Read Boot Banner
	Create Machine
	Create Terminal Tester    ${UART}

	Start Emulation

	Wait For Line On Uart     hello
