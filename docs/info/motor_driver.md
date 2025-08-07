# Motor Driver

Notes on the motor brushed motor driver.

There are two types control according to [DRV883x datasheet](https://lcsc.com/datasheet/lcsc_datasheet_2410122006_Texas-Instruments-DRV8837DSGR_C39159.pdf)

* __PWM (IN/IN):__ This is the case of the motor driver used in this project. The speed is controlled by setting one of the inputs to a fixed value, and having the PWM on the other. Then, to go on reverse, the PWM and fixed output are swapped.
* __Phase-Enable (PH/EN):__ This is the most straight forward case, the speed is controlled through the enable input and the direction is controlled by the phase input.

The problem with the PWM control approach is that the PWM needs to be applied through a different pin depending on the motor direction.
Normally, this is not ideal since each timer output is assigned to a given pin. This requires having two channels per motor or adding extra logic.
An example of this can be seen in the [following repository](https://github.com/NicholasBerryman/GenericMotorDriver/tree/master), where the motor is controlled
using two PWM output pins.
The problem with the phase-enable motor drivers is that they are significantly less available. Thus, depending on them will make the design harder or more costly.
There are other type of interfaces which are a combination using two pins for the direction control and one enable pin to handle the PWM
(e.g. [L298N](https://www.st.com/resource/en/datasheet/l298.pdf))

## PWM (IN/IN) Motor Drivers

These drivers operate according to the following table (or a similar version of it).

| IN1 | IN2 | OUT1 | OUT2 | Mode    |
|-----|-----|------|------|------   |
| 0   | 0   | Z    | Z    | Coast   |
| 1   | 0   | H    | L    | Forward |
| 0   | 1   | L    | H    | Reverse |
| 1   | 1   | L    | L    | Break   |

There are two options to apply the PWM:

1. The _off_ state is coast, so both terminal of the H-bridge are off.
2. The _off_ state is break, so both terminals of the H-bridge are shorted through GND.

With option 1, during the _off_ state, the energy of the motor discharges slowly through the MOSFET's body diodes. If the frequency is too low the motor may not start (MX1616 driver)
With option 2, the motor breaks during the _off_ state. This enables accurate speed control. However, if the frequency is too low, the breaking may make the spinning be not smooth.

Test option 1 and option 2 with the following frequencies: 1kHz, 10kHz, 50kHz
Following tests: running freely, apply some load
Duty: 0, 25, 50, 75, 100