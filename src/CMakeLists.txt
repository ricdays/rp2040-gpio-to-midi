# Add your source files
add_executable(${DEVICE_NAME}
    main.cpp # <-- Add source files here!
    usb_descriptors.c
)

pico_enable_stdio_usb(${DEVICE_NAME} 1)
pico_enable_stdio_uart(${DEVICE_NAME} 0)


target_include_directories(${DEVICE_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(${DEVICE_NAME} 
    ${MY_LIBRARIES}
    tinyusb_device tinyusb_board
)

# create map/bin/hex file etc.
pico_add_extra_outputs(${DEVICE_NAME})