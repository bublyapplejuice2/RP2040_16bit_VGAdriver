add_executable(ray_tracer)

# must match with executable name and source file names
target_sources(ray_tracer PRIVATE main.cpp stb_image.h camera.h color.h hittable.h hittable_list.h interval.h material.h ray.h rtw_stb_image.h rtweekend.h sphere.h vec3.h )

# Add pico_multicore which is required for multicore functionality
target_link_libraries(ray_tracer pico_stdlib pico_multicore pico_bootsel_via_double_reset hardware_pwm hardware_irq hardware_dma hardware_adc hardware_pio hardware_i2c)

# create map/bin/hex file etc.
pico_add_extra_outputs(ray_tracer)