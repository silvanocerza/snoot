Snoot
=====

An HTTP access log monitor in C++.

## Usage

`./snoot [-f FILE_PATH] [-r REFRESH_RATE] [-d ALERT_DURATION] [-t ALERT_THRESHOLD]`

* `-h`, `--help` shows the help menu
* `-f`, `--file` path to access log file, defaults to `/tmp/access.log`
* `-t`, `--threshold` amount of hits per second to trigger an alert, default to 10
* `-d`, `--alert-duration` number of seconds to look back when searching for the number of logs necessary to trigger an alert, defaults to 120
* `-r`, `--refresh-rate` number of seconds after which the displayed information is refreshed, defaults to 10

A simple generator is included for testing, to run it:

```
./generator.sh >> fakeLogFile
```

Then in another terminal start Snoop:

```
./snoop --file=<path_to_fakeLogFile> --refresh-rate=1
```

**NOTE: Snoot parses dates using the system locale.**

## Build and Test

### Dependencies

Building this project require these tools:

* C++17 compliant compiler (GCC 7, Clang 6, MSVC 19.14)
* CMake >= 3.1

And these libraries:

* [`date`](https://github.com/HowardHinnant/date/)
* [`args`](https://github.com/Taywee/args)
* [`catch2`](https://github.com/catchorg/Catch2) (only for testing)

### Build steps

* Fetch libraries

```
git submodule update --init
```

* Generate build files using CMake

```
mkdir build
cd build
cmake ..
```

* Build

```
make
```

* Run tests

```
cmake -DBUILD_TESTING=ON ..
make
ctest --output-on-failure
```

## Possible improvements

* Trigger additional alerts, for `5xx`, or when a certain resource has too many hits
* Send a mail when alert triggers and when recovers
* Track more stats like methods and status codes
* Make log line format configurable
