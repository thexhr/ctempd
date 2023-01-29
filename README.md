# Screen Color Temperature Changing Daemon

Small daemon to set screen temperature.  ctempd can either set the screen's temperature once with the -s option or run in the background and set the
screen's temperature automatically based on the time of day.

## Features

* Set screen temperature continuously in the background
* Set screen temperature once

## Installation

ctempd is written in C and known to work on the operating systems listed in the table below.  To compile it you need the following things:

* A recent C compiler (tested with both clang >= 11 and GCC >= 8)
* make (tested with both BSD and GNU make)
* [libXrandr](https://www.x.org/wiki/libraries/libxrandr/)
* [libX11 core development libraries](https://www.x.org/releases/current/doc/libX11/libX11/libX11.html)

### Dependencies

Install the dependencies as follows:

| Operating System | Commands and Notes |
| --- | --- |
| OpenBSD | All installed by default |
| Void Linux| `xbps-install gcc make libXrandr-devel libX11-devel` |

If your operating system does not have `pkg-config` installed by default, you have to install it as well.

### Compilation and Installation

By default, the `Makefile` looks for external includes and libraries in `/usr/local/{include,lib}`, `/usr/X11R6/{include,lib}`.  If your distribution uses special path, you have to modify the Makefile accordingly.

Compile and install with the following commands:

```
$ make
# make install
```

## Usage

Let ctempd automatically adapt the screen temperature based on the time of day.  The highest temperature at noon is 5400K:

```
$ ctempd 5400
```

ctempd logs via syslog, you can check the current temperature in your system's log messages.

Set the screen temperature to 3200K once.

```
$ ctempd -s 3200
```

## License

isscrolls was written by Matthias Schmidt and is public domain.  The algorithm to set the screen temperature was written by Ted Unangst and is also public domain.
