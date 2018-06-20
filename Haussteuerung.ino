/**@file Haussteuerung.ino */
//----------------------------------------------------------------------
// Titel		:	Klingelanlage
//----------------------------------------------------------------------
// Funktion		:	- Steuerung der Klingelanlage mit mehreren Eingängen
//					- Zusätzliche Ausgabe an verschiedenen Blinkanlagen
//----------------------------------------------------------------------
// Sprache		:	Arduino C
// Datum		:	14. Dezember 2017
// Updated		:	16. Dezember 2017
// Version		:	1.1
// Autor		:	Timm Schütte & Till
// Modul		:	Arduino UNO
// Frequenz		:	16 Mhz
//----------------------------------------------------------------------

#include <Metro.h>						// für Zeiten ohne Timer
#include <Arduino.h>

#define OUT_BLINKLED	9				// Blinksignal-LED
#define OUT_TESTLED		10				// Test-LED
#define OUT_TRELAIS		5				// Relais für den Türsummer
#define IN_TIMM			2				// Klingel-Relais Timm
#define IN_BOBBY		4				// Klingel-Relais Bobby
#define IN_TILL			6				// Klingel-Relais Till
#define IN_TOBI			7				// Klingel-Relais Tobi
#define IN_FRANZ		8				// Klingel-Relais Franz
#define IN_KLINGEL		3				// Klingelsignal , MUSS 3 sein, da Arduino Interrupts beim Uno nur in PIN 2/3 vorhanden sind

#define DAUER			10000.0			// in Millisekunden
#define SPEED			1.0				// Geschwindigkeit des Blinkes wenn Taste nicht gedrückt (Je höher desto langsamer)
#define SLOWRATE		0.1				// Multiplikator der Geschwindigkeit des Blinkes wenn Taste NICHT gedrückt

//----------------------------------------------------------------------
//	GLOBALS & KONSTANTEN
//----------------------------------------------------------------------

volatile byte BLastState = 0;									//	Merker der verschiedenen States
float counter = 0.0;

Metro MetroLED		= Metro((long) SPEED);						//	Helligkeitsgeschwindigkeit
Metro MetroKlingel	= Metro((long) DAUER);						//	Klingeldauer

//----------------------------------------------------------------------
// State-Bits:
//----------------------------------------------------------------------
// 0	=	Startsequenz
// 1	=	Klingel-Routine gestartet
// 2	=	Klingel wurde betätigt
// 3	=	Wird gerade Tür geöffnet?
// 4	=	Hell/Dunkler
// 5	=	TESTSTATE
// 6	=	UNUSED
// 7	=	UNUSED
//----------------------------------------------------------------------
//	Arduino Setup-Routine
//----------------------------------------------------------------------

void setup()
{
	pinMode( OUT_BLINKLED,	OUTPUT );
	pinMode( OUT_TESTLED,	OUTPUT );
	pinMode( OUT_TRELAIS,	OUTPUT );
	pinMode( IN_TIMM,		INPUT_PULLUP );
	pinMode( IN_BOBBY,		INPUT_PULLUP );
	pinMode( IN_TILL,		INPUT_PULLUP );
	pinMode( IN_TOBI,		INPUT_PULLUP );
	pinMode( IN_FRANZ,		INPUT_PULLUP );
	pinMode( IN_KLINGEL,	INPUT_PULLUP );

	Serial.begin(115200);				//	für serielle Ausgabe, kann deaktiviert bleiben
}

//----------------------------------------------------------------------
//	MAIN
//----------------------------------------------------------------------

void loop()
{
	//----------------------------------------------------------------------
	//	Start-Sequenz
	if (!bitRead( BLastState, 0 ))
	{
		digitalWrite( OUT_TRELAIS, HIGH );
		// Aktivieren des Interrupts
		attachInterrupt(digitalPinToInterrupt(IN_KLINGEL), Klingeln, FALLING);
		bitSet( BLastState, 0 );

		//### DEBUG
		Serial.println(("counter ") + String (counter) + (" START ") );
		//###
	}

	float faktor = 1;

	//----------------------------------------------------------------------
	//	Oeffnungsroutine
	if (!( digitalRead(IN_TIMM) && digitalRead(IN_BOBBY) && digitalRead(IN_TILL) && digitalRead(IN_TOBI) && digitalRead(IN_FRANZ) ))
	{
		// Falls Routine nicht bereits gestartet wurde
		if (!bitRead( BLastState, 3 ))
		{
			// Öffne Tür
			digitalWrite( OUT_TRELAIS, LOW );
			//digitalWrite( OUT_TESTLED, LOW );
			bitSet( BLastState, 3 );
		}
	}

	//----------------------------------------------------------------------
	//	Falls keiner mehr drückt, und die Routine noch aktiv ist, deaktiviere sie
	else if (bitRead( BLastState, 3 ))
	{
		// Schließe Tür
		digitalWrite( OUT_TRELAIS, HIGH );
		bitClear( BLastState, 3 );
	}

	//----------------------------------------------------------------------
	//	Wenn Klingel betätigt wurde, und Routine nicht bereits ausgeführt
	if ( !(bitRead( BLastState, 1 )) && bitRead( BLastState, 2 ) )
	{
		bitSet( BLastState, 1 );
		MetroKlingel.reset();
	}

	//----------------------------------------------------------------------
	//	Klingelroutine
	if (bitRead( BLastState, 1 ))
	{
		// Blink-Routinne, wenn Öffnerroutine nicht aktiv ist
		if (!bitRead( BLastState, 3 ))
		{
			if (MetroLED.check() == 1)
			{
				faktor *= SLOWRATE;
			}
		}

		//Abschluss der Klingelroutine
		if (MetroKlingel.check() == 1)
		{
			//### DEBUG
			Serial.println(("LastState ") + String (BLastState) + (" START ") );
			//###

			bitClear( BLastState, 1);
			bitClear( BLastState, 2);
			bitClear( BLastState, 5);

			// Interrupt wieder aktivieren
			attachInterrupt(digitalPinToInterrupt(IN_KLINGEL), Klingeln, FALLING);
		}
	}
	LichtRoutine(faktor);
}

void Klingeln()
{
	bitSet( BLastState, 2 );
	detachInterrupt(IN_KLINGEL);
	//### DEBUG
	Serial.println(("RINgRING "));
	//###
}

void LichtRoutine(float faktor)
{
	if (MetroLED.check() == 1)
	{
		if (bitRead( BLastState, 4 ))
		{
			if (faktor>=counter)
			{
				counter = 0.0;
				bitClear( BLastState, 4);
				//### DEBUG
				Serial.println(("ZURUCK ") + String (BLastState) + (" back ") );
				//###
			}
			else counter-=faktor;
		}
		else
		{
			if (255<=counter+faktor)
			{
				counter = 255.0;
				bitSet( BLastState, 4);
				//### DEBUG
				Serial.println(("VOR ") + String (BLastState) + (" vor ") );
				//###
			}
			else counter+=faktor;
		}
	}
	if ( bitRead( BLastState, 3 ) || bitRead( BLastState, 1 ) )
	{
		analogWrite( OUT_TESTLED, (int) counter );
	}
	else
	{
		digitalWrite( OUT_TESTLED, LOW );
	}
}
