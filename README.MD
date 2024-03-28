
This is for thermal camera model CION417RG2
ubuntu 22.04 usb3.0 SDK

----


It look like:

![](https://github.com/AirLab-Drone/Thermal_CION417RG2/blob/main/doc/thermal_img.png)
![](https://github.com/AirLab-Drone/Thermal_CION417RG2/blob/main/doc/thermalInRos2Node.png)


### build
```
git clone https://github.com/AirLab-Drone/Thermal_CION417RG2.git/.
mkdir build && cd build
cmake ..
make
```

### Run with image
```
./test_new_SDK_demo
```

### Run with ros2 node
```
./coin417rg2_ros2_node
```