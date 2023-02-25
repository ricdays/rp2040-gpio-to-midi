# Add your source files
add_executable(${DEVICE_NAME}
    main.cpp # <-- Add source files here!
)

target_link_libraries(${DEVICE_NAME} 
    ${MY_LIBRARIES}
)

pico_enable_stdio_usb(${DEVICE_NAME} 1)
pico_enable_stdio_uart(${DEVICE_NAME} 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(${DEVICE_NAME})
