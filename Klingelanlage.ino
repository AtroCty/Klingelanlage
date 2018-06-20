//----------------------------------------------------------------------
// Titel		:	Klingelanlage
//----------------------------------------------------------------------
// Funktion	:		- Steuerung der Klingelanlage mit mehreren Eingängen
//					- Zusätzliche Ausgabe an verschiedenen Blinkanlagen
//----------------------------------------------------------------------
// Sprache	:	Arduino C
// Datum	:	14. Dezember 2017
// Updated	:	24. Mai 2018
// Version	:	1.2
// Autor	:	Timm Schütte & Till Westphalen
// Modul	:	Arduino UNO
// Frequenz	:	16 Mhz
//----------------------------------------------------------------------

#include <Metro.h>							// für exakte Zeiten

#define OUT_BLINKLED	9					// Blinksignal-LED
#define OUT_TESTLED		10					// Test-LED
#define OUT_TRELAIS		5					// Relais für den Türsummer
#define IN_TIMM			2					// Klingel-Relais Timm
#define IN_BOBBY		4					// Klingel-Relais Bobby
#define IN_TILL			6					// Klingel-Relais Till
#define IN_TOBI			7					// Klingel-Relais Tobi
#define IN_FRANZ		8					// Klingel-Relais Franz
#define IN_KLINGEL		3					// Klingelsignal, MUSS 3 sein, da Arduino Interrupts beim Uno nur in PIN 2/3 vorhanden sind

#define DAUER			2000				// in Millisekunden
#define SPEED			10					// Geschwindigkeit des Blinkes wenn Taste nicht gedrückt
#define SLOWRATE		4					// Multiplikator der Geschwindigkeit des Blinkes wenn Taste gedrückt

//----------------------------------------------------------------------
//	GLOBALS & KONSTANTEN
//----------------------------------------------------------------------

volatile byte BLastState = 0;									//	Merker der verschiedenen States
uint32_t counter = 0;
uint32_t iEntpreller = 0;

Metro MetroLED		= Metro((long) SPEED);						//	Helligkeitsgeschwindigkeit
Metro MetroKlingel 	= Metro((long) DAUER);						//	Klingeldauer

//----------------------------------------------------------------------
// State-Bits:
//----------------------------------------------------------------------
// 0	= Startsequenz
// 1	= Klingel-Routine gestartet
// 2	= Klingel-Signal empfangen
// 3	= Wird gerade Tür geöffnet?
// 4	= Hell/Dunkler
// 5	= TESTSTATE
// 6	= Klingel-Signal bestätigt
// 7	= UNUSED
//----------------------------------------------------------------------
//	Arduino Setup-Routine
//----------------------------------------------------------------------

/**
 * @brief		Setzen der einzelnen PINs
 */
void setup()
{
	pinMode( OUT_BLINKLED,		OUTPUT );
	pinMode( OUT_TESTLED,		OUTPUT );
	pinMode( OUT_TRELAIS,		OUTPUT );
	pinMode( IN_TIMM,			INPUT_PULLUP );
	pinMode( IN_BOBBY,			INPUT_PULLUP );
	pinMode( IN_TILL,			INPUT_PULLUP );
	pinMode( IN_TOBI,			INPUT_PULLUP );
	pinMode( IN_FRANZ,			INPUT_PULLUP );
	pinMode( IN_KLINGEL,		INPUT_PULLUP );
	Serial.begin(115200);					// für serielle Ausgabe, kann deaktiviert bleiben
}

//----------------------------------------------------------------------
//	MAIN
//----------------------------------------------------------------------
/**
 * @brief      Hauptroutine
 */
void loop()
{
	//----------------------------------------------------------------------
	//	Start-Sequenz
	if (!bitRead( BLastState, 0 ))
	{
		digitalWrite( OUT_TRELAIS, HIGH );
		attachInterrupt(digitalPinToInterrupt(IN_KLINGEL), Klingeln, FALLING);
		bitSet( BLastState, 0 );
		//### DEBUG
		Serial.println(String(DAUER));
		Serial.println(("counter ") + String(counter) + (" BENIS ") );
		//###
	}
	//----------------------------------------------------------------------
	//	Oeffnungsroutine
	if (!( digitalRead(IN_TIMM) && digitalRead(IN_BOBBY) && digitalRead(IN_TILL) && digitalRead(IN_TOBI) && digitalRead(IN_FRANZ) ))
	{
		LichtRoutine(1);
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
		// Öffne Tür
		digitalWrite( OUT_TRELAIS, HIGH );
		bitClear( BLastState, 3 );
	}

	//----------------------------------------------------------------------
	//	Überprüfung ob ein Fehl-Signal gesendet wurde
	if (bitRead( BLastState, 1 ))
	{
		if (iEntpreller < 50)
		{
			iEntpreller++;

			/**
			 * Falls kein Signal mehr anliegt, setze zurück
			 */
			if (!digitalRead(IN_KLINGEL))
			{
				iEntpreller = 0;
				bitClear( BLastState, 1);
				attachInterrupt(digitalPinToInterrupt(IN_KLINGEL), Klingeln, FALLING);
			}
		}
		else
		{
			bitSet( BLastState, 6 );
			bitClear( BLastState, 1);
			iEntpreller = 0;
		}

	}


	//----------------------------------------------------------------------
	//	Wenn Klingel betätigt wurde, und Routine nicht bereits ausgeführt
	if ( !(bitRead( BLastState, 6 )) && bitRead( BLastState, 2 ) )
	{
		bitSet( BLastState, 1 );
		MetroLED.reset();
	}
	//----------------------------------------------------------------------
	//	Klingelroutine
	if (bitRead( BLastState, 6 ))
	{
		// Blink-Routinne, wenn Oeffnerroutine nicht aktiv ist
		LichtRoutine(1);
		if (!bitRead( BLastState, 3 ))
		{
			if (MetroLED.check() == 1)
			{
				bool bAnAus = bitRead( BLastState, 5 );
				//digitalWrite( OUT_TESTLED, bAnAus );
				bitWrite( BLastState, 5 , !bAnAus );
				//### DEBUG
				Serial.println(("counter ") + String(counter) + (" ZEIT: ") + String(bAnAus) );
				//###
			}
		}
		//Abschluss der Klingelroutine
		if (counter >= (int) DAUER)
		{
			bitClear( BLastState, 6);
			bitClear( BLastState, 2);
			bitClear( BLastState, 5);
			counter = 0;
			// Interrupt wieder aktivieren
			attachInterrupt(digitalPinToInterrupt(IN_KLINGEL), Klingeln, FALLING);
		}
	}
}

/**
 * @brief      Setzt Flag, dass das Klingel-Signal betätigt wurde.
 */
void Klingeln()
{
	bitSet( BLastState, 2 );
	detachInterrupt(IN_KLINGEL);
}

/**
 * @brief      Algorrithmus der Lichtroutrine
 *
 * @param[in]  faktor  Geschwindigkeit des Blinkens
 */
void LichtRoutine(uint32_t faktor)
{
	if (bitRead( BLastState, 4 ))
	{
		if (faktor >= counter)
		{
			counter = 0;
			bitClear( BLastState, 4);
		}
		else
		{
			counter -= faktor;
		}
	}
	else
	{
		if (255 <= counter + faktor)
		{
			counter = 255;
			bitSet( BLastState, 4);
		}
		else
		{
			counter += faktor;
		}
	}
	if ( bitRead( BLastState, 3 ) || bitRead( BLastState, 1 ) )
	{
		analogWrite( OUT_TESTLED, counter );
	}
	else
	{
		digitalWrite( OUT_TESTLED, LOW );
	}
}
