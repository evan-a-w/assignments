// Assignment 2 21T1 COMP1511: Beats by CSE
// beats.c
//
// This program was written by Evan Williams (z5368211)
// on 06/04/2021 - 
//
// Version 1.0.0: Assignment released.

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Add any extra #includes your code needs here.

#include "ext.h"
#include <limits.h>
#include <stdbool.h>
#include <string.h>

// Add your own #defines here.
#define OCTAVE_MIN 0
#define OCTAVE_MAX 10
#define KEY_MIN 0
#define KEY_MAX 12

//////////////////////////////////////////////////////////////////////

// You don't have to use the provided struct track, you are free to
// make your own struct instead.
// If you use the provided struct track, you will have to add fields
// to store other information.

struct track {
    // TODO: You will have to add extra fields here in Stage 2.
    struct beat *head;
    struct beat *selected_beat;
};

// You don't have to use the provided struct beat, you are free to
// make your own struct instead.
// If you use the provided struct beat, you may add fields
// to it to store other information.

struct beat {
    // You may choose to add or change fields in this struct.
    struct note *notes;
    struct beat *next;
};

// You don't have to use the provided struct note, you are free to
// make your own struct instead.
// If you use the provided struct note, you add fields
// to it to store other information.

typedef struct note {
    // You may choose to add or change fields in this struct.
    int octave;
    int key;
    struct note *next;
} *Note;

// Add any other structs you define here.

//////////////////////////////////////////////////////////////////////

// Add prototypes for any extra functions you create here.

Note create_note(int octave, int key);
void insert_note_to_beat(Beat beat, int octave, int key);
Beat get_beat_range(Beat first_beat, int range_length);
int reverse_beat_range(Beat first_beat, Beat end_beat);

// Return a malloced Beat with fields initialized.
Beat create_beat(void) {
    Beat new_beat = malloc(sizeof (struct beat));
    assert(new_beat != NULL);

    new_beat->next = NULL;
    new_beat->notes = NULL;

    // You do not need to change this function, unless you change
    // the implementation of struct beat.

    return new_beat;
}

// You need to implement the following functions.
// You can find descriptions of what each function should do in beats.h

//////////////////////////////////////////////////////////////////////
//                        Stage 1 Functions                         //
//////////////////////////////////////////////////////////////////////

// Add a note to the end of a beat.
int add_note_to_beat(Beat beat, int octave, int key) {
    // If the octave is not valid, return INVALID_NOTE
    if (OCTAVE_MIN > octave || octave >= OCTAVE_MAX) {
        return INVALID_OCTAVE;
    // Do the same for the key
    } else if (KEY_MIN > key || key >= KEY_MAX) {
        return INVALID_KEY;
    }
   
    // If beat->notes is NULL, the new note must be valid so we add it to the
    // notes and return.
    if (beat->notes == NULL) {
        beat->notes = create_note(octave, key);
        return VALID_NOTE;
    }

    // Create a new pointer to the notes. 
    Note curr_note = beat->notes;

    // Get to the last note. This note has the max octave and max key for that
    // octave since it is a sorted list.
    while (curr_note->next != NULL) {
        curr_note = curr_note->next;
    }
    
    int max_octave = curr_note->octave;
    int max_key = curr_note->key; 

    // If the octave is lower than the max, it is invalid.
    if (octave < max_octave) {
        return NOT_HIGHEST_NOTE;
    // If the octaves are equal but the key is not greater than the max, it is
    // also invalid.
    } else if (octave == max_octave && key <= max_key) {
        return NOT_HIGHEST_NOTE;
    }

    // All checks have passed, so we now add the note to the end of the notes.
    // The last note is curr_note, so we simply add to that.
    curr_note->next = create_note(octave, key); 

    return VALID_NOTE;
}

// Malloc a note pointer and assign its octave and key values to the parameters
// of the function. This pointer is then returned.
Note create_note(int octave, int key) {
    Note new_note = malloc(sizeof(struct note));
    new_note->octave = octave;
    new_note->key = key;
    new_note->next = NULL;
    return new_note;
}

// Print the contents of a beat.
void print_beat(Beat beat) {
    struct note *curr_note = beat->notes;
    
    // Traverse the list of notes and print values.
    while (curr_note != NULL) {
        // Print the octave and key.
        printf("%d %02d", curr_note->octave, curr_note->key);

        // Only print the pipe if there is an upcoming note.
        if (curr_note->next != NULL) {
            printf(" | "); 
        }
        curr_note = curr_note->next;
    }
    printf("\n");

    return;
}

// Count the number of notes in a beat that are in a given octave.
int count_notes_in_octave(Beat beat, int octave) {
    struct note *curr_note = beat->notes;

    // First get to the initial note of the given octave.
    while (curr_note != NULL && curr_note->octave != octave) {
        curr_note = curr_note->next;
    }

    // If we are already at the end, there are 0 notes of the given octave.
    if (curr_note == NULL) {
        return 0;
    }

    // Continue traversing the list until the end or we get past the octave.
    int count = 0;
    while (curr_note != NULL && curr_note->octave == octave) {
        curr_note = curr_note->next;
        count++;
    }
    return count;
}

//////////////////////////////////////////////////////////////////////
//                        Stage 2 Functions                         //
//////////////////////////////////////////////////////////////////////

// Return a malloced track with fields initialized.
Track create_track(void) {
    // Note: there is no fprintf in this function, as the
    // Stage 1 autotests call create_track but expect it to return NULL
    // (so this function should do nothing in Stage 1).
    Track track = malloc(sizeof(struct track));
    track->head = NULL;
    track->selected_beat = NULL;
    return track;
}

// Add a beat after the current beat in a track.
void add_beat_to_track(Track track, Beat beat) {
    // If the track is playing, insert the new beat after the current beat.
    if (track->selected_beat != NULL) {
        beat->next = track->selected_beat->next;
        track->selected_beat->next = beat;

    // Otherwise, put it as the first element.
    } else {
        beat->next = track->head;
        track->head = beat;
    }
    return;
}

// Set a track's current beat to the next beat.
int select_next_beat(Track track) {
    // If track is stopped, set the current beat to the first beat and return
    // that the track is playing.
    if (track->selected_beat == NULL) {
        track->selected_beat = track->head;

    // Otherwise, change the current beat to the beat after the current beat.
    } else {
        track->selected_beat = track->selected_beat->next;
    }

    // If the current beat is NULL, we are at the end of the track, so 
    // return that it is stopped.
    if (track->selected_beat == NULL) {
        return TRACK_STOPPED;
    }

    // Otherwise, the track is playing.
    return TRACK_PLAYING;
}

// Print the contents of a track.
void print_track(Track track) {
    // Traverse the list of beats, printing info concerning each beat.
    Beat curr_beat = track->head;
    Beat selected_beat = track->selected_beat;
    int index = 1;
    while (curr_beat != NULL) {
        if (curr_beat == selected_beat) {
            putchar('>');
        } else {
            putchar(' ');
        }
        
        printf("[%d] ", index);
        
        // Print the beat. This also outputs a newline at the end.
        print_beat(curr_beat);

        curr_beat = curr_beat->next; 
        index++;
    }
    return;
}

// Count beats after the current beat in a track.
int count_beats_left_in_track(Track track) {
    int count = 0;
   
    // The beat should start as the beat after the selected beat i
    // if the track is playing, or the first beat if not. 
    Beat curr_beat;
    if (track->selected_beat != NULL) {
        curr_beat = track->selected_beat->next;
    } else {
        curr_beat = track->head;
    }
    
    // Count all the beats after and including curr_beat.
    while (curr_beat != NULL) {
        count++;
        curr_beat = curr_beat->next;
    }

    return count;
}

//////////////////////////////////////////////////////////////////////
//                        Stage 3 Functions                         //
//////////////////////////////////////////////////////////////////////

// Free the memory of a beat, and any memory it points to.
void free_beat(Beat beat) {
    // Note: there is no printf in this function, as the
    // Stage 1 & 2 autotests call free_beat but don't check whether
    // the memory has been freed (so this function should do nothing in
    // Stage 1 & 2, rather than print an error).

    // Traverse the beats.
    while (beat != NULL) {
        // Traverse and free all notes.
        Note curr_note = beat->notes;
        while (curr_note != NULL) {
            Note next = curr_note->next;
            free(curr_note);
            curr_note = next;
        }
        Beat next = beat->next;
        free(beat);
        beat = next;
    }
    return;
}

// Free the memory of a track, and any memory it points to.
void free_track(Track track) {
    // Note: there is no printf in this function, as the
    // Stage 1 & 2 autotests call free_track but don't check whether
    // the memory has been freed (so this function should do nothing in
    // Stage 1 & 2, rather than print an error).

    // Free all beats in the track with the free_beat function.
    // Since the selected beat is in the list of beats, it is also freed.
    free_beat(track->head);
    free(track);
    return;
}

// Remove the currently selected beat from a track.
int remove_selected_beat(Track track) {
    Beat selected_beat = track->selected_beat;
    if (selected_beat == NULL) {
        return TRACK_STOPPED;
    }
    
    Beat curr_beat = track->head;
    if (curr_beat == selected_beat) {
        // Set the head to the next beat and free the current beat.
        track->head = curr_beat->next;
        track->selected_beat = track->head;
        curr_beat->next = NULL;
        free_beat(curr_beat);    
    } else {
        // Traverse the beats until the next beat is the selected beat.
        while (curr_beat->next != selected_beat) {
            curr_beat = curr_beat->next; 
        }

        // Remove the selected beat and set selected_beat to the beat after.
        curr_beat->next = selected_beat->next;
        track->selected_beat = curr_beat->next;

        // Free the old selected beat.
        selected_beat->next = NULL;
        free_beat(selected_beat);
    }

    
    // If there is a selected beat, return TRACK_PLAYING. Otherwise, return
    // TRACK_STOPPED.
    if (track->selected_beat != NULL) {
        return TRACK_PLAYING;
    }

    return TRACK_STOPPED;
}

//////////////////////////////////////////////////////////////////////
//                        Stage 4 Functions                         //
//////////////////////////////////////////////////////////////////////

// Add note to beat, given its 'musical notation'.
int add_musical_note_to_beat(Beat beat, char *musical_notation) {
    int len = strlen(musical_notation);
    // Check if the note is of a valid length
    if (len < 2) {
        return INVALID_MUSICAL_NOTE;
    }
    
    int octave = musical_notation[0];
    char letter = musical_notation[1];
    
    // Check whether each value is valid.
    int invalid_octave = '0' > octave || octave > '9';
    int invalid_letter = 'A' > letter || letter > 'G';

    int key = letter - 'A';

    if (invalid_octave || invalid_letter) {
        return INVALID_MUSICAL_NOTE;
    }
     
    int index = 2; 
    while (index < len && musical_notation[index] != '\0') {
        if (musical_notation[index] != '#') {
            return INVALID_MUSICAL_NOTE;
        }
        key++;
        if (key >= KEY_MAX) {
            key = key - KEY_MAX;
            octave++;
        }
    } 
    
    insert_note_to_beat(beat, octave, key);

    return VALID_NOTE;
}

// This function inserts a note into a beat 
// that does not have to be larger than all present notes.
void insert_note_to_beat(Beat beat, int octave, int key) {
    Note curr_note = beat->notes;

    // Booleans to track the status of the note and loop.
    int exists_in_notes = 0;
    int exit_loop = 0;

    // Loop over the list of notes until the end or any end condition.
    while (curr_note->next != NULL && !exit_loop) {
        int next_octave = curr_note->next->octave;
        int next_key = curr_note->next->key; 

        if (next_octave > octave) {
            exit_loop = 1;
        } else if (next_octave == octave && next_key >= key) {
            if (next_key == key) {
                exists_in_notes = 1;
            }
            exit_loop = 1;
        }
        curr_note = curr_note->next;
    }

    // Insert the note after curr_note.
    if (!exists_in_notes) {
        Note new_note = create_note(octave, key);  
        new_note->next = curr_note->next;
        curr_note->next = new_note;
    } 
}


//////////////////////////////////////////////////////////////////////
//                        Stage 5 Functions                         //
//////////////////////////////////////////////////////////////////////

// Cut a range of beats to the end of a track.
void cut_range_to_end(Track track, int range_length) {
    if (range_length < 1) {
        return;
    }
    
    Beat curr_beat = track->selected_beat;
    
    // If track is stopped, do nothing.
    if (curr_beat == NULL) {
        return;
    }

    Beat first_beat = curr_beat->next;
    Beat end_beat = get_beat_range(first_beat, range_length);

    // If the next beat is NULL, don't do anything because the
    // subsection is already at the end.
    if (end_beat->next == NULL) {
        return;
    }

    track->selected_beat->next = curr_beat->next;

    // Traverse the list again until we find the end.
    curr_beat = curr_beat->next;
    while (curr_beat->next != NULL) {
        curr_beat = curr_beat->next;
    }

    curr_beat->next = first_beat;
    end_beat->next = NULL;
    
    return;
}

// Reverse a list of beats within a range of a track.
int reverse_range(Track track, int range_length) {
    if (track == NULL || range_length < 1) {
        return 0;
    }

    Beat first_beat = track->selected_beat;
    
    // If track is stopped, do nothing.
    if (first_beat == NULL) {
        return 0;
    }

    Beat end_beat = get_beat_range(first_beat, range_length - 1); 

    int count = reverse_beat_range(track->head, first_beat, end_beat);
    
    return count;
}

// This function returns the beat range_length beats away from first_beat,
// or the last beat in the list if it is too short.
Beat get_beat_range(Beat first_beat, int range_length) {
    Beat curr_beat = first_beat;
    int i = 0;
    while (curr_beat->next != NULL && i < range_length) {
        curr_beat = curr_beat->next;
        i++;
    }
    return curr_beat;
}

// Reverses the range of beats from first_beat to end_beat.
// Assumes end_beat is not NULL. Returns the count of beats reversed.
int reverse_beat_range(Beat head, Beat first_beat, Beat end_beat) {
    // Get to the Beat before the first beat.
    Beat insert_after = head;
    while (insert_after->next != first_beat) {
        insert_after = insert_after->next;
    }

    Beat curr_beat = first_beat;
    Beat prev = end_beat->next;

    int count = 0;
    while (curr_beat != end_beat && curr_beat != NULL) {
        Beat next = curr_beat->next;
        curr_beat->next = prev;
        prev = curr_beat;
        curr_beat = next;
        count++;
    }
    
    insert_after->next = prev;

    return count; 
}
