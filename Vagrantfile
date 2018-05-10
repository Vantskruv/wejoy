# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|
  config.vm.box = "ubuntu/xenial64"
  config.vm.synced_folder ".", "/vagrant_data"

  config.vm.provision "shell", inline: <<-SHELL
     sudo modprobe uinput
     cd $HOME
     wget https://cmake.org/files/v3.10/cmake-3.10.3-Linux-x86_64.sh
     sudo sh cmake-3.10.3-Linux-x86_64.sh --prefix=/usr/local --exclude-subdir
     sudo apt-get update
     sudo apt-get install -y make g++ lua5.2-dev libudev-dev libevdev-dev linux-headers-$(uname -r) python libx11-dev lib32z1-dev ntp
     cd /vagrant_data
     ./build_all.sh
     ./run_tests.sh
   SHELL
end
