# DPX_Metdata_Editor

V0.9

View, Edit and Modify DPX file headers

This software originated as an in house Colorlab utility for editing dpx headers.  
Mac and Win64 binaries available in the release tab.
Current Version 
https://github.com/ColorlabMD/DPX_Metadata_Editor/releases/download/0.9/DPX_Metadata_Editor_Mac_V0_9.zip
https://github.com/ColorlabMD/DPX_Metadata_Editor/releases/download/0.9/DPX_Metadata_Editor_WIN64_V0_9.zip

libdpx is used for header reading. A couple changes to libdpx had to be made to errors in original code.

The following fields are calculated and increment when edited: frame position, timecode, keycode

Keycode generation can be done using perf or frame counting. The dpx standard says offset in perfs but many users use frames.
Kodak further complicated the issue with 65mm film has 120 perfs per count and the dpx standard is only 2 digits (ascii characters). 
Keycode format - 	mfg and gauge code - 1st digit manufacturer 0-orwo 1-agfa 2-kodak 3-fuji 2nd digit film gauge 0-35mm 1-16mm 2-65mm
			Film type - emulsion code
			prefix - 6digits 
			count - 4digits
			offset - 2digits

Known issues: Image Viewer -- libdpx image decoding does not work with certain size and type dpx files. 
10Bit RGB does not use libdpx and works appropriately. This does not affect header editor function just image display.

Timecode uses industry header film for calculations when writing new timecode. 
This seems to be preferred field for editing software rather than the industry header television vertical rate.

DPX 2.0 SMPTE 268M-1994 http://www.simplesystems.org/users/bfriesen/dpx/S268M_Revised.pdf
