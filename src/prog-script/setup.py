from setuptools import setup, find_packages
setup(
    name='at89progscript',
    version='0.1.0',
    author_email='andreizaulet@yahoo.com',
    packages=find_packages(include=['prog_script', 'prog_script.*', 'cli.*', 'serialPortUtils.*'])
)