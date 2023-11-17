//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

// #include "rtweekend.h"

// #include "camera.h"
// #include "color.h"
// #include "hittable_list.h"
// #include "material.h"
// #include "sphere.h"

#include "vga_graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"

// #include "pt_cornell_rp2040_v1.h"


// R = 0-4
// G = 5-10
// B = 11-15

volatile int color_to_use;

void get_mem_addr(int (&gpio_arr)[19], int (&mem_arr)[19]) {
    mem_arr[18] = gpio_arr[8]; // A18 --> GPIO 10
    mem_arr[17] = gpio_arr[0];  // A17 --> GPIO 0
    mem_arr[16] = gpio_arr[1];  // A16 --> GPIO 1
    mem_arr[15] = gpio_arr[2];  // A15 --> GPIO 2
    mem_arr[14] = gpio_arr[9]; // A14 --> GPIO 11
    mem_arr[13] = gpio_arr[10]; // A13 --> GPIO 12
    mem_arr[12] = gpio_arr[11]; // A12 --> GPIO 13
    mem_arr[11] = gpio_arr[12]; // A11 --> GPIO 14
    mem_arr[10] = gpio_arr[13]; // A10 --> GPIO 15
    mem_arr[9]  = gpio_arr[14]; // A9 --> GPIO 16
    mem_arr[8]  = gpio_arr[15]; // A8 --> GPIO 17
    mem_arr[7]  = gpio_arr[16]; // A7 --> GPIO 18
    mem_arr[6]  = gpio_arr[17]; // A6 --> GPIO 19
    mem_arr[5]  = gpio_arr[18]; // A5 --> GPIO 20
    mem_arr[4]  = gpio_arr[7];  // A4 --> GPIO 8
    mem_arr[3]  = gpio_arr[6];  // A3 --> GPIO 7
    mem_arr[2]  = gpio_arr[5];  // A2 --> GPIO 6
    mem_arr[1]  = gpio_arr[4];  // A1 --> GPIO 5
    mem_arr[0]  = gpio_arr[3];  // A0 --> GPIO 4
}

void get_gpio_arr(int (&gpio_arr)[19], int (&mem_arr)[19]) {
    gpio_arr[8]   = mem_arr[18];// A18 --> GPIO 10
    gpio_arr[0]   = mem_arr[17]; // A17 --> GPIO 0
    gpio_arr[1]   = mem_arr[16]; // A16 --> GPIO 1
    gpio_arr[2]   = mem_arr[15]; // A15 --> GPIO 2
    gpio_arr[9]   = mem_arr[14];// A14 --> GPIO 11
    gpio_arr[10]  = mem_arr[13]; // A13 --> GPIO 12
    gpio_arr[11]  = mem_arr[12]; // A12 --> GPIO 13
    gpio_arr[12]  = mem_arr[11]; // A11 --> GPIO 14
    gpio_arr[13]  = mem_arr[10]; // A10 --> GPIO 15
    gpio_arr[14]  = mem_arr[9]; // A9 --> GPIO 16
    gpio_arr[15]  = mem_arr[8]; // A8 --> GPIO 17
    gpio_arr[16]  = mem_arr[7]; // A7 --> GPIO 18
    gpio_arr[17]  = mem_arr[6]; // A6 --> GPIO 19
    gpio_arr[18]  = mem_arr[5]; // A5 --> GPIO 20
    gpio_arr[7]   = mem_arr[4]; // A4 --> GPIO 8
    gpio_arr[6]   = mem_arr[3]; // A3 --> GPIO 7
    gpio_arr[5]   = mem_arr[2]; // A2 --> GPIO 6
    gpio_arr[4]   = mem_arr[1]; // A1 --> GPIO 5
    gpio_arr[3]   = mem_arr[0]; // A0 --> GPIO 4
}

void set_color(int color) {
    if (color < 0 || color >= 65536) {
        printf("color out of range");
        return;
    }
    // for (int i = 0; i < 16; i++) {
    //     gpio_put(i, color & 1);
    //     color >>=1;
    // }
    gpio_put(0, color & 1);
    color >>= 1;
    gpio_put(1, color & 1);
    color >>= 1;
    gpio_put(2, color & 1);
    color >>= 1;
    gpio_put(3, color & 1);
    color >>= 1;
    gpio_put(4, color & 1);
    color >>= 1;
    gpio_put(5, color & 1);
    color >>= 1;
    gpio_put(6, color & 1);
    color >>= 1;
    gpio_put(7, color & 1);
    color >>= 1;
    gpio_put(8, color & 1);
    color >>= 1;
    gpio_put(9, color & 1);
    color >>= 1;
    gpio_put(10, color & 1);
    color >>= 1;
    gpio_put(11, color & 1);
    color >>= 1;
    gpio_put(12, color & 1);
    color >>= 1;
    gpio_put(13, color & 1);
    color >>= 1;
    gpio_put(14, color & 1);
    color >>= 1;
    gpio_put(15, color & 1);
    color >>= 1;
}

// static PT_THREAD (protothread_vga(struct pt *pt)) {
//     PT_BEGIN(pt);
//     for (int i = 0; i < 16; i++) {
//         gpio_init(i);
//         gpio_set_dir(i, GPIO_OUT);
//     }
//     while (true) {
//         if (gpio_get(16) == 0) {
//             set_color(0);
//         } else {
//             set_color(color);
//         }
//     }
//     PT_END(pt);
// }

int main(void) {
    // hittable_list world;

    // auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    // world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    // for (int a = -11; a < 11; a++) {
    //     for (int b = -11; b < 11; b++) {
    //         auto choose_mat = random_double();
    //         point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

    //         if ((center - point3(4, 0.2, 0)).length() > 0.9) {
    //             shared_ptr<material> sphere_material;

    //             if (choose_mat < 0.8) {
    //                 // diffuse
    //                 auto albedo = color::random() * color::random();
    //                 sphere_material = make_shared<lambertian>(albedo);
    //                 world.add(make_shared<sphere>(center, 0.2, sphere_material));
    //             } else if (choose_mat < 0.95) {
    //                 // metal
    //                 auto albedo = color::random(0.5, 1);
    //                 auto fuzz = random_double(0, 0.5);
    //                 sphere_material = make_shared<metal>(albedo, fuzz);
    //                 world.add(make_shared<sphere>(center, 0.2, sphere_material));
    //             } else {
    //                 // glass
    //                 sphere_material = make_shared<dielectric>(1.5);
    //                 world.add(make_shared<sphere>(center, 0.2, sphere_material));
    //             }
    //         }
    //     }
    // }

    // auto material1 = make_shared<dielectric>(1.5);
    // world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    // auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    // world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    // auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    // world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    // camera cam;

    // cam.aspect_ratio      = 16.0 / 9.0;
    // cam.image_width       = 1200;
    // cam.samples_per_pixel = 10;
    // cam.max_depth         = 20;

    // cam.vfov     = 20;
    // cam.lookfrom = point3(13,2,3);
    // cam.lookat   = point3(0,0,0);
    // cam.vup      = vec3(0,1,0);

    // cam.defocus_angle = 0.6;
    // cam.focus_dist    = 10.0;

    // cam.render(world);

    set_sys_clock_khz(258000, true);
    // initialize stio
    stdio_init_all();

    

    initVGA();

    // // Image

    // int image_width = 800;
    // int image_height = 600;

    // // Render

    // // std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    // // printf("P3\n" << %d << ' ' <<  << "\n255\n")

    // for (int j = 0; j < image_height; ++j) {
    //     for (int i = 0; i < image_width; ++i) {
    //         auto r = double(i) / (image_width-1);
    //         auto g = double(j) / (image_height-1);
    //         auto b = 0;

    //         int ir = static_cast<int>(31.999 * r);
    //         int ig = static_cast<int>(63.999 * g);
    //         int ib = static_cast<int>(31.999 * b);

    //         int color = (ir << 11) | (ig << 5) | ib;



    //         std::cout << ir << ' ' << ig << ' ' << ib << '\n';
    //     }
    // }

    int colors[] = {
    64624, 18931, 6288, 9830, 20824,
    36141, 21519, 26624, 51410, 33061,
    2023, 52754, 16253, 36016, 13760,
    8542, 37204, 15812, 42434, 2734
    };
    int j = 0;
    int wait = 0;
    for (int i = 0; i < 16; i++) {
        gpio_init(i);
        gpio_set_dir(i, GPIO_OUT);
    }
    while (true) {
        if (gpio_get(16) == 0) {
            set_color(0);
        } 
        else {
            set_color(colors[j]);
            if (j >= 20) {
                j = 0;
            }
            if (wait >= 2000000) {
                j++;
                wait = 0;
            }
            wait++;
            
        }
    }
    // pt_add_thread(protothread_vga);
    return 0;
}