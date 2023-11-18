# x86-64 emulator

The goal of this project is to be able to see how a computer executes a piece of software. The emulator is able to run high level user-space programs that are designed to run on a x86 GNU/Linux system. The project contains a command line emulator, and an optional GUI for easiert understanding of the emulator's output.

The project is our Thesis for the Science Conference of University of Debrecen.

## Disclaimer

This project is at most in **Alpha** stage! Bugs may occur at runtime!

## What it can do

- Run high level, user-space programs
- Run programs from C/Assembly source code, or from ELF binaries targeting x86-64 GNU/Linux systems.

## What it can't do

- Interrupt handling
- System calls

# Installation

We do not provide any precompiled binaries, so you'll have to build them yourself.

## Dependencies

For the emulator to run and compile, you'll need to install a few dependencies first.

For the emulator, you'll need to install:

- gcc
- git
- make
- objdump

On Ubuntu based systems, these packages are most likely installed already. If not, you can install them with the following:

```bash
sudo apt update &&
sudo apt install git gcc objdump    # TODO: objdump is not the valid package name
```

If you want to use the UI, you'll need to install the following:

- python
- pip
- QT
- PyQT5

On Ubuntu based systems, Python is most likely already installed.

Before we can install `PyQT5`, we need to install `pip`:

```bash
sudo apt install python3-pip
```

To install PyQT5, run the following command:

```bash
python3 -m pip install PyQt5
```

## Compilation

First you'll need to clone the repo. This can be done with the following commands:

```bash
git clone https://github.com/norbert204/assembly-emulator.git &&
cd assembly-emulator
```

After that, you can compile the emulator with `make`:

```bash
make build
```

# Usage

You can either run the emulator as a command-line tool, and work with the output file, which can be found at `/tmp/asemu_output` path by default, or you can use the GUI.

## Command-line

You can use the emulator from the command line by running the compiled `asemu` executable.

To run a program from a C/Assembly sourcecode file, run the following:

```bash
./asemu <file>
```

If you want to run a pre-compiled executable, you can use the following command:

```bash
./asemu -e <file>
```

**NOTE:** The executable must be compiled for x86-64 Linux support, and must use static-linking for shared libraries.

If you have a premade RAM file for the emulator, you can use it with the following:

```bash
./asemu -r <file>
```

**NOTE:** A RAM file is what the preprocessor creates in the emulator. For the currently running program, the RAM file can be found at `/tmp/asemu_ram` path.

For all the other possible parameters, run the following:

```bash
./asemu -h
```

## GUI

To run the UI, run the following command from the root directory of the repository:

```bash
cd ui &&
python3 main.py
```

# Authors

- Arnold Róth (@DEarniroth)
- Balazs Sajó (@balazssajo)
- Norbert Horváth (@norbert204)

Special thanks for our teacher for the immense support during the project:

- Imre Varga PhD. (@VargaImre79)
