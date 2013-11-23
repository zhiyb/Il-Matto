/*
 *  Author:  Klaus-Peter Zauner
 *  Licence: This work is licensed under the Creative Commons Attribution License.
 *           View this license at http://creativecommons.org/about/licenses/
 *
 *   
 *  Frequency table for an equal-tempered scale
 *  generated with the formula:
 *     f =  f_base * (2^{1/12})^halfsteps
 *  following:
 *     [http://www.phy.mtu.edu/~suits/NoteFreqCalcs.html]
 *   
 *  Base frequency:     440 Hz
 *  Half-steps down:     34
 *  Half-steps up:       41
 *   
 */

#include <inttypes.h>

#define ET_SCALE_C    26 // +12 //26
#define ET_SCALE_TOP  75
    
uint16_t et_scale[] = {
		62,   /*   61.74 Hz */
		65,   /*   65.41 Hz */
		69,   /*   69.30 Hz */
		73,   /*   73.42 Hz */
		78,   /*   77.78 Hz */
		82,   /*   82.41 Hz */
		87,   /*   87.31 Hz */
		92,   /*   92.50 Hz */
		98,   /*   98.00 Hz */
	       104,   /*  103.83 Hz */
	       110,   /*  110.00 Hz */
	       117,   /*  116.54 Hz */
	       123,   /*  123.47 Hz */
	       131,   /*  130.81 Hz */
	       139,   /*  138.59 Hz */
	       147,   /*  146.83 Hz */
	       156,   /*  155.56 Hz */
	       165,   /*  164.81 Hz */
	       175,   /*  174.61 Hz */
	       185,   /*  185.00 Hz */
	       196,   /*  196.00 Hz */
	       208,   /*  207.65 Hz */
	       220,   /*  220.00 Hz */
	       233,   /*  233.08 Hz */
	       247,   /*  246.94 Hz */
	       262,   /*  261.63 Hz,  middle C */
	       277,   /*  277.18 Hz */
	       294,   /*  293.66 Hz */
	       311,   /*  311.13 Hz */
	       330,   /*  329.63 Hz */
	       349,   /*  349.23 Hz */
	       370,   /*  369.99 Hz */
	       392,   /*  392.00 Hz */
	       415,   /*  415.30 Hz */
	       440,   /*  440.00 Hz */
	       466,   /*  466.16 Hz */
	       494,   /*  493.88 Hz */
	       523,   /*  523.25 Hz */
	       554,   /*  554.37 Hz */
	       587,   /*  587.33 Hz */
	       622,   /*  622.25 Hz */
	       659,   /*  659.26 Hz */
	       698,   /*  698.46 Hz */
	       740,   /*  739.99 Hz */
	       784,   /*  783.99 Hz */
	       831,   /*  830.61 Hz */
	       880,   /*  880.00 Hz */
	       932,   /*  932.33 Hz */
	       988,   /*  987.77 Hz */
	      1047,   /* 1046.50 Hz */
	      1109,   /* 1108.73 Hz */
	      1175,   /* 1174.66 Hz */
	      1245,   /* 1244.51 Hz */
	      1319,   /* 1318.51 Hz */
	      1397,   /* 1396.91 Hz */
	      1480,   /* 1479.98 Hz */
	      1568,   /* 1567.98 Hz */
	      1661,   /* 1661.22 Hz */
	      1760,   /* 1760.00 Hz */
	      1865,   /* 1864.66 Hz */
	      1976,   /* 1975.53 Hz */
	      2093,   /* 2093.00 Hz */
	      2217,   /* 2217.46 Hz */
	      2349,   /* 2349.32 Hz */
	      2489,   /* 2489.02 Hz */
	      2637,   /* 2637.02 Hz */
	      2794,   /* 2793.83 Hz */
	      2960,   /* 2959.96 Hz */
	      3136,   /* 3135.96 Hz */
	      3322,   /* 3322.44 Hz */
	      3520,   /* 3520.00 Hz */
	      3729,   /* 3729.31 Hz */
	      3951,   /* 3951.07 Hz */
	      4186,   /* 4186.01 Hz */
	      4435,   /* 4434.92 Hz */
};   /* et_scale */
