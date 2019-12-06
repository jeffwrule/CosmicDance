#ifndef INTERRUPT_DEV_H
#define INTERRUPT_DEV_H

#include <EnableInterrupt.h>

/* not we don't support pins 0 and 1 */
// #define INTER_MAX_PIN 4

class InterruptDev {

  private:

    uint8_t  index;                       // my index into list of interrupt devices
    uint8_t  pin;                         // pin to recieve interupts on
    uint32_t debounce_ms;                 // how long to wait after interrupt to record the next
    boolean is_pullup;                    // make the input pin a pullup pin
    uint8_t edge;                         // which edge to fire the interrupt on
    volatile uint32_t cur_ms=0;           // current time in ms
    volatile uint32_t last_int_ms=0;      // time of last interrupt
    volatile uint32_t num_interrupts=0;   // total interrupts seen
    
    void isr_internal();        // instance routine to act upon the interrupt

  public:

    static void isr2();                        // service routine to catch interrupt 
    static void isr3();                        // service routine to catch interrupt 
    static void isr4();                        // service routine to catch interrupt 
    static void isr5();                        // service routine to catch interrupt 
    static void isr6();                        // service routine to catch interrupt 
    static void isr7();                        // service routine to catch interrupt 
    static void isr8();                        // service routine to catch interrupt 
    
    static InterruptDev  *id_instance[INTERRUPT_DEV_NUM_DEVICES];     // instance of the interrupt device
    
    uint32_t get_num_interrupts();        // return the number of total interrupts
    uint8_t  get_pin_state();             // return the current pin state
    void display();                       // dump the details about this device

    InterruptDev(uint8_t index, uint8_t pin, uint32_t debounce_ms, boolean is_pullup, uint8_t edge) : 
        index(index),
        pin(pin),
        debounce_ms(debounce_ms), 
        is_pullup(is_pullup),
        edge(edge) {
      Serial.println(F("InterruptDev Constructor Starting"));
      if (index > INTERRUPT_DEV_NUM_DEVICES) return;                      // we can only support this many interrupts so far
      if (is_pullup) pinMode(pin, INPUT_PULLUP);
      id_instance[index] = this;
      switch (index) {
        case 2:
                enableInterrupt(pin, isr2, edge);
                break;
        case 3:
                enableInterrupt(pin, isr3, edge);
                break;
        case 4:
                enableInterrupt(pin, isr4, edge);
                break;
        case 5:
                enableInterrupt(pin, isr5, edge);
                break;   
        case 6:
                enableInterrupt(pin, isr6, edge);
                break;   
        case 7:
                enableInterrupt(pin, isr7, edge);
                break;   
        case 8:
                enableInterrupt(pin, isr8, edge);
                break;   
        default:
                break; 
      }
    }
};

static InterruptDev*  InterruptDev::id_instance[INTERRUPT_DEV_NUM_DEVICES] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};   // initialize the instance variable

static void InterruptDev::isr2() {
  if (InterruptDev::id_instance[2]) id_instance[2]->isr_internal();
}

static void InterruptDev::isr3() {
  if (InterruptDev::id_instance[3]) id_instance[3]->isr_internal();
}

static void InterruptDev::isr4() {
  if (InterruptDev::id_instance[4]) id_instance[4]->isr_internal();
}

static void InterruptDev::isr5() {
  if (InterruptDev::id_instance[5]) id_instance[5]->isr_internal();
}

static void InterruptDev::isr6() {
  if (InterruptDev::id_instance[6]) id_instance[6]->isr_internal();
}

static void InterruptDev::isr7() {
  if (InterruptDev::id_instance[7]) id_instance[7]->isr_internal();
}

static void InterruptDev::isr8() {
  if (InterruptDev::id_instance[8]) id_instance[8]->isr_internal();
}

void InterruptDev::isr_internal() {
  cur_ms = millis();
  if (cur_ms_wrap) last_int_ms = 0;
  if (cur_ms - last_int_ms >= debounce_ms) {
    num_interrupts++;
    last_int_ms = cur_ms;
  }
}

// return the current pin state
uint8_t  InterruptDev::get_pin_state() {
  uint8_t read_sum=0;
  for (uint8_t i=0; i<4; i++) {
    read_sum += digitalRead(pin);    
  }
  if (read_sum >= 2.5 ) {
    return(INTERRUPT_DEV_OFF); 
  } else {
    return(INTERRUPT_DEV_ON);
  }
}


uint32_t InterruptDev::get_num_interrupts() {
  return num_interrupts;
}

void InterruptDev::display() {
  Serial.print(F("InterruptDev::display(): index="));
  Serial.print(index);
  Serial.print(F(", pin="));
  Serial.print(pin);
  Serial.print(F(", debounce_ms="));
  Serial.print(debounce_ms);
  Serial.print(F(", is_pullup="));
  Serial.print(is_pullup);
  Serial.print(F(", edge="));
  Serial.print(edge);
  Serial.print(F(", get_pin_state()="));
  Serial.print(get_pin_state());
  Serial.print(F(", num_interrupts="));
  Serial.print(num_interrupts);
  Serial.print(F(", cur_ms="));
  Serial.print(cur_ms);
  Serial.print(F(", last_int_ms="));
  Serial.print(last_int_ms);
  Serial.println(F(""));
}

#endif
