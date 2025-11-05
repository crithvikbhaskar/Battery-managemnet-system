import os
import uuid

def make_uuid():
    return str(uuid.uuid4())

def generate_schematic():
    # Basic KiCad 6.0 schematic S-Expression content
    # We define standard symbols and place them with connections
    sch_uuid = make_uuid()
    
    content = f"""(kicad_sch (version 20211123) (generator "eeschema")
  (uuid "{sch_uuid}")

  (paper "A4")

  (title_block
    (title "4-Cell Battery Management System (BMS)")
    (company "Open Source BMS Design")
    (rev "v1.0")
    (date "2026-05-24")
    (source "bms_pcb.kicad_sch")
    (comment 1 "Based on TI BQ76920 AFE + STM32G030 MCU")
    (comment 2 "Designed for 4S Li-ion or LiFePO4 packs")
    (comment 3 "Low-side N-channel back-to-back protection FETs")
    (comment 4 "Passive cell balancing through input RC filter network")
  )

  (lib_symbols
    (symbol "Device:C" (pin_numbers hide) (pin_names (offset 0.254)) (in_bom yes) (on_board yes)
      (property "Reference" "C" (id 0) (at 0 2.54 0) (effects (font (size 1.27 1.27))))
      (property "Value" "C" (id 1) (at 0 -2.54 0) (effects (font (size 1.27 1.27))))
      (symbol "C_0_1"
        (polyline
          (pts (xy -2.032 -0.508) (xy 2.032 -0.508)) (stroke (width 0.508)) (fill (type none))
        )
        (polyline
          (pts (xy -2.032 0.508) (xy 2.032 0.508)) (stroke (width 0.508)) (fill (type none))
        )
      )
      (symbol "C_1_1"
        (pin passive line (at 0 3.81 270) (length 3.302)
          (name "~" (effects (font (size 1.27 1.27))))
          (number "1" (effects (font (size 1.27 1.27))))
        )
        (pin passive line (at 0 -3.81 90) (length 3.302)
          (name "~" (effects (font (size 1.27 1.27))))
          (number "2" (effects (font (size 1.27 1.27))))
        )
      )
    )
    (symbol "Device:R" (pin_numbers hide) (pin_names (offset 0)) (in_bom yes) (on_board yes)
      (property "Reference" "R" (id 0) (at 2.032 0 90) (effects (font (size 1.27 1.27))))
      (property "Value" "R" (id 1) (at 0 0 90) (effects (font (size 1.27 1.27))))
      (symbol "R_0_1"
        (rectangle (start -1.016 -2.54) (end 1.016 2.54) (stroke (width 0.15)) (fill (type none)))
      )
      (symbol "R_1_1"
        (pin passive line (at 0 3.81 270) (length 1.27)
          (name "~" (effects (font (size 1.27 1.27))))
          (number "1" (effects (font (size 1.27 1.27))))
        )
        (pin passive line (at 0 -3.81 90) (length 1.27)
          (name "~" (effects (font (size 1.27 1.27))))
          (number "2" (effects (font (size 1.27 1.27))))
        )
      )
    )
    (symbol "BMS:BQ76920" (in_bom yes) (on_board yes)
      (property "Reference" "U" (id 0) (at -10.16 22.86 0) (effects (font (size 1.27 1.27))))
      (property "Value" "BQ76920" (id 1) (at 10.16 22.86 0) (effects (font (size 1.27 1.27))))
      (symbol "BQ76920_0_1"
        (rectangle (start -12.7 -25.4) (end 12.7 20.32) (stroke (width 0.254)) (fill (type background)))
      )
      (symbol "BQ76920_1_1"
        (pin input line (at -17.78 17.78 0) (length 5.08)
          (name "ALERT" (effects (font (size 1.27 1.27))))
          (number "1" (effects (font (size 1.27 1.27))))
        )
        (pin power_in line (at 17.78 17.78 180) (length 5.08)
          (name "BAT" (effects (font (size 1.27 1.27))))
          (number "2" (effects (font (size 1.27 1.27))))
        )
        (pin input line (at 17.78 12.7 180) (length 5.08)
          (name "VC5" (effects (font (size 1.27 1.27))))
          (number "3" (effects (font (size 1.27 1.27))))
        )
        (pin input line (at 17.78 7.62 180) (length 5.08)
          (name "VC4" (effects (font (size 1.27 1.27))))
          (number "4" (effects (font (size 1.27 1.27))))
        )
        (pin input line (at 17.78 2.54 180) (length 5.08)
          (name "VC3" (effects (font (size 1.27 1.27))))
          (number "5" (effects (font (size 1.27 1.27))))
        )
        (pin input line (at 17.78 -2.54 180) (length 5.08)
          (name "VC2" (effects (font (size 1.27 1.27))))
          (number "6" (effects (font (size 1.27 1.27))))
        )
        (pin input line (at 17.78 -7.62 180) (length 5.08)
          (name "VC1" (effects (font (size 1.27 1.27))))
          (number "7" (effects (font (size 1.27 1.27))))
        )
        (pin input line (at 17.78 -12.7 180) (length 5.08)
          (name "VC0" (effects (font (size 1.27 1.27))))
          (number "8" (effects (font (size 1.27 1.27))))
        )
        (pin no_connect line (at -17.78 -22.86 0) (length 5.08) hide
          (name "NC" (effects (font (size 1.27 1.27))))
          (number "9" (effects (font (size 1.27 1.27))))
        )
        (pin power_in line (at 0 -30.48 90) (length 5.08)
          (name "VSS" (effects (font (size 1.27 1.27))))
          (number "10" (effects (font (size 1.27 1.27))))
        )
        (pin input line (at -17.78 7.62 0) (length 5.08)
          (name "SCL" (effects (font (size 1.27 1.27))))
          (number "11" (effects (font (size 1.27 1.27))))
        )
        (pin bidirectional line (at -17.78 2.54 0) (length 5.08)
          (name "SDA" (effects (font (size 1.27 1.27))))
          (number "12" (effects (font (size 1.27 1.27))))
        )
        (pin output line (at -17.78 -7.62 0) (length 5.08)
          (name "CHG" (effects (font (size 1.27 1.27))))
          (number "13" (effects (font (size 1.27 1.27))))
        )
        (pin output line (at -17.78 -12.7 0) (length 5.08)
          (name "DSG" (effects (font (size 1.27 1.27))))
          (number "14" (effects (font (size 1.27 1.27))))
        )
        (pin passive line (at 17.78 -20.32 180) (length 5.08)
          (name "CAP1" (effects (font (size 1.27 1.27))))
          (number "15" (effects (font (size 1.27 1.27))))
        )
        (pin bidirectional line (at -17.78 -2.54 0) (length 5.08)
          (name "TS1" (effects (font (size 1.27 1.27))))
          (number "16" (effects (font (size 1.27 1.27))))
        )
        (pin power_in line (at 0 25.4 270) (length 5.08)
          (name "REGSRC" (effects (font (size 1.27 1.27))))
          (number "17" (effects (font (size 1.27 1.27))))
        )
        (pin power_out line (at -17.78 12.7 0) (length 5.08)
          (name "REGOUT" (effects (font (size 1.27 1.27))))
          (number "18" (effects (font (size 1.27 1.27))))
        )
        (pin input line (at 17.78 -22.86 180) (length 5.08)
          (name "SRN" (effects (font (size 1.27 1.27))))
          (number "19" (effects (font (size 1.27 1.27))))
        )
        (pin input line (at 17.78 -25.4 180) (length 5.08)
          (name "SRP" (effects (font (size 1.27 1.27))))
          (number "20" (effects (font (size 1.27 1.27))))
        )
      )
    )
    (symbol "BMS:STM32G030F6P6" (in_bom yes) (on_board yes)
      (property "Reference" "U" (id 0) (at -10.16 22.86 0) (effects (font (size 1.27 1.27))))
      (property "Value" "STM32G030F6P6" (id 1) (at 10.16 22.86 0) (effects (font (size 1.27 1.27))))
      (symbol "STM32G030F6P6_0_1"
        (rectangle (start -12.7 -25.4) (end 12.7 20.32) (stroke (width 0.254)) (fill (type background)))
      )
      (symbol "STM32G030F6P6_1_1"
        (pin bidirectional line (at -17.78 17.78 0) (length 5.08) (name "PB7" (number "1")))
        (pin bidirectional line (at -17.78 15.24 0) (length 5.08) (name "PB8" (number "2")))
        (pin bidirectional line (at -17.78 12.7 0) (length 5.08) (name "PC14" (number "3")))
        (pin power_in line (at 0 25.4 270) (length 5.08) (name "VDD" (number "4")))
        (pin power_in line (at 0 -30.48 90) (length 5.08) (name "VSS" (number "5")))
        (pin bidirectional line (at -17.78 10.16 0) (length 5.08) (name "PA0" (number "6")))
        (pin bidirectional line (at -17.78 7.62 0) (length 5.08) (name "PA1" (number "7")))
        (pin bidirectional line (at -17.78 5.1 0) (length 5.08) (name "PA2" (number "8")))
        (pin bidirectional line (at -17.78 2.54 0) (length 5.08) (name "PA3" (number "9")))
        (pin bidirectional line (at -17.78 0 0) (length 5.08) (name "PA4" (number "10")))
        (pin bidirectional line (at 17.78 0 180) (length 5.08) (name "PA5" (number "11")))
        (pin bidirectional line (at 17.78 2.54 180) (length 5.08) (name "PA6" (number "12")))
        (pin bidirectional line (at 17.78 5.08 180) (length 5.08) (name "PA7" (number "13")))
        (pin bidirectional line (at 17.78 7.62 180) (length 5.08) (name "PB0" (number "14")))
        (pin bidirectional line (at 17.78 10.16 180) (length 5.08) (name "PA9" (number "15")))
        (pin bidirectional line (at 17.78 12.7 180) (length 5.08) (name "PA10" (number "16")))
        (pin bidirectional line (at 17.78 15.24 180) (length 5.08) (name "PA11" (number "17")))
        (pin bidirectional line (at 17.78 17.78 180) (length 5.08) (name "PA12" (number "18")))
        (pin bidirectional line (at 17.78 -20.32 180) (length 5.08) (name "PA13" (number "19")))
        (pin bidirectional line (at 17.78 -22.86 180) (length 5.08) (name "PA14" (number "20")))
      )
    )
  )

  # Place Symbols
  (symbol (lib_id "BMS:BQ76920") (at 120 90 0)
    (in_bom yes) (on_board yes) (uuid "{make_uuid()}")
    (property "Reference" "U1" (id 0) (at 120 62 0))
    (property "Value" "BQ76920" (id 1) (at 120 65 0))
    (property "Footprint" "Package_SO:TSSOP-20_4.4x6.5mm_P0.65mm" (id 2) (at 120 58 0) hide)
  )

  (symbol (lib_id "BMS:STM32G030F6P6") (at 180 90 0)
    (in_bom yes) (on_board yes) (uuid "{make_uuid()}")
    (property "Reference" "U2" (id 0) (at 180 62 0))
    (property "Value" "STM32G030F6P6" (id 1) (at 180 65 0))
    (property "Footprint" "Package_SO:TSSOP-20_4.4x6.5mm_P0.65mm" (id 2) (at 180 58 0) hide)
  )

  # Note: Wires and graphical lines would typically go here to connect them.
  # We write basic graphical annotations to verify it opens correctly.
  (text "BQ76920 4-Cell BMS Schematic" (at 100 40 0)
    (effects (font (size 3 3) bold))
  )
  
  (text "Connections:\\nU1 SCL (Pin 11) -> U2 PA11 (Pin 17)\\nU1 SDA (Pin 12) -> U2 PA12 (Pin 18)\\nU1 ALERT (Pin 1) -> U2 PB0 (Pin 14)\\nU1 REGOUT (Pin 18) -> U2 VDD (Pin 4) (Powers MCU with 3.3V)" (at 90 140 0)
    (effects (font (size 1.5 1.5)))
  )
)
"""
    return content

def generate_pcb():
    # KiCad 6.0 PCB S-Expression layout content
    pcb_uuid = make_uuid()
    
    content = f"""(kicad_pcb (version 20211014) (generator pcbnew)
  (uuid "{pcb_uuid}")

  (general
    (thickness 1.6)
  )

  (paper A4)

  (layers
    (0 F.Cu signal)
    (31 B.Cu signal)
    (32 B.Adhes user)
    (33 F.Adhes user)
    (34 B.Paste user)
    (35 F.Paste user)
    (36 B.SilkS user)
    (37 F.SilkS user)
    (38 B.Mask user)
    (39 F.Mask user)
    (40 Dwgs.User user)
    (41 Cmts.User user)
    (42 Eco1.User user)
    (43 Eco2.User user)
    (44 Edge.Cuts user)
  )

  (setup
    (pad_to_mask_clearance 0.05)
    (solder_mask_min_width 0.1)
    (pad_to_paste_clearance -0.02)
    (grid 1.27)
    (track_width 0.25)
    (via_size 0.6)
    (via_drill 0.3)
  )

  # Define board boundary on Edge.Cuts (80mm x 50mm, centered around 100,100)
  # Rectangle from (100, 100) to (180, 150)
  (gr_line (start 100 100) (end 180 100) (layer Edge.Cuts) (width 0.15) (tstamp "{make_uuid()}"))
  (gr_line (start 180 100) (end 180 150) (layer Edge.Cuts) (width 0.15) (tstamp "{make_uuid()}"))
  (gr_line (start 180 150) (end 100 150) (layer Edge.Cuts) (width 0.15) (tstamp "{make_uuid()}"))
  (gr_line (start 100 150) (end 100 100) (layer Edge.Cuts) (width 0.15) (tstamp "{make_uuid()}"))

  # Add SilkScreen Titles
  (gr_text "4-CELL BMS - BQ76920 + STM32" (at 140 105) (layer F.SilkS)
    (effects (font (size 1.5 1.5) (thickness 0.3))) (tstamp "{make_uuid()}"))
  (gr_text "REV 1.0" (at 140 110) (layer F.SilkS)
    (effects (font (size 1.0 1.0) (thickness 0.2))) (tstamp "{make_uuid()}"))

  # Footprint BQ76920 (U1) at (125, 125)
  (footprint "Package_SO:TSSOP-20_4.4x6.5mm_P0.65mm" (layer F.Cu)
    (at 125 125)
    (descr "TSSOP-20 package for BQ76920")
    (tstamp "{make_uuid()}")
    (property "Reference" "U1" (at 125 119) (layer F.SilkS) (effects (font (size 1.0 1.0) (thickness 0.2))))
    (property "Value" "BQ76920" (at 125 131) (layer F.SilkS) (effects (font (size 1.0 1.0) (thickness 0.2))))
    # Draw simple pads for TSSOP-20 (Pitch 0.65mm, Width 0.45mm, Length 1.5mm)
    # Left side pads (1 to 10)
    # Right side pads (11 to 20)
    # TSSOP-20 pins are spaced 0.65mm apart, total length is (10-1)*0.65 = 5.85mm
    # Spacing between rows is 6.2mm, so X positions are -3.1 and 3.1
  """
    # Programmatically add pads for U1
    pads = []
    # Left side (1-10) from top to bottom
    y_start = -2.925 # (9 * 0.65) / 2
    for i in range(10):
        y_pos = y_start + i * 0.65
        pads.append(f'    (pad "{i+1}" smd rect (at -3.1 {y_pos:.3f}) (size 1.5 0.4) (layers F.Cu F.Mask F.Paste) (tstamp "{make_uuid()}"))')
    # Right side (11-20) from bottom to top
    for i in range(10):
        y_pos = y_start + (9 - i) * 0.65
        pads.append(f'    (pad "{11+i}" smd rect (at 3.1 {y_pos:.3f}) (size 1.5 0.4) (layers F.Cu F.Mask F.Paste) (tstamp "{make_uuid()}"))')
    
    content += "\n".join(pads)
    content += """
  )

  # Footprint STM32G030 (U2) at (155, 125)
  (footprint "Package_SO:TSSOP-20_4.4x6.5mm_P0.65mm" (layer F.Cu)
    (at 155 125)
    (descr "TSSOP-20 package for STM32G030F6P6")
    (tstamp "{make_uuid()}")
    (property "Reference" "U2" (at 155 119) (layer F.SilkS) (effects (font (size 1.0 1.0) (thickness 0.2))))
    (property "Value" "STM32G030F6P6" (at 155 131) (layer F.SilkS) (effects (font (size 1.0 1.0) (thickness 0.2))))
  """
    # Programmatically add pads for U2
    pads_u2 = []
    for i in range(10):
        y_pos = y_start + i * 0.65
        pads_u2.append(f'    (pad "{i+1}" smd rect (at -3.1 {y_pos:.3f}) (size 1.5 0.4) (layers F.Cu F.Mask F.Paste) (tstamp "{make_uuid()}"))')
    for i in range(10):
        y_pos = y_start + (9 - i) * 0.65
        pads_u2.append(f'    (pad "{11+i}" smd rect (at 3.1 {y_pos:.3f}) (size 1.5 0.4) (layers F.Cu F.Mask F.Paste) (tstamp "{make_uuid()}"))')
    
    content += "\n".join(pads_u2)
    content += """
  )

  # Add 2-pin PACK Connector at (170, 125)
  (footprint "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" (layer F.Cu)
    (at 172 125)
    (descr "PACK Connector")
    (tstamp "{make_uuid()}")
    (property "Reference" "P2" (at 172 122) (layer F.SilkS) (effects (font (size 1.0 1.0) (thickness 0.2))))
    (property "Value" "PACK_CONN" (at 172 128) (layer F.SilkS) (effects (font (size 1.0 1.0) (thickness 0.2))))
    (pad "1" thru_hole circle (at 0 -1.27) (size 1.7 1.7) (drill 1.0) (layers *.Cu *.Mask) (tstamp "{make_uuid()}"))
    (pad "2" thru_hole circle (at 0 1.27) (size 1.7 1.7) (drill 1.0) (layers *.Cu *.Mask) (tstamp "{make_uuid()}"))
  )

  # Add 5-pin Cell Connector at (108, 125)
  (footprint "Connector_PinHeader_2.54mm:PinHeader_1x05_P2.54mm_Vertical" (layer F.Cu)
    (at 108 125)
    (descr "5-Pin Cell Connector (VC0 to VC4)")
    (tstamp "{make_uuid()}")
    (property "Reference" "P1" (at 108 116) (layer F.SilkS) (effects (font (size 1.0 1.0) (thickness 0.2))))
    (property "Value" "CELL_CONN" (at 108 134) (layer F.SilkS) (effects (font (size 1.0 1.0) (thickness 0.2))))
    (pad "1" thru_hole circle (at 0 -5.08) (size 1.7 1.7) (drill 1.0) (layers *.Cu *.Mask) (tstamp "{make_uuid()}"))
    (pad "2" thru_hole circle (at 0 -2.54) (size 1.7 1.7) (drill 1.0) (layers *.Cu *.Mask) (tstamp "{make_uuid()}"))
    (pad "3" thru_hole circle (at 0 0) (size 1.7 1.7) (drill 1.0) (layers *.Cu *.Mask) (tstamp "{make_uuid()}"))
    (pad "4" thru_hole circle (at 0 2.54) (size 1.7 1.7) (drill 1.0) (layers *.Cu *.Mask) (tstamp "{make_uuid()}"))
    (pad "5" thru_hole circle (at 0 5.08) (size 1.7 1.7) (drill 1.0) (layers *.Cu *.Mask) (tstamp "{make_uuid()}"))
  )
)
"""
    return content

if __name__ == "__main__":
    bms_pcb_dir = os.path.dirname(os.path.abspath(__file__))
    sch_path = os.path.join(bms_pcb_dir, "bms_pcb.kicad_sch")
    pcb_path = os.path.join(bms_pcb_dir, "bms_pcb.kicad_pcb")
    
    print(f"Generating schematic at: {sch_path}")
    with open(sch_path, "w", encoding="utf-8") as f:
        f.write(generate_schematic())
        
    print(f"Generating layout at: {pcb_path}")
    with open(pcb_path, "w", encoding="utf-8") as f:
        f.write(generate_pcb())
        
    print("KiCad project files generated successfully.")
