#define data_pin A0
#define NUM_AVG 25
#define PRINT_EVERY_MS 1000
#define PRESSURE_CUTOFF 500
#define SHUTOFF_DELAY 2000

int     values[NUM_AVG];
int     index = 0;
long    index_loops;                // number of times we have looped around the size of our index
long    cur_millis=0;                  // current time, gathered at the beginning of each loop
long    last_print_ms=0;               // list time we sent out a status message
long    last_pressure_on_ms=0;         // last time the pressure_sensor tested ON
int     led_state=LOW;                   // track led state so we don't write to it so often

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);

    Serial.println("Begin Setup DistanceSensorTest");

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, led_state);

    pinMode(data_pin, INPUT);

    for (int i=0; i<NUM_AVG; i++) {
        values[i] = 0;
    }

    Serial.println("End Setup");

}


void loop() {


    double avg;
    double avg_sum;

    cur_millis = millis();
    
    // put your main code here, to run repeatedly:
    values[index] = analogRead(data_pin);

    avg_sum=0;
    for (int i=0; i<NUM_AVG; i++) {
        avg_sum = avg_sum + values[i];
    }
    
    avg = avg_sum / NUM_AVG;

    if (cur_millis - last_print_ms > PRINT_EVERY_MS) {
        last_print_ms = cur_millis;
        Serial.print("Avg Value: ");
        Serial.print(avg);
        Serial.print(", avg_sum=");
        Serial.print(avg_sum);
        Serial.print(", index_loops=");
        Serial.print(index_loops);
        Serial.print(", Values: ");        
        Serial.println("");   
        }


    if (avg >= PRESSURE_CUTOFF) {
        if (led_state != HIGH) {
            led_state = HIGH;
            digitalWrite(LED_BUILTIN, led_state);        
        }
        last_pressure_on_ms=cur_millis;
    } else {
        if (cur_millis - last_pressure_on_ms > SHUTOFF_DELAY) {
            if (led_state != LOW) {
                led_state = LOW;
                digitalWrite(LED_BUILTIN, led_state);
            }
        }
    }

    index++;
    if ( index == NUM_AVG ) {
        index=0;
        index_loops++;
    }


}
