from unittest import TestLoader, TextTestRunner
from subprocess import run as subprocess_run

if __name__ == '__main__':
    suite = TestLoader().discover(start_dir='tests')
    TextTestRunner().run(suite)
