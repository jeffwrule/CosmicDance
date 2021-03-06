#include <EnableInterrupt.h>

#define INTERRUPT_PIN 4
class A {

public:
  static A *isr_instance;    // using this declaration of isr_instance causes link errors
  volatile unsigned long ticks=0;

  static void isr();
  void isr_function();

  A() {
    A::isr_instance = this;
    pinMode(INTERRUPT_PIN, INPUT_PULLUP);
    enableInterrupt(INTERRUPT_PIN, isr, RISING);
    // attachPCINT(digitalPinToPCINT(INTERRUPT_PIN), isr, RISING); 
  }

};

// have to remove this when initializing the variable in the class def
static A *A::isr_instance=NULL;

static void A::isr() {
    if (A::isr_instance) A::isr_instance->isr_function();
}

void A::isr_function() {
  ticks++;
}

A *my_a;

void setup() {

  Serial.begin(250000);

  my_a = new A();
}

void loop() {

  static unsigned long last_print_ms = 0;
  static unsigned long cur_ms;

  cur_ms = millis();

  if (cur_ms - last_print_ms > 300) {
    Serial.print(F("ticks: "));
    Serial.print(my_a->ticks);
    Serial.print(F(", pin_state="));
    Serial.println(digitalRead(INTERRUPT_PIN));
    last_print_ms = cur_ms;
  }
}
