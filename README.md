Ping-Pong CS
---

A simple multi-threaded client-server_create demo in [C11](1) using [OpenMP](2).

---
## Prerequisites

### Install CMake 3 on CentOS

```bash
$ sudo yum -y install epel-release
$ sudo yum -y install cmake3
```

### Install CMake on MacOS

On `macos` you'll need `gcc` as `clang` doesn't support OpenMP
yet. So, if you haven't yet, first install [brew](3). Then:

```bash
$ brew install gcc7
$ brew install cmake
```

## Build

Substitute `<RELEASE_NAME>` below with either `Release` of `Debug`.
After build is finished the executable name will be `ping-pong`.

```bash
$ ./ping-pong -h
```

### Build on CentOS

```bash
$ mkdir linux
$ cd linux
$ cmake3 -DCMAKE_BUILD_TYPE=<RELEASE_NAME> ../
$ make
```

### Build on MacOS

```bash
$ mkdir darwin
$ cd darwin
$ cmake -DCMAKE_BUILD_TYPE=<RELEASE_NAME> -DCMAKE_C_COMPILER=gcc-7  ../
$ make
```

[1]: https://en.wikipedia.org/wiki/C11_(C_standard_revision)
[2]: http://www.openmp.org
[3]: https://brew.sh/index_fr.html
