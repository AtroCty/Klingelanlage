//------------------------------------------------------------------------------
/// @file Klingelanlage.h
/// @mainpage Projekt Klingelanlage
/// @brief      Steuerung der Klingelanlage mit mehreren Eingängen. Zusätzliche
///             Ausgabe an verschiedenen Blinkanlagen.
/// @author     Timm Schütte
/// @author     Till Westphalen
/// @version    2.0.3
/// @date       24. Juni 2018 - Einführung Namenskonvention
/// @date       21. Juni 2018 - Auslagerung in Header
/// @date       20. Juni 2018 - Update 2.0
/// @date       14. Dezember 2017 - Entwurf
/// @copyright  GNU Public License. This program is free software; you can
///             redistribute it and/or modify it under the terms of the GNU
///             General Public License as published by the Free Software
///             Foundation; either version 2 of the License, or (at your option)
///             any later version. This program is distributed in the hope that
///             it will be useful, but WITHOUT ANY WARRANTY; without even the
///             implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
///             PURPOSE.  See the GNU General Public License for more details.
///             You should have received a copy of the GNU General Public
///             License along with this program; if not, write to the Free
///             Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
///             Boston, MA 02110-1301, USA.
/// @section naming_sec Namenskonvention
/// Typische Variablen-Namensgebungen:
/// @subsection settings_subsec Eigenschaft
/// Eigenschaft    	| Naming 		| Beispiel
/// -------------	| -------------	| -------------
/// Global  		| g_			| g_intZaehler
/// unsigned		| u_			| u_chr, u_lng, u_int
/// Konstanten 		| GROSSCHREIBEN	| OUT_KABEL
///
/// @subsection datatype_subsec Datentypen
/// Datentyp      	| Naming 		| Beispiel
/// -------------	| -------------	| -------------
/// String  		| str			| strInputEncoder
/// bool			| b				| bSchalter
/// byte (uint8_t)	| byt			| bytStates
/// char			| chr			| chrBuchstabe
/// double			| dbl			| dblLangeVariable
/// float			| flt			| fltFliessend
/// int				| int			| intZaehler
/// long			| lng			| lngLangeSachen
/// short			| srt			| srtZehner
/// struct			| struct		| structTimer
///

#ifndef KLINGELANLAGE_H
#define KLINGELANLAGE_H

#include <Arduino.h>

//------------------------------------------------------------------------------
/// @brief      Timer des Programmes.
/// @note       Alle Timer müssen nacheinander stehen, vor dem bytState! Maximal
///             8 Timer!
///
typedef struct
{
	unsigned long u_lngLaufzeit;		/*!< Gesamte Laufzeit */
	unsigned long u_lngLeuchtdauer;		/*!< Dauer des Leuchtens */
	unsigned long u_lngEntpreller;		/*!< Entprell-Buffer */
	uint8_t bytState;					/*!< Merker der aktiven Timer */
} structTimer;

extern volatile uint8_t bytLastState;
#define ADRESS_STATES_GENERIC 		&bytLastState
extern volatile structTimer structTimings;
#define ADRESS_STATES_TIMER 		&structTimings.bytState


///=============================================================================
/// Funktionsprototypen
///-----------------------------------------------------------------------------
///
extern void StartRoutine();
extern bool bGetState( int intPos, volatile uint8_t *bytStates );
extern void SetState( int intPos, volatile uint8_t *bytStates, bool bState );
extern void InteruptKlingeln();
extern void TimerControl(int intTimer, bool bStartStop);
extern bool bButtonPushed();
extern void UpdateTimings();
extern void KlingelRoutine();
extern void LeuchtRoutine();
extern void ResetRoutine();

///=============================================================================
/// Konstanten & Makros
///-----------------------------------------------------------------------------
///
/// @defgroup   PINS Pin-Belegung
/// @{
///
#define OUT_BLINKLED		9			/*!< Blinksignal-LED */
#define OUT_TESTLED			10			/*!< Test-LED */
#define OUT_TRELAIS			5			/*!< Relais für den Türsummer */
#define OUT_RESET			13			/*!< Reset des Boards */
#define IN_TIMM				2			/*!< Klingel-Relais Timm */
#define IN_BOBBY			4			/*!< Klingel-Relais Bobby */
#define IN_TILL				6			/*!< Klingel-Relais Till */
#define IN_TOBI				7			/*!< Klingel-Relais Tobi */
#define IN_FRANZ			8			/*!< Klingel-Relais Franz */
#define IN_KLINGEL			3			/*!< Klingelsignal , MUSS 3 sein, da Arduino Interrupts beim Uno nur in PIN 2/3 vorhanden sind */
/// @}

//------------------------------------------------------------------------------
/// @defgroup   SPEED Geschwindigkeiten
/// @{
///
#define CONST_LEUCHTDAUER		10000.0			/*!< in Milli-Sekunden */
#define CONST_ENTPRELLDAUER		200.0			/*!< in Milli-Sekunden */
#define CONST_LEUCHTFREQUENZ	1000.0			/*!< Geschwindigkeit des Blinkes wenn Taste nicht gedrückt (Je höher desto langsamer) */
#define CONST_LEUCHT_MULT		0.3				/*!< Multiplikator der Geschwindigkeit des Blinkes wenn Taste gedrückt */
#define CONST_MIN_ANALOG		40
#define CONST_MAX_ANALOG		255
/// @}

//------------------------------------------------------------------------------
/// @defgroup   STATES State-Bits
/// @{
///
#define STATE_KLINGEL_PUSHED	1		/*!< Klingel wurde betätigt */
#define STATE_KLINGEL_ROUTINE	2		/*!< Klingel-Routine gestartet */
#define STATE_DOOR_OPEN			4		/*!< Wird gerade Tür geöffnet? */
#define STATE_LIGHT				8		/*!< Leuchten die Lampen? */
#define STATE_DEBUG				16		/*!< TESTSTATE */
/// Timer States
#define STATE_TIMER_LEUCHTDAUER	1		/*!< Leuchtdauer */
#define STATE_TIMER_ENTPRELLER	2		/*!< Entpreller */
/// @}

#endif
