add_executable(bird_beep)

# must match with pio filename and executable name from above
pico_generate_pio_header(bird_beep ${CMAKE_CURRENT_LIST_DIR}/hsync.pio)
pico_generate_pio_header(bird_beep ${CMAKE_CURRENT_LIST_DIR}/vsync.pio)
pico_generate_pio_header(bird_beep ${CMAKE_CURRENT_LIST_DIR}/rgb.pio)

# must match with executable name and source file names
target_sources(bird_beep PRIVATE bird_beep.c vga_graphics.c)

# Add pico_multicore which is required for multicore functionality
target_link_libraries(bird_beep PRIVATE pico_stdlib pico_multicore hardware_pio hardware_dma hardware_sync hardware_spi pico_bootsel_via_double_reset)

# create map/bin/hex file etc.
pico_add_extra_outputs(bird_beep)