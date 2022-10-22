
#define PRINT_AFTER_MS 5000



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Setup Starting for: iterations_per_second");
  
  #ifdef ESP32
    Serial.print("CPU Frequency is=");
    Serial.print(getCpuFrequencyMhz()); //Get CPU clock
    Serial.print("MHz");
    Serial.println();
  #endif

  Serial.println("Setup Complete");
}


void loop() {
    static unsigned long next_print=PRINT_AFTER_MS;
    static unsigned long iter_start_ms=0;
    static unsigned long iters=0;
    static double elapsed_sec=0;

    unsigned long cur_ms=millis();

    iters++;
     
    if (cur_ms >= next_print ) {

        elapsed_sec = (cur_ms - iter_start_ms) / 1000.0;
        
        #ifdef ESP32
          Serial.print("loop() running on core="); Serial.print(xPortGetCoreID()); Serial.print(", ");
        #endif
        Serial.print("iters/second="); Serial.print(iters/elapsed_sec);
        Serial.print(", elapsed_sec="); Serial.print(elapsed_sec);
        Serial.print(", iters="); Serial.print(iters);
        Serial.print(", iter_start_ms="); Serial.print(iter_start_ms);
        Serial.print(", cur_ms="); Serial.print(cur_ms);
        iter_start_ms=millis();
        next_print = iter_start_ms + PRINT_AFTER_MS;   
        iters=0; 
        Serial.print(", next_print="); Serial.println(next_print);
    }
}
