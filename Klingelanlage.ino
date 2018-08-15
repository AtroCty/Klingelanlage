//------------------------------------------------------------------------------
/// @file Klingelanlage.ino
/// @brief      Steuerung der Klingelanlage mit mehreren Eingängen. Zusätzliche
///             Ausgabe an verschiedenen Blinkanlagen.
/// @author     Timm Schütte
/// @author     Till Westphalen
/// @version    2.0.4
/// @date       15. August 2018 - Endlikch mal anbringen...
/// @date       27. Juni 2018 	- Entfernen des Interrupts
/// @date       24. Juni 2018 	- Einführung Namenskonvention
/// @date       21. Juni 2018 	- Auslagerung in Header
/// @date       20. Juni 2018 	- Update 2.0
/// @date       14. Dezember 2017 - Entwurf
/// @copyright  GNU Public License.

#include "Klingelanlage.h"				/*!< Variablen-Deklarationen */
#include <Arduino.h>					/*!< Nur für Clang-Completion, wird eh immer eingebunden */
#include <math.h>

//------------------------------------------------------------------------------
/// @brief      Merker der verschiedenen States.
volatile uint8_t bytLastState = 0;
/// @brief      Timer des Programmes.
volatile structTimer structTimings =
{
	.u_lngLaufzeit = 0,
	.u_lngLeuchtdauer = 0,
	.u_lngEntpreller = 0,
	.bytState = 0,
};

void(* ResetFunc) (void) = 0;

//------------------------------------------------------------------------------
/// @brief      Wenn Klingel betätigt wurde, setze die States, und unterbreche
///             den Interrupt.
///


//------------------------------------------------------------------------------
/// @brief      Untersucht den aktuellen State auf Gültigkeit.
///
/// @param      intPos     Position des States. Siehe dazu Konstanten.
/// @param      bytStates  Adresse der Merker-States. (Siehe ADRESS_STATES_)
///
/// @return     @c true falls aktiv, ansonsten @c false.
///
bool bGetState( int intPos, volatile uint8_t *bytStates )
{
	return bitRead( *bytStates, intPos );
}

//------------------------------------------------------------------------------
/// @brief      Setzt den State nach Belieben.
///
/// @param      intPos     Position des States. Siehe dazu Konstanten.
/// @param      bytStates  Adresse der Merker-States. (Siehe ADRESS_STATES_)
/// @param      bState     Gewünschter State.
///
void SetState( int intPos, volatile uint8_t *bytStates, bool bState )
{
	if (bState)
	{
		bitSet( *bytStates, intPos );
	}
	else
	{
		bitClear( *bytStates, intPos );
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
	Serial.println("NEUSTART");
	ResetRoutine();
}

//------------------------------------------------------------------------------
/// @brief      Funktion zum Regeln der verschiedenen Timer.
///
/// @param      intTimer    Timer-ID. (siehe Header)
/// @param      bStartStop  @c true für starten, @c false für Stop/Reset.
///
void TimerControl(int intTimer, bool bStartStop)
{
	if (bStartStop)
	{
		if (bGetState(intTimer, ADRESS_STATES_TIMER))
		{
			SetState(intTimer, ADRESS_STATES_TIMER, true);
		}
	}
7	else
	{
		if (!(bGetState(intTimer, ADRESS_STATES_TIMER)))
		{
			SetState(intTimer, ADRESS_STATES_TIMER, false);
			long *p;
			p = (long*) &structTimings;
			*(p + ((long) intTimer)) = 0;
		}
	}
	return;
}

//------------------------------------------------------------------------------
/// @brief      Wird gerade ein Öffner betätigt?
///
/// @return     @c true falls ja, ansonsten @c false.
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
	if ( millis() > 0xFFA0 )
	{
		ResetFunc();
	}
	unsigned long u_lngUpdateTime;
	u_lngUpdateTime = millis() - structTimings.u_lngLaufzeit;
	structTimings.u_lngLaufzeit += u_lngUpdateTime;
	// Alle Timer aktualisieren bei Bedarf
	int i;
	long *p;
	p = (long*) &structTimings;
	for (i = 0; i >= 7; i++)
	{
		if ( bGetState( i, ADRESS_STATES_TIMER ))
		{
			*(p + ((long) i)) += u_lngUpdateTime;
		}
	}
	return;
}

//------------------------------------------------------------------------------
/// @brief      Überprüft, ob Klingel betätigt wurde, und Entprellzeiten
///             eingehalten wurde
///
void KlingelRoutine()
{
	if (bGetState( STATE_KLINGEL_ROUTINE, ADRESS_STATES_GENERIC ))
	{
		if (bGetState( STATE_TIMER_LEUCHTDAUER, ADRESS_STATES_TIMER ))
		{
			if (structTimings.u_lngLeuchtdauer >= CONST_LEUCHTDAUER)
			{
				ResetRoutine();
			}
		}
		else
		{
			SetState( STATE_TIMER_LEUCHTDAUER, ADRESS_STATES_TIMER, true );
			digitalWrite( OUT_TRELAIS, LOW );
		}
	}
	else if (bGetState( STATE_KLINGEL_PUSHED, ADRESS_STATES_GENERIC ))
	{
		if (( CONST_ENTPRELLDAUER < structTimings.u_lngEntpreller ) && ( !digitalRead(IN_KLINGEL)) )
		{
			SetState( STATE_KLINGEL_ROUTINE, ADRESS_STATES_GENERIC, true );
		}
		else
		{
			ResetRoutine();
		}
	}
}

//------------------------------------------------------------------------------
/// @brief      Reset der Routine & Timer
///
void ResetRoutine()
{
	SetState( STATE_TIMER_LEUCHTDAUER, ADRESS_STATES_TIMER, false );
	SetState( STATE_TIMER_ENTPRELLER, ADRESS_STATES_TIMER, false );
	SetState( STATE_KLINGEL_PUSHED, ADRESS_STATES_GENERIC, false );
	SetState( STATE_KLINGEL_ROUTINE, ADRESS_STATES_GENERIC, false );
	structTimings.u_lngLeuchtdauer = 0;
	structTimings.u_lngEntpreller = 0;
	attachInterrupt(digitalPinToInterrupt(IN_KLINGEL), InteruptKlingeln, FALLING);
}


//------------------------------------------------------------------------------
/// @brief      Leucht-Routine
///
void LeuchtRoutine()
{
	if ( bButtonPushed() )
	{
		analogWrite( OUT_BLINKLED, intAnalogValue(CONST_LEUCHTFREQUENZ * CONST_LEUCHT_MULT) );
		analogWrite( OUT_TESTLED, intAnalogValue(CONST_LEUCHTFREQUENZ * CONST_LEUCHT_MULT) );
		return;
	}
	else if (bGetState( STATE_KLINGEL_PUSHED, ADRESS_STATES_GENERIC ))
	{
		analogWrite( OUT_BLINKLED, intAnalogValue(CONST_LEUCHTFREQUENZ) );
		analogWrite( OUT_TESTLED, intAnalogValue(CONST_LEUCHTFREQUENZ) );
		return;
	}
	digitalWrite( OUT_BLINKLED, LOW );
	digitalWrite( OUT_TESTLED, LOW );
	return;
}

//------------------------------------------------------------------------------
/// @brief      Konvertiert in die korrekte analoge Variante
///
/// @return     Wert zwischen 0-255
///
uint8_t intAnalogValue(float fltSpeed)
{
	int intRange = CONST_MAX_ANALOG - CONST_MIN_ANALOG;
	float buffer = fmod(structTimings.u_lngLeuchtdauer, fltSpeed) / fltSpeed;
	if (buffer < 0.5)
	{
		buffer = (buffer * intRange);
	}
	else
	{
		buffer = (1 - buffer) * intRange;
	}
	return (int) (CONST_MIN_ANALOG + buffer);
}

//------------------------------------------------------------------------------
/// @brief      Arduino Setup-Routine. Setzen der PINS & Serieller Debugger.
///
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
	Serial.begin(115200);				/* für serielle Ausgabe zum debuggen, kann deaktiviert bleiben */
	delay(200);
	StartRoutine();
}

//------------------------------------------------------------------------------
/// @brief      Main-Loop.
///
void loop()
{
	///////////////////////////////////////////////////////////////////////////////
	///  #1 Button-Check, und Tueroeffnungsroutine. Hoechste Prioritaet.        ///
	///////////////////////////////////////////////////////////////////////////////
	if ((structTimings.u_lngEntpreller > 0) || (structTimings.u_lngLeuchtdauer > 0) )
	{
		Serial.print("\nTimer Entpreller: ");
		Serial.print(structTimings.u_lngEntpreller);
		Serial.print("\nTimer Leuchtdauer: ");
		Serial.print(structTimings.u_lngLeuchtdauer);
	}

	if ( bButtonPushed() )
	{
		if ( !bGetState(STATE_KLINGEL_PUSHED, ADRESS_STATES_GENERIC) )
		{
			// Öffne Tür
			digitalWrite( OUT_TRELAIS, LOW );
			SetState(STATE_KLINGEL_PUSHED, ADRESS_STATES_GENERIC, true);
		}
	}
	else if (bGetState(STATE_KLINGEL_PUSHED, ADRESS_STATES_GENERIC) )
	{
		// Schließe Tür
		digitalWrite( OUT_TRELAIS, HIGH );
		SetState(STATE_KLINGEL_PUSHED, ADRESS_STATES_GENERIC, false);
	}
	//----------------------------------------------------------------------
	// #2 Synchronisation aller Timer.
	UpdateTimings();
	//----------------------------------------------------------------------
	// #3 Klingel-Routine
	KlingelRoutine();
	// #4 Leucht-Routine
	LeuchtRoutine();
}
