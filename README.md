Program traces
====================================

## Overview

The program extracts traces of software startup
from the Windows operating system

## Documentation

To create the documentation, follow these steps:

1. Install doxygen

```bash
sudo apt update && sudo apt install doxygen -y
```

2. Generate the documentation

```bash
doxygen docs/Doxyfile
```

## Dependencies

### Installing common dependencies

```bash
sudo apt update && sudo apt install cmake libboost-dev libspdlog-dev git autoconf automake libtool pkg-config gcc g++ make libfuse-dev -y
```

### Install libregf

1. Clone the repository

```bash
git clone https://github.com/libyal/libregf.git
cd libregf
```

2. Build and install the library "libregf"

```bash
./synclibs.sh
./autogen.sh
./configure
make
sudo make install
```

3. Update the library cache

```bash
sudo ldconfig
```

4. Checking the installation

```bash
regfinfo
```

### Install libscca

1. Clone the repository

```bash
git clone https://github.com/libyal/libscca.git
cd libscca
```

2. Build and install the library "libregf"

```bash
./synclibs.sh
./autogen.sh
./configure
make
sudo make install
```

3. Update the library cache

```bash
sudo ldconfig
```

4. Checking the installation

```bash
sccainfo
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
