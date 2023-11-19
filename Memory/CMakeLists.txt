add_executable(memory)

# must match with executable name and source file names
target_sources(memory PRIVATE main.cpp)

# Add pico_multicore which is required for multicore functionality
target_link_libraries(memory pico_stdlib pico_multicore pico_bootsel_via_double_reset hardware_pwm hardware_irq hardware_dma hardware_adc hardware_pio hardware_i2c)

# create map/bin/hex file etc.
pico_add_extra_outputs(memory)