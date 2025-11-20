import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import os

# Define cell parameters
# 4 cells with slightly different capacities (Ah) and internal resistances (Ohms)
cell_caps = np.array([2.50, 2.45, 2.55, 2.48])       # Ah
cell_res = np.array([0.030, 0.035, 0.025, 0.032])     # Ohms
# Initial State of Charge (SoC) - mismatched to show balancing
cell_socs = np.array([0.15, 0.10, 0.20, 0.12])        # Fraction (0.0 to 1.0)

# Target thresholds
CELL_MAX_VOLTAGE = 4.20     # V
BALANCE_THRESHOLD = 4.00    # V
BALANCE_DIFF = 0.020        # V (20 mV)
BALANCE_CURRENT = 0.042     # A (42 mA, based on 100 Ohm shunt bypass at ~4.2V)

# Charge configuration
CHARGE_CURRENT_CC = 1.5     # A (CC charge rate)
DT = 1.0                    # Time step (seconds)
MAX_TIME = 7200             # Max simulation time (2 hours)

# Simple OCV vs SoC model for standard Li-ion cell
# Interpolates between typical SoC and voltage data points
soc_points = np.array([0.0, 0.05, 0.10, 0.20, 0.30, 0.40, 0.50, 0.60, 0.70, 0.80, 0.90, 0.95, 1.0])
ocv_points = np.array([3.0, 3.35, 3.45, 3.55, 3.62, 3.68, 3.73, 3.77, 3.82, 3.90, 4.02, 4.10, 4.2])

def get_ocv(soc):
    # Clip SoC to [0.0, 1.0]
    soc = np.clip(soc, 0.0, 1.0)
    return np.interp(soc, soc_points, ocv_points)

# Simulation loop variables
time_s = []
voltages = []
socs = []
currents = []
balancing_states = []
charge_mode = []

current_total = CHARGE_CURRENT_CC
mode = "CC"

print("Starting BMS Cell Balancing Simulation...")

for t in range(int(MAX_TIME)):
    # Calculate cell Open Circuit Voltages
    ocvs = get_ocv(cell_socs)
    
    # Check balancing conditions:
    # 1. Any cell > 4.00V
    # 2. Imbalance delta V > 20mV
    # 3. Only balance the cells that are higher than the minimum cell by > 20mV
    bal_mask = np.zeros(4)
    if np.any(ocvs > BALANCE_THRESHOLD):
        min_ocv = np.min(ocvs)
        for i in range(4):
            if ocvs[i] - min_ocv > BALANCE_DIFF:
                bal_mask[i] = 1.0 # Enable balancing for this cell
                
    # Calculate effective charge current for each cell
    # Balanced cells have their charge current reduced by the shunt bypass current
    cell_charge_currents = np.zeros(4)
    for i in range(4):
        cell_charge_currents[i] = current_total - (bal_mask[i] * BALANCE_CURRENT)
        
    # Calculate terminal voltages (V = OCV + I*R)
    v_terminal = ocvs + (cell_charge_currents * cell_res)
    
    # Save states
    time_s.append(t)
    voltages.append(v_terminal.copy())
    socs.append(cell_socs.copy() * 100.0) # Store as percentage
    currents.append(current_total)
    balancing_states.append(bal_mask.copy())
    charge_mode.append(mode)
    
    # Check for Constant Voltage (CV) mode trigger
    # Trigger CV mode when any cell terminal voltage reaches 4.20V
    max_v = np.max(v_terminal)
    if max_v >= CELL_MAX_VOLTAGE:
        mode = "CV"
        # Reduce charge current to regulate the highest cell at 4.20V
        # I = (4.20 - OCV_max) / R_max
        max_idx = np.argmax(v_terminal)
        req_current_cell = (CELL_MAX_VOLTAGE - ocvs[max_idx]) / cell_res[max_idx]
        current_total = req_current_cell + (bal_mask[max_idx] * BALANCE_CURRENT)
        current_total = np.clip(current_total, 0.0, CHARGE_CURRENT_CC)
        
    # Update state of charge (SoC) for each cell
    # SoC_new = SoC_old + (I * dt) / (Capacity * 3600)
    for i in range(4):
        cell_socs[i] += (cell_charge_currents[i] * DT) / (cell_caps[i] * 3600.0)
        
    # Stop simulation if charging current drops below 50mA (standard charge termination in CV mode)
    if mode == "CV" and current_total < 0.05:
        print(f"Simulation terminated: Charge complete at {t} seconds.")
        break
    
    # Or stop if all cells reach 100% SoC
    if np.all(cell_socs >= 1.0):
        print(f"Simulation terminated: All cells fully charged at {t} seconds.")
        break

# Convert lists to numpy arrays for plotting
time_s = np.array(time_s)
voltages = np.array(voltages)
socs = np.array(socs)
balancing_states = np.array(balancing_states)

# Create output directories if they don't exist
os.makedirs("simulation", exist_ok=True)

# Generate Plot
plt.figure(figsize=(12, 8))

# Subplot 1: Cell Voltages
plt.subplot(2, 1, 1)
colors = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728']
for i in range(4):
    plt.plot(time_s / 60.0, voltages[:, i], label=f"Cell {i+1} Voltage (Cap: {cell_caps[i]}Ah)", color=colors[i], linewidth=2)
plt.axhline(y=CELL_MAX_VOLTAGE, color='r', linestyle='--', alpha=0.7, label="OV Target (4.20V)")
plt.axhline(y=BALANCE_THRESHOLD, color='g', linestyle=':', alpha=0.7, label="Balancing Limit (4.00V)")
plt.title("4-Cell BMS Simulation: Charging & Passive Balancing", fontsize=14, fontweight='bold')
plt.ylabel("Voltage (V)", fontsize=12)
plt.grid(True, linestyle=':', alpha=0.6)
plt.legend(loc="lower right")

# Subplot 2: Balancing status and total current
plt.subplot(2, 1, 2)
for i in range(4):
    # Plot balancing state as filled regions
    plt.fill_between(time_s / 60.0, 0, balancing_states[:, i] * (i + 1), alpha=0.3, color=colors[i], label=f"Cell {i+1} Bal Active")

ax2 = plt.gca().twinx()
ax2.plot(time_s / 60.0, currents, 'k-', linewidth=1.5, label="Total Charge Current")
ax2.set_ylabel("Charge Current (A)", color='k', fontsize=12)

plt.xlabel("Time (minutes)", fontsize=12)
plt.ylabel("Active Balancing Status", fontsize=12)
plt.grid(True, linestyle=':', alpha=0.6)

# Combine legends from twin axes
lines, labels = plt.gca().get_legend_handles_labels()
lines2, labels2 = ax2.get_legend_handles_labels()
plt.legend(lines + lines2, labels + labels2, loc="upper right")

plt.tight_layout()
plot_path = "simulation/cell_voltages.png"
plt.savefig(plot_path, dpi=300)
print(f"Saved simulation plot to {plot_path}")

# Write to Excel / CSV
df_dict = {
    "Time (s)": time_s,
    "Pack Current (A)": currents,
    "Mode": charge_mode,
}

for i in range(4):
    df_dict[f"Cell {i+1} Voltage (V)"] = voltages[:, i]
    df_dict[f"Cell {i+1} SoC (%)"] = socs[:, i]
    df_dict[f"Cell {i+1} Balancing Active"] = balancing_states[:, i]

df = pd.DataFrame(df_dict)

csv_path = "simulation/cell_voltages.csv"
df.to_csv(csv_path, index=False)
print(f"Saved simulation data to CSV at {csv_path}")

# Try writing to Excel (.xlsx) using pandas.to_excel
try:
    xlsx_path = "simulation/cell_voltages.xlsx"
    df.to_excel(xlsx_path, index=False)
    print(f"Saved simulation data to Excel at {xlsx_path}")
except Exception as e:
    print(f"Could not save to Excel (.xlsx) format: {e}. (A standard CSV was saved instead.)")

print("Simulation script execution finished.")
