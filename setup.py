from setuptools import setup
from setuptools.command.install import install
import sys
import os


def _build_cmake():
    if sys.platform == 'win32':
        print('CMake build for Windows')
    else:
        print('CMake build for Linux')
        os.system('/bin/bash cmake_build.sh')


setup(
    name = 'libsvmintel',
    version = '0.0.1',
)

_build_cmake()
