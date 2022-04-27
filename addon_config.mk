meta:
	ADDON_NAME = ofxOrbbecAsPro
	ADDON_DESCRIPTION = OF wrapper Orbbec Astra Pro SDK 2.1.0
	ADDON_AUTHOR = StudioRF
	ADDON_TAGS = "orbbec" "astra" "pro" "depth camera"
	ADDON_URL = 

common:
	ADDON_DEPENDENCIES = ofxOpenCv ofxCv
	ADDON_INCLUDES  = libs/astra/include/
	ADDON_INCLUDES += src/

linuxarm6l:
	# Only needs to link with these three libs, not any of the Plugins folder
	ADDON_LIBS  = libs/astra/lib/arm/libastra_core_api.so
	ADDON_LIBS += libs/astra/lib/arm/libastra_core.so
	ADDON_LIBS += libs/astra/lib/arm/libastra.so

	# All of the libs, including those inside the Plugins folder, do need to be
	# available for loading at runtime, so copy them into the bin folder,
	# maintaining the folder structure
	ADDON_DLLS_TO_COPY = libs/astra/lib/arm/