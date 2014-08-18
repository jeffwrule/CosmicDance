const int fobA = A5;
const int v12Switch = A0;
const int v5Switch = A1;


int fobA_state;
int fobB_state ;
int fobC_state;
int fobD_state;

void setup() {
  Serial.begin(9600);
  Serial.println("starting setup");
  pinMode(v12Switch, OUTPUT);
  pinMode(v5Switch, OUTPUT);
  pinMode(fobA, INPUT);
  digitalWrite(v12Switch, LOW);
  digitalWrite(v5Switch, LOW);
  digitalWrite(fobA, LOW);
  fobA_state = -1;

  delay(2000);
  Serial.println("Setup complete...");
}

int checkState(int fob, int last_state, char fob_name) {
  int cur_state = digitalRead(fob);
  if (cur_state != last_state) {
    Serial.print("Fob");
    Serial.print(fob_name);
    Serial.print(" changed sate from: ");
    Serial.print(last_state);
    Serial.print(" to: ");
    Serial.println(cur_state);
    return cur_state;
  }
}

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void loop () {
  if (Serial.available()) {
    char command = Serial.read();
    switch (command) {
      case '1': digitalWrite(v12Switch, HIGH); digitalWrite(v5Switch, HIGH); break;
      case '0': digitalWrite(v12Switch, LOW); digitalWrite(v5Switch, LOW); break;
    }
  }
  fobA_state = checkState(fobA, fobA_state, 'A');
  digitalWrite(v12Switch, fobA_state);
  digitalWrite(v5Switch, fobA_state);
}

