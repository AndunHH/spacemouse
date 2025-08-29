# This script sets the correct names and hardware IDs for Spacemouse emulation to work.
# From https://stackoverflow.com/a/76049354.

Import("env")
board_config = env.BoardConfig()
board_config.update("build.hwids", [["0x256f", "0xc631"]])
board_config.update("build.usb_product", "SpaceMouse Pro Wireless (cabled)")
board_config.update("vendor", "3Dconnexion")
