# 4-Cell BMS Safety Design Documentation

This document explains the safety-critical design decisions, electrical calculations, thermal physics, and board layout considerations for the 4-cell Battery Management System (BMS) featuring the **Texas Instruments BQ76920** analog front-end (AFE) and the **STM32G030** microcontroller.

---

## 1. Safety Threshold Calculations

The BQ76920 monitors individual cell voltages and pack current against hardware-comparator thresholds to trigger automatic shutdown of the protection FETs.

### A. Cell Overvoltage (OV) Protection
*   **Target Threshold**: $4.22\text{ V}$ ($4220\text{ mV}$)
*   **Purpose**: Prevent lithium plating, thermal runaway, and gas generation in standard Li-ion cells.
*   **AFE Configuration Math**:
    $$OV\_TRIP = \frac{V_{OV} - Offset_{mV}}{Gain_{\mu V/LSB}} \times 1000$$
    Using nominal factory calibration parameters ($Gain = 382\ \mu\text{V/LSB}$, $Offset = 0\text{ mV}$):
    $$OV\_TRIP = \frac{4220}{382/1000} \approx 11047\text{ counts}$$
    The `OV_TRIP` register (0x09) stores the upper 8 bits of the 14-bit ADC trip point (shifted right by 6 bits):
    $$OV\_TRIP\_REG = \frac{11047}{64} \approx 172 \text{ (0xAC)}$$
*   **Hardware Delay**: Configured via `PROTECT3` register to $1.0\text{ second}$ to ignore transient spikes during regenerative braking or high current pulses.

### B. Cell Undervoltage (UV) Protection
*   **Target Threshold**: $2.80\text{ V}$ ($2800\text{ mV}$)
*   **Purpose**: Prevent copper shunt formation and irreversible capacity loss due to over-discharge.
*   **AFE Configuration Math**:
    $$UV\_TRIP = \frac{2800}{0.382} \approx 7330\text{ counts}$$
    $$UV\_TRIP\_REG = \frac{7330}{64} \approx 114 \text{ (0x72)}$$
*   **Hardware Delay**: Configured to $1.0\text{ second}$ to prevent false UV trips during high-inrush loads.

### C. Overcurrent in Discharge (OCD) Protection
*   **Target Threshold**: $22.0\text{ A}$ continuous
*   **Sense Resistor ($R_{sns}$)**: $1.0\ \text{m}\Omega$ ($1000\ \mu\Omega$)
*   **Threshold Voltage ($V_{OCD}$)**:
    $$V_{OCD} = I_{OCD} \times R_{sns} = 22\text{ A} \times 1\text{ m}\Omega = 22\text{ mV}$$
*   **AFE Register Configuration**:
    Looking at the BQ76920 datasheet's `PROTECT2` table:
    *   $V_{OCD} = 22\text{ mV}$ maps directly to threshold code `0x0B` ($22\text{ mV}$ nominal with $RSNS = 1$).
*   **OCD Delay**: Configured to $320\text{ ms}$ to allow for transient inrush currents without opening protection switches.

### D. Short-Circuit in Discharge (SCD) Protection
*   **Target Threshold**: $89.0\text{ A}$ instant peak
*   **Threshold Voltage ($V_{SCD}$)**:
    $$V_{SCD} = I_{SCD} \times R_{sns} = 89\text{ A} \times 1\text{ m}\Omega = 89\text{ mV}$$
*   **AFE Register Configuration**:
    Looking at the `PROTECT1` register table:
    *   $V_{SCD} = 89\text{ mV}$ maps to threshold code `0x03` ($89\text{ mV}$ nominal with $RSNS = 1$).
*   **SCD Delay**: Configured to $200\ \mu\text{s}$ (code `0x00`) to protect the MOSFETs from thermal failure under dead-short conditions.

---

## 2. Cell Balancing Physics & Calculations

The BMS implements **passive cell balancing**, where excess energy from higher-voltage cells is dissipated as heat.

```
       Cell (+) ────┬──────────────────────────┐
                    │                          │
                   [R_f] (100 Ohm)             │
                    │                    [External Bypass Path]
                    ├───────┐ (Gate)           │
                    │     [ Q_bal ] (PMOS)     │
                    │       │                  │
                [C_filter]  ├───[R_bal] (39R) ──┤
                    │       │                  │
       Cell (-) ────┴───────┴──────────────────┘
```

### A. Balancing Current Calculation (Internal Balancing)
In internal balancing mode, the BQ76920 turns on an internal bypass FET with $R_{DS(on)} \approx 25\ \Omega$ between $VC_n$ and $VC_{n-1}$. The current is drawn through the input filter resistors ($R_f = 100\ \Omega$):
$$I_{bal} = \frac{V_{cell}}{R_f(VC_n) + R_{DS(on)} + R_f(VC_{n-1})} = \frac{4.2\text{ V}}{100\ \Omega + 25\ \Omega + 100\ \Omega} = \frac{4.2\text{ V}}{225\ \Omega} \approx 18.7\text{ mA}$$

*   **Internal Power Dissipation**:
    $$P_{int} = I_{bal}^2 \times R_{DS(on)} = (0.0187\text{ A})^2 \times 25\ \Omega \approx 8.7\text{ mW per channel}$$
    If balancing 2 cells simultaneously: $P_{total\_int} \approx 17.4\text{ mW}$. This is negligible and represents zero thermal risk to the AFE chip.

### B. Balancing Current Calculation (External Balancing)
For larger battery packs (e.g., $>10\text{ Ah}$), $18.7\text{ mA}$ is too small. We can add an external PMOS and a power balancing resistor ($R_{bal} = 39\ \Omega$):
$$I_{bal\_ext} = \frac{V_{cell}}{R_{bal}} = \frac{4.2\text{ V}}{39\ \Omega} \approx 108\text{ mA}$$

*   **External Power Dissipation**:
    $$P_{ext} = I_{bal\_ext}^2 \times R_{bal} = (0.108\text{ A})^2 \times 39\ \Omega \approx 0.45\text{ W}$$
    The balancing resistor must be a $1\text{ W}$ rated SMT resistor (e.g. 2512 package) to safely handle this heat.

---

## 3. Low-Side Switching Implications

The BMS uses a low-side switching topology, meaning the N-channel protection FETs are placed in the negative return path between $BAT-$ and $PACK-$.

```
   BAT+ ────────────────────────────────────────────────────────── PACK+
   
   BAT- ───[ Sense Resistor ]───[ DSG FET ]───[ CHG FET ]───────── PACK-
               (VSS ground)                                  (System Ground)
```

### A. Ground Reference Separation
*   **FETs Closed (Normal)**: $PACK-$ is connected to $BAT-$. The system ground reference ($PACK-$) and chip reference ($VSS$) are at nearly the same potential (separated only by the millivolt drop of $R_{sns}$ and the FET $R_{DS(on)}$).
*   **FETs Open (Fault State)**: When the FETs turn off due to a fault, the connection between $BAT-$ and $PACK-$ is severed. If a charger is attached, $PACK-$ can float to the full charger voltage (e.g., $+16.8\text{ V}$) relative to $BAT-$.

### B. External Communication Hazard
If the host microcontroller or an external debugger (like an ST-Link) is referenced to $PACK-$ (system ground) and communicates with the BMS (referenced to $VSS$ ground) via non-isolated I2C:
*   Opening the FETs creates a ground loop, driving the AFE I2C pins ($SCL/SDA$) negative relative to the floating $PACK-$.
*   This will exceed the absolute maximum ratings of the microcontroller pins, leading to hardware destruction.
*   **Solution**: All external communication links crossing the low-side isolation boundary must use digital isolators (e.g., ADuM1250 for I2C) to isolate ground references completely.

---

## 4. Hardware Safeguards and PCB Layout Rules

To ensure safety and measurement accuracy, the physical board layout must follow these guidelines:

1.  **Kelvin Connections**: The trace routes from the sense resistor ($R_{sns}$) to the $SRP$ and $SRN$ pins must be routed as a differential pair directly from the sense pads, keeping them away from high-current switching paths.
2.  **AFE Filter Capacitor Placement**: The RC filter components for the cell inputs ($VC0$ to $VC4$) must be placed as close to the BQ76920 pins as possible to filter high-frequency switching noise before it reaches the internal ADC.
3.  **High Current Thermal Sinks**: The copper planes connecting the protection FETs must be wide and thick (2oz copper recommended) with thermal vias to dissipate heat from high charge/discharge current loads.
