# Hey! I'm Filing Here

- Implement a mountable ext2 file system image
- The superblock number setting
- Write inode bitmap and table

## Building

```shell
make
./ext2-create
```

## Running

```shell
dumpe2fs cs111-base.img # dumps the filesystem information to help debug
fsck.ext2 cs111-base.img # check that your filesystem is correct
mkdir mnt # create a directory to mnt your filesystem to
sudo mount -o loop cs111-base.img mnt # mount your filesystem, loop lets you use a file
```

## Cleaning up

```shell
sudo umount mnt
rmdir mnt
```

```shell
make clean
```
