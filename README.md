Ping-Pong CS
---

A simple multi-threaded server demo in [C11](1) using [pthreads](2).

---

## Running

Once build (see below) run `ping-pong -h`

## Prerequisites

### Install CMake 3 on CentOS

```bash
$ sudo yum -y install epel-release
$ sudo yum -y install cmake3
```

### Install CMake on MacOS

On `macos` you'll need if you haven't yet, first install [brew](3). Then:

```bash
$ brew install cmake
```

## Build

Substitute `<RELEASE_NAME>` below with either `Release` of `Debug`.
After build is finished the executable name will be `ping-pong`.

```bash
$ src/ping-pong -h
```

### Build on CentOS

```bash
$ mkdir linux
$ cd linux
$ cmake3 -DCMAKE_BUILD_TYPE=<RELEASE_NAME> ../
$ make
```

### Build on MacOS

I.e. for a specific version of `gcc`:

```bash
$ mkdir darwin
$ cd darwin
$ cmake -DCMAKE_BUILD_TYPE=<RELEASE_NAME> -DCMAKE_C_COMPILER=gcc-7  ../
$ make
```

[1]: https://en.wikipedia.org/wiki/C11_(C_standard_revision)
[2]: https://computing.llnl.gov/tutorials/pthreads/
[3]: https://brew.sh/index_fr.html
