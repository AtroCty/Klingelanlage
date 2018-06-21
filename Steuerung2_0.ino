//------------------------------------------------------------------------------
/// @file Steuerung2_0.ino
/// @mainpage ProjeKt Klingelanlage
/// @brief      Steuerung der Klingelanlage mit mehreren Eingängen. Zusätzliche
///             Ausgabe an verschiedenen Blinkanlagen
/// @author     Timm Schütte
/// @author     Till Westphalen
/// @version    2.0.2
/// @date       14. Dezember 2017 - Entwurf
/// @date       20. Juni 2018 - Update
/// @copyright  GNU Public License.

#include <Steuerung2_0.h>				/*!< Variablen-Deklarationen */

//------------------------------------------------------------------------------
/// @brief      Merker der verschiedenen States
volatile unsigned byte BLastState = 0;
/// @brief      Timer des Programmes
volatile structTimer Timings =
{
	.Laufzeit = 0,
	.Leuchtdauer = 0,
	.Entpreller = 0,
	.State = 0,
};

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
}

//------------------------------------------------------------------------------
/// @brief      Main-Loop
///
void loop()
{
	//-----------------------------------------------------------------------------
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
	// #2 Synchronisation
	UpdateTimings();

}

//------------------------------------------------------------------------------
/// @brief      Anfangsroutine bei Start des Programmes Startet den Interrrupt
///             und deaktiviert das Relais.
///
void StartRoutine()
{
	digitalWrite( OUT_TRELAIS, HIGH );
	attachInterrupt(digitalPinToInterrupt(IN_KLINGEL), interuptKlingeln, FALLING);
	SetState( STATE_START, true )
}

//------------------------------------------------------------------------------
/// @brief      Untersucht den aktuellen State auf Gültigkeit
///
/// @param[in]  iPos     Position des States. Siehe dazu Konstanten.
/// @param      BStates  Merker-Bytes
///
/// @return     TRUE falls aktiv, ansonsten FALSE
///
bool bGetState( int iPos, byte *BStates )
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
void SetState( int iPos, bool bState, byte *BStates )
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
/// @brief      Wenn Klingel betätigt wurde, setze die States, und unterbreche
///             den Interrupt
///
void interuptKlingeln()
{
	bSetState( STATE_KLINGEL_PUSHED, true );
	detachInterrupt(IN_KLINGEL);
	//### DEBUG
	Serial.println(("RINgRING "));
	//###
}

//------------------------------------------------------------------------------
/// @brief      Funktion zum Regeln des Lichtes
///
/// @param[in]  fFaktor  Geschwindigkeit des Blinkens
///
void LightControl(float fFaktor)
{
	return;
}

//------------------------------------------------------------------------------
/// @brief      Wird gerade ein Öffner betätigt?
///
/// @return     TRUE falls ja, ansonsten FALSE
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

	for (i = 0; i>=7; i++)
	{
		if ( bGetState( i, &Timings.State ))
		{
			*(p + ((long) i)) += 
		}
	}
	return;
}