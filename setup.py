from setuptools import setup
from setuptools.command.install import install
import sys
import os
import shutil


def _build_cmake():
    if sys.platform == 'win32':
        print('CMake build for Windows')
    else:
        print('CMake build for Linux')
        os.system('/bin/bash cmake_build.sh')

def _create_symbolic_link():
    root_path = os.path.split(__file__)[0]
    src_path = os.path.join(root_path, 'python')
    dst_path = os.path.join(root_path, 'libsvmintel')
    if sys.platform == 'win32':
        # Symbolic link in Windows 10 would require admin privileges, just copy files across
        if os.path.exists(dst_path):
            shutil.rmtree(dst_path)
        shutil.copytree(src_path, dst_path)
    else:
        if not os.path.exists(dst_path):
            os.symlink(src_path, dst_path)


setup(
    name = 'libsvmintel',
    version = '0.0.1',
)

_build_cmake()
_create_symbolic_link()
