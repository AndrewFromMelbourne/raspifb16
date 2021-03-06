# raspinfo
Program to display Raspberry Pi specific system information directly on the
framebuffer.
# usage
	raspinfo <options>

	--daemon,-D - start in the background as a daemon
	--device,-d - framebuffer device to use (default is /dev/fb1)
	--help,-h - print usage and exit
	--pidfile,-p <pidfile> - create and lock PID file (if being run as a daemon)
# build
see main readme.
# install
From within the build directory
## Raspian Wheezy
	sudo make install
	sudo cp ../raspinfo/raspinfo.init.d /etc/init.d/raspinfo
	sudo update-rc.d raspinfo defaults
	sudo service raspinfo start
## Raspian Jessie
	sudo make install
	sudo cp ../raspinfo/raspinfo@.service /etc/systemd/system/
	sudo systemctl daemon-reload
	sudo systemctl enable raspinfo@1.service
	sudo systemctl start raspinfo@1
# uninstall
## Raspian Wheezy
	sudo service raspinfo stop
	sudo update-rc.d -f raspinfo remove
	sudo rm /etc/init.d/raspinfo
	sudo rm /usr/local/bin/raspinfo
## Raspian Jessie
	sudo systemctl stop raspinfo@1
	sudo systemctl disable raspinfo@1.service
	sudo rm /etc/systemd/system/raspinfo@.service
	sudo rm /usr/local/bin/raspinfo
