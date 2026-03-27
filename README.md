# Overview

A simple tool to convert Daz Studio 8 bit images to: avif, heic, jpeg xl or jpeg.
It has additional settings like chromatic aberration, banding reduction and a logo overlay.
It can save 8 or 10 bit images, either SDR Rec709 or HDR PQ.

# Current Features

* a simple UI
* .png and .tif files can be converted to avif using nvenc, vulkan or software
* 8 + 10 bit SDR \& 10 bit HDR images suppored as export
* image enhancement, chromatic aberration and logo effects
* saveable settings

# Third Party Libraries

ffDAZmpeg uses:

* ffmpeg (download separately): https://ffmpeg.org/download.html
* Dear ImGui (GLFW Vulkan): https://github.com/ocornut/imgui

