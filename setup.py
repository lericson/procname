from distutils.core import setup, Extension

readme_conts = open("README.rst", "U").read()

procname_ext = Extension("procname", ["procnamemodule.c"])
setup(name="procname", version="0.3",
      url="http://github.com/lericson/procname/",
      author="Ludvig Ericson", author_email="ludvig@lericson.se",
      description="Set process titles in Python programs",
      long_description=readme_conts,
      ext_modules=[procname_ext])
