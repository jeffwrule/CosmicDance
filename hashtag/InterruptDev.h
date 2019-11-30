#ifndef INTERRUPT_DEV_H
#define INTERRUPT_DEV_H

#include <PinChangeInterrupt.h>

/* not we don't support pins 0 and 1 */
#define INTER_MAX_PIN 4

class InterruptDev {

  private:

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
    static InterruptDev  *id_instance[INTER_MAX_PIN+1];     // instance of the interrupt device
    
    uint32_t get_num_interrupts();        // return the number of total interrupts
    void display();                  // dump the details about this device

    InterruptDev(uint8_t pin, uint32_t debounce_ms, boolean is_pullup, uint8_t edge) : 
        pin(pin),
        debounce_ms(debounce_ms), 
        is_pullup(is_pullup),
        edge(edge) {
      if (pin > 4) return;                      // we don't support interrupts on pins above 4 yet
      if (is_pullup) pinMode(pin, INPUT_PULLUP);
      id_instance[pin] = this;
      switch (pin) {
        case 2:
                // attachInterrupt(digitalPinToInterrupt(pin), isr2, edge); 
                attachPCINT(digitalPinToPCINT(pin), isr2, edge); 
                break;
        case 3:
                // attachInterrupt(digitalPinToInterrupt(pin), isr3, edge); 
                attachPCINT(digitalPinToPCINT(pin), isr3, edge); 
                break;
        case 4:
                attachPCINT(digitalPinToPCINT(pin), isr4, edge); 
                break;   
        default:
                break; 
      }
    }
};

static InterruptDev*  InterruptDev::id_instance[INTER_MAX_PIN+1] = {NULL, NULL, NULL, NULL, NULL};   // initialize the instance variable

static void InterruptDev::isr2() {
  if (InterruptDev::id_instance[2]) id_instance[2]->isr_internal();
}

static void InterruptDev::isr3() {
  if (InterruptDev::id_instance[3]) id_instance[3]->isr_internal();
}

static void InterruptDev::isr4() {
  if (InterruptDev::id_instance[4]) id_instance[4]->isr_internal();
}

void InterruptDev::isr_internal() {
  cur_ms = millis();
  if (cur_ms < last_int_ms) last_int_ms = 0;
  if (cur_ms - last_int_ms >= debounce_ms) {
    num_interrupts++;
    last_int_ms = cur_ms;
  }
}

uint32_t InterruptDev::get_num_interrupts() {
  return num_interrupts;
}

    volatile uint32_t cur_ms=0;           // current time in ms
    volatile uint32_t last_int_ms=0;      // time of last interrupt
    volatile uint32_t num_interrupts=0;   // total interrupts seen

void InterruptDev::display() {
  Serial.print(F("InterruptDev: pin="));
  Serial.print(pin);
  Serial.print(F(", debounce_ms="));
  Serial.print(debounce_ms);
  Serial.print(F(", is_pullup="));
  Serial.print(is_pullup);
  Serial.print(F(", edge="));
  Serial.print(edge);
  Serial.print(F(", num_interrupts="));
  Serial.print(num_interrupts);
  Serial.print(F(", cur_ms="));
  Serial.print(cur_ms);
  Serial.print(F(", last_int_ms="));
  Serial.print(last_int_ms);
  Serial.print(F(", id_instance="));
  Serial.print((uint8_t) id_instance[pin]);
  Serial.println(F(""));
}

#endif
