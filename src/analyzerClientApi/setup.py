#!/usr/bin/env python
 
from distutils.core import setup
from distutils.extension import Extension
 
setup(name="pyPhronesis",
    ext_modules=[
        Extension("pyPhronesis", ["pyPhronesismodule.cpp"],
        libraries = ["boost_python", "boost_serialization", "boost_system", "boost_thread"])
    ])
