#include <IRremote.h>

const int RECV_PIN = 38;
IRrecv irrecv(RECV_PIN); // Make pin SIG for IR diode
decode_results results;
String foo;

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn();
}

void loop()
{
  if (irrecv.decode(&results))
  {
    foo = String(results.value, HEX);
    // Serial.println(results.value, HEX);
    Serial.println(foo);
    irrecv.resume();
  }
}

////// IR remote hex codes //////
// for "C.J. SHOP DAOKI IR Infrared Wireless Remote Control Kit"
// Shoutout to @replayreb on instructables for the decoder:
// https://www.instructables.com/id/IR-Control-of-IKEA-FYRTUR-Shades/
/*

1: e318261b
2: 511dbb
3: ee886d7f
4: 52a3d41f
5: d7e84b1b
6: 20FE4DBB
7: f076c13b
8: a3c8eddb
9: E5CFBD7F
*: c101e57b
0: 97483bfb
#: F0C41643
Left: 8C22657B
Right: 449E79F
Up: 3D9AE3F7
Down: 1BC0157B
OK: 488F3CBB

*/