
class A { public: int val; A(int val) : val(val) {};  };

//void do_switch(A, A);
//void swap_member(A, A);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(250000);

}

void loop() {
  // put your main code here, to run repeatedly:
  A a(1);
  A b(2);

  Serial.print("Initial: ");
  Serial.print(a.val);
  Serial.print(" , ");
  Serial.println(b.val);

  do_switch(a,b);

  Serial.print("do_switch: ");
  Serial.print(a.val);
  Serial.print(" , ");
  Serial.println(b.val);  

  A c(10);
  A d(20);

  Serial.print("Initial: ");
  Serial.print(c.val);
  Serial.print(" , ");
  Serial.println(d.val);
  
  swap_member(c, d);

  Serial.print("swap_member: ");
  Serial.print(c.val);
  Serial.print(" , ");
  Serial.println(d.val); 

  int x=1;
  int y=2;

  Serial.print("Initial int: ");
  Serial.print(x);
  Serial.print(" , ");
  Serial.println(y);

  do_switch_i(x,y);

  Serial.print("do_switch int: ");
  Serial.print(x);
  Serial.print(" , ");
  Serial.println(y);
}

void do_switch(A left, A right) {
  A tmp(5);
  tmp=left;
  left=right;
  right=tmp;
}

void do_switch_i(int left, int right) {
  int tmp(5);
  tmp=left;
  left=right;
  right=tmp;
}

void swap_member(A left, A right) {
  A tmp(0);
  tmp.val = left.val;
  left.val = right.val;
  right.val = tmp.val;

  Serial.print("swap_member(): ");
  Serial.print(left.val);
  Serial.print(" , ");
  Serial.println(right.val); 
}
