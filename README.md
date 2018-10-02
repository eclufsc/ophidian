<img src="https://gitlab.com/eclufsc/ophidian/raw/master/logo.png" width=80 /> 

# Ophidian
Open-Source Library for Physical Design Research and Teaching

## Build status:
Master: [![pipeline status](https://gitlab.com/eclufsc/eda/ophidian/badges/master/pipeline.svg)](https://gitlab.com/eclufsc/eda/ophidian/commits/master)

Development: [![pipeline status](https://gitlab.com/eclufsc/eda/ophidian/badges/development/pipeline.svg)](https://gitlab.com/eclufsc/eda/ophidian/commits/development)

## About:
Ophidian is an open-source library developed at [Embedded Computing Lab (ECL)](http://eclab.paginas.ufsc.br/), [Federal University of Santa Catarina (UFSC)](http://ufsc.br/), [Brazil](https://www.google.com.br/maps/place/Departamento+de+Inform%C3%A1tica+e+Estat%C3%ADstica/@-27.600561,-48.520762,17z/data=!3m1!4b1!4m5!3m4!1s0x95273900f56d3f99:0xc2fd35f96d3fb9b3!8m2!3d-27.6005658!4d-48.5185733?hl=en). The main objective of Ophidian project is to provide a multi-platform library to facilitate the research and teaching of topics associated to the physical design of integrated circuits.

Ophidian was conceived by [Chrystian Guth](http://csguth.com), [Renan Netto](https://renannetto.github.io/), and [Vinicius Livramento](http://vinilivramento.com), under the advise of [Prof. José Güntzel](https://www.inf.ufsc.br/~j.guntzel/), during their research activities at Federal University of Santa Catarina.

## Index:
* [Getting Started](#getting-started)
    * [Debian Family](#debian-family)
    * [Arch Linux](#arch-linux)
    * [Others](#others)
* [Hello World!](#hello-world)
* [Contributing](#contributing)
    * [Project Structure](#project-structure)
    * [Build and Installation Guide](#build-and-installation-guide)
    * [Build Dependencies](#build-dependencies)
    * [Compiling Ophidian](#compiling-ophidian)
* [Credits](#credits)

# Getting Started!
Get Ophidian! We support a few of the most used linux environments.

## Debian Family:
Add the [Ophidian repository ppa](https://launchpad.net/~eclufsc/+archive/ubuntu/ophidian).

### Ubuntu artful (17.10) or newer:
```
$ sudo add-apt-repository -y -u ppa:eclufsc/ophidian
$ sudo apt install libophidian-dev
```

### Debian Sid:
First you will need to add the **Launchpad PPA for eclufsc** gpg key:
```
$ sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys A62C1A5EFCB3D41A1FDB35758179B53998C15E64 && apt-get update -qq
```

Then add the launchpad repository and install ophidian.
```
$ sudo add-apt-repository -y -u -m "deb http://ppa.launchpad.net/eclufsc/ophidian/ubuntu artful main"
$ sudo apt install libophidian-dev
```

## Arch Linux:
Just install it via the AUR package [ophidian-git](https://aur.archlinux.org/packages/ophidian-git/):
```
$ pacaur -S ophidian-git
```

## Others:
**If we do not support your distro, consider [building it from source!](#build-and-installation-guide)**

# Hello World!
So you want to code? We make avaliable a [project template](https://gitlab.com/eclufsc/eda/ophidian_project_template) to make your life easier!


# Contributing:
Make shure to follow [CppCoreGuidelines](http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines).

For code style, try to keep very close to the [C++ Standard Library](https://en.cppreference.com/w/cpp).

Feel free to fork Ophidian and create a merge request!

So you want to push new code to Ophidian? First you should have an idea of the project structure:

## Project Structure:
```
ophidian
|   README.md
|   CMakeLists.txt
|   build_dependencies.sh
|   uncrustify.cfg
|   ...
|
+---cmake
|   |   FindLemon.cmake
|   |   uncrustify_helper.cmake
|   |   ...
|
+---3rdparty
|   |   +---catch
|   |   +---DEF
|   |   +---LEF
|   |   ...
|
+---ophidian
|   |   CMakeLists.txt
|   |   +---circuit
|   |   +---design
|   |   +---floorplan
|   |   ...
|
+---apps
|   |   CMakeLists.txt
|   |   +---gui
|
+---test
    |    CMakeLists.txt
    |   main.cpp
    |   +---circuit
    |   +---design
    |   +---floorplan
    |   ...

```

### The main [`ophidian/CMakeLists.txt`](CMakeLists.txt) file:
This is the main Cmake file for the ophidian project, it's goals are set up project variables,
handle dependencies and include the project subdirectories.

### The [`ophidian/build_dependencies.sh`](build_dependencies.sh) script:
This script serves as a c++ `npm`, `gem`, `pip`, equivalent.
It will fetch all third party git submodules located at `ophidian/3rdparty`, 
compile and install them to `ophidian/dependencies` by default.

### The [`ophidian/cmake`](cmake) directory:
Here you will have all custom made **cmake modules** used by the `CMakeLists.txt` files.

### The [`ophidian/3rdparty`](3rdparty) directory:
This directory holds the git submodules with the source code of ophidian's dependencies.

### The [`ophidian/ophidian`](ophidian) directory:
Here you will have directories for each library developed by the ophidian team.
Each library directory will have its own `CMakeLists.txt` file with its own compilation instructions.

### The [`ophidian/ophidian/CMakeLists.txt`](ophidian/CMakeLists.txt) file:
This file handles the installation rules for the ophidian library.

### The [`ophidian/apps`](apps) directory:
Here you will have directories for each ophidian application developed by the ophidian team.
Each application directory will have its own `CMakeLists.txt` file with its own compilation instructions.

### The [`ophidian/apps/CMakeLists.txt`](apps/CMakeLists.txt) file:
This file handles application directories.

### The [`ophidian/apps/gui`](apps/gui) directory:
Here you will find graphical user interface directories following the MVC model.

### The [`ophidian/apps/gui/CMakeLists.txt`](apps/gui/CMakeLists.txt) file:
This file handles the installation rules for the graphical user interface.

### The [`ophidian/test`](test) directory:
Here you will find directories for each ophidian library unit tests.

### The [`ophidian/test/CMakeLists.txt`](test/CMakeLists.txt) file:
This file handles the compilation instructions for the unit tests binary.


## Build and Installation Guide:
This section will guide you on the compilation process of the Ophidian library.
The Ophidian library depends on the following third party software:

### Dependencies:
* [Cmake](https://cmake.org/) (>= 3.5.2)
* [Boost](http://www.boost.org/) (>= 1.63)
* [Lemmon](http://lemon.cs.elte.hu/trac/lemon)
* [Units](https://gitlab.com/eclufsc/eda/units) 
* [Flute](https://gitlab.com/eclufsc/eda/Flute)
* [Verilog-Parser](https://gitlab.com/eclufsc/eda/verilog-parser)
* [DEF](https://gitlab.com/eclufsc/eda/DEF)
* [LEF](https://gitlab.com/eclufsc/eda/LEF)
* [LibertyParser](https://gitlab.com/eclufsc/eda/libertyparser.git)

Although some of this dependencies like Cmake and Boost are realy well supported
on any operational system, the others can be a hassle to have on your system. 
So you can try [building them yourself!](#build-dependencies) 

## Build Dependencies:
First, make shure you have all dependencies dependencies:

### Dependencies Dependencies:
* [zlib](https://www.zlib.net/)
* [Bison](https://www.gnu.org/software/bison/)
* [Flex](https://www.gnu.org/software/flex/)

Second, if you are going to use the graphical user interface, make shure you have all gui's dependencies:

### GUI Dependencies:
* [SFML](https://www.sfml-dev.org/) (>= 2.0)
* [QT](https://www.qt.io/) (>= 5.0)

### Using the [build_dependencies.sh](#the-ophidianbuild_dependenciessh-script) script:
Run the provided script **build_dependencies.sh** located at the source root. 

```
$ bash build_dependencies.sh
```

## Compiling Ophidian:
Ophidian uses Cmake as a build system generator, if you are familiar with it you probably know the following:
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

> **Cmake Configuration options:**

> **-DOPHIDIAN_TESTS_FULLY_STATIC=ON** will staticly link libc and libc++.

> **-DUNCRUSTIFY_IT=ON** will run uncrustify in all files.

> **-DUNCRUSTIFY_CHECK=ON** will run check all files formating with uncrustify.

> **-DOPHIDIAN_GUI=ON** will able building ophidian graphic user interface.

In order to run the ophidian unity tests, inside the build directory, execute the folowing:

```
cd test
./ophidian_tests
```
> **P.S.** ophidian_tests needs to be executed from the test directory due to the inputfile's path being hardcoded.

In order to run the graphic user interface, inside the build directory, execute the folowing:

```
cd apps/gui
./ophidian_gui
```

## [optional] Install Dependencies on System:
If you wish to install dependencies on your system run:

```
$ sudo bash build_dependencies.sh --install_to /usr/local
```

This will install all dependencies to the `/usr/local` directory.

## [optional] Install Ophidian on System:
If you wish to install Ophidian on your system please make shure your have
installed all [dependencies on your system](#optional-install-dependencies-on-system) first then run:

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
* [Tiago Augusto Fontana](https://tiagoafontana.github.io)
* Sheiny Almeida
* [Vinicius Livramento](http://vinilivramento.com)
* João Vicente Souto
