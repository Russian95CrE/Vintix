#!/usr/bin/bash
clear

ISO_PATH="build/out/kernel.iso"
QEMU_COMMAND="qemu-system-x86_64 -cdrom $ISO_PATH -vga vmware -machine hpet=on -m 512M -serial mon:stdio"

# try to fix any missing or incorrect headers
if ! ./header.sh ; then
    echo -e "\n\e[33mTried to fix missing or incorrect headers, but failed!\e[0m\n"
    exit 1
fi

# try to format the src/ folder
if ! find src/ -name "*.[ch]" -exec clang-format -i {} +; then
    echo -e "\n\e[33mTried to format src/ folder using .clang-format, but failed!\e[0m\n"
fi

# try to... well... it's self-explanatory
if ! make clean; then
    echo -e "\n\e[31mTried to clean installation, but failed!\e[0m\n"
    exit 1
fi

# try to compile kernel using all threads
if ! make -j"$(nproc)"; then
    echo -e "\n\e[31mBuild failed!\e[0m\n"
    exit 1
fi

# run
if ! $QEMU_COMMAND; then
    echo -e "\n\e[31mFailed to run QEMU!\e[0m\n"
    exit 1
fi
