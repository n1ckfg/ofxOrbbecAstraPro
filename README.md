# ofxOrbbecAstraPro

Simple wrapper for Orbbec Astra camera series in Linux ARM x86 architecture only.


# Dependencies

- Check [Orbbec Develop](https://orbbec3d.com/develop/) for [Astra SDK 2.1.0](http://dl.orbbec3d.com/dist/astra/v2.1.0/AstraSDK-v2.1.0-9bced77c41-Linux-arm.zip)
- Open Frameworks v0.11.0 or higher

![astra_sdk_update](/uploads/9894ab8001419811dd0da1906743088a/astra_sdk_update.png)

Latest updated SDK from Orbbec implemented into the addon.

# Setting project

The addon is provided with an example for preview the Color and Depth Streams.
The config.make file was configured in order to include the following libs:
```
 PROJECT_LDFLAGS=-Wl,-rpath=./libs
 PROJECT_LDFLAGS += ../libs/astra/lib/libastra.so
 PROJECT_LDFLAGS += ../libs/astra/lib/libastra_core.so
 PROJECT_LDFLAGS += ../libs/astra/lib/libastra_core_api.so
```

# Hardware results

Tested only on Raspberry Pi 4 Model B with 2GB RAM with the latest SDK (see above).
Using **ARCH Linux ARMv6l** (linuxarmv6l)

# Known issues

When using **ofxOpenCv** and **ofxCv** dependencies in your projects, beceause there's
to use `ofxCvColorImage` for instance, an error will popup when the device is trying
to initialize using `astra::initialize()`.

Nevertheless, the error is handled the camera will open and perform as expected.

# Acknowledgements

Thanks to [Matt Felsen](https://github.com/mattfelsen) for the addon [ofxOrbbecAstra](https://github.com/mattfelsen/ofxOrbbecAstra)
