# Plyto

Plyto is a Framework for writing Event driven Applications.

# Prequisits 

Plyto is a Wrapper around a Set of binary Executables and shared Libraries.
You have to make sure, that these Executables and Libraries are installed properly before going of with Plyto.

# Setup

To Test Plyto you have to set your Project up first.
Create a virtual Environment with

    python -m venv .venv

and install the Requirements into this Environment

    source .venv/bin/activate && pip install -r requirements.txt

# Test

If you want to run Tests, execute this command

    python -m unittest discover test

# Build

Since you have run all the Tests you know Plyto is running correctly on your Machine.
Now you want to build a distributable Package which contain Plyto.

    python -m build --sdist

This will package Plyto and store it in the directory dist/plyto.x.y.z.tar.gz

# Useage

Plyto reads some Environment Variables which are 
    
    PLUTO_WORKDIR 
    PLUTO_BINARYDIR

If you want to customize Plytos behavior, assign your own values to these Variables.
For each Meaning of the Variables see the Sections bellow.

Copy the Packge to its destination and install this package using 

    pip install plyto.x.y.z.tar.gz

## Environment Variable PLUTO_WORKDIR

This Variable defaults to '/tmp/pluto/workdir/'.
Pluto will store its Configuration Files in this Directory.

## Environment Variable PLUTO_BINARYDIR

This Variable defaults to '/usr/local/bin/'.
Pluto trys to find the binary Executables in this Directory.

