#!/bin/sh

echo "Removing old test directories...\n"

if [ -d "a" ]; then
	rm -rf a
fi

echo "Generating new test environment...\n"
unzip test_area.zip

if [ -d "__MACOSX" ]; then
	rm -rf __MACOSX
fi

echo "Renaming files and folders...\n"

./replacer.o -d ./a -p lorem -r hello -m 0

echo "Searching file for 'tempor' and placing 'lorem' in front...\n"

./replacer.o -d ./a -p lorem -r tempor -m 1
