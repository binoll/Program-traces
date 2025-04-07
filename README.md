Program traces
====================================

## Overview

The program extracts traces of software startup
from the Windows operating system

## Dependencies

1. Install the dependencies

```bash
sudo apt update && sudo apt install git autoconf automake autopoint libtool pkg-config python3-dev libspdlog-dev
```

2. Clone the repository

```bash
git clone https://github.com/libyal/libregf.git
cd libregf
```

3. Build and install the library "libregf"

```bash
./synclibs.sh
./autogen.sh
./configure
make
sudo make install
```

4. Update the library cache

```bash
sudo ldconfig
```

## Build

From the root of the repository:

```bash
mkdir build && cd build
```

```bash
cmake ..
```

```bash
cmake --build .
```
