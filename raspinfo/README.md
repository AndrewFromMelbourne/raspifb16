# raspinfo
Program to display Raspberry Pi specific system information directly on the
framebuffer.

# usage

    raspinfo <options>

    --daemon - start in the background as a daemon
    --device - framebuffer device to use (default is /dev/fb1)
    --help - print usage and exit
    --pidfile <pidfile> - create and lock PID file (if being run as a daemon)


# build
see main readme.

#install
From withing the build directory
    sudo make install
    sudo cp ../raspinfo/raspinfo.init.d /etc/init.d/raspinfo
    sudo update-rc.d raspinfo defaults
    sudo service raspinfo start

#uninstall

    sudo service raspinfo stop
    sudo update-rc.d -f raspinfo remove
    sudo rm /usr/local/bin/raspinfo
    sudo rm /etc/init.d/raspinfo
