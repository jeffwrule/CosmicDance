#include <TimerOne.h>

class A {

public:
  static A *isr_instance=NULL;    // using this declaration of isr_instance causes link errors
  volatile unsigned long ticks=0;

  static void isr();
  void isr_function();

  A() {
    A::isr_instance = this;
  }

};

// have to remove this when initializing the variable in the class def
//A *A::isr_instance=NULL;

static void A::isr() {
  if (A::isr_instance) A::isr_instance->isr_function();
}

void A::isr_function() {
  ticks++;
}

A *my_a;

void setup() {

  my_a = new A();
  Timer1.initialize(100000); // set a 1/10 of a second just to test this out.
  Timer1.attachInterrupt( A::isr ); // attach the service routine here
  Serial.begin(250000);
}

void loop() {

  static unsigned long last_print_ms = 0;
  static unsigned long cur_ms;

  cur_ms = millis();

  if (cur_ms - last_print_ms > 1000) {
    Serial.print(F("ticks: "));
    Serial.println(my_a->ticks++);
    last_print_ms = cur_ms;
    my_a->ticks = 0;
  }
}
