# Hey! I'm Filing Here

In this lab, I successfully implemented the following:

- Writing the superblock metadata
- The creation of block group descriptor table
- Allocation of block bitmap and inode bitmap free space
- The creation of a filesystem contains a root directory, a lost+found directory, a regular file named hello-world, and a symbolic link named hello that points to hello-world.

## Building

```
make
```

## Running

```
./ext2-create
mkdir mnt
sudo mount -o loop cs111 -base.img mnt
ls -ain mnt/
```

output:

```
total 7
      2 drwxr-xr-x 3    0    0 1024 Jun  1 11:22 .
1203470 drwxr-xr-x 3 1000 1000 4096 Jun  1 11:23 ..
     13 lrw-r--r-- 1 1000 1000   11 Jun  1 11:22 hello -> hello-world
     12 -rw-r--r-- 1 1000 1000   12 Jun  1 11:22 hello-world
     11 drwxr-xr-x 2    0    0 1024 Jun  1 11:22 lost+found
```

## Cleaning up

```
sudo umount mnt
rmdir mnt
make clean
```
