from distutils.core import setup, Extension

procname_ext = Extension("procname", ["procnamemodule.c"])
setup(name="procname", version="0.3", author="Ludvig Ericson",
      description="Set process titles in Python programs",
      ext_modules=[procname_ext])
