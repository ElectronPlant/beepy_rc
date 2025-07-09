# Notes on the SBUS

Repositories:

* [bolderflight SBUS implementation](https://github.com/bolderflight/sbus/blob/main/src/sbus.h)
* [bolderflight SBUS description](https://github.com/bolderflight/sbus/tree/main)
* [betaflight SBUS implementation](https://github.com/betaflight/betaflight/blob/master/src/main/rx/sbus_channels.c#L44)

Binding process:

* [video](https://www.youtube.com/watch?v=aDZjEpZ-ut0)

OpenTX:

* [download](https://www.open-tx.org/2022/04/22/opentx-2.3.15)

Inverted SBUS

* [extracting inverted SBUS from the module](https://oscarliang.com/uninverted-sbus-smart-port-frsky-receivers/)
* [Hardware inverter](https://www.diyengineers.com/2020/12/17/2n2222-transistor-npn/)

FrSky RSII:

* [RSSI is on CH16](https://drones.stackexchange.com/questions/803/how-do-i-set-up-the-rssi-readout-on-an-xm-receiver-channel)

SBUS information:

* [info](https://uwarg-docs.atlassian.net/wiki/spaces/ZP/pages/2238283817/SBUS+Protocol)


Update Taranis

* https://blog.georgi-yanev.com/quick-tips/how-to-flash-taranis-q-x7-internal-module/
* https://oscarliang.com/flash-taranis-internal-module/
* Software: https://www.frsky-rc.com/taranis-q-x7-3/

Nice to have:

* https://raw.githubusercontent.com/mrRobot62/betaflight_processing/refs/heads/BF4.3/bf-4.3_processing-workflow.drawio.svg


Process:

* First trying to connect SBUS (inverted signal)
* Binding... another problem + having to update the radio.
* Problem with the HAL
* Transision to LL_HAL + working.
* Improvements: DMA, reduce the memory consumption.
* Further work, translate the signals.