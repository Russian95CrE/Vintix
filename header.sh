#!/bin/bash

headers_bad=false

for file in $(find . -type f -name "*.c"); do
    filename=$(basename "$file")
    expected="/* $filename */"

    line1=$(sed -n 1p "$file")
    line2=$(sed -n 2p "$file")

    if [[ "$line1" != "$expected" || "$line2" != "" ]]; then
        echo "Incorrect or missing header in: $file"
        headers_bad=true  # Set the flag to true

        read -p "Do you want to fix it? [y/N] " answer

        if [[ "$answer" == "y" || "$answer" == "Y" ]]; then
            tmpfile="$file.tmp"

            {
                echo "$expected"
                echo ""
                cat "$file"
            } > "$tmpfile"

            mv "$tmpfile" "$file"
            echo "Fixed!"
        else
            echo "Skipped."
        fi
    fi
done

if [ "$headers_bad" = false ]; then
    echo "Nop, everything is good."
fi
