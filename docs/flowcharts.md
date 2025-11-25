# 4-Cell BMS Operational Flowcharts

This document provides visual flowcharts of the core firmware logic, represented in Mermaid syntax.

---

## 1. System Boot and AFE Initialization Sequence

This flowchart shows how the STM32 controller wakes up the BQ76920 AFE, reads the factory calibration coefficients, and enables the system protection circuits.

```mermaid
graph TD
    A([Power On / Reset]) --> B[GPIO and System Clock Config]
    B --> C[AFE_BOOT Pin PB1 Set HIGH]
    C --> D[Delay 3 ms for TS1 boot pulse]
    D --> E[AFE_BOOT Pin PB1 Set LOW]
    E --> F[Delay 10 ms for REGOUT stabilization]
    F --> G[Initialize I2C1 Peripheral]
    G --> H[Read BQ76920 Calibration: ADCGAIN & ADCOFFSET]
    H --> I[Configure SYS_CTRL1: Enable ADC, Set Ext Thermistor]
    I --> J[Configure SYS_CTRL2: Enable Coulomb Counter]
    J --> K[Configure Protection Limits: SCD, OCD, OV, UV]
    K --> L[Clear Status Register: Write 0xFF to SYS_STAT]
    L --> M[Enable Protection FETs: CHG_ON = 1, DSG_ON = 1]
    M --> N([BMS Main Loop Active])
    
    style C fill:#f9f,stroke:#333,stroke-width:2px
    style H fill:#bbf,stroke:#333,stroke-width:2px
    style M fill:#bfb,stroke:#333,stroke-width:2px
```

---

## 2. ALERT Interrupt and Fault Handling Routine

The BQ76920 drives the ALERT pin high when it detects a hardware fault. This triggers an EXTI interrupt on the STM32 to protect the battery pack immediately.

```mermaid
graph TD
    A([AFE ALERT Pin PB0 transitions LOW -> HIGH]) --> B[EXTI Rising Edge Interrupt ISR]
    B --> C[Read SYS_STAT Register via I2C]
    C --> D{Read Successful?}
    D -- No --> E[Emergency Shutdown: Force CHG_ON=0, DSG_ON=0]
    D -- Yes --> F{Any Fault Bits Active?}
    F -- No --> G[Exit Interrupt]
    F -- Yes --> H[Update global hBms.fault_status]
    H --> I{Decode Fault Type}
    
    I -->|SCD / OCD / DEVICE_XREADY| J[Critical Pack Fault: Disable CHG and DSG FETs]
    I -->|Cell Overvoltage - OV| K[Overcharge Fault: Disable CHG FET]
    I -->|Cell Undervoltage - UV| L[Overdischarge Fault: Disable DSG FET]
    
    J --> M[Enter Safe Lock-out State]
    K --> N[Wait for OV condition to clear]
    L --> O[Wait for UV condition to clear]
    
    N --> P[Write 1 to clear OV bit in SYS_STAT]
    O --> Q[Write 1 to clear UV bit in SYS_STAT]
    P --> R{OV cleared successfully?}
    Q --> S{UV cleared successfully?}
    
    R -- Yes --> T[Re-enable CHG FET]
    S -- Yes --> U[Re-enable DSG FET]
    
    style E fill:#f99,stroke:#333,stroke-width:2px
    style J fill:#f99,stroke:#333,stroke-width:2px
    style T fill:#bfb,stroke:#333,stroke-width:2px
    style U fill:#bfb,stroke:#333,stroke-width:2px
```

---

## 3. Passive Cell Balancing State Machine

This algorithm runs in the main loop every 5 seconds. It determines if cell balancing is required and selects the target cell to bypass charge current.

```mermaid
graph TD
    A([Balancing Task Triggered]) --> B{Any active faults present?}
    B -- Yes --> C[Disable Balancing: Write 0 to CELLBAL1]
    B -- No --> D{Is Charging Active?}
    D -- No --> C
    D -- Yes --> E[Read Cell Voltages: cell_voltages_mv 0..3]
    E --> F[Find max_voltage, min_voltage, and max_index]
    F --> G[Calculate delta_V = max_voltage - min_voltage]
    G --> H{max_voltage > 4.00V AND delta_V > 20mV?}
    H -- No --> C
    H -- Yes --> I[Enable Balance Bypass for Cell max_index]
    I --> J[Write 1 << max_index to CELLBAL1 Register]
    J --> K[Update hBms.balancing_mask]
    K --> L([Return to Main Loop])
    C --> L
    
    style C fill:#fbb,stroke:#333,stroke-width:2px
    style J fill:#bfb,stroke:#333,stroke-width:2px
```
