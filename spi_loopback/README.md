
![foo](https://user-images.githubusercontent.com/22987307/108810162-5ac2f080-755f-11eb-88b1-3d5e50e59393.jpg)

The blog [Adding a Peripheral to an NCS Zephyr project](https://devzone.nordicsemi.com/nordic/nordic-blog/b/blog/posts/adding-a-peripheral-to-an-ncs-zephyr-project/) uses a barebones board.dts and adds an SPIS and SPIM peripheral, and provides loopback setup information.

The project was built upon NCS 1.4.2 and was orriginally located as subdirectory of ..\ncs\v1.4.2\zephyr\samples\basic\spi_loopback

**Objectives**
 - Provide a brief primer to NCS and Zephyr
 - Provide a quick-start guide to adding peripherals to an existing sample project
 - Demonstrate running both foreground and background tasks within Zephyr
 
When starting with nRF Connect SDK and Zephyr, there are many well-documented resources to wade through. This document aims to allow you to temporarily skip to the “good parts” and at the same time guide you where to find the necessary details.

That blog provides a high-level, incomplete, and sometimes over generalized answer regarding adding a peripheral to your project. If any information contained within this blog is contradicted by referenced documentation, the reference documentation is assumed to be more accurate.

![barr](https://user-images.githubusercontent.com/22987307/108810268-9d84c880-755f-11eb-8879-bf94f0d502bf.jpg)
