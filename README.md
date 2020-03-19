# Tar Files Reader
 C library  that mounts uncompressed tar file using Libfuse.

## Prerequisite
Libfuse 3 must be installed 

## HOW TO MOUNT
Clone this repository.

run " git clone git@github.com:7mh/readtar.git "

run ./unmount_make <br/> 
This will create a "./mountdir" if not already there and run  Makefile.

run ./aMOUNT_TAR [NAME OF TAR FILE]  <br/>
This mounts tar file at mountdir directory.

<br/><br/>
Now navigate tar file inside mountdir.

