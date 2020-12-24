void setup() {

  Serial.begin(115200);

  long upperBound = 0;
  long lowerBound = 0;
  long seedValue  = 0;

  seedValue = 1;

  for (uint8_t i = 0; i <= 9; i++)
  {
    if (seedValue == 0) seedValue = 123459876L;                           // Prevent the Seed Value from being zero
    upperBound = seedValue / 127773L;                                     // Create an upper bound to prevent overflowing 31 bits
    lowerBound = seedValue % 127773L;                                     // Create a lower bound  to prevent overflowing 31 bits

    /*
       THIS IS WHERE THE "RANDOM" VALUE IS GENERATED.
       The calculation performed below is designed to generate a value that won't overflow and then the result
       is used to seed the next calculation.
    */
    
    seedValue = 16807L * lowerBound - 2836L * upperBound;

    if (seedValue < 0) seedValue += 0x7fffffffL;  // If seedValue is now negative for some reason, flip it to a positive value.

    /*
       RANDOM_MAX is defined in stdlib as 0x7FFFFFFF, 0x01 below where a signed long variable becomes negative
       This prevents overflow, because if seedValue is between 0 and RANDOM_MAX then seedValue modulo RANDOM_MAX will return seedValue.
       If it's too big, then the modulo of seedValue returns a much smaller number.
    */
    uint32_t danValue = ((seedValue) % ((unsigned long)RANDOM_MAX + 1));
    
    uint32_t avrValue = random();
    Serial.print   ("Dan Generated Random :     0x");
    Serial.println (danValue, HEX);
    Serial.print   ("AVRLIBC Generated Random : 0x");
    Serial.println (avrValue, HEX);
    Serial.print   ("Difference = ");
    Serial.println (danValue - avrValue);
    Serial.println ();
  }
}

void loop() {
}
