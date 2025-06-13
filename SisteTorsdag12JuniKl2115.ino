#include <WS2812.h> // Importerer biblioteket som gjør at vi kan bruke RGB-lyset
WS2812 LED(1); // Angir at vi skal koble til én WS2812 diode
  cRGB value; 

// Setter variabler av toner til ulike kHz verdier
const float tone_F4 = 349.23;
const float tone_A4 = 440;
const float tone_C5 = 523.25;
const float tone_D5 = 587.33;
const float tone_Dess5 = 622.25;
const float tone_E5 = 659.25;
const float tone_F5 = 698.46;
const float tone_G5 = 783.99;

// Setter hvilke pins LED-lysene som skal representere garn-nøster er koblet til
const int red = 5;
const int blue = 4;
const int yellow = 3;
const int green = 2;

// Lager en array av de overnevnte LED-lysene, for å bl.a. lettere kunne kalle på dem i for-løkker
int fargePins[] = { red, blue, yellow, green };


// Lager en multi-dimensjonal Array for RGB-verdier. Den ytterste arrayen r hvilke farger de skal representere.
// Den innerste arrayen er tall-verdiene for de ulike fargene. 
const int farger[4][3] = {
  { 255, 0, 0 },   // Rød
  { 0, 0, 255 },   // Blå
  { 0, 255, 0 },   // Grønn
  { 255, 255, 0 }  // Gul
};

// Setter pinnen potentiometeret er koblet til:
const int potPin = A0;

// Setter pinnen piezoen er koblet til:
const int buzzerPin = 7;

// Setter pinnen trykk-knappen er koblet til:
const int buttonPin = 6;

// Setter pinnene poenglampene er koblet til:
const int poeng1 = 13;
const int poeng2 = 12;
const int poeng3 = 11;

// Lager en array med poenglampene:
int poengLamper[] = { poeng1, poeng2, poeng3 };

// Initialiserer start-verdien til buttonState. Vanligvis er det 0, men siden vi bruker INPUT_PULLUP, den innebygde resistoren i den digitale pinnen,
// blir det motsatt.
int buttonState = 1;

// Setter verdien på hvor lang sekvensen skal være. Kan endres, ettersom metodene referer til variabelen, ikke et magisk tall.
const int lengdeSekvens = 3;  
//Oppretter to arrayer, et for systemet for å lage en tilfeldig sekvens, og en som brukeren kan fylle med verdier:
int systemSekvens[lengdeSekvens];
int brukerSekvens[lengdeSekvens];

int poeng = 0; // Setter poeng til å starte på 0

bool fortsett = true; //Boolsk verdi som sier om spillet er over eller ikke

void setup() {
  LED.setOutput(9); //Setter hvilken port RGB-lyset er koblet til
  Serial.begin(9600); //Setter baud rate til 9600, starter utveksling av data mellom Arduino og monitoren. Gjør feilsøking lettere

  pinMode(buttonPin, INPUT_PULLUP);// Konfigurerer pinnen til trykk-knapp til input, og gjør at vi kan bruke den innebygde resistoren i porten

  // Konfigurerer LED-lamper og piezo til output:
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


  //Leser tilfeldige tall fra port analog port 1, hvor ingenting er koblet til.
  randomSeed(analogRead(A1));  
}

void loop() { 
  settFarge(0, 0, 0); // Setter RGB-lyset til å starte avslått
  int verdi = analogRead(potPin);  // Setter variabelen verdi til å være verdien som leses fra potentiometeret
  //LED-lampene starter med høy voltage, altså på:
  for (int farge : fargePins){
    digitalWrite(farge, HIGH);
  }

  buttonState = digitalRead(buttonPin);  // Variabelen buttonState settes til å være verdien som leses fra porten til knappen, altså om den er trykket eller ikke
  if(buttonState == LOW){ // Hvis state er LOW, er knappen trykket. Da kalles metoden spill()
    spill();
  }
  
  delay(250); // Delay for å unngå buffer
}

void spill() {
  genererOgVisSekvens();
  sammenlign();
  sjekkSeier();
}


void genererOgVisSekvens() { // Genererer og viser en tilfeldig sekvens
  // Oppretter en array for å holde styr på at fargene ikke gjentas flere ganger i samme sekvens
  int fargeTeller[] = { 0, 0, 0, 0 };

  for (int i = 0; i < lengdeSekvens; i++) {
    int farge = 0;
    while (fargeTeller[farge] >= 1) { // En farge skal ikke gjentas mer enn én gang i en sekvens
      farge = random(0, 4); // Variabelen farge settes til å være et tilfeldig tall fra 0 til 3, som representerer hver sin farge
    }
    systemSekvens[i] = farge; // Fargen(heltallet) settes inn i array systemSekvens
    fargeTeller[farge]++; //Fargetelleren øker for å gi beskjed om at fargen er lagt til én gang
  }

  // Viser sekvensen
  for (int i = 0; i < lengdeSekvens; i++) {
    int systemFarge = systemSekvens[i];
    Serial.println("Systemsekvens: " + systemSekvens[i]); // Sender dataen til serial monitor
    settFarge(farger[systemFarge][0], farger[systemFarge][1], farger[systemFarge][2]);//Setter fargen på RGB-lyset via multi-dimensjonale arrayen
    delay(500);         // Vis fargen i et halvt sekund
    settFarge(0, 0, 0);  // Slå lyset av
    delay(500);         // Vent et halvt sekund
  }
  
  delay(100); // Buffer
}

void sammenlign() {
  for (int i = 0; i < lengdeSekvens; i++) {
    int valgt = brukerInput();  // Sjekker hva brukeren har valgt, ett lys om gangen
    digitalWrite(valgt, LOW); // Setter det valgte lyset til å slå seg av, for feedback
    delay(1000); // Venter 1 sekund før det skrus på igjen
    digitalWrite(valgt, HIGH); // Skrur det på
    delay(250); // Buffer
    
    
    if (valgt == 5){// Fikser feil med at tallene som representerer ulike farger i system- og brukersekvens er forskjellige
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

    brukerSekvens[i] = valgt; // Setter valgt farge inn i brukerSekvens
    Serial.println("Brukersekvens" + brukerSekvens[i]); // Printer ut verdiene
  }

  // Sammenligner system- og brukerSekvens
  bool riktigSvar = true;
  for (int i = 0; i < lengdeSekvens; i++) {
    if (brukerSekvens[i] == systemSekvens[i]) {
      riktigSvar = true; //Om korrekt svar, fortsett for-løkka
      continue;
    }
    else if (brukerSekvens[i] != systemSekvens[i]) {
     riktigSvar = false; //Om feil svar, bryt for-løkka
     break;
      }
    }
  // Kjør feil() eller rikig() metode basert på svar
  if(riktigSvar == true){
    riktig();
  }
  else if (riktigSvar == false){
    feil();
  }
}

int lesPotensiometer() {
  int verdi = analogRead(potPin); // Setter verdi til verdien lest fra potensiometeret

  if (verdi >= 0 && verdi < 300) return 2;         // Om potensiometeret leser en verdi over lik 0, men under 300, returner 2 som representerer grønn
  else if (verdi < 493) return 3;                 // Gul
  else if (verdi < 645) return 4;                 // Blå
  else return 5;  // Rød
}

int brukerInput() { // Leser hva brukeren peker på med potensiometeret. Går evig frem til brukerSekvens er fylt med input.  
// Setter fargen som returneres til å være den fargen brukeren peker på i 500 ms.              
  int naaFarge = lesPotensiometer(); // Variabelen naaFarge settes lik verdien lest av metoden lesPotensiometer()
  unsigned long startTid = millis(); // Starter en telling med millis

  while (true) { // Kjører til den brytes
    delay(10);  // Buffer
    int nyFarge = lesPotensiometer(); //variabelen nyFarge settes lik verdien lest av metoden lesPoensiometer()

    if (nyFarge != naaFarge) { // Hvis nyFarge ikke er den samme som naaFarge
      naaFarge = nyFarge;  // Endres naaFarge til nyFarge
      startTid = millis(); //Starter telling med millis på nytt
    }

    if (millis() - startTid > 500) {  // Teller til 500 ms
      return naaFarge; // Returnerer fargen man har pekt på i 500 ms.
    }
  }
}

void sjekkSeier() { // Sjekker om brukeren har vunnet
  if (poeng >= 3) { //Har man 3 poeng, trigges seier()
    seier();
    poeng = 0; // Deretter restarter man poeng-telleren, så man kan spille en ny runde
    fortsett = false; // Avbryter loopen
  } else {
    fortsett = true; // Om man ikke har 3 poeng, bare fortsetter programmet.
  }
}

void settFarge( int r, int g, int b){// Setter fargen til RGB 
value.b = b; value.g = g; value.r = r;// Fra library
LED.set_crgb_at(0, value); // Bruker metoden fra library
LED.sync(); // Fra library
delay(200); // Buffer
}


void riktig() {//Svarer man riktig, trigges riktig()
  poeng++; // Poengsum inkrementerer med 1
  int toner[] = { tone_C5, tone_E5, tone_G5 }; // Array med toner
  int varighet = 200;

  for (int i = 0; i < 3 ; i++) {// Itererer gjennom toner[] og spiller av tonen på indeks [i] 
    tone(buzzerPin, toner[i], varighet);
    delay(260);
  }

  // Har brukeren 1 poeng naa, skrus poeng1 på
  if (poeng == 1) {
    digitalWrite(poeng1, HIGH);
    digitalWrite(poeng2, LOW);
    digitalWrite(poeng3, LOW);
  }
  // Har brukeren to poeng nåå, skrurs poeng1 og poeng2 på
  else if (poeng == 2) {
    digitalWrite(poeng1, HIGH);
    digitalWrite(poeng2, HIGH);
    digitalWrite(poeng3, LOW);
  }
  // Har brukeren 3 poeng, skrurs alle poenglamper på (seier() trigges i sjekkSeier())
  else if (poeng == 3) {
    digitalWrite(poeng1, HIGH);
    digitalWrite(poeng2, HIGH);
    digitalWrite(poeng3, HIGH);
  }
}

void feil() {
  int toner[] = { tone_E5, tone_Dess5, tone_D5 }; // Array med toner
  int varighet = 200;
  for (int i = 0; i < 3; i++) { // Itererer gjennom array med toner og spiller av
    tone(buzzerPin, toner[i], varighet);
  delay(260);
  }

  if (poeng == 1) { // Har man ett poeng slukkes det lyset
    poeng--; // Nå har de 0 poeng
    digitalWrite(poeng1, LOW);
    digitalWrite(poeng2, LOW);
    digitalWrite(poeng3, LOW);
  } else if (poeng == 2) { //Har de to poeng slukkes poeng2
    poeng--; // Nå har de 1 poeng
    digitalWrite(poeng1, HIGH);
    digitalWrite(poeng2, LOW);
    digitalWrite(poeng3, LOW);
  }
  // Bruker kan ikke ha tre poeng her og så få feil, ettersom de vinner med en gang de når tre poeng

}

void seier() {
  Serial.println("Seier");

  //Array med seiers-toner
  float toner[] = { tone_F4, tone_A4, tone_C5, tone_F5, tone_C5, tone_F5 };
  int varighet[] = { 200, 200, 200, 150, 250, 600 };

 for (int i = 0; i < 6; i++) {
    tone(buzzerPin, toner[i], varighet[i]); //Itererer gjennom toner[] og spiller av med verdien man skal ha i varighet[]
    delay(varighet[i] * 1.3);  //Tid til neste tone//delay burde væære 130% av varigheten til hver tone
  }

  // Alle lys på brette blinker
  for (int i = 0; i < 5; i++) { 
    // Vurderte en for-each løkke, men det ga litt delay og vi vil at alle lys blinker samtidig
    digitalWrite(poeng1, HIGH);
    digitalWrite(poeng2, HIGH);
    digitalWrite(poeng3, HIGH);

    digitalWrite(red, HIGH);
    digitalWrite(blue, HIGH);
    digitalWrite(yellow, HIGH);
    digitalWrite(green, HIGH);

    delay(10); //buffer siden RBG er litt raskere
    settFarge(255, 255, 255);  // Grønt lys på RGB
    
    delay(30);  // Alle lys lyser i et halvt sekund

    // Skrur av alle lys
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