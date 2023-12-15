# 16-bit color VGA driver implemented using PIO and DMA on Raspberry Pi Pico RP2040
#### [Howard Hua](hth27@cornell.edu)
#### [Michael Wei](mw756@cornell.edu)
#### [Peter Cao](ppc49@cornell.edu)

## What is this?

This is a 16-bit color VGA driver implemented on the RP2040. We created a display adapter which communicates over UART to deliver an exceptional 65,000+ color gamut and 400x300 resolution from a $4 microcontroller. We preloaded 6 test images using the flash of the RP2040 and can display them on a VGA monitor. We also modified a ray-tracing program from Ray-Tracing in One Weekend that allows a Pico to render a raytraced scene and display it on a VGA monitor. We finally modified that same ray-tracing program to run on a laptop and send the rendered image to the Pico over UART. The Pico then displays the image on the VGA monitor.

This project was completed as part of ECE 4760 at Cornell University.

> - [Project website](https://ece4760.github.io)