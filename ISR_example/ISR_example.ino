#define INTERRUPT_WAIT_MS 300

long last_millis=0;

#define WAIT_BETWEEN_READS_MS 200
long sensor_last_read_ms=0;

#define NUM_SHORT_READS 10

#define LONG_READ_LENGTH_MS 5000
long analog_reads_long[5000 / WAIT_BETWEEN_READS_MS + 1] ;

long num_reads=LONG_READ_LENGTH_MS/WAIT_BETWEEN_READS_MS + 1;
long sensor_index=0;
long sensor_read_avg=0;

#define NUM_ANALOG_VALUES 50
long read_histogram[NUM_ANALOG_VALUES];

// sensor read stats
long total_analog_reads=0;
long max_read=0;

void setup() {
  Serial.begin(250000);
  Serial.print("Num Values in average: ");
  Serial.println(num_reads);
  Serial.print("NUM_ANALOG_VALUES: ");
  Serial.println(NUM_ANALOG_VALUES);
  pinMode(A0, INPUT);

  for (long i=0; i < num_reads; i++) {
    analog_reads_long[i]=0;
  }
  sensor_read_avg=0;

  Serial.println("Setup Complete");
  
}


long short_read() {
  long short_read_total;
  for(long i=0; i < NUM_SHORT_READS; i++) {
    short_read_total += analogRead(A0);
    total_analog_reads++;
  }

  return (int) (short_read_total / NUM_SHORT_READS);
}

long cur_millis;

void loop() {

  long total_reads;
  

  cur_millis = millis();

  analog_reads_long[sensor_index] = analogRead(A0);
  total_analog_reads++;
  sensor_index++;
  if (sensor_index >= num_reads) {
    sensor_index=0;
    // Serial.println("looping...");
  }
  

  if (cur_millis - last_millis > 3000) {
    last_millis = cur_millis;

    /* compute the average */   
    for (long i=0, total_reads=0; i < num_reads; i++) {
      long cur_read;
      cur_read = analog_reads_long[i];
      if (cur_read < NUM_ANALOG_VALUES) {
        read_histogram[cur_read] += 1;
        }
      total_reads += cur_read;
    }
    sensor_read_avg = total_reads/num_reads;
    
    Serial.print("Sensor_index: ");
    Serial.print(sensor_index);
    Serial.print(", total analog reads: ");
    Serial.print(total_analog_reads);
    Serial.print(", Sensor Avg Value: ");
    Serial.println(sensor_read_avg);

    for (long i=0; i< NUM_ANALOG_VALUES; i++) {
      if (read_histogram[i] > 0 ) {
        Serial.print(i);
        Serial.print("=");
        Serial.println(read_histogram[i]);
      }
    }

    /* clear the histogram */
    for (long i=0; i < NUM_ANALOG_VALUES; i++) {
      read_histogram[i] = 0;
    }
 }

//  int signal_value = analogRead(A0);
//  Serial.print(" ");
//  Serial.print(signal_value);

}
