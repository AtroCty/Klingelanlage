#pragma once

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
	else
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
