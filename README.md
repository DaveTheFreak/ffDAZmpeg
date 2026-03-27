# Overview

A simple tool to convert Daz Studio 8 bit images to: avif, heic, jpeg xl or jpeg.
It has additional settings like chromatic aberration, banding reduction and a logo overlay.
It can save 8 or 10 bit images, either SDR Rec709 or HDR PQ.

# Current Features

* a simple UI
* supported input formats
  * .png
  * .tif
* supported output formats
  * .avif
    * bit Depths: 8 bit SDR, 8 + 10 bit HDR
    * encoders: NVENC, Vulkan & Software
  * .jpeg xl
    * bit Depths: 8 bit SDR, 8 + 10 bit HDR
    * encoder: Software
  * jpeg
    * bit Depths: 8 bit SDR
    * encoder: Software
* effects
  * chromatic aberration
  * image enhancement (debanding)
  * hdr:
    * brightness curve
    * saturation
  * logo overlay
* saveable settings

# Third Party Libraries

ffDAZmpeg uses:

* ffmpeg (download separately): https://ffmpeg.org/download.html
* Dear ImGui (GLFW Vulkan): https://github.com/ocornut/imgui

