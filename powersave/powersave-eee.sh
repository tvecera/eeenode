#!/bin/sh
# A script to enable EEE 1000H laptop power saving features for Ubuntu/Debian like Linux.

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Author: Tomas Vecera - https://github.com/tvecera
#
# Dependencies:
#  - ethtool - For turn off Wake On Lan
#  - 
# 
# Sources:
#  - http://crunchbanglinux.org/forums/topic/11954
#  - https://wiki.archlinux.org/index.php/Power_saving
#  - https://wiki.archlinux.org/index.php/ASUS_Eee_PC_1000HE

# ----------- Configuration -----------------------------------------

# List of modules to unload, space seperated.
modlist="uvcvideo"
# Bus list for setup runtime power management - power/control="auto".
buslist="pci"
# Bus list for remove - remove=1
pcilist="0000:00:00.0 0000:00:02.1 0000:00:1c.0 0000:00:1c.3"
# Cpu freq
# Description:
# Change CPU clock configuration.
# On the Eee PC 1000H there are three available clock configuration:
#    * 0 -> Super Performance Mode
#    * 1 -> High Performance Mode
#    * 2 -> Power Saving Mode 
cpufv=2
# Scaling governor
# ondemand - Dynamically switch between CPU(s) available if at 95% cpu load
# performance - Run the cpu at max frequency
# conservative - Dynamically switch between CPU(s) available if at 75% load
# powersave - Run the cpu at the minimum frequency
# userspace - Run the cpu at user specified frequencies
scalinggovernor="performance"
# Writeback Time
# Increasing the VM dirty writeback time can help to aggregate I/O together - reducing disk writes, and decreasing power usage:
writeback=1500

# ------------ Power saving options ---------------------------------
 
# Super Hybrid Engine
echo $cpufv > /sys/devices/platform/eeepc/cpufv
# Scaling governor
echo $scalinggovernor > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
# Turn Off Camera
echo 0 > /sys/devices/platform/eeepc/camera
# Turn Off Bluetooth
echo 0 > /sys/class/rfkill/rfkill1/state
# Remove internal 950 GMA Graphics - PCI 0000:00:02.0
echo 1 > /sys/bus/pci/drivers/i915/0000\:00\:02.0/remove
# Setup FAN SPEED - TODO
echo 50 > /sys/class/hwmon/hwmon2/pwm1

# Disable NMI wathcdog.
# The NMI watchdog is a hardware debugging feature. On some systems it can generate a lot of interrupts.
echo 0 > /proc/sys/kernel/nmi_watchdog
# Reduce disk writes 
echo $writeback > /proc/sys/vm/dirty_writeback_centisecs
# Turn off Wake On Lan
ethtool -s eth0 wol d
# USB powersaving - setup autosuspend for all usb devices
for i in /sys/bus/usb/devices/*/power/autosuspend; do
  echo 1 > $i
done
# Disable hardware modules to save power
for mod in $modlist; do
  grep $mod /proc/modules >/dev/null || continue
  modprobe -r $mod 2>/dev/null
done
# Remove all PCI devices from the list to decrease power usage
for pci in $pcilist; do
  for i in /sys/bus/pci/devices/$pci/remove; do
    echo 1 > $i
  done
done
# Enable runtime power management
for bus in $buslist; do
  for i in /sys/bus/$bus/devices/*/power/control; do
    echo auto > $i
  done
done
