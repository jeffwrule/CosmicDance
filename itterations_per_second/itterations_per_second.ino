
long start_time=0;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    Serial.println("Setup Starting");

    start_time=millis();
      
}

#define PRINT_AFTER 5000

void loop() {
    // put your main code here, to run repeatedly:

    long cur_ms=millis();
    static long start_sample_ms=0;
    static long next_print=0;
    static long itters=0;
    static long elapsed_sec=0;

    itters++;

    elapsed_sec = (cur_ms - start_sample_ms) / 1000.0;
     
    if (cur_ms >= next_print ) {
        Serial.print("itters/second=");
        Serial.print(itters/elapsed_sec);
        Serial.print(", seconds=");
        Serial.print(elapsed_sec);
        Serial.print(", itters=");
        Serial.print(itters);
        Serial.print(", cur_ms=");
        Serial.println(cur_ms);
        start_sample_ms=cur_ms;
        next_print = start_sample_ms + PRINT_AFTER;
        itters=0;
    }

}
