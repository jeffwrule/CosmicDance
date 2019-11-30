
#define NUM_THINGS 4

class XThing {
  private: 
    int pin;

  public: 
  
    XThing ( int pin) : pin(pin) {};

    void display();
};

void XThing::display() {
  Serial.print("    XThing pin: ");
  Serial.println(pin);
}

class IntList {

  private:
  
    const int *my_list;
    const int list_size;

  public:
  
    IntList(int *my_list, int list_size) : my_list(my_list), list_size(list_size) {} ;

    void display();
      
};

void IntList::display() {
  Serial.print("IntList: ");
  for (uint8_t i=0; i<list_size; i++) {
    Serial.print(my_list[i]);
    i < list_size - 1 ? Serial.print(" , ") : Serial.println("");
  }
}

class IntPtrList {

  private:
  
    const int **my_list;
    const int list_size;

  public:
  
    IntPtrList(int **my_list, int list_size) : my_list(my_list), list_size(list_size) {} ;

    void display();
      
};

void IntPtrList::display() {
  Serial.print("Class IntList, passed an array of integers: ");
  for (uint8_t i=0; i<list_size; i++) {
    Serial.print(*my_list[i]);
    i < list_size - 1 ? Serial.print(" , ") : Serial.println("");
  }
}


class ClassPtrList {

  private:
  
    const XThing **my_list;
    const int list_size;

  public:
  
    ClassPtrList(XThing **my_list, int list_size) : my_list(my_list), list_size(list_size) {} ;

    void display();
      
};

void type_of(XThing **a){Serial.println("it's an XThing **");}
void type_of(XThing *a){Serial.println("it's an XThing *");}
void type_of(XThing a){Serial.println("it's an XThing");}

void ClassPtrList::display() {
  Serial.println("List of xthings classes passed to another class: ");
  for (uint8_t i=0; i<list_size; i++) {
    my_list[i]->display();
  }
}

void typesof(String a){Serial.println("it's a String");}

XThing *things[NUM_THINGS];
int    int_list[NUM_THINGS];
IntList *my_intlist;
int *int_ptr_list[NUM_THINGS];
ClassPtrList *xthing_handler;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(250000);

  /*
   * send of list of integers to IntList to process
   */
  for (uint8_t i=0; i<NUM_THINGS; i++) {
    int_list[i] = i*10+10;
  }
  my_intlist = new IntList(int_list, 4);
  my_intlist->display();  

  /*
   * initialize a list of integer pointers
   */
  for(uint8_t i=0; i<NUM_THINGS; i++) {
    int_ptr_list[i] = new int(i*100+100);
  }
  Serial.print("List of integer pointers: ");
  for (uint8_t i=0; i<NUM_THINGS; i++) {
    Serial.print(*int_ptr_list[i]);
    i < NUM_THINGS - 1 ? Serial.print(" , ") : Serial.println("");
  }


  /*
   * Process a list of XThing class pointers
   */
  for (uint8_t i=0; i<NUM_THINGS; i++) {
    things[i] = new XThing(i);
  }

  Serial.println("List of xthings class....");
  for (uint8_t i=0; i<NUM_THINGS; i++) {
    things[i]->display();
  }
  
  /*
   * send a list of xthing pointers to a class to process
   */
  xthing_handler = new ClassPtrList(things, NUM_THINGS);
  xthing_handler->display();
   
}

void loop() {
  // put your main code here, to run repeatedly:

}
