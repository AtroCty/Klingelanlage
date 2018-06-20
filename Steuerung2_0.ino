/** @file Steuerung2_0.ino */
//----------------------------------------------------------------------
// @par Project:	Klingelanlage
//----------------------------------------------------------------------
// @brief		:	- Steuerung der Klingelanlage mit mehreren Eingängen
//					- Zusätzliche Ausgabe an verschiedenen Blinkanlagen
//----------------------------------------------------------------------
// Sprache		:	Arduino C
// Datum		:	14. Dezember 2017
// Updated		:	06. Juni 2018
// Version		:	2.0
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

volatile unsigned byte BLastState = 0;								//	Merker der verschiedenen States

//----------------------------------------------------------------------
// State-Bits:
//----------------------------------------------------------------------
#define STATE_START				1
#define STATE_KLINGEL_ROUTINE	2
#define STATE_KLINGEL_PUSHED	4
#define STATE_DOOR_OPEN			8
#define STATE_DENSITY_TOGGLE	16
#define STATE_DEBUG				32

// 1	=	STATE_START				=	Startsequenz
// 2	=	STATE_KLINGEL_ROUTINE	=	Klingel-Routine gestartet
// 4	=	STATE_KLINGEL_PUSHED	=	Klingel wurde betätigt
// 8	=	STATE_DOOR_OPEN			=	Wird gerade Tür geöffnet?
// 16	=	STATE_DENSITY_TOGGLE	=	Hell/Dunkler werden des Lichtes
// 32	=	STATE_DEBUG				=	TESTSTATE
// 64	=	STATE_DEBUG				=	UNUSED
// 128	=	STATE_DEBUG				=	UNUSED
//----------------------------------------------------------------------
//	Arduino Setup-Routine
//----------------------------------------------------------------------

/**
 * @brief      Setzen der PINS & Serieller Debugger
 */
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
	Serial.begin(115200);				//	für serielle Ausgabe zum debuggen, kann deaktiviert bleiben
}

//----------------------------------------------------------------------
//	MAIN
//----------------------------------------------------------------------

/**
 * @brief      Hauptschleife
 */
void loop()
{
	//----------------------------------------------------------------------
	// #0 Start-Routine, wird nur einmal ausgeführt.
	if ( !bGetState( STATE_START ))
	{
		StartRoutine();
	}
	//----------------------------------------------------------------------
	// #1 Button-Check, und Tueroeffnungsroutine. Hoechste Prioritaet.
	if ( bButtonPushed() )
	{
		if ( !bGetState(STATE_KLINGEL_PUSHED) )
		{
			// Öffne Tür
			digitalWrite( OUT_TRELAIS, LOW );
			bSetState(STATE_KLINGEL_PUSHED, true);
		}
	}
	else if (bGetState(STATE_KLINGEL_PUSHED))
	{
		// Schließe Tür
		digitalWrite( OUT_TRELAIS, HIGH );
		bSetState(STATE_KLINGEL_PUSHED, false);
	}
	//----------------------------------------------------------------------
	// #2 
}

/**
 * @brief      Anfangsroutine bei Start des Programmes
 */
void StartRoutine()
{
	digitalWrite( OUT_TRELAIS, HIGH );
	// Aktivieren des Interrupts
	attachInterrupt(digitalPinToInterrupt(IN_KLINGEL), interuptKlingeln, FALLING);
	bSetState( STATE_START, true )
}

/**
 * @brief      Untersucht den aktuellen State auf gültigkeit
 *
 * @param[in]  iPos  Position des States. Siehe dazu Konstanten.
 *
 * @return     { TRUE falls aktiv, ansonsten FALSE }
 */
bool bGetState( int iPos )
{
	return bitRead;
}

/**
 * @brief      Setzt den State nach belieben
 *
 * @param[in]  iPos    Position des States. Siehe dazu Konstanten.
 * @param[in]  bState  Gewuenschter State
 */
void SetState( int iPos, bool bState )
{
	if (bState)
	{
		bitSet( BLastState, iPos );
	}
	else
	{
		bitClear( BLastState, iPos );
	}
	return;
}

/**
 * @brief      Wenn Klingel betätigt wurde, setze die States, und unterbreche den Interrupt
 */
void interuptKlingeln()
{
	bSetState( STATE_KLINGEL_PUSHED, true );
	detachInterrupt(IN_KLINGEL);
	//### DEBUG
	Serial.println(("RINgRING "));
	//###
}

/**
 * @brief      { function_description }
 *
 * @param[in]  fFaktor  The f faktor
 */
void LightControl(float fFaktor)
{
}

/**
 * @brief      { Wird gerade ein Öffner betätigt? }
 *
 * @return     { TRUE falls ja, ansosnten FALSE }
 */
bool bButtonPushed()
{
	return !( digitalRead(IN_TIMM) && digitalRead(IN_BOBBY) && digitalRead(IN_TILL) && digitalRead(IN_TOBI) && digitalRead(IN_FRANZ) );
}