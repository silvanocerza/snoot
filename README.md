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

Note that the generator might not output enough valid logs to let `Snoot` trigger alerts, in that case either lower the alert threshold with `--threshold` or the alert duration with `--alert-duration`.
You could also lower, or remove, the `sleep` amount inside `generator.sh`.

**NOTE: Snoot parses dates using the system locale.
If you use the Dockerfile to run it you MUST use `en_US.UTF-8 locale`
or no date will be parsed.**


## Sample output

```
================================
Most visited during last period
--------------------------------
Resources                | Hits
--------------------------------
/docs                    |  576
/api                     |  304
/report                  |  163


--------------------------------
Total                    | 1043
--------------------------------
High traffic generated an alert - hits = 1201, triggered at 2019-08-28 21:21:55
Previous alert recovered at 2019-08-28 21:23:36
High traffic generated an alert - hits = 1201, triggered at 2019-08-28 21:23:43
Previous alert recovered at 2019-08-28 21:23:45
================================
          General info
--------------------------------
Elapsed time:            00:02:30
Total hits:              1783
Total traffic:           177.176KB

Alert threshold:         10 per second
Alert duration:          120 seconds

```

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

To circumvent the issue use the included Dockerfile.

### Using Dockerfile

To ease compilation on multiple platforms I've included a Dockerfile to build and run Snoot.

* Fetch libraries
```
git submodule update --init
```

* Build the image

```
docker build -t snoot .
```

* Run generator in background, **this will generate date using the system locale**, if it's not `en_US.UTF-8` Snoot might not be able to parse dates correctly and no log will appear.

```
./generator >> logFile &
```

* Run Snoot

```
docker run --rm -i -v $(pwd):/snoot/logs -a stdout -a stderr snoot -f./logs/logFile -r1 -t3 -d5
```

### Without Dockerfile

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

* Build and run tests

```
make
ctest --output-on-failure
```

On Windows you should use Visual Studio after generating its project files with CMake.

* Run Snoot

```
./path/to/generator.sh >> logFile
./path/to/snoot -f/path/to/logFile -r1 -t3 -d5
```

## Possible improvements

* Trigger additional alerts, for `5xx`, or when a certain resource has too many hits
* Send a mail when alert triggers and when recovers
* Track more stats like methods and status codes
* Make log line format configurable
