add_executable(memory)

# must match with pio filename and executable name from above
pico_generate_pio_header(memory ${CMAKE_CURRENT_LIST_DIR}/hsync.pio)
pico_generate_pio_header(memory ${CMAKE_CURRENT_LIST_DIR}/vsync.pio)
pico_generate_pio_header(memory ${CMAKE_CURRENT_LIST_DIR}/rgb.pio)
pico_generate_pio_header(memory ${CMAKE_CURRENT_LIST_DIR}/rgb2.pio)

# must match with executable name and source file names
target_sources(memory PRIVATE main.cpp vga_graphics.cpp vga_graphics.h)

# Add pico_multicore which is required for multicore functionality
target_link_libraries(memory pico_stdlib pico_multicore pico_bootsel_via_double_reset hardware_pwm hardware_irq hardware_dma hardware_adc hardware_pio hardware_i2c)

# create map/bin/hex file etc.
pico_add_extra_outputs(memory)