# Release V3 of the DIY spacemouse changes the parameter names. This is necessary because the new features allow the serial console to print parameter names and values. 
# Therefore the parameter name itself is stored in the programm. To long parameter name take to much program size. 
# Because changing the parameter names breaks the working config files of user, this scripts is meant to 
# update your config.h file.
# This python script reads spacemouse-keys/config.h, applies replacements, writes spacemouse-keys/config_renamed.h
# Please compare and check the new file and replace the old file with it, if you like it.

replacements = {
    "TRANSX_SENSITIVITY": "SENS_TX",
    "TRANSY_SENSITIVITY": "SENS_TY",
    "POS_TRANSZ_SENSITIVITY": "SENS_PTZ",
    "NEG_TRANSZ_SENSITIVITY": "SENS_NTZ",
    "GATE_NEG_TRANSZ": "GATE_NTZ",
    "GATE_ROTX": "GATE_RX",
    "GATE_ROTY": "GATE_RY",
    "GATE_ROTZ": "GATE_RZ",
    "ROTX_SENSITIVITY": "SENS_RX",
    "ROTY_SENSITIVITY": "SENS_RY",
    "ROTZ_SENSITIVITY": "SENS_RZ",
    "SLOPE_AT_ZERO": "MOD_A",
    "SLOPE_AT_END": "MOD_B",
    "COMP_ENABLED": "COMP_EN",
    "COMP_NO_OF_POINTS": "COMP_NR",
    "COMP_WAIT_TIME": "COMP_WAIT",
    "COMP_MIN_MAX_DIFF": "COMP_MDIFF",
    "COMP_CENTER_DIFF": "COMP_CDIFF",
    "EXCLUSIVEMODE": "EXCLUSIVE",
    "PRIO_Z_EXCLUSIVEMODE": "EXCL_PRIOZ",
    "ECHOES": "RAXIS_ECH",
    "SIMSTRENGTH": "RAXIS_STR",
}

with open("spacemouse-keys/config.h", encoding="utf-8") as f:
    text = f.read()

for old, new in replacements.items():
    text = text.replace(old, new)

with open("spacemouse-keys/config_renamed.h", "w", encoding="utf-8") as f:
    f.write(text)

print("Replacements complete. Please check config_renamed.h and replace the original file with it, if you are happy.")