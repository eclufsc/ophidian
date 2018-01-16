# <img src="https://raw.githubusercontent.com/eclufsc/ophidian/master/logo.png" width=80> Ophidian
Open-Source Library for Physical Design Research and Teaching

## Build status
[![pipeline status](https://gitlab.com/eclufsc/ophidian/badges/master/pipeline.svg)](https://gitlab.com/eclufsc/ophidian/commits/master)

## Installation
We suport a few of the most used linux environments.

### Debian Family:
Add the Ophidian repository via [package cloud](https://packagecloud.io/tarberd/ophidian) using the provided script.
```
$ curl -s https://packagecloud.io/install/repositories/tarberd/ophidian/script.deb.sh | sudo bash
```

### Arch Linux:
Just install it via the AUR package.
```
$ pacaur -S ophidian
```

### Red Hat Family:
We do not support rpm packages yet, please take a look at 

## Compile from Source
### Prerequisites:
* zlib
* Flex
* Bison
* C compiler
* c++ 14 compiler 

### Compiling dependencies:
Run the provided script **build_dependencies.sh** located at the source root. It will fetch the dependencies submodules located at 3rdparty, compile and install to a local fakeroot
```
$ sh build_dependencies.sh
```
All dependencies are now installed to the dependencies directory at the source root.

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

### Install Ophidian:
If ophidian_tests did not fail you may install Ophidian.
```
cd build
make install
```

# Credits:
Ophidian is an open-source library developed at [Embedded Computing Lab (ECL)](http://eclab.paginas.ufsc.br/), [Federal University of Santa Catarina (UFSC)](http://ufsc.br/), [Brazil](https://www.google.com.br/maps/place/Departamento+de+Inform%C3%A1tica+e+Estat%C3%ADstica/@-27.600561,-48.520762,17z/data=!3m1!4b1!4m5!3m4!1s0x95273900f56d3f99:0xc2fd35f96d3fb9b3!8m2!3d-27.6005658!4d-48.5185733?hl=en). The main objective of Ophidian project is to provide a multi-platform library to facilitate the research and teaching of topics associated to the physical design of integrated circuits.

Ophidian was conceived by [Chrystian Guth](http://csguth.com), [Renan Netto](https://renannetto.github.io/), and [Vinicius Livramento](http://vinilivramento.com), under the advise of [Prof. José Güntzel](https://www.inf.ufsc.br/~j.guntzel/), during their research activities at Federal University of Santa Catarina.

Currently, Ophidian has many collaborators in the same university that support its development.
* [Bernardo Mendonça](http://tarberd.me)
* [Chrystian Guth](http://csguth.com)
* [José Güntzel](https://www.inf.ufsc.br/~j.guntzel/)
* [Renan Netto](https://renannetto.github.io/)
* Thiago Barbato
* Tiago Fontana
* Sheiny Almeida
* [Vinicius Livramento](http://vinilivramento.com)
