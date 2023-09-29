add_executable(boids)

add_compile_options(-Ofast)

# must match with pio filename and executable name from above
pico_generate_pio_header(boids ${CMAKE_CURRENT_LIST_DIR}/../Animation_Demo/hsync.pio)
pico_generate_pio_header(boids ${CMAKE_CURRENT_LIST_DIR}/../Animation_Demo//vsync.pio)
pico_generate_pio_header(boids ${CMAKE_CURRENT_LIST_DIR}/../Animation_Demo//rgb.pio)

# must match with executable name and source file names
target_sources(boids PRIVATE boids.c vga_graphics.c)

# must match with executable name
target_link_libraries(boids PRIVATE pico_stdlib pico_divider pico_multicore pico_bootsel_via_double_reset hardware_pio hardware_dma hardware_adc hardware_irq hardware_clocks hardware_pll)

# must match with executable name
pico_add_extra_outputs(boids)
