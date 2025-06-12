//INPUT PULLUP OG BUTTON HIGH/LOW MÅ BYTTES
#include <WS2812.h>
WS2812 LED(1);
  cRGB value;

//toner:
const float tone_F4 = 349.23;
const float tone_A4 = 440;
const float tone_C5 = 523.25;
const float tone_D5 = 587.33;
const float tone_Dess5 = 622.25;
const float tone_E5 = 659.25;
const float tone_F5 = 698.46;
const float tone_G5 = 783.99;

//lampene til hvert fargede garnnøste//unike farger i en sekvens:
const int red = 5;
const int blue = 4;
const int yellow = 3;
const int green = 2;

int fargePins[] = { red, blue, yellow, green };

// RGB pins 
//fjernes?
const int rgbRed = 10;
const int rgbGreen = 8;
const int rgbBlue = 9;

// RGB multi-dimensjonal array
const int farger[4][3] = {
  { 255, 0, 0 },   // Rød
  { 0, 0, 255 },   // Blå
  { 0, 255, 0 },   // Grønn
  { 255, 255, 0 }  // Gul
};

//ordbok mulig løsning

//potentiometer pin
const int potPin = A0;

//piezo pin
const int buzzerPin = 7;

//start-knapp pin
const int buttonPin = 6;

// Poenglamper
const int poeng1 = 13;
const int poeng2 = 12;
const int poeng3 = 11;

//array med poenglampene
int poengLamper[] = { poeng1, poeng2, poeng3 };

//Start-knapp variabel
int buttonState = 1;

//variabel lengde på sekvensen
const int lengdeSekvens = 3;  //burde egt ikke være const
int systemSekvens[lengdeSekvens];
int brukerSekvens[lengdeSekvens];

//setter poeng til å starte på 0
int poeng = 0;

//boolsk verdi som sier om spillet er over eller ikke
bool fortsett = true;

void setup() {
  LED.setOutput(9);
  Serial.begin(9600);
  //gjør at vi kan printe til serial monitoren om jeg forstod det riktig, slik at vi kan legge inn print og feilsøke mer

  //start-knapp
  pinMode(buttonPin, INPUT_PULLUP);//HUSK Å TA MED!!!!

  //RGB
  pinMode(rgbRed, OUTPUT);
  pinMode(rgbGreen, OUTPUT);
  pinMode(rgbBlue, OUTPUT);

  pinMode(buzzerPin, OUTPUT);  //piezo

  for (int poeng : poengLamper) {  //poenglamper
    pinMode(poeng, OUTPUT);
  }

  for (int farge : fargePins) {  //sekvenslamper
    pinMode(farge, OUTPUT);
  }


  //leser tilfeldig tall fra A1
  randomSeed(analogRead(A1));  //kand obbelsjekkes
}

void loop() {  //tanken er at spillet fortsetter en og en runde frem til man oppnår tre poeng. checks out?
  settFarge(0, 0, 0);
  int verdi = analogRead(potPin);  
  Serial.println(verdi);

  for (int farge : fargePins){
    digitalWrite(farge, HIGH);
  }

  buttonState = digitalRead(buttonPin);
  if(buttonState == LOW){
    spill();
  }
  
  delay(250);
}

void spill() {
  Serial.println("Starter spillet");
  //if (buttonState == HIGH) {//burde gjøre at du bare trenger å trykke på knappen en gang (ikke holde inne, da er det feil)
  genererOgVisSekvens();
  sammenlign();  // Bruker input + evaluering
  sjekkSeier();
}


void genererOgVisSekvens() {
  Serial.println("genererer sekvens");
  // Tellere for hver farge for å begrense maks to av samme farge
  int fargeTeller[] = { 0, 0, 0, 0 };

  for (int i = 0; i < lengdeSekvens; i++) {
    int farge = 0;
    while (fargeTeller[farge] >= 1) {
      farge = random(0, 4);  //0-3, fire farger
       //settFarge(255, 0, 0);//grønn
  //settFarge(0, 255, 0); //rød
  //settFarge(0, 0, 255); //blå
  //settFarge(255, 255, 09; //gul
    }
    systemSekvens[i] = farge;
    fargeTeller[farge]++;
  }

  // Vis sekvensen
  for (int i = 0; i < lengdeSekvens; i++) {
    int systemFarge = systemSekvens[i];
    Serial.println("Systemsekvens: " + systemSekvens[i]);
    settFarge(farger[systemFarge][0], farger[systemFarge][1], farger[systemFarge][2]);
    delay(500);         // vis farge i 1 sekund //tidligere 500, endrer til 200
    settFarge(0, 0, 0);  // slå av
    delay(500);         // vent før neste //tidligere 500, endrer til 200
  }
  
  delay(100); //buffer
}

void sammenlign() {
  Serial.println("sammenligner");
  //brukerInput array
  //int brukerSekvens[lengdeSekvens]; 

  //Serial.println("Gjenta sekvensen med potensiometeret:");
  for (int i = 0; i < lengdeSekvens; i++) {
    int valgt = brukerInput();  //sjekker brukerinput, et lys om gangen
    digitalWrite(valgt, LOW);
    delay(1000); //tidligere 2000, endrer til 1000
    digitalWrite(valgt, HIGH);
    delay(250);
    
    
    if (valgt == 5){//fikser feil med at brukersekens og systemtall er forskjellige
      valgt = 2;
    }
    else if(valgt == 4){
      valgt = 1;
    }
    else if(valgt == 3){
       valgt = 3;
    }
    else if(valgt == 2){
      valgt = 0;
    }

    brukerSekvens[i] = valgt;
    Serial.println("Brukersekvens" + brukerSekvens[i]);
  

    // Slukk lyset brukeren har valgt når valget er registrert(feedback), så på igjen

    //digitalWrite(valgt, LOW);
    //delay(2000);
    //digitalWrite(valgt, HIGH);
   // delay(250);  //unngå buffring
    //kanskje sette andre lys, test
  }

  // sammenlign
  bool riktigSvar = true;
  for (int i = 0; i < lengdeSekvens; i++) {
    if (brukerSekvens[i] == systemSekvens[i]) {
      riktigSvar = true;
      continue;
    }
    else if (brukerSekvens[i] != systemSekvens[i]) {
     riktigSvar = false;
     break;
      }
    }

  if(riktigSvar == true){
    riktig();
  }
  else if (riktigSvar == false){
    feil();
  }
}

int lesPotensiometer() {                          //verdier må ferdigstilles når kretsen er ferdigbygget
  int verdi = analogRead(potPin);   
  while(verdi > 950 && verdi < 1023){ //delay når pekeren r helt ned til venstre
    delay(1000); //
  }            
   
 // if (verdi > 0 && verdi < 350) return 2;//grønn
  //else if (verdi > 379 && verdi < 485) return 3;//gul
  //else if (verdi > 558 && verdi < 650) return 4;//blå
  //else if (verdi > 729 && verdi < 900) return 5; //rød
  //else return 5; //i tilfelle brukeren ikke klarer å treffe, velges grønt. burde egt vært noe annet, er bare error handling

  //større range for alle fargene
  if (verdi >= 0 && verdi < 300) return 2;         // grønn
  else if (verdi < 493) return 3;                 // gul (350–524)
  else if (verdi < 645) return 4;                 // blå (525–674)
  else return 5; 
}

int brukerInput() {                   //per farge i sekvens
  int naaFarge = lesPotensiometer();  //antar gyldige verdier
  unsigned long startTid = millis();  //unsignlong startTid;

  while (true) {
    delay(10);  //unngå buffer
    int nyFarge = lesPotensiometer();

    if (nyFarge != naaFarge) { //antar at siste farge er den riktige
      naaFarge = nyFarge;  //
      startTid = millis();
    }

    if (millis() - startTid > 500) {  //"nedtelling" to sek
      return naaFarge; //returnerer fargen brukeren valgte
    }
  }
}

void sjekkSeier() {
  if (poeng >= 3) {
    seier();
    poeng = 0;
    fortsett = false;
  } else {
    fortsett = true;
  }
}

void settFarge( int r, int g, int b){
//må laste ned  librar
value.b = b; value.g = g; value.r = r;
LED.set_crgb_at(0, value);
LED.sync();
delay(200); //tidligere 500
}


void riktig() {
  poeng++;
  int toner[] = { tone_C5, tone_E5, tone_G5 };
  int varighet = 200;

  for (int i = 0; i < 3 ; i++) {
    tone(buzzerPin, toner[i], varighet);
    delay(260);
  }

  //lys
  if (poeng == 1) {
    digitalWrite(poeng1, HIGH);
    digitalWrite(poeng2, LOW);
    digitalWrite(poeng3, LOW);
  }


  else if (poeng == 2) {
    digitalWrite(poeng1, HIGH);
    digitalWrite(poeng2, HIGH);
    digitalWrite(poeng3, LOW);
  }


  else if (poeng == 3) {
    digitalWrite(poeng1, HIGH);
    digitalWrite(poeng2, HIGH);
    digitalWrite(poeng3, HIGH);
  }
}

void feil() {
  int toner[] = { tone_E5, tone_Dess5, tone_D5 };
  int varighet = 200;
  for (int i = 0; i < 3; i++) {
    tone(buzzerPin, toner[i], varighet);
  delay(260);
  }

  if (poeng == 1) {
    poeng--; //nå har de 0 poeng
    digitalWrite(poeng1, LOW);
    digitalWrite(poeng2, LOW);
    digitalWrite(poeng3, LOW);
  } else if (poeng == 2) {
    poeng--; //nå har de 1 poeng
    digitalWrite(poeng1, HIGH);
    digitalWrite(poeng2, LOW);
    digitalWrite(poeng3, LOW);
  }

  //bruker kan ikke ha tre poeng her og så få feil, ettersom de vinner med en gang de når tre poeng

}


void seier() {
  Serial.println("Seier");

  //victory sound
  float toner[] = { tone_F4, tone_A4, tone_C5, tone_F5, tone_C5, tone_F5 };
  int varighet[] = { 200, 200, 200, 150, 250, 600 };

 for (int i = 0; i < 6; i++) {
    tone(buzzerPin, toner[i], varighet[i]);
    delay(varighet[i] * 1.3);  //tid til neste tone//delay burde væære 130% av varigheten til hver tone
  }

  //alle lys på brettet blinker woop woop, RGB grønn
  for (int i = 0; i < 5; i++) { //skru på alle lys
    //vurderte en for-each løkke, men det ga litt delay og vi vil at alle lys blinker samtidig
    digitalWrite(poeng1, HIGH);
    digitalWrite(poeng2, HIGH);
    digitalWrite(poeng3, HIGH);

    digitalWrite(red, HIGH);
    digitalWrite(blue, HIGH);
    digitalWrite(yellow, HIGH);
    digitalWrite(green, HIGH);

    delay(10); //buffer siden RBG er litt raskere
    settFarge(255, 255, 255);  //grønt lys på RGB
    
    delay(30);  //alle lys lyser i et halvt sekund

    //skru av alle lys
    digitalWrite(poeng1, LOW);
    digitalWrite(poeng2, LOW);
    digitalWrite(poeng3, LOW);

    digitalWrite(red, LOW);
    digitalWrite(blue, LOW);
    digitalWrite(yellow, LOW);
    digitalWrite(green, LOW);

    delay(10); //buffer siden RBG er litt raskere
    settFarge(0, 0, 0);

    delay(30);  //alle lys er av i et halv sekund
  }
}