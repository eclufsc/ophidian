<img src="https://gitlab.com/eclufsc/ophidian/raw/master/logo.png" width=80 /> 

# Ophidian
Open-Source Library for Physical Design Research and Teaching

## Build status:
Master: [![pipeline status](https://gitlab.com/eclufsc/ophidian/badges/master/pipeline.svg)](https://gitlab.com/eclufsc/ophidian/commits/master)

Development: [![pipeline status](https://gitlab.com/eclufsc/ophidian/badges/development/pipeline.svg)](https://gitlab.com/eclufsc/ophidian/commits/development)

## About:
Ophidian is an open-source library developed at [Embedded Computing Lab (ECL)](http://eclab.paginas.ufsc.br/), [Federal University of Santa Catarina (UFSC)](http://ufsc.br/), [Brazil](https://www.google.com.br/maps/place/Departamento+de+Inform%C3%A1tica+e+Estat%C3%ADstica/@-27.600561,-48.520762,17z/data=!3m1!4b1!4m5!3m4!1s0x95273900f56d3f99:0xc2fd35f96d3fb9b3!8m2!3d-27.6005658!4d-48.5185733?hl=en). The main objective of Ophidian project is to provide a multi-platform library to facilitate the research and teaching of topics associated to the physical design of integrated circuits.

Ophidian was conceived by [Chrystian Guth](http://csguth.com), [Renan Netto](https://renannetto.github.io/), and [Vinicius Livramento](http://vinilivramento.com), under the advise of [Prof. José Güntzel](https://www.inf.ufsc.br/~j.guntzel/), during their research activities at Federal University of Santa Catarina.

## Index:
* [Getting Started](#getting-started)
    * [Debian Family](#debian-family)
    * [Arch Linux](#arch-linux)
    * [Red Hat Family](#red-hat-family)
* [Hello World!](#hello-world)
* [Compiling from source](#compiling-from-source)
    * [Prerequisites](#prerequisites)
    * [Compiling dependencies](#compiling-dependencies)
    * [Install dependencies to system](#install-dependencies-to-system)
    * [Compiling Ophidian](#compiling-ophidian)
    * [Install Ophidian to system](#install-ophidian-to-system)
* [Credits](#credits)

# Getting Started!
Get Ophidian! We support a few of the most used linux environments.

### Debian Family:
If you use Ubuntu zesty (17) or Debian stretch (7) and newer you are covered! Add the Ophidian repository via [package cloud](https://packagecloud.io/tarberd/ophidian) using the provided script.
```
$ curl -s https://packagecloud.io/install/repositories/tarberd/ophidian/script.deb.sh | sudo bash
```

Then install the ophidian package using apt
```
$ apt install ophidian
```
> **Linux Mint** users should either wait for Linux Mint Tara (19) or [compile it from source.](#compiling-from-source).

### Arch Linux:
Just install it via the AUR package [ophidian-git](https://aur.archlinux.org/packages/ophidian-git/):
```
$ pacaur -S ophidian-git
```

### Red Hat Family:
We do not support rpm packages yet. Sorry :(

### Others:
**If we do not support your distro, consider [compiling it from source!](#compiling-from-source)**

# Hello World!
So you want to code? We make avaliable a [project template](https://gitlab.com/eclufsc/ophidian_project_template) to make your life easier!

```
git clone https://gitlab.com/eclufsc/ophidian_project_template hello_world
cd hello_world
mkdir build
cd build
cmake ..
make
./hello_world ../input_files/simple/simple.v ../input_files/simple/simple.def ../input_files/simple/simple.lef
``` 

# Compiling from Source:
This section will guide you on the compilation process of the Ophidian library. First make shure you have the following software installed on your system:
## Prerequisites:
* C compiler
* C++ 14 compiler 
* cmake (>= 3.5.2)
* Boost (>= 1.59)
* Flex
* Bison
* zlib

### Compiling dependencies:
There are a few more binaries we need to compile beforehand that are not suported by any distribution.

Run the provided script **build_dependencies.sh** located at the source root. It will fetch the dependencies sources through the submodules located at `"ophidian_source_root"/3rdparty`, compile and install them.
```
$ sh build_dependencies.sh
```
All dependencies are now compiled and installed to `"ophidian_source_root"/dependencies`.

### Install dependencies on system:
If you wish to install dependencies on your system run:
```
$ sudo sh build_dependencies.sh --install_to_usr_local
```
This will install all dependencies to the `/usr/local` directory.

### Compiling Ophidian:
Ophidian uses Cmake as a build system generator, if you are familiar with it you probably know the folowing:
```
mkdir build
cd build
cmake ..
make
```
In order to run the unity tests, inside the build directory, execute the folowing:
```
cd test
./ophidian_tests
```
> **ps.:** ophidian_tests needs to be executed from the test directory due to the input file's path being hardcoded.

### Install Ophidian on system:
If you wish to install Ophidian on your system please make shure your have installed all [dependencies on your system](#install-dependencies-to-system) first then run:
```
 $ sudo make install
```
This will install the Ophidian Library to the `/usr/local` directory.

# Credits
Currently, Ophidian has many collaborators in the same university that support its development.
* [Bernardo Mendonça](http://tarberd.me)
* [Chrystian Guth](http://csguth.com)
* [José Güntzel](https://www.inf.ufsc.br/~j.guntzel/)
* [Renan Netto](https://renannetto.github.io/)
* Thiago Barbato
* Tiago Fontana
* Sheiny Almeida
* [Vinicius Livramento](http://vinilivramento.com)
