int sek = 0; // tid
int minutt = 0; // tid
int timen = 15; // tid
unsigned long fMillis = 0;  // variabel for å holde styr på om klokka har blitt oppdatert
String tilPrint =""; // tid i sringformat
boolean ok = false; // klokka? stilt status
unsigned long lastDB = 0; // forrige debouncetid
int dBDelay = 50; // delay for debonce
int buttonState; // til debounce
int lastButtonState = LOW; // til debounce
int opp = 6; // opp knapp- klokkestilling
int neste = 7; // nesteknapp -klokkestillign
unsigned long stiller = 0;
unsigned long forrigeOppdatering = 0;
#include <LiquidCrystal.h> // lcd bibliotek


//Initialiserer LCD med arduino. LiquidCrystal(rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(12, 10, 5, 4, 3, 2);

void setup() {
  // skrur på lcd
  lcd.begin(16, 2);
  Serial.begin(9600); // BARE TIL TESTING
  pinMode(opp, INPUT);  // oek klokkevariabel
  pinMode(neste, INPUT); // knapp for neste variabelstilling
  pinMode(A1, INPUT); // doersensor
  pinMode(A2, INPUT); // trykksensor
  pinMode(A0, OUTPUT); // piezo
  pinMode(A3, OUTPUT); // LED
  
}
void loop() {
  
    signaliser(); // sjekker om brukeren skal ha signal
    stillKlokka(); // kaller på still klokkemetoden som ku utføres en gang pga en intern ifsjekk
    tid(); // oppdaterer tiden
    oppdaterLcd(); // oppdaterer lcd

}
void konverter(){ // konverterer tidvariablene til en lesbar string for lcd
  String tiden ="";
  if (timen< 10){
    tiden +=  "0" + String(timen) + ":";
  }else {tiden += String(timen)  + ":";
  }if (minutt< 10){
    tiden += "0" + String(minutt)+ ":";
  }else {tiden += String(minutt) + ":";
  }if (sek< 10){
    tiden += "0" + String( sek);
  }else {tiden += String(sek);
  }
  tilPrint = tiden;
}
void tid(){  // oppdaterer tiden som egt bare øker sekundene basert på millis (arduinos interne klokke)
  // og er ikke sensitiv for millis sin overflow så den resetes ikke og kan kjøre uten stopp
 unsigned long nMillis = millis();
 if((nMillis - fMillis) >= 1000){
 sek++;
 
 if (sek >= 60){ // om det har gått 60 sek økes min og sek resettes
  sek = 0;
  minutt++;
 }
 if(minutt >= 60){ // om det har gått 60 min økes time og min resettes
  minutt = 0;
  timen ++;
 }
 if (timen >= 24){ // om det har gått 24 t resettes time
  timen = 0;
 }
 fMillis = nMillis; // variabel for forrige oppdatering oppdateres 
 }
 konverter(); // konverterer tid dtil lesbar streng
}
 void stillKlokka(){ // metode for å stille klokka
  int pos = 0; // holder styrpå om det er timer eller minutter vi vil øke
    while(!ok){ // så lenge ikke klokka er stilt holder vi oss i denne løkka
      unsigned long oppdatert = millis();   // samme prinsipp for "debouncing" av tid for å få en liten "delay" mellom knappetrykk
      if(oppdatert-stiller >= 150){
      stiller = oppdatert;
       tid(); // holder styr på tiden allikevel
       if(pos == 2) ok = true; // hvis vi har stilt både timer og minutter låses denne metoden
       if (registrerTrykk(opp) and pos == 0){timen++; // oppdaterer time
       }
       if (registrerTrykk(opp) and pos == 1) minutt++; // oppdaterer minutt
       if( registrerTrykk(neste))pos++;
       oppdaterLcd(); // skriver ut nye verdier på lcd
      }
  }
 }

boolean registrerTrykk(int trykk){ // debounce metode for knappene så jeg slipper å debounce flere ganger
  int reading = digitalRead(trykk);
  if (reading != lastButtonState) {
    lastDB = millis();
  }
  boolean trykket = reading;
  if ((millis() - lastDB) > dBDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        trykket = true;
      }
    }
  }
  lastButtonState = reading;
  return trykket;
}
void oppdaterLcd(){ // metode for å oppdatereskjerm ( skjermen oppdateres hvert halvsekund)
  unsigned long oppdatert = millis();
  if (oppdatert - forrigeOppdatering >= 500){
    forrigeOppdatering = oppdatert;
    lcd.clear();
    konverter();
    lcd.print(tilPrint);
  }
}

void signaliser(){ // metoe for å sjekke om vi skal gi signal til bruker m å huske matpakka, om kl er mellom 8 og 16 (skoletid) 
  // og det er mat i matboksen og dorea aapnes faar bruker signal om dette
  if(timen>8 and timen <16){
    if(digitalRead(A1) == LOW and digitalRead(A2) == HIGH){
      digitalWrite(A0, HIGH);
      digitalWrite(A3, HIGH);
  }else{
    digitalWrite(A0, LOW);
    digitalWrite(A3, LOW);
    
  }
}
}
