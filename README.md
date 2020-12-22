# Tar Files Reader
 C code  that mounts uncompressed tar file using Libfuse without extracting entire TAR file.

## Prerequisite
Libfuse 3 must be installed 


## HOW TO MOUNT
1.  run `./unmount_make` <br/> 
    This will create dir called "./mountdir" if not already there and run  Makefile.

2.  run ./MOUNT_TAR [NAME OF TAR FILE]  <br/>
    eg: `./MOUNT_TAR test_tar_files/bigdir.tar`
    This mounts tar file at mountdir directory.


<br/><br/>
Now navigate tar file inside mountdir.
