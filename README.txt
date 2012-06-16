0 Greetings
:::::::::::

Thanks for deciding for LoQuality as a music player. Our intend is to let
you gather as much music on your local hard disc as you like.

In this document, you can find:

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

Then, you are ready for installation!

3 Requirements
::::::::::::::

For the installation, of course, you'll need C++.
You need the usual qt libraries and headers (mostly called devel packages).
Also, make sure you have the package installed containing "qdbusxml2cpp".

For running LoQuality, you will need mplayer, and, if you like, mplayer2, too.
For the Flash Downloader, please install ffmpeg.

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

