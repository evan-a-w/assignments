// Assignment 2 20T1 COMP1511: CS Beats
// synth.c
//
// Version 1.0.0: Release
//
// This file turns .beats files into .wav files.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "make_wav.h"

// Constants
#define MAX_BEATS_HEADER 1024

#define MAX_NUM_NOTES 128
#define MAX_NOTE_LENGTH 8
#define NOTES_IN_OCTAVE 12

#define ERROR_PREMATURE_EOF 1
#define ERROR_EXPECTED_NUMBER 2
#define ERROR_INDEX_NOT_FOUND 3
#define ERROR_INVALID_NUMBER 4

#define NOTE_LENGTH_TOLERANCE 250

// This controls the volume of a track.
#define AMPLITUDE 10000

const static char *error_msgs[] = {
    [ERROR_PREMATURE_EOF]     = "Reached EOF in an invalid place.",
    [ERROR_EXPECTED_NUMBER]   = "Found something other than a number.",
    [ERROR_INDEX_NOT_FOUND]   = "Expected the line to start with an index.",
    [ERROR_INVALID_NUMBER]    = "Keys and Octaves should be ordered, positive, and within a valid range.",
};

// A 4A note in music is a 440Hz - set to 400 if you're baroque.
#define FREQ_4A 440

struct error {
    int line_num;
    int char_num;
    int message_code;
    struct error *next;
};

struct beat {
    int tones[MAX_NUM_NOTES];
    struct beat *next;
};

struct freq_node {
    float freqs[MAX_NUM_NOTES];
    float hold_freqs[MAX_NUM_NOTES];
    struct freq_node *next;
};

float gradient(short earlier, short later, float sample_rate) {
    float gradient = 1.0 / abs(later - earlier) * sample_rate ;
    printf("%f\n", gradient);
    return gradient;
}

// Return if two floats went from negative to positive
int upwards_cross(short earlier, short later, float sample_rate) {
    return earlier < 0 && later > 0 && gradient(earlier, later, sample_rate) < 100;
}

void increment_amplitude(float freqs[], float phases[], float radian_shifts[]) {
    int freq_i = 0;
    while (freqs[freq_i]) {
        phases[freq_i] += radian_shifts[freq_i];

        freq_i++;
    }
}

short calculate_amplitude(float freqs[], float phases[]) {
    float phase = 0;
    int freq_i = 0;
    while (freqs[freq_i]) {
        phase += sin(phases[freq_i]);

        freq_i++;
    }
    return (AMPLITUDE * phase / freq_i);
}

// Write notes of given frequencies over (end_index - start_index) samples,
// into a buffer. Return the number of samples actually written.
int write_notes(short buffer[], int start_index, int end_index, int samples_per_beat, int sample_rate, float freqs[]) {
    // This array contains the current phase of any waveform.
    float phases[MAX_NUM_NOTES] = {};

    // This array is a series of constants - for each waveform,
    // how far does it shift in a single sample
    float radian_shifts[MAX_NUM_NOTES] = {};

    // This controls whether a note stops early.
    int short_notes[MAX_NUM_NOTES] = {};

    int num_freqs = 0;
    while (freqs[num_freqs]) {
        if (freqs[num_freqs] < 0) {
            short_notes[num_freqs] = 1;
            freqs[num_freqs] *= 1;
        } else {
            short_notes[num_freqs] = 0;
        }
        radian_shifts[num_freqs] = freqs[num_freqs] * (2 * M_PI) / sample_rate;
        num_freqs++;
    }

    if (num_freqs == 0) {
        // if there are no frequencies, we can assume the buffer is zeroed out.
        return end_index;
    }
    int num_nonzero_freqs = num_freqs;

    // Write samples, and manipulate the phase array.
    int sample = start_index;
    int has_cut_short = 0;

    /*
    if (start_index != 0) {
        sample++;
        increment_amplitude(freqs, phases, radian_shifts);
        short new_amplitude = calculate_amplitude(freqs, phases);
        buffer[start_index] = (new_amplitude - buffer[start_index - 1]) / 2;
        } */
    while (sample < end_index) {
        short new_amplitude = calculate_amplitude(freqs, phases);
        if (!has_cut_short && (sample - start_index) > (6.6/10.0) * samples_per_beat) {
            // 2/3 of the note finished, short notes should stop here
            if (upwards_cross(buffer[sample - 1], new_amplitude, sample_rate)) {
                has_cut_short = 1;
                int stop_i = 0;
                while (freqs[stop_i]) {
                    if (short_notes[stop_i]) {
                        num_nonzero_freqs--;
                        radian_shifts[stop_i] = 0;
                        phases[stop_i] = 0;
                    }
                    stop_i++;
                }
                if (num_nonzero_freqs != num_freqs) {
                    // reset all phases if we reset any
                    stop_i = 0;
                    while (freqs[stop_i]) {
                        phases[stop_i] = 0;
                        stop_i++;
                    }
                    increment_amplitude(freqs, phases, radian_shifts);
                    new_amplitude = calculate_amplitude(freqs, phases);
                }
                
            }
        }
        increment_amplitude(freqs, phases, radian_shifts);
        buffer[sample] = new_amplitude;
        sample++;
    }

    // Work backwards to the last time the waveform crosses 0, and chop it there.
    int last_zero = end_index - 1;
    while (!upwards_cross(buffer[last_zero - 1], buffer[last_zero], sample_rate)) {
        last_zero--;
    }

    if (end_index - last_zero < NOTE_LENGTH_TOLERANCE) {
        sample = last_zero;
        while (sample < end_index) {
            buffer[sample] = 0;
            sample++;
        }
    } else {
        if (num_nonzero_freqs > 0) {
            // This means we would have to go back too far to find the
            // last crossing. Change NOTE_LENGTH_TOLERANCE to be bigger
            // if this is coming up.
            printf("Cannot smooth waveform - expect a choppy sound.\n");
        }
        last_zero = end_index;
    }

    return last_zero;

}

// A hacky way to append an error to the end of a list.
void append_error(struct error **head, int line_num, int char_num, int message_code) {
    while ((*head)) {
        head = &((*head)->next);
    }
    (*head) = calloc(sizeof(struct error), 1);
    (*head)->line_num = line_num;
    (*head)->char_num = char_num;
    (*head)->message_code = message_code;
    (*head)->next = NULL;
}

// A hacky way to append a beat to the end of a list.
void append_beat(struct beat **head, int *tones) {
    while ((*head)) {
        head = &((*head)->next);
    }
    (*head) = calloc(sizeof(struct beat), 1);

    int i = 0;
    while (tones[i] != 0) {
        (*head)->tones[i] = tones[i];
        i++;
    }
    (*head)->next = NULL;
}

// Recursively print the errors in the error list.
void print_and_free_errors(char *file_name, struct error *error) {
    if (!error) return;

    printf(
        "%s:%d:%d\t: %s\n",
        file_name,
        error->line_num,
        error->char_num,
        error_msgs[error->message_code]
    );
    print_and_free_errors(file_name, error->next);
    free(error);
}

// free the beats in a list of beats.
void free_beats(struct beat *beat) {
    struct beat *temp = NULL;
    while (beat) {
        temp = beat;
        beat = beat->next;
        free(temp);
    }
}

#define STATE_ERROR 0
#define STATE_INDEX 1
#define STATE_NUMBER 2
#define STATE_FIND_NUMBER 3
#define STATE_EOL 4

void clear_string(char *str) {
    int i = 0;
    while (str[i] != '\0') {
        str[i++] = '\0';
    }
}

void clears(int *array) {
    int i = 0;
    while (array[i] != 0) {
        array[i++] = 0;
    }
}

struct beat *parse_body(FILE *fp, char *input_file) {
    struct error* errors = NULL;
    struct beat* beats = NULL;
 
    int line_num = 2;
    int char_num = 0;

    int state = STATE_INDEX;
    int c;

    int note_index = 0;
    char note[MAX_NOTE_LENGTH] = {};
    int beat_note_index = 0;
    int beat_notes[MAX_NUM_NOTES] = {};

    // This while loop defies a state machine to parse .beats files.
    while (1) {
        c = fgetc(fp);
        if (c == ' ') continue;
        if (state == STATE_INDEX) {
            if (c == ']') {
                state = STATE_FIND_NUMBER;
            } else if (c == '\n' || c == '|') {
                append_error(&errors, line_num, char_num, ERROR_INDEX_NOT_FOUND);
                state = STATE_ERROR;
            } else if (c == -1) {
                state = STATE_EOL;
            }
        } else if (state == STATE_FIND_NUMBER) {
            if (isdigit(c)) {
                note_index = 0;
                note[0] = c;
                note_index++;
                state = STATE_NUMBER;
            } else if (c == '\n' || c == EOF) {
                state = STATE_EOL;
            } else if (c == '|') {
                state = STATE_FIND_NUMBER;
            } else {
                append_error(&errors, line_num, char_num, ERROR_EXPECTED_NUMBER);
                state = STATE_ERROR;
            }
        } else if (state == STATE_NUMBER) {
            int last_num = 0;
            if (isdigit(c)) {
                note[note_index] = c;
                note_index++;
            } else if (c == '\n' || c == EOF) {
                state = STATE_EOL;
                last_num = 1;
            } else if (c == '|') {
                state = STATE_FIND_NUMBER;
                last_num = 1;
            } else {
                append_error(&errors, line_num, char_num, ERROR_EXPECTED_NUMBER);
                state = STATE_ERROR;
            }

            if (last_num) {
                int new_note = 0;
                sscanf(note, "%d", &new_note);
                if (new_note < 0 || new_note >= 2000 || new_note % 100 >= NOTES_IN_OCTAVE) {
                    append_error(&errors, line_num, char_num, ERROR_INVALID_NUMBER);
                    state = STATE_ERROR;
                } else {
                    beat_notes[beat_note_index] = new_note;
                    beat_note_index++;
                }
                // reset note.
                while (note_index >= 0) {
                    note[note_index] = '\0';
                    note_index--;
                }
            }
        } else if (state == STATE_ERROR) {
            while (c != EOF && c != '\n' && (c = fgetc(fp))) {
                // skip to end of line
            }
            state = STATE_EOL;
        }

        char_num++;

        if (state == STATE_EOL) {
            line_num++;
            char_num = 0;

            append_beat(&beats, beat_notes);
            
            beat_note_index = 0;
            clear_string(note);
            clears(beat_notes);

            state = STATE_INDEX;
        }
        if (c == EOF) {
            break;
        }
    }

    if (errors) {
        print_and_free_errors(input_file, errors);
        free_beats(beats);
        exit(1);
    }

    return beats;
    
    
}

float note_to_frequency(int note) {
    int octave = note / 100;
    int key_id = note % 100;
    int piano_key = octave*12 + key_id + 1;
    return pow(2, (piano_key-49.0)/12.0) * 440.0;
}

void find_overlap_freqs(float *overlaps, float *freq1, float *freq2) {
    int num_overlaps = 0;
    int i = 0;
    while (freq1[i]) {
        int j = 0;
        while (freq2[j]) {
            if (freq1[i] == freq2[j]) {
                overlaps[num_overlaps] = freq1[i];
                num_overlaps++;
            }
            j++;
        }
        i++;
    }
}

int main(int argc, char *argv[]) {
    FILE *fp;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        exit(1);
    }

    char *input_file = argv[1];
    char *output_file = argv[2];

    char last_char = input_file[strlen(input_file) - 1];
    if (last_char == 'c' || last_char == 'h') {
        fprintf(stderr, "%s::\tInput file may not end with letter c or h.\n", input_file);
        exit(1);
    }

    last_char = output_file[strlen(output_file) - 1];
    if (last_char == 'c' || last_char == 'h') {
        fprintf(stderr, "%s::\tOutput file may not end with letter c or h.\n", output_file);
        exit(1);
    }
 
    fp = fopen(input_file, "r");
    if (fp == NULL) {
        fprintf(stderr, "%s::\tFile not found, or could not be opened!\n", input_file);
        exit(1);
    }
 
    int bpm = -1;
 
    char beats_header[MAX_BEATS_HEADER];
 
    fgets(beats_header, MAX_BEATS_HEADER, fp);
    int scan = sscanf(beats_header, "%d|", &bpm);
    if (scan != 1 || bpm < 1) {
        fprintf(stderr, "%s:1:1\tExpected a header like '<bpm>|' as first line.\n", input_file);
        exit(1);
    }

    struct beat *beats = parse_body(fp, input_file);

    int num_beats = 0;
    struct beat *curr_beat = beats;
    while (curr_beat) {
        num_beats++;
        curr_beat = curr_beat->next;
    }

    fclose(fp);

    if (beats) {
        int sample_rate_ps = 20000;
        int samples_per_beat = sample_rate_ps * (60.0/ bpm);
        int total_samples = num_beats * samples_per_beat;

        curr_beat = beats;

        int buffer_loc = 0;
        int b = 0;
        short *buffer = calloc(sizeof(short), total_samples);

        while (curr_beat) {
            float freqs[MAX_NUM_NOTES] = {};
            int freq_i = 0;
            while (curr_beat->tones[freq_i]) {
                if (curr_beat->tones[freq_i] >= 1000) {
                    freqs[freq_i] = note_to_frequency(curr_beat->tones[freq_i] - 1000);
                    // negative frequency means "beat should be seperate"
                    freqs[freq_i] *= -1;
                } else {
                    freqs[freq_i] = note_to_frequency(curr_beat->tones[freq_i]);
                }
                freq_i++;
            }

            buffer_loc = write_notes(buffer, buffer_loc, (b+1)*samples_per_beat, samples_per_beat, sample_rate_ps, freqs);
            b++;
            curr_beat = curr_beat->next;

        }


        free_beats(beats);

        write_wav(output_file, total_samples, buffer, sample_rate_ps);
        free(buffer);
    }
}
