#!/bin/sh

# rezip with .DS_Store because they are annoying.
# zip -r test_area.zip a b -x "*.DS_Store"

#remove old test dirs
echo "Removing old test directories...\n"
if [ -d "a" ]; then
  rm -rf a
fi

if [ -d "b" ]; then
  rm -rf b
fi

#rehash test dirs
echo "Generating new test environment...\n"
unzip test_area.zip

if [ -d "test_area" ]; then
  if [ ! -d "a" && -d "test_area/a" ]; then
    cp -R test_area/a ./a
  fi
  if [ ! -d "b" && -d "test_area/b" ]; then
    cp -R test_area/b ./b
  fi
  rm -rf test_area
fi

if [ -d "__MACOSX" ]; then
  rm -rf __MACOSX
fi

# compile
# gcc sync.c csapp.c -o sync.o -g -lpthread

#run
echo "Running main.o...\n"
#./sync.o -m ./a -s ./b -v
./sync.o ./a ./b

echo "\n"