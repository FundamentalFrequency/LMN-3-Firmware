#include <Arduino.h>
#include <Control_Surface.h>
#include <ResponsiveAnalogRead.h>
#include <AH/Timing/MillisMicrosTimer.hpp>
 
// ResponsiveAnalogRead is used to read the pitch bend
// pin since the value is quite noisy :/
// ControlSurface seems to have some filtering but it does not seem to be working
const int HORIZONTAL_PB_PIN = A15;

// CC values
static constexpr int ENCODER_1 = 3;
static constexpr int ENCODER_2 = 9;
static constexpr int ENCODER_3 = 14;
static constexpr int ENCODER_4 = 15;
static constexpr int ENCODER_1_BUTTON = 20;
static constexpr int ENCODER_2_BUTTON = 21;
static constexpr int ENCODER_3_BUTTON = 22;
static constexpr int ENCODER_4_BUTTON = 23;
static constexpr int UNDO_BUTTON = 24;
static constexpr int TEMPO_BUTTON = 25;
static constexpr int SAVE_BUTTON = 26;
static constexpr int SETTINGS_BUTTON = 85;
static constexpr int TRACKS_BUTTON = 86;
static constexpr int MIXER_BUTTON = 88;
static constexpr int PLUGINS_BUTTON = 89;
static constexpr int MODIFIERS_BUTTON = 90;
static constexpr int SEQUENCERS_BUTTON = 102;   
static constexpr int LOOP_IN_BUTTON = 103;
static constexpr int LOOP_OUT_BUTTON = 104;
static constexpr int LOOP_BUTTON = 105;
static constexpr int CUT_BUTTON = 106;
static constexpr int PASTE_BUTTON = 107;
static constexpr int SLICE_BUTTON = 108;
static constexpr int RECORD_BUTTON = 109;
static constexpr int PLAY_BUTTON = 110;
static constexpr int STOP_BUTTON = 111;
static constexpr int CONTROL_BUTTON = 112;
static constexpr int OCTAVE_CHANGE = 117;
static constexpr int PLUS_BUTTON = 118;
static constexpr int MINUS_BUTTON = 119;
static constexpr int DUMMY = 31;

// Instantiate a MIDI over USB interface.
USBMIDI_Interface midi;

ResponsiveAnalogRead analog(HORIZONTAL_PB_PIN, true);

CCRotaryEncoder enc1 = {
    {5, 6}, // pins
    {ENCODER_1},         // MIDI address (CC number + optional channel)
    1,      // optional multiplier if the control isn't fast enough
};

CCRotaryEncoder enc2 = {
    {26, 27}, // pins
    {ENCODER_2},         // MIDI address (CC number + optional channel)
    1,      // optional multiplier if the control isn't fast enough
};

CCRotaryEncoder enc3 = {
    {29, 30}, // pins
    {ENCODER_3},         // MIDI address (CC number + optional channel)
    1,      // optional multiplier if the control isn't fast enough
};

CCRotaryEncoder enc4 = {
    {31, 32}, // pins
    {ENCODER_4},         // MIDI address (CC number + optional channel)
    1,      // optional multiplier if the control isn't fast enough
};

PitchBendSender<12> pbSender;

// N.B This did not work on the 4.1. The reading was noisy
// https://github.com/tttapa/Control-Surface/issues/726
// I had to use ResponsiveAnalogRead instead
// CCPotentiometer pitchBendPotentiometer = {
//     A14,        // Analog pin connected to potentiometer
//     {27},       // MIDI address (CC number + optional channel)
// };

Timer<millis> timer = 500; // milliseconds
const int maxTransposition = 4;
const int minTransposition = -1 * maxTransposition;
const int transpositionSemitones = 12;
Transposer<minTransposition, maxTransposition>transposer(transpositionSemitones);

const AddressMatrix<2, 14> noteAddresses = {{
                                                {1, 54, 56, 58, 1, 61, 63, 1, 66, 68, 70, 1, 73, 75},
                                                {53, 55, 57, 59, 60, 62, 64, 65, 67, 69, 71, 72, 74, 76},  
                                            }};

Bankable::NoteButtonMatrix<2, 14> noteButtonMatrix = {
    transposer,
    {35, 28}, // row pins
    {9, 8, 7, 4, 3, 2, 1, 0, 25, 14, 13, 41, 40, 36},    // column pins
    noteAddresses,    // address matrix
    CHANNEL_1,    // channel and cable number
};

const AddressMatrix<3, 11> ccAddresses = {{
                                              {LOOP_BUTTON, LOOP_IN_BUTTON, LOOP_OUT_BUTTON, PLUS_BUTTON, MINUS_BUTTON, DUMMY, ENCODER_1_BUTTON, ENCODER_2_BUTTON, DUMMY, ENCODER_3_BUTTON, ENCODER_4_BUTTON},
                                              {CUT_BUTTON, PASTE_BUTTON, SLICE_BUTTON, SAVE_BUTTON, UNDO_BUTTON, DUMMY, DUMMY, DUMMY, DUMMY, DUMMY, DUMMY},
                                              {CONTROL_BUTTON, RECORD_BUTTON, PLAY_BUTTON, STOP_BUTTON, SETTINGS_BUTTON, TEMPO_BUTTON, MIXER_BUTTON, TRACKS_BUTTON, PLUGINS_BUTTON, MODIFIERS_BUTTON, SEQUENCERS_BUTTON}
                                         }};

CCButtonMatrix<3, 11> ccButtonmatrix = {
    {24, 23, 34}, // row pins
    {4, 3, 2, 1, 0, 25, 14, 13, 41, 40, 36},    // column pins
    ccAddresses,    // address matrix
    CHANNEL_1,    // channel and cable number
};

bool shiftPressed = false;
bool plusPressed = false;
bool minusPressed = false;
bool shouldUpdateOctave = false;
void updateOctave() {
    // check if shift is down
    // getPrevState uses (col, row)
    if (ccButtonmatrix.getPrevState(0, 2) == 0) {
        shiftPressed = true;
        // // Check if plus was released and we arent at the max octave
        if (ccButtonmatrix.getPrevState(3, 0) == 0) {
            plusPressed = true;
            
        } else {
            if (plusPressed) {
                if (transposer.getTransposition() != maxTransposition) {
                    transposer.setTransposition(transposer.getTransposition() + 1);
                    shouldUpdateOctave = true;
                }
                plusPressed = false;
            }
        }

        if (ccButtonmatrix.getPrevState(4, 0) == 0) {
            minusPressed = true;
        } else {
            if (minusPressed) {
                if (transposer.getTransposition() != minTransposition) {
                    transposer.setTransposition(transposer.getTransposition() - 1);
                    shouldUpdateOctave = true;
                }
                minusPressed = false;
            }
        }

        if (shouldUpdateOctave) {
            Control_Surface.sendControlChange(MIDIAddress(OCTAVE_CHANGE, CHANNEL_1), transposer.getTransposition());
            shouldUpdateOctave = false;
        }
    } else {
        if (shiftPressed) {
            Serial.write("Shift released");
            shiftPressed = false;
        }
    }
}

void setup() {
    analog.setAnalogResolution(4096);
    analog.setActivityThreshold(10.0f);
    Control_Surface.begin(); // Initialize Control Surface
}

void loop() {
    Control_Surface.loop(); // Update the Control Surface
    analog.update();
    if(analog.hasChanged()) {
        // Remap so that pushing stick to the right increases the value
        int remapped =  map(analog.getValue(), 0, 4095, 4095, 0);
        pbSender.send(remapped, CHANNEL_1);
    }

    updateOctave();
}