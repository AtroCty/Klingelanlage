///=============================================================================
/// @file Steuerung2_0.h
/// @mainpage ProjeKt Klingelanlage
/// @brief      Steuerung der Klingelanlage mit mehreren Eingängen. Zusätzliche
///             Ausgabe an verschiedenen Blinkanlagen
/// @author     Timm Schütte
/// @author     Till Westphalen
/// @version    2.0.2
/// @date       14. Dezember 2017 - Entwurf
/// @date       20. Juni 2018 - Update 2.0
/// @date       21. Juni 2018 - Auslagerung in Header
/// @copyright  GNU Public License.
///  * This program is free software; you can redistribute it and/or modify
///  * it under the terms of the GNU General Public License as published by
///  * the Free Software Foundation; either version 2 of the License, or
///  * (at your option) any later version.
///
///  * This program is distributed in the hope that it will be useful,
///  * but WITHOUT ANY WARRANTY; without even the implied warranty of
///  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///  * GNU General Public License for more details.
///
///  * You should have received a copy of the GNU General Public License
///  * along with this program; if not, write to the Free Software
///  * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
///  * MA 02110-1301, USA.
///=============================================================================

#ifndef Steuerung2_0_h
#define Steuerung2_0_h

typedef struct
{
	unsigned long Laufzeit;
	unsigned long Leuchtdauer;
	unsigned long Entpreller;
	unsigned byte State;
} structTimer;

///=============================================================================
/// Konstanten & Makros
///-----------------------------------------------------------------------------
///
/// @defgroup   PINS Pin-Belegung
/// @{
///
#define OUT_BLINKLED	9				/*!< Blinksignal-LED */
#define OUT_TESTLED		10				/*!< Test-LED */
#define OUT_TRELAIS		5				/*!< Relais für den Türsummer */
#define OUT_RESET		13				/*!< Reset des Boards */
#define IN_TIMM			2				/*!< Klingel-Relais Timm */
#define IN_BOBBY		4				/*!< Klingel-Relais Bobby */
#define IN_TILL			6				/*!< Klingel-Relais Till */
#define IN_TOBI			7				/*!< Klingel-Relais Tobi */
#define IN_FRANZ		8				/*!< Klingel-Relais Franz */
#define IN_KLINGEL		3				/*!< Klingelsignal , MUSS 3 sein, da Arduino Interrupts beim Uno nur in PIN 2/3 vorhanden sind */
/// @}

//------------------------------------------------------------------------------
/// @defgroup   SPEED Geschwindigkeiten
/// @{
///
#define LEUCHTDAUER		10000.0			/*!< in Milli-Sekunden */
#define ENTPRELLDAUER	200.0			/*!< in Milli-Sekunden */
#define SPEED			1.0				/*!< Geschwindigkeit des Blinkes wenn Taste nicht gedrückt (Je höher desto langsamer) */
#define SLOWRATE		0.1				/*!< Multiplikator der Geschwindigkeit des Blinkes wenn Taste NICHT gedrückt */
/// @}

//------------------------------------------------------------------------------
/// @defgroup   STATES State-Bits
/// @{
///
#define STATE_START				1		/*!< Startsequenz */
#define STATE_KLINGEL_ROUTINE	2		/*!< Klingel-Routine gestartet */
#define STATE_KLINGEL_PUSHED	4		/*!< Klingel wurde betätigt */
#define STATE_DOOR_OPEN			8		/*!< Wird gerade Tür geöffnet? */
#define STATE_DENSITY_TOGGLE	16		/*!< Hell/Dunkler werden des Lichtes */
#define STATE_DEBUG				32		/*!< TESTSTATE */
/// Timer States
#define TIMER_ENTPRELLER		2		/*!< Entpreller */
#define TIMER_LEUCHTDAUER		1		/*!< Leuchtdauer */
/// @}

#endif