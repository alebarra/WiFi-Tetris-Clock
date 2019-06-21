
// Set a timezone using the following list
// https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
#define MYTIMEZONE "Europe/Rome"

// Sets whether the clock should be 12 hour format or not.
bool twelveHourFormat = true;

// If this is set to false, the number will only change if the value behind it changes
// e.g. the digit representing the least significant minute will be replaced every minute,
// but the most significant number will only be replaced every 10 minutes.
// When true, all digits will be replaced every minute.
bool forceRefresh = true;
// -----------------------------

// ----- Wiring -------
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_OE 2
#define P_D 12
#define P_E 0
// ---------------------

#include <PxMatrix.h>
// The library for controlling the LED Matrix
// Can be installed from the library manager
// https://github.com/2dom/PxMatrix
// PxMATRIX display(32,16,P_LAT, P_OE,P_A,P_B,P_C);
PxMATRIX display(64,32,P_LAT, P_OE,P_A,P_B,P_C,P_D);
//PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D, P_E);



String driver="FM6126A";
