#ifndef CALIBRATION_CHECKS_h
#define CALIBRATION_CHECKS_h

/* Compile-time parameter validation checks
 * This file contains all static assertions and preprocessor checks
 * to validate configuration parameters in config.h during compilation.
 */

// Check that NUMKILLKEYS does not exceed total number of keys
#if (NUMKILLKEYS > NUMKEYS)
#error "Number of Kill Keys can not be larger than total number of keys"
#endif

// Check that kill key indices are within valid range
#if (NUMKILLKEYS > 0 && ((KILLROT > NUMKEYS) || (KILLTRANS > NUMKEYS)))
#error "Index of killkeys must be smaller than the total number of keys"
#endif

// If LEDRING is defined, LEDpin must also be defined
#ifdef LEDRING
#ifndef LEDpin
#error "LEDpin must be defined if LEDRING is used"
#endif
#endif

// Check KEYLIST size matches NUMKEYS
#if NUMKEYS > 0
constexpr int _keyListCompile[] = KEYLIST;
static_assert(sizeof(_keyListCompile) / sizeof(_keyListCompile[0]) == NUMKEYS,
              "KEYLIST element count does not match NUMKEYS definition in config.h");

constexpr bool _isValueInArray(const int *arr, int size, int idx, int value) {
  return (idx >= size)         ? false
         : (arr[idx] == value) ? true
                               : _isValueInArray(arr, size, idx + 1, value);
}
// Check that LEDpin is not in KEYLIST to avoid pin conflicts
#ifdef LEDpin
static_assert(!_isValueInArray(_keyListCompile, NUMKEYS, 0, LEDpin),
              "LEDpin conflicts with a pin in KEYLIST in config.h");
#endif

// Check that ENCODER_CLK is not in KEYLIST to avoid pin conflicts
#ifdef ENCODER_CLK
static_assert(!_isValueInArray(_keyListCompile, NUMKEYS, 0, ENCODER_CLK),
              "ENCODER_CLK conflicts with a pin in KEYLIST in config.h");
#endif

// Check that ENCODER_DT is not in KEYLIST to avoid pin conflicts
#ifdef ENCODER_DT
static_assert(!_isValueInArray(_keyListCompile, NUMKEYS, 0, ENCODER_DT),
              "ENCODER_DT conflicts with a pin in KEYLIST in config.h");
#endif
#endif

// Check ROTARY_AXIS is in valid range (0-6)
#if defined(ROTARY_AXIS) && (ROTARY_AXIS < 0 || ROTARY_AXIS > 6)
#error "ROTARY_AXIS must be between 0 and 6 (0=disabled, 1-6=axis choice)"
#endif

// Ensure ROTARY_AXIS and ROTARY_KEYS are not both enabled at the same time
#if defined(ROTARY_AXIS) && (ROTARY_AXIS > 0) && defined(ROTARY_KEYS) && (ROTARY_KEYS > 0)
#error "Only one of ROTARY_AXIS and ROTARY_KEYS may be enabled at the same time"
#endif

#endif // CALIBRATION_CHECKS_h
