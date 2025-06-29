# Vintix

Vintix is an experimental x86_64 operating system. The goal is simple: build a modern, minimal OS that actually works.

## What Works

- Basic kernel and bootloader (Multiboot2)
- Framebuffer graphics (text output, test patterns)
- Keyboard input (en-US layout only for now)
- Simple shell (`inish`) with basic commands
- Early groundwork for a custom filesystem (VXFS)

## Docs

Click [here](docs/Main.md) to read the docs.

## Why?

Because I wanted a simple OS for daily use — and to run legacy tasks in a modern environment.

## Building

You’ll need a cross-compiler for x86_64 ELF and NASM installed.  
This project assumes you know what you're doing.  
If not, start reading the code.

## Running

Vintix is designed to run in QEMU, Bochs, or on real hardware (if you're brave).  
Boot it with Multiboot2 support.

> [!IMPORTANT]  
> I haven't tested on real hardware yet. For the next few weeks (or months), I’ll stick with QEMU.

## Philosophy

- No magic: if something breaks, you should be able to figure out why.
- No marketing: if a feature is missing, it's because nobody wrote it yet.
- No hand-holding: if you want to contribute, read the code and send patches.

## License

This project is licensed under the [FREES License](LICENSE).
