# Installing MAVSDK
[MAVSDK Setup](https://mavsdk.mavlink.io/v2.0/en/cpp/quickstart.html)

```sh
wget https://github.com/mavlink/MAVSDK/releases/download/v3.0.0/libmavsdk-dev_3.0.0_ubuntu24.04_amd64.deb
sudo dpkg -i libmavsdk-dev_3.0.0_ubuntu24.04_amd64.deb
```

# Installing PX4

[PX4 Development Environment Setup](https://docs.px4.io/main/en/dev_setup/dev_env_linux_ubuntu.html#simulation-and-nuttx-pixhawk-targets)

```sh
git clone https://github.com/PX4/PX4-Autopilot.git --recursive
bash ./PX4-Autopilot/Tools/setup/ubuntu.sh
```

# Installing QGroundControl

[QGroundControl Download and Install](https://docs.qgroundcontrol.com/master/en/qgc-user-guide/getting_started/download_and_install.html)

```sh
sudo usermod -a -G dialout $USER
sudo apt-get remove modemmanager -y
sudo apt install gstreamer1.0-plugins-bad gstreamer1.0-libav gstreamer1.0-gl -y
sudo apt install libfuse2 -y
sudo apt install libxcb-xinerama0 libxkbcommon-x11-0 libxcb-cursor-dev -y

chmod +x ./QGroundControl.AppImage
./QGroundControl.AppImage  # or double click
```

# Running SITL PX4

[PX4 SITL with Gazebo](https://docs.px4.io/main/en/sim_gazebo_gz/)

```sh
cd /path/to/PX4-Autopilot
make px4_sitl gz_x500
```