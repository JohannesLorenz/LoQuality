0 Greetings
:::::::::::

Thanks for deciding for LoQuality as a music player. Our intent is to let
you gather as much music on your local hard disc as you like.

In this document you can find:

1 Status
2 Getting LoQuality
3 Requirements
4 Installation
5 Known Bugs

1 Status
::::::::

It is still in a "pre-alpha" phase, so please do not expect too much!
Documentation of the player will be added soon.

2 Getting LoQuality
:::::::::::::::::::

Please install git on your computer. Then, in a console, switch to where you
want LoQuality to be put, and enter:

	git clone git://github.com/JohannesLorenz/LoQuality.git
	cd LoQuality

Then you are ready for installation!

3 Requirements
::::::::::::::

For the installation, of course, you'll need a C++ compiler.
You need the usual Qt libraries and headers (mostly called devel packages).

To install the Qt development files, run the following command:
sudo apt-get install libqt4-dev (on Ubuntu / Debian)
sudo zypper in libqt4-devel (on SuSE)

Also, make sure you have the package installed containing "qdbusxml2cpp" (should
also be available in the Qt4 development package).

For running LoQuality, you will need:
* mplayer or mplayer2
* ffmpeg for the flash downloader
* taglib and its headers
* youtube-dl and wget for the playlist downloader
  (youtube-dl must be the newest possible version)
* ssh for the file syncher

4 Installation
::::::::::::::

Compile LoQuality in 3 steps!

1. Read and accept license
2. ./configure
3. make

5 Known Bugs
::::::::::::

* You must wait at least 60 seconds after your flash video was downloaded in
  firefox. Then, you can start the flash downloader. If you start earlier,
  crashes occur...

