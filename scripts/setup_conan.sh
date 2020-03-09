#!/bin/bash

TARGET_DIR="$(pwd)/$1"
WORK_DIR=$TARGET_DIR

if [ ! -d "${WORK_DIR}/venv" ]
then
    virtualenv "${WORK_DIR}/venv"
fi

source "${WORK_DIR}/venv/bin/activate"

pip3 list | grep "conan" >/dev/null

if [ $? -eq 1 ]
then
    echo "Installing conan..."
    pip3 install conan
else
    echo "Conan already installed."
fi

conan remote list | grep "bincrafters/stable" >/dev/null

if [ $? -eq 1 ]
then
    echo "Setuping repository..."
    conan remote add "bincrafters/stable" "https://api.bintray.com/conan/bincrafters/public-conan"
else
    echo "Repository already setup."
fi

echo "Creating default profile..."
conan profile new default --detect

if [ "$OSTYPE" == "linux-gnu" ]
then
    echo "Linux detected. Changing default settings..."
    conan profile update settings.compiler.libcxx=libstdc++11 default
fi

echo "Installing project dependencies using conan..."
cd "${TARGET_DIR}" && conan install .. --build missing

if [ ! $? -eq 0 ]
then
    exit $?
fi

echo "Done."