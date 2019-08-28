Snoot
=====

An HTTP access log monitor in C++.

## Usage

`./snoot [-f FILE_PATH] [-t ALERT_THRESHOLD] [-d ALERT_DURATION] [-r REFRESH_RATE] [-s ALERT_HISTORY]`

* `-h`, `--help` shows the help menu
* `-f`, `--file` path to access log file, defaults to `/tmp/access.log`
* `-t`, `--threshold` amount of hits per second to trigger an alert, default to 10
* `-d`, `--alert-duration` number of seconds to look back when searching for the number of logs necessary to trigger an alert, defaults to 120
* `-r`, `--refresh-rate` number of seconds after which the displayed information is refreshed, defaults to 10
* `-s`, `--alert-history` number of alerts to show in list of triggered alerts

A simple generator is included for testing, to run it:

```
./generator.sh >> fakeLogFile
```

Then in another terminal start Snoop:

```
./path/to/snoop --file=<path/to/fakeLogFile> --refresh-rate=1
```

**NOTE: Snoot parses dates using the system locale.**

## Build and Test

### Dependencies

Building this project require these tools:

* C++17 compliant compiler (GCC 7, Clang 7, MSVC 19.14)
* CMake >= 3.1


And these libraries:

* [`date`](https://github.com/HowardHinnant/date/)
* [`args`](https://github.com/Taywee/args)
* [`catch2`](https://github.com/catchorg/Catch2) (only for testing)


### Build steps

Mac OS X is not supported since the toolchain shipped with XCode supports C++17 language features but
not C++17 standard library features, so it doesn't find the `<filesystem>` library.
To circument the issue I suggest running a VM with Linux.


* Fetch libraries

```
git submodule update --init
```

* Generate build files using CMake

```
mkdir build
cd build
cmake -DBUILD_TESTING=ON ..
```

* Build

```
make
```

* Run tests

```
ctest --output-on-failure
```

On Windows you should use Visual Studio after generating its project files with CMake.

## Possible improvements

* Trigger additional alerts, for `5xx`, or when a certain resource has too many hits
* Send a mail when alert triggers and when recovers
* Track more stats like methods and status codes
* Make log line format configurable
