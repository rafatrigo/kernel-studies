#kernel #kernelDev

# Install the virtualization packages

```bash
# install the virtualization main group
sudo dnf install @virtualization

# make sure that the virt-manager is installed
sudo dnf install virt-manager
```

**virt-manager** is a virtual machine management GUI tool  that uses **QEMU**.

# Start and enable the `libvirtd`  service

```bash
sudo systemctl start libvirtd
sudo systemctl enable libvirtd
```

# Add your user to the `libvirt` group

When you add your user to the `libvirt` group you don't need to use `sudo` to execute a **libvirt** command.

```bash
sudo usermod -a -G libvirt $(whoami)
```

After this **logout**  so the changes can be applied.

# Using External Storage

When using an external storage the vm-manager could not work as expected, so here is how to fix it.

### 1. Create a fixed mount point

We can not use `/run/media/...` because it is temporary and managed by our user.

**Create a directory that will be the mount point**
```bash
sudo mkdir /mnt/vm-storage
```


**Identify your external storage UUID**

Mount your device and execute the command:
```bash
sudo blkid
```
Search for the partition that represents your HD and copy the UUID.

**Add to fstab to mount automatically**

Open the fstab:
```bash
sudo nvim /etc/fstab
```

Paste inside the file:
```
UUID=<the-device-uuid> /mnt/vm-storage ext4 defaults,nofail 0 2
```

Unmount the HD and run the command to mount all the partitions inside **fstab**.
```bash
sudo moount -a
```

### 2. Fix the SELinux context

We need to say to the SELinux that `/mnt/vm-storage` is a save directory to `libvirt` to read and write.

**Apply a new security context**

The command bellow say that the directory `/mnt/vm-storage` and every thing that is inside it (`/.*`) will be labeled as `virt_image_t`.

```bash
sudo semanage fcontext -a -t virt_image_t "/mnt/vm-storage(/.*)?"
```

**Refresh the context**
```bash
sudo restorecon -Rv /mnt/vm-storage
```

### 3. Finish

Now you can create the VM with the vm-manager without problems

# Compiling the kernel

Update the system
```bash
sudo dnf update -y
```

Install development tools (git,  make, etc.)
```bash
sudo group install development-tools -y
```

Cloning the kernel
```bash 
git clone git clone https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git
```
This clones  Torvalds mainline the development **bleeding-edge**, where all the new code goes.

OBS.: if you want to work on a stable version just checkout in the branch of the version will want.


### Configuring the kernel

Configure to run in your VM as base.

Copy the config of your VM kernel:
```bash
cp /boot/config-$(uname -r) .config
```

Open the configure interface:
```bash
# the repository you cloned
cd linux

#  open the interface
make menuconfig
```

Just save.

### Compiling

Use the `nproc` to tell make to use all the CPU cores.

```bash
make -j$(nproc)
```
This may take some time.

**Install the kernel**

```bash
# install the modules (drivers)
sudo make modules_install

# install the kernel and update GRUB (bootloader)
sudo make install
```

Reboot the system.

See if you are using the new kernel:
```bash
uname -r
```
The command return should be the new kernel versison.

### Disable the SBAT
The **sbat** was throwing an error when I tried to compile the kernel, because in the configuration that I copy from the OS  it is present, but in the kernel that I cloned it is not.

To disabled it open the .config file in the linux folder and comment the SBAT config.
```
CONFIG_EFI_SBAT=y
CONFIG_EFI_SBAT_FILE="kernel.sbat"

#CONFIG_EFI_SBAT is not set
#CONFIG_EFI_SBAT_FILE is not set
```

To refresh the configuration:
```bash
make oldconfig
```

# Installing the Compiled Kernel

```bash
# install the drivers at /lib/modules
sudo make modules_install

# install the kernel and update the boot menu
sudo make install

# restart the system
sudo reboot

# check kernel version
uname -r
```

# SSH Connection

In the VM:
```bash
sudo dnf install openssh-server -y

# start the service
sudo systemctl start sshd

# enable the service to start on boot
sudo systemctl enable sshd

# add the ssh service to the firewall
sudo firewall-cmd --add-service=ssh --permanent

# reload the firewall
sudo firewall-cmd --reload

# find the VM ip
ip addr show
```

In the host:
```bash
ssh <VM-name>@<VM-ip>
```