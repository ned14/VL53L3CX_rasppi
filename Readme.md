# Port of ST VL53L3CX (2020) driver library to Raspberry Pi

This is a port of the support library to Raspberry Pi for the VL53L3CX
Time-of-Flight ranging sensor with advanced multi-object detection.
Thanks to the advanced API of the 2020 edition of ST's library,
you can extract a great deal of subtle information from what the
laser sensor can see within its ~15 degree <= 5000 mm field of view.

ST supply the original copy of this library at
https://www.st.com/en/imaging-and-photonics-solutions/vl53l3cx.html#tools-software
where the STSW-IMG015 download is this support library for the STM32
Nucleo board. Also at the same link is the STSW-IMG021 download which
is a Linux kernel driver formulation of the same code. This port takes
the Nucleo library and repoints it at userspace I2C for Raspberry Pi,
as a kernel driver is way overkill for this sensor.

Related alternatives:

- https://github.com/stm32duino/VL53L3CX is for STM32 and Arduino,
however the VL53L3CX library they used as their base is the 2018
edition, which is a completely different codebase and much less
sophisticated. It exposes considerably less detail.

- https://github.com/cassou/VL53L0X_rasp is pretty much a straight
wrap of the STSW-IMG005 ST VL53L0X sensor driver library for Raspberry
Pi. Note that the VL53L0X sensor is much simpler than the VL53L3CX,
it can only track the distance of a single object, whereas the
VL53L3CX reports more of a statistical dispersion to be quantitatively
analyssed

- https://github.com/pimoroni/VL53L0X-python is a Raspberry Pi
Python library for the VL53L0X sensor, essentially a higher convenient
wrap into Python of the preceding library. Enterprising readers may
wish to consider forking their repo and enhancing it to support this
repo.

## Build instructions

I personally find the Raspberry Pi Zero fast enough to build on device:

```
git clone https://github.com/ned14/VL53L3CX_rasppi.git
cd VL53L3CX_rasppi
make
```

The ST original distro comes with an example program to demonstrate
the sensor in action. Assuming your sensor is connected to the first
I2C (if not adjust the source code), to build and run this example:

```
make example
bin/example
```

The demonstration program will print output such as:

```
Count=  141, #Objs=2 status=0, D=   57mm, S=  99mm, Signal=5.32 Mcps, Ambient=1.92 Mcps
                     status=0, D= 1567mm, S=  99mm, Signal=0.58 Mcps, Ambient=1.92 Mcps
Count=  142, #Objs=2 status=0, D=   57mm, S=  99mm, Signal=5.52 Mcps, Ambient=1.92 Mcps
                     status=0, D= 1593mm, S=  99mm, Signal=0.59 Mcps, Ambient=1.92 Mcps
```

This shows how many objects the sensor detects, how far away from
the sensor each of those objects is, and the sigma of that object.
`Signal` is a measure of how reflective the object is, and `Ambient`
is a measure of the ambient light.
