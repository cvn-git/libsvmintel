from setuptools import setup
from setuptools.command.develop import develop
import sys
import os
import shutil


def _build_cmake():
    if sys.platform == 'win32':
        print('CMake build for Windows')

        vcvars_script = 'vcvars64.bat'
        vcvars_paths = [
            r'C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build',
        ]
        for vcvars_path in vcvars_paths:
            if os.path.exists(vcvars_path):
                os.system(f'pip_install.bat "{os.path.join(vcvars_path, vcvars_script)}"')
                break

    else:
        print('CMake build for Linux')
        os.system('/bin/bash pip_install.sh')


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
        # Run the standard PyPi copy
        develop.run(self)

        # Custom install
        _build_cmake()
        _create_symbolic_link()


setup(
    name='libsvmintel',
    version='0.0.1',
    cmdclass={'develop': DevelopWrapper},
)
