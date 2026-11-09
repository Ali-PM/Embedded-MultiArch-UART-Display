int p1[] = {2,3,4,5,6,7,8};
int p2[] = {10,11,12,13,14,15,16};
int btn = 9;
int num = 0;
int last = HIGH;

byte hex[10] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};

void setup() {
  for(int i=0;i<7;i++){pinMode(p1[i],OUTPUT); pinMode(p2[i],OUTPUT);}
  pinMode(btn, INPUT_PULLUP);
  show(0);
}

void loop() {
  int r = digitalRead(btn);
  if(r==LOW && last==HIGH){num=(num+1)%100; show(num); delay(250);}
  last=r;
}

void show(int n){showDigit(n/10,p1); showDigit(n%10,p2);}

void showDigit(int d, int p[]){
  byte pat = hex[d];
  for(int i=0;i<7;i++) digitalWrite(p[i], (pat>>i)&1);
}