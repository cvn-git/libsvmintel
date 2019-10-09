from setuptools import setup
from setuptools.command.develop import develop
import sys
import os
import shutil
import struct


def _build_cmake():
    if sys.platform == 'win32':
        print('CMake build for Windows')

        all_arch_script = 'vcvarsall.bat'
        if struct.calcsize("P") == 4:
            arch_script = 'vcvars32.bat'
            arch_arg = 'x86'
        else:
            arch_script = 'vcvars64.bat'
            arch_arg = 'amd64'

        toolchains = [
            (r'C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build', True),
            (r'C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC', False),
            (r'C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC', False),
        ]
        for toolchain in toolchains:
            found = False
            if os.path.exists(toolchain[0]):
                print(f'Building libsvmintel using MSVC toolchain in {toolchain[0]}')
                if toolchain[1]:
                    retval = os.system(f'pip_install.bat "{os.path.join(toolchain[0], arch_script)}"')
                else:
                    retval = os.system(f'pip_install.bat "{os.path.join(toolchain[0], all_arch_script)}" {arch_arg}')
                found = True
                break
        if not found:
            raise ValueError('Cannot find MSVC toolchain')

    else:
        print('CMake build for Linux')
        retval = os.system('/bin/bash pip_install.sh')

    if retval != 0:
        raise SystemError('CMake build failed')


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


class DevelopWrapper(develop):
    def run(self):
        # Custom install
        _build_cmake()
        _create_symbolic_link()

        # Run the standard PyPi copy
        develop.run(self)


setup(
    name='libsvmintel',
    version='1.0.0',
    cmdclass={'develop': DevelopWrapper},
)
