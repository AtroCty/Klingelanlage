//------------------------------------------------------------------------------
/// @file Klingelanlage.ino
/// @mainpage ProjeKt Klingelanlage
/// @brief      Steuerung der Klingelanlage mit mehreren Eingängen. Zusätzliche
///             Ausgabe an verschiedenen Blinkanlagen
/// @author     Timm Schütte
/// @author     Till Westphalen
/// @version    2.0.2
/// @date       14. Dezember 2017 - Entwurf
/// @date       20. Juni 2018 - Update
/// @copyright  GNU Public License.

#include "Klingelanlage.h"				/*!< Variablen-Deklarationen */

//------------------------------------------------------------------------------
/// @brief      Merker der verschiedenen States
volatile byte BLastState = 0;
/// @brief      Timer des Programmes
volatile structTimer Timings =
{
	.Laufzeit = 0,
	.Leuchtdauer = 0,
	.Entpreller = 0,
	.State = 0,
};

//------------------------------------------------------------------------------
/// @brief      Wenn Klingel betätigt wurde, setze die States, und unterbreche
///             den Interrupt
///
void InteruptKlingeln()
{
	SetState( STATE_KLINGEL_PUSHED, &BLastState, true );
	detachInterrupt(IN_KLINGEL);
	//### DEBUG
	Serial.println(("RINgRING "));
	//###
}

//------------------------------------------------------------------------------
/// @brief      Untersucht den aktuellen State auf Gültigkeit
///
/// @param[in]  iPos     Position des States. Siehe dazu Konstanten.
/// @param      BStates  Merker-Bytes
///
/// @return     true falls aktiv, ansonsten false
///
bool bGetState( int iPos, volatile byte *BStates )
{
	return bitRead( *BStates, iPos );
}

//------------------------------------------------------------------------------
/// @brief      Setzt den State nach belieben
///
/// @param[in]  iPos     Position des States. Siehe dazu Konstanten
/// @param[in]  bState   Gewünschter State
/// @param      BStates  Merker-Bytes
///
void SetState( int iPos, volatile byte *BStates, bool bState )
{
	if (bState)
	{
		bitSet( *BStates, iPos );
	}
	else
	{
		bitClear( *BStates, iPos );
	}
	return;
}

//------------------------------------------------------------------------------
/// @brief      Anfangsroutine bei Start des Programmes Startet den Interrrupt
///             und deaktiviert das Relais.
///
void StartRoutine()
{
	digitalWrite( OUT_TRELAIS, HIGH );
	attachInterrupt(digitalPinToInterrupt(IN_KLINGEL), InteruptKlingeln, FALLING);
	SetState( STATE_START, &BLastState, true );
}

//------------------------------------------------------------------------------
/// @brief      Funktion zum Regeln der verschiedenen Timer
///
/// @param[in]  iTimer      Timer-ID (siehe Header)
/// @param[in]  bStartStop  true für starten, false für Stop/Reset
///
void TimerControl(int iTimer, bool bStartStop)
{
	if (bStartStop)
	{
		if (bGetState(iTimer, &Timings.State))
		{
			SetState(iTimer, &Timings.State, true);
		}
	}
	else
	{
		if (!(bGetState(iTimer, &Timings.State)))
		{
			SetState(iTimer, &Timings.State, false);
			long *p;
			p = (long*) &Timings;
			*(p + ((long) iTimer)) = 0;
		}
	}
	return;
}

//------------------------------------------------------------------------------
/// @brief      Wird gerade ein Öffner betätigt?
///
/// @return     true falls ja, ansonsten false
///
bool bButtonPushed()
{
	return !( digitalRead(IN_TIMM) && digitalRead(IN_BOBBY) && digitalRead(IN_TILL) && digitalRead(IN_TOBI) && digitalRead(IN_FRANZ) );
}

//------------------------------------------------------------------------------
/// @brief      Aktualisiert die vergangene Zeit, und resettet bei Überlauf
///
void UpdateTimings()
{
	// Bei Überlauf (alle paar Tage) Board resetten
	if ( millis() > 0xFFAF )
	{
		digitalWrite( OUT_RESET, LOW );
	}
	unsigned long lUpdateTime;
	lUpdateTime = millis() - Timings.Laufzeit;
	Timings.Laufzeit += lUpdateTime;
	// Alle Timer aktualisieren bei Bedarf
	int i;
	long *p;
	p = (long*) &Timings;
	for (i = 0; i >= 7; i++)
	{
		if ( bGetState( i, &Timings.State ))
		{
			*(p + ((long) i)) += lUpdateTime;
		}
	}
	return;
}

//------------------------------------------------------------------------------
/// @brief      Überprüft, ob Klingel betätigt wurde, und Entprellzeiten
///             eingehalten wurde
///
void CheckKlingel()
{
	if (bGetState( STATE_KLINGEL_PUSHED, &BLastState))
	{
		if (ENTPRELLDAUER > Timings.Entpreller)
		{
		}
	}
}


//------------------------------------------------------------------------------
/// Arduino Setup-Routine
/// @brief      Setzen der PINS & Serieller Debugger
///
void setup()
{
	// WICHTIG! Sonst startet das Board permanent neu!
	digitalWrite( OUT_RESET, HIGH );
	pinMode( OUT_BLINKLED,	OUTPUT );
	pinMode( OUT_TESTLED,	OUTPUT );
	pinMode( OUT_TRELAIS,	OUTPUT );
	pinMode( OUT_RESET,		OUTPUT );
	pinMode( IN_TIMM,		INPUT_PULLUP );
	pinMode( IN_BOBBY,		INPUT_PULLUP );
	pinMode( IN_TILL,		INPUT_PULLUP );
	pinMode( IN_TOBI,		INPUT_PULLUP );
	pinMode( IN_FRANZ,		INPUT_PULLUP );
	pinMode( IN_KLINGEL,	INPUT_PULLUP );
	Serial.begin(115200);				/*!< für serielle Ausgabe zum debuggen, kann deaktiviert bleiben */
	StartRoutine();
}

//------------------------------------------------------------------------------
/// @brief      Main-Loop
///
void loop()
{
	// //-----------------------------------------------------------------------------
	// // #0 Start-Routine, wird nur einmal ausgeführt.
	// if ( !bGetState( STATE_START ))
	// {
	// 	StartRoutine();
	// }
	//----------------------------------------------------------------------
	// #1 Button-Check, und Tueroeffnungsroutine. Hoechste Prioritaet.
	if ( bButtonPushed() )
	{
		if ( !bGetState(STATE_KLINGEL_PUSHED, &BLastState) )
		{
			// Öffne Tür
			digitalWrite( OUT_TRELAIS, LOW );
			SetState(STATE_KLINGEL_PUSHED, &BLastState, true);
		}
	}
	else if (bGetState(STATE_KLINGEL_PUSHED, &BLastState) )
	{
		// Schließe Tür
		digitalWrite( OUT_TRELAIS, HIGH );
		SetState(STATE_KLINGEL_PUSHED, &BLastState, false);
	}
	//----------------------------------------------------------------------
	// #2 Synchronisation aller Timer
	UpdateTimings();
	//----------------------------------------------------------------------
	// #3 Wurde Klingel betätigt? Checke Entprellung
	CheckKlingel();
}
