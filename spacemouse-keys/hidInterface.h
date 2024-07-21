// hidInterface.h


bool send_command(int16_t rx, int16_t ry, int16_t rz, int16_t x, int16_t y, int16_t z, uint8_t *keys, int debug);
bool IsNewHidReportDue(unsigned long now);
#ifdef ADV_HID_JIGGLE
bool jiggleValues(uint8_t val[6], bool lastBit);
#endif