# raspinfo
Program to display Raspberry Pi specific system information directly on the
framebuffer.
# usage
	raspinfo <options>

	--device,-d - device to use
	--font,-f - font file to use
	--help,-h - print usage and exit
	--kmsdrm,-k - use KMS/DRM dumb buffer
    --off,-o - do not display at start
# build
    see main readme.
# install
    From within the build directory

	sudo make install
	sudo cp ../raspinfo/raspinfo@.service /etc/systemd/system/
	sudo systemctl daemon-reload
	sudo systemctl enable raspinfo@1.service
	sudo systemctl start raspinfo@1
# uninstall

	sudo systemctl stop raspinfo@1
	sudo systemctl disable raspinfo@1.service
	sudo rm /etc/systemd/system/raspinfo@.service
	sudo rm /usr/local/bin/raspinfo
