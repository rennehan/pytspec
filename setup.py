from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

tspec_extension = Extension(
    name="pytspec",
    sources=["pytspec.pyx"],
    libraries=["tspec"],
    library_dirs=["lib"],
    include_dirs=["lib"]
)
setup(
    name="pytspec",
    ext_modules=cythonize([tspec_extension])
)
