#kernel #kernelDev 

**Show kernel logs**
```bash
sudo dmesg

# show new lines in the kernel logs
sudo dmesg -W
```

**Insert/remove kernel module**
```bash
# insert model
sudo insmod <.ko file of your module>

# remove module
sudo rmmod <module>
```

**Modules info**
```bash
modinfo <module>
```

**List all modules**
```bash
lsmod
```

**Coping from the VM to the host**
```bash
# -a (archive mode), -v (verbose), -z (compress)
rsync -avz <VM-user>@<VM-ip>:<VM-path> <host-path>
```
Only copy what has changed.