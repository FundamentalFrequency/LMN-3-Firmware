#include <Arduino.h>
#include <Encoder.h>
#include <Control_Surface.h>


// CC values
const int encoder1          = 3;
const int encoder2          = 9;
const int encoder3          = 14;
const int encoder4          = 15;
const int encoder5          = 20;
const int encoder6          = 21;
const int encoder7          = 22;
const int encoder8          = 23;
const int encoder9          = 118;
const int encoderButton1    = 24;
const int encoderButton2    = 25;
const int encoderButton3    = 26;
const int encoderButton4    = 27;
const int encoderButton5    = 113;
const int encoderButton6    = 114;
const int encoderButton7    = 115;
const int encoderButton8    = 116;
const int settingsButton    = 85;
const int tracksButton      = 86;
const int metronomeButton   = 87;
const int mixerButton       = 88;
const int pluginsButton     = 89;
const int modifiersButton   = 90;
const int sequencersButton  = 102;
const int loopInButton      = 103;
const int loopOutButton     = 104;
const int loopButton        = 105;
const int cutButton         = 106;
const int pasteButton       = 107;
const int sliceButton       = 108;
const int recordButton      = 109;
const int playButton        = 110;
const int stopButton        = 111;
const int shiftButton       = 112;
const int octaveChange      = 117;
const int dummy             = 119;


// Instantiate a MIDI over USB interface.
USBMIDI_Interface midi;

CCRotaryEncoder enc1 = {
        {33, 32}, // pins
        {encoder1},         // MIDI address (CC number + optional channel)
        1,      // optional multiplier if the control isn't fast enough
};

CCRotaryEncoder enc2 = {
        {9, 8}, // pins
        {encoder2},         // MIDI address (CC number + optional channel)
        1,      // optional multiplier if the control isn't fast enough
};

CCRotaryEncoder enc3 = {
        {4, 3}, // pins
        {encoder3},         // MIDI address (CC number + optional channel)
        1,      // optional multiplier if the control isn't fast enough
};

CCRotaryEncoder enc4 = {
        {0, 27}, // pins
        {encoder4},         // MIDI address (CC number + optional channel)
        1,      // optional multiplier if the control isn't fast enough
};

CCRotaryEncoder enc5 = {
        {31, 30}, // pins
        {encoder5},         // MIDI address (CC number + optional channel)
        1,      // optional multiplier if the control isn't fast enough
};

CCRotaryEncoder enc6 = {
        {29, 28}, // pins
        {encoder6},         // MIDI address (CC number + optional channel)
        1,      // optional multiplier if the control isn't fast enough
};

CCRotaryEncoder enc7 = {
        {7, 5}, // pins
        {encoder7},         // MIDI address (CC number + optional channel)
        1,      // optional multiplier if the control isn't fast enough
};

CCRotaryEncoder enc8 = {
        {2, 1}, // pins
        {encoder8},         // MIDI address (CC number + optional channel)
        1,      // optional multiplier if the control isn't fast enough
};

CCRotaryEncoder enc9 = {
        {35, 34}, // pins
        {encoder9},         // MIDI address (CC number + optional channel)
        1,      // optional multiplier if the control isn't fast enough
};



int previousPbValue = -1000;
PBPotentiometer pitchBendPotentiometer = {
        A7,        // Analog pin connected to potentiometer
        CHANNEL_1, // MIDI Channel 1
};


Transposer<-4, +4>transposer(12);

IncrementDecrementSelector<transposer.getNumberOfBanks()> selector = {
        transposer,
        {36, 37},
        Wrap::Clamp,
};



const AddressMatrix<2, 14> noteAddresses = {{
                                                    {53, 55, 57, 59, 60, 62, 64, 65, 67, 69, 71, 72, 74, 76},
                                                    {1, 54, 56, 58, 1, 61, 63, 1, 66, 68, 70, 1, 73, 75},
                                            }};

Bankable::NoteButtonMatrix<2, 14> noteButtonMatrix = {
        transposer,
        {16, 14}, // row pins
        {42, 41, 40, 39, 38, 18, 19, 20, 21, 22, 23, 24, 25, 26},    // column pins
        noteAddresses,    // address matrix
        CHANNEL_1,    // channel and cable number
};

const AddressMatrix<7, 17> ccAddresses = {{
                                                  {settingsButton, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, encoderButton1, dummy, encoderButton2, dummy, encoderButton3, dummy, encoderButton4, dummy},
                                                  {tracksButton, metronomeButton, mixerButton, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy},
                                                  {pluginsButton, modifiersButton, sequencersButton, dummy, dummy, dummy, dummy, dummy, dummy, encoderButton5, dummy, encoderButton6, dummy, encoderButton7, dummy, encoderButton8, dummy},
                                                  {loopInButton, loopOutButton, loopButton, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy},
                                                  {cutButton, pasteButton, sliceButton, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy},
                                                  {recordButton, playButton, stopButton, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy},
                                                  {dummy, dummy, shiftButton, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy},
                                          }};

CCButtonMatrix<7, 17> ccButtonmatrix = {
        {10, 11, 12, 13, 14, 15, 16}, // row pins
        {17, 44, 43, 42, 41, 40, 39, 38, 18, 19, 20, 21, 22, 23, 24, 25, 26},    // column pins
        ccAddresses,    // address matrix
        CHANNEL_1,    // channel and cable number
};

void setup() {

    Control_Surface.begin(); // Initialize Control Surface
}

void loop() {
    Control_Surface.loop(); // Update the Control Surface

    // check if shift is down
    if (ccButtonmatrix.getPrevState(2, 6) == 0)
    {
        if (pitchBendPotentiometer.getValue() == 0 && previousPbValue != 0)
        {
            // decrement octave
            selector.decrement(Wrap::Clamp);
            Control_Surface.MIDI().sendCC(MIDIAddress(octaveChange, CHANNEL_1), selector.get());

        }

        if (pitchBendPotentiometer.getValue() == 1023 && previousPbValue != 1023)
        {

            selector.increment(Wrap::Clamp);
            Control_Surface.MIDI().sendCC(MIDIAddress(octaveChange, CHANNEL_1), selector.get());


        }

    }

    previousPbValue = pitchBendPotentiometer.getValue();

}