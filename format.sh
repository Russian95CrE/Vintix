#!/usr/bin/bash

# try to format the src/ folder
if ! find src/ -name "*.[ch]" -exec clang-format -i {} +; then
    echo -e "\n\e[33mTried to format src/ folder using .clang-format, but failed!\e[0m\n"
fi