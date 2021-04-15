// Assignment 2 21T1 COMP1511: CS bEats
// beats.c
//
// This program was written by Evan Williams (z5368211)
// on 13/04/2021
//
// Version 1.0.0: Assignment released.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Add any extra #includes your code needs here.
//
// But note you are not permitted to use functions from string.h
// so do not #include <string.h>
//
#include "beats.h"
#include <limits.h>
#include <stdbool.h>

#include "ext_beats.h"
#include "ext_save.h"

// Add your own #defines here.
#define OCTAVE_MIN 0
#define OCTAVE_MAX 10
#define KEY_MIN 0
#define KEY_MAX 12

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
typedef struct string {
    char *str;
    int capacity;
    int size;
} String;

//////////////////////////////////////////////////////////////////////

// Add prototypes for any extra functions you create here.

int is_lower(int octave, int key, Note note);
Note create_note(int octave, int key);
void merge_into(Beat result, Beat merge);
int merge_range(Beat dest, int range);
Note insert_before(Note src, Note dest, Note head);
void add_beat_end_track(Track trk, Beat beat);

String *new_string(int capacity);
void push_back(String *str, char ch);
void pop_back(String *str);
void free_string(String *str);

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
    // Track 
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
    
    if (is_lower(octave, key, curr_note)) {
        return NOT_HIGHEST_NOTE;
    }

    // All checks have passed, so we now add the note to the end of the notes.
    // The last note is curr_note, so we simply add to that.
    curr_note->next = create_note(octave, key); 

    return VALID_NOTE;
}

// Returns whether a given octave and key are smaller than a note.
// Returns 2 if the notes are equal. Always returns 1 if note == NULL.
int is_lower(int octave, int key, Note note) {
    // If the octave is lower than the max, it is lower.
    // ALso, if the octaves are equal but the key is not greater, it is lower.
    if (note == NULL || octave < note->octave || (octave == note->octave && key < note->key)) {
        return 1;
    } else if (octave == note->octave && key == note->key) {
        return 2;
    }

    return 0;
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
/////////////////////////////////////////////////////////////////////

//                Extension -- Stage 4 Functions                      //
////////////////////////////////////////////////////////////////////////

// Merge `beats_to_merge` beats into `merged_beats`
void merge_beats(Track track, int beats_to_merge, int merged_beats) {
    if (beats_to_merge <= merged_beats) {
        return;
    }
    int quotient = beats_to_merge / merged_beats;
    int remainder = beats_to_merge % merged_beats;

    int count_merged = 0;
    if (track->selected_beat == NULL) {
        return;
    }
    Beat dest = track->selected_beat;
    if (dest == NULL) {
        return;
    }

    // First merge quotient + remainder beats into one.
    // Then just do quotient.
    while (dest != NULL && dest->next != NULL && count_merged < beats_to_merge) {
        int range;
        if (count_merged == 0) {
            range = quotient + remainder;
        } else {
            range = quotient;
        }
        count_merged += merge_range(dest, range);
        if (dest != NULL && dest->next != NULL && count_merged < beats_to_merge) {
            dest = dest->next;
        }
    }

    track->selected_beat = dest->next;
}

// This function merges range - 1 beats starting from curr into dest,
// modifying count_merged to reflect the number of beats merged.
int merge_range(Beat dest, int range) {
    if (dest == NULL || dest->next == NULL) {
        return 0;
    }

    int i = 1;
    while (dest->next != NULL && i < range) {
        Beat tmp = dest->next->next;
        merge_into(dest, dest->next);
        dest->next = tmp;
        i++;
    }

    return i;
}

// Merge the merge Beat into the result Beat. This works for sure.
void merge_into(Beat result, Beat merge) {
    if (merge->notes == NULL) {
        return;
    }
    if (result->notes == NULL) {
        result->notes = merge->notes;
        result->next = merge->next;
        free(merge);
        return;
    }
    Note src = merge->notes;
    Note dest = result->notes;
    while (dest != NULL && src != NULL) {
        int curr_lower = is_lower(src->octave, src->key, dest);
        int next_lower = is_lower(src->octave, src->key, dest->next);

        // If src is lower than the current thing, insert before it. This only
        // happens when dest is the head.
        if (curr_lower == 1) {
            Note tmp = src->next; 
            src->next = dest;
            dest = src;
            result->notes = src;
            src = tmp;
        } else if (curr_lower == 2 || next_lower == 2) {
            Note tmp = src->next;
            free(src);
            src = tmp;
        } else if (next_lower == 1) {
            Note tmp = src->next;
            Note dest_tmp = dest->next;
            dest->next = src;
            src->next = dest_tmp;
            src = tmp;
        } else {
            dest = dest->next; // dest->next != NULL since lower == 1 if so
        }
    }
    free(merge);
}

// Insert a Note before a given Note, changing dest to be src and src
// to be the beat after src.
Note insert_before(Note src, Note dest, Note head) {
    if (dest == head) {
        Note tmp_src = src->next;
        src->next = dest;
        dest = src;
        src = tmp_src;
        return dest;
    }
    return head;
}

////////////////////////////////////////////////////////////////////////
//                Extension -- Stage 5 Functions                      //
////////////////////////////////////////////////////////////////////////

void save_track(Track track, char *name) {
    String *str = new_string(50);
    // TODO dont save the current beat :(
    // Traverse the list of beats, printing info concerning each beat.
    Beat curr_beat = track->head;
    while (curr_beat != NULL) {
        Note curr_note = curr_beat->notes;
        
        while (curr_note != NULL) {
            push_back(str, '0' + curr_note->octave);
            push_back(str, '0' + curr_note->key / 10);
            push_back(str, '0' + curr_note->key % 10);
            
            curr_note = curr_note->next;
        }
        push_back(str, '\n');

        curr_beat = curr_beat->next; 
    }
    push_back(str, '\0');
    save_string(name, str->str);
    free_string(str);
    return;
}

Track load_track(char *name) {
    Track trk = create_track();
    char *contents = load_string(name);
    Beat new_beat;

    int i = 0;
    while (contents[i] != '\0') {
        new_beat = create_beat();
        
        // Go note by note now
        while (contents[i] != '\n') {
            int octave = contents[i] - '0';         
            int key = (contents[i + 1] - '0') * 10 + contents[i + 2] - '0';
            
            add_note_to_beat(new_beat, octave, key);
            i += 3;    
        } 
         
        add_beat_end_track(trk, new_beat);
        i++;
    }
    free(contents);
    return trk;
}

void add_beat_end_track(Track trk, Beat beat) {
    if (trk->head == NULL) trk->head = beat;
    else {
        Beat curr = trk->head;
        while (curr->next != NULL) curr = curr->next;
        curr->next = beat;
    }
}

String *new_string(int capacity) {
    String *new = malloc(sizeof(String));
    new->capacity = capacity;
    new->size = 0;
    new->str = calloc(capacity, sizeof(char));
    return new;
}

void push_back(String *str, char ch) {
    if (str->size == str->capacity) {
        str->capacity *= 2;
        str->str = realloc(str->str, sizeof(char) * str->capacity);
    }
    str->str[str->size++] = ch;
}

void pop_back(String *str) {
    str->size--;
}

void free_string(String *str) {
    free(str->str);
    free(str);
}
