This directory contains a pre-configured version the Java version of the [milagro crypto library](github.com/milagro-crypto/amcl) as a maven project.
The code here is configured using config64.py.
This config64.py is a modified version of [the original config64.py](https://github.com/milagro-crypto/amcl/blob/master/version3/java/config64.py),
modified to contain all curves and options, 
places the files in a package 'com.ibm.zurich.amcl', and keeps the source files rather than compiling to jar.

The code can be compiled with maven using `mvn install`.