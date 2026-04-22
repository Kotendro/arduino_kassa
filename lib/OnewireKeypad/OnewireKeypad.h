#ifndef OnewireKeypad_h
#define OnewireKeypad_h
#include <Arduino.h>
#include "BitBool.h"

#define NO_KEY '\0'
#define WAITING 0
#define PRESSED 1
#define RELEASED 2
#define HELD 3

//This number is based on 1/3 the lowest AR value from the ShowRange function.
#define KP_TOLERANCE 20														

struct MissingType {};

#ifdef LiquidCrystal_I2C_h
typedef LiquidCrystal_I2C LCDTYPE;
#else
typedef MissingType LCDTYPE;
#endif



template<class T> inline Print &operator<<(Print &Outport, T str) {
	Outport.print(str);
	return Outport;
}

/*--- Main Class ---*/
template< typename T, unsigned MAX_KEYS >
class OnewireKeypad {
  public:
    OnewireKeypad( T &port, char KP[], uint16_t adcThresholds[], uint8_t Rows, uint8_t Cols, uint8_t Pin)
      : port_( port ), latchedKey( BitBool< MAX_KEYS >() ), _Data( KP ), _adcThresholds(adcThresholds), _Rows( Rows ), _Cols( Cols ), _Pin( Pin ), holdTime( 500 ),debounceTime(200), startTime( 0 ), lastState( 0 ), lastRead( 0 ), index( 0 ) { }

    char	getkey();
    void	setHoldTime(unsigned long setH_Time) { holdTime = setH_Time; }
    void	setDebounceTime(unsigned long setD_Time) { debounceTime = setD_Time; }
    uint8_t	keyState();
    bool	readPin() { return analogRead(_Pin) > KP_TOLERANCE; }
    void	latchKey();
    bool	checkLatchedKey(char _key);
    void	addEventKey(void (*userFunc)(void), char key);
    void	deleteEventKey(char key);
    void	listenforEventKey();
	uint8_t _Pin;

  protected:
    T &port_;

  private:
    BitBool< MAX_KEYS > latchedKey;
    uint16_t *_adcThresholds;
    char 	*_Data;
    uint8_t _Rows, _Cols; //, _Pin;
    enum { SIZE = MAX_KEYS };
    uint8_t index;
    unsigned long time;
    unsigned long holdTime;
    unsigned long startTime;
    bool 	state, lastState, lastRead;
    unsigned long debounceTime, lastDebounceTime;
	float 	lastReading;
    struct {
		void(*intFunc)();
		char keyHolder;
    } Event[MAX_KEYS];
	void errorMSG(const char* msg);
};

template < typename T, typename U > struct IsSameType {
	enum { Value = false };
};

template < typename T > struct IsSameType< T, T > {
	enum { Value = true };
};

template < typename T, unsigned MAX_KEYS >
void OnewireKeypad<T, MAX_KEYS>::errorMSG(const char* msg) {
	if ( IsSameType< T, LCDTYPE >::Value) {
		port_.print(msg); // Lcd display
	} else {
		port_.println(msg); // Serial display
	}
}

template <typename T, unsigned MAX_KEYS>
char OnewireKeypad<T, MAX_KEYS>::getkey() {
    uint16_t pinReading = analogRead(_Pin);
    boolean state = pinReading > KP_TOLERANCE;

    unsigned long currentMillis = millis();

    if (state != lastReading) {
        lastDebounceTime = currentMillis;
        lastReading = state;
    }

    if ((currentMillis - lastDebounceTime) <= debounceTime) {
        return NO_KEY;
    }

    if (!state) {
        return NO_KEY;
    }

    for (uint8_t i = 0; i < MAX_KEYS; i++) {
        if (pinReading >= _adcThresholds[i]) {
            return _Data[i];
        }
    }

    return NO_KEY;
}

template < typename T, unsigned MAX_KEYS >
uint8_t OnewireKeypad< T, MAX_KEYS >::keyState() {
	if ((state = readPin()) != lastState) {
		return ( (lastState = state) ? PRESSED : RELEASED); //MOD
	} else if (state) {
		time = millis();

		while (readPin()) {
			if ((millis() - time) > holdTime) { return HELD; }
		}
		lastState = 0;
		return RELEASED;
	}
	return WAITING;
}


template < typename T, unsigned MAX_KEYS >
void OnewireKeypad<T, MAX_KEYS >::latchKey() {
	char output[20];
	bool PRINT = false;
	char read = getkey();
	
	if (read != lastRead) {
		if ( read ) {
			for ( int idx = 0; idx < SIZE; idx++ ) {
				if (read == _Data[idx] ) {
					strcpy_P( output, ( latchedKey[idx] = !latchedKey[idx] ) ? PSTR( "Key X was Latched" ) : PSTR( "Key X was Unlatched" ) ); //MOD
					PRINT = true;
					output[ 4 ] = read; //MOD <- very clever
				}
			}
		}
    
		lastRead = read;
		if ( PRINT ) {
			errorMSG(output);
		}
	}
}

template < typename T, unsigned MAX_KEYS >
bool OnewireKeypad< T, MAX_KEYS >::checkLatchedKey(char _key) {
	for ( uint8_t idx = 0; idx < SIZE; idx++ ) {
		if ( _key == _Data[idx] ) { return latchedKey[idx]; }
	}
	return false;
}

template < typename T, unsigned MAX_KEYS >
void OnewireKeypad< T, MAX_KEYS >::addEventKey(void (*userFunc)(void), char key) {
	Event[index].intFunc = userFunc;
	Event[index].keyHolder = key;
	if (index < MAX_KEYS) {
		index++;
	} else {
		errorMSG("Too Many EventKeys");
	}
}

template < typename T, unsigned MAX_KEYS >
void OnewireKeypad< T, MAX_KEYS >::deleteEventKey(char key) {
	for (uint8_t idx = 0; idx < index; idx++) {
		if (key == Event[ idx ].keyHolder) {
			Event[ idx ].intFunc = NULL;
			Event[ idx ].keyHolder = '~'; // CHANGED from '\0' to '~', because '\0' was causing issues.
			break;
		}
	}
}

template < typename T, unsigned MAX_KEYS >
void OnewireKeypad< T, MAX_KEYS >::listenforEventKey() {
	for (uint8_t idx = 0; idx < index; idx++) {
		if (getkey() == Event[ idx ].keyHolder) {
			if (keyState() == RELEASED) {
				Event[ idx ].intFunc();
				break;
			}
		}
	}
}

#endif
