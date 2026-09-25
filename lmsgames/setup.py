# -*- coding: utf-8 -*-
from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension, build_ext

ext_modules = [
    Pybind11Extension(
        "gamelogic",
        ["gamelogic.cpp"],
        cxx_std=17,
        # 关键：强制 MSVC 用 UTF-8 读取源文件，解决中文编码报错
        extra_compile_args=['/utf-8'],
    ),
]

setup(
    name="gamelogic",
    version="1.0.0",
    description="LMG Xiuxian Game C++ Logic Module",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
)
