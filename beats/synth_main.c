//
// DO NOT CHANGE THIS FILE
//
// You do not submit this file. This file is not marked.
// If you think you need to change this file you have
// misunderstood the assignment - ask in the course forum.
//
// Assignment 2 20T1 COMP1511: CS Beats
// synth_main.c --- like `main.c', but with the `S' command
//
// Version 1.0.0: Release
// Version 1.0.1: Remove undefined behaviour.
// Version 1.1.0: Release `synth_main.c'
// Version 1.1.1: Add documentation for `do_synth_track'.
//
// This file allows you to interactively test the functions you
// implement in beats.c

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "beats.h"

#define MAX_LINE 2048

// Complete
#define COMMAND_HELP '?'
#define COMMAND_QUIT 'q'

// Stage 1
#define COMMAND_ADD_NOTE 'a'
#define COMMAND_PRINT_BEAT 'p'
#define COMMAND_COUNT_NOTES_IN_OCTAVE 'c'

// Stage 2
#define COMMAND_ADD_BEAT 'A'
#define COMMAND_PRINT_TRACK 'P'
#define COMMAND_NEXT_BEAT '>'
#define COMMAND_COUNT_BEATS_REMAINING 'C'

// Stage 3
#define COMMAND_REMOVE_BEAT 'R'

// Stage 4
#define COMMAND_ADD_MUSICAL_NOTE 'm'

// Stage 5
#define COMMAND_CUT_RANGE 'X'
#define COMMAND_REVERSE_RANGE 'V'

// Utility
#define COMMAND_SAVE_TRACK 'S'

// Command Returns
#define RETURN_EXIT 0
#define RETURN_SILENT 1
#define RETURN_PRINT 2


typedef struct _state {
    Beat building_beat;
    Track track;
} *State;


// Helper Functions
static void do_print_intro(void);
static void *not_null(void *ptr);
static int get_command(char *command, int max_command_length);
static int run_command(State state, char *line);

// Do: Completed
static void do_print_help();

// Do: Stage 1
static void do_add_note(State state, char *line);
static void do_print_beat(State state, char *line);
static void do_count_notes_in_octave(State state, char *line);

// Do: Stage 2
static void do_add_beat(State state, char *line);
static void do_select_next_beat(State state, char *line);
static void do_print_track(State state, char *line);
static void do_count_beats_left_in_track(State state, char *line);

// Do: Stage 3
static void do_remove_beat(State state, char *line);

// Do: Stage 4
static void do_add_musical_note_to_beat(State state, char *line);

// Do: Stage 5
static void do_cut_range_to_end(State state, char *line);
static void do_reverse_range(State state, char *line);

// Do: Other
static void do_save_track(State state, char *line);

int main(void) {
    do_print_intro();

    char line[MAX_LINE];
    int command_number = 1;

    struct _state state_struct;
    State state = &state_struct;

    state->track = create_track();

    state->building_beat = NULL;

    int read_command = RETURN_PRINT;
    while (read_command) {
        if (state->building_beat == NULL) {
            state->building_beat = create_beat();
        }
        if (read_command == RETURN_PRINT) {
            printf("\n");
            printf("[%03d] Beat Being Constructed: ", command_number);
            print_beat(state->building_beat);
        }
        printf("[%03d]: ", command_number++);
        read_command = get_command(line, MAX_LINE);
        if (read_command) read_command *= run_command(state, line);
    }

    free_beat(state->building_beat);
    free_track(state->track);

    return 0;
}

static void do_print_intro(void) {
    printf("============================[ CS bEats ]============================\n");
    printf("Welcome to CS bEats! Type '?' to see help.\n");
    printf("====================================================================\n");
}

static void *not_null(void *ptr) {
    if (ptr != NULL) {
        return ptr;
    }
    fprintf(stderr, "This solution was going to pass a NULL pointer.\n");
    fprintf(stderr, "You have probably not implemented a required function.\n");
    fprintf(stderr, "If you believe this message is in error, post on the course forum.\n");
    exit(1);
}

static int get_command(char *command, int max_command_length) {

    if (fgets(command, max_command_length, stdin) == NULL) {
        return 0;
    }

    // remove '\n' if present
    command[strcspn(command, "\n")] = '\0';

    int leading_whitespace = 0;
    while (isspace(command[leading_whitespace])) {
        leading_whitespace++;
    }
    memmove(command, command+leading_whitespace, MAX_LINE - leading_whitespace);

    return 1;
}

static int run_command(State state, char *line) {
    if (line[0] == COMMAND_HELP) {
        do_print_help();
        return RETURN_SILENT;
    } else if (line[0] == COMMAND_QUIT) {
        return RETURN_EXIT;
    } else if (line[0] == COMMAND_ADD_NOTE) {
        do_add_note(state, line);
        return RETURN_PRINT;
    } else if (line[0] == COMMAND_PRINT_BEAT) {
        do_print_beat(state, line);
        return RETURN_PRINT;
    } else if (line[0] == COMMAND_COUNT_NOTES_IN_OCTAVE) {
        do_count_notes_in_octave(state, line);
        return RETURN_PRINT;
    } else if (line[0] == COMMAND_ADD_BEAT) {
        do_add_beat(state, line);
        return RETURN_PRINT;
    } else if (line[0] == COMMAND_NEXT_BEAT) {
        do_select_next_beat(state, line);
        return RETURN_PRINT;
    } else if (line[0] == COMMAND_COUNT_BEATS_REMAINING) {
        do_count_beats_left_in_track(state, line);
        return RETURN_PRINT;
    } else if (line[0] == COMMAND_PRINT_TRACK) {
        do_print_track(state, line);
        return RETURN_PRINT;
    } else if (line[0] == COMMAND_REMOVE_BEAT) {
        do_remove_beat(state, line);
        return RETURN_PRINT;
    } else if (line[0] == COMMAND_ADD_MUSICAL_NOTE) {
        do_add_musical_note_to_beat(state, line);
        return RETURN_PRINT;
    } else if (line[0] == COMMAND_CUT_RANGE) {
        do_cut_range_to_end(state, line);
        return RETURN_PRINT;
    } else if (line[0] == COMMAND_REVERSE_RANGE) {
        do_reverse_range(state, line);
        return RETURN_PRINT;
    } else if (line[0] == COMMAND_SAVE_TRACK) {
        do_save_track(state, line);
        return RETURN_SILENT;
    } else if (line[0] == '\0') {
        return RETURN_SILENT;
    } else {
        printf("Invalid command %c, use '?' to get help!\n", line[0]);
        return RETURN_SILENT;
    }
}


static void do_print_help() {
    printf(""
        "============================[ Help ]============================\n"
    );

    printf(""
        "  %c\n"
        "    Show this help screen\n",
        COMMAND_HELP
    );
    printf(""
        "  %c\n"
        "    Quit this program.\n",
        COMMAND_QUIT
    );
    printf("\n---------------------------[ Stage 1 ]---------------------------\n");
    printf(""
        "  %c <octave_number> <note_number>\n"
        "    Add a note to the current beat.\n",
        COMMAND_ADD_NOTE
    );
    printf(""
        "  %c\n"
        "    Print the beat you are constructing (happens automatically).\n",
        COMMAND_PRINT_BEAT
    );
    printf(""
        "  %c <octave>\n"
        "    Count the number of notes in the beat you are constructing that\n"
        "    are in the given octave.\n",
        COMMAND_COUNT_NOTES_IN_OCTAVE
    );
    printf("\n---------------------------[ Stage 2 ]---------------------------\n");
    printf(""
        "  %c\n"
        "    Adds the beat you are building to the track, after the currently \n"
        "    selected beat. If there is no selected beat, the beat you are\n"
        "    building becomes the first beat. The beat you are building is\n"
        "    cleared, ready to build another beat.\n",
        COMMAND_ADD_BEAT
    );
    printf(""
        "  %c\n"
        "    Print the whole track, beat by beat.\n",
        COMMAND_PRINT_TRACK
    );
    printf(""
        "  %c\n"
        "    Move the currently selected beat to the next beat. \n"
        "    Stop the track if the currently selected is the last beat.\n"
        "    If the track is stopped, this command sets the currently selected \n"
        "    beat to the first beat.\n",
        COMMAND_NEXT_BEAT
    );
    printf(""
        "  %c\n"
        "    Count the number of beats left in the track.\n",
        COMMAND_COUNT_BEATS_REMAINING
    );
    printf("\n---------------------------[ Stage 3 ]---------------------------\n");
    printf(""
        "  %c\n"
        "    Remove the currently selected beat, if there is one.\n",
        COMMAND_REMOVE_BEAT
    );
    printf("\n---------------------------[ Stage 4 ]---------------------------\n");
    printf(""
        "  %c <musical_note>\n"
        "    Add the musical note to the beat being built.\n",
        COMMAND_ADD_MUSICAL_NOTE
    );
    printf("\n---------------------------[ Stage 5 ]---------------------------\n");
    printf(""
        "  %c <range_length>\n"
        "    Cut `range_length` notes, starting at the currently selected note, and\n"
        "    paste them at the end of the track.\n",
        COMMAND_CUT_RANGE
    );
    printf(""
        "  %c <range_length>\n"
        "    Reverse the order of `range_length` notes, starting from the currently\n"
        "    selected note.\n",
        COMMAND_REVERSE_RANGE
    );
    printf("\n---------------------------[ Other ]---------------------------\n");
    printf("NOTE: These functions are not required for the assignment, but are\n");
    printf("      useful for making your own compositions.\n");
    printf(""
        "  %c <speed> <file_name>\n"
        "    Save the output of the 'P' command to a file named <file_name>.\n"
        "    That file will also contain the speed the track is meant to be\n"
        "    played back at (in beats per minute) - 60 is slow, 300 is fast.\n",
        COMMAND_SAVE_TRACK
    );

}

////////////////////////////////////////////////////////////////////////
//                         Stage 1 Functions                          //
////////////////////////////////////////////////////////////////////////

static void do_add_note(State state, char *line) {
    int octave, key;
    char command;

    int scanf_return = sscanf(line, "%c %d %d", &command, &octave, &key);

    if (scanf_return != 3) {
        printf("Command Invalid. The correct format is: %c <octave> <key>.\n", command);
        return;
    }

    int add_note_return = add_note_to_beat(not_null(state->building_beat), octave, key);

    if (add_note_return == INVALID_OCTAVE) {
        printf("Invalid octave - octave %d must non-negative and less than 10.\n", octave);
    } else if (add_note_return == INVALID_KEY) {
        printf("Invalid key - key %d must be non-negative and less than 12.\n", key);
    } else if (add_note_return == NOT_HIGHEST_NOTE) {
        printf("Invalid note - Notes must be entered in strictly ascending order!\n");
    } else if (add_note_return == VALID_NOTE) {
        printf("Added note successfully!\n");
    } else {
        printf("ERROR: Unknown return value!\n");
    }
}

static void do_print_beat(State state, char *line) {
    // we pass line for consistency, but don't need it.
    (void) line;

    print_beat(not_null(state->building_beat));
}

static void do_count_notes_in_octave(State state, char *line) {
    int octave;
    char command;

    int scanf_return = sscanf(line, "%c %d", &command, &octave);

    if (scanf_return != 2) {
        printf("Command Invalid. The correct format is: %c <octave> .\n", command);
        return;
    }

    int count = count_notes_in_octave(not_null(state->building_beat), octave);
    printf("In the beat being constructed, there are %d notes in octave %d.\n", count, octave);

}

////////////////////////////////////////////////////////////////////////
//                         Stage 2 Functions                          //
////////////////////////////////////////////////////////////////////////

static void do_add_beat(State state, char *line) {
    // we pass line for consistency, but don't need it.
    (void) line;

    add_beat_to_track(state->track, state->building_beat);

    state->building_beat = NULL;
}

static void do_select_next_beat(State state, char *line) {
    // we pass line for consistency, but don't need it.
    (void) line;

    int track_status = select_next_beat(
        not_null(state->track)
    );

    if (track_status == TRACK_PLAYING) {
        printf("Moved to next Beat.\n");
    } else if (track_status == TRACK_STOPPED) {
        printf("Track Stopped.\n");
    } else {
        printf("ERROR: Unknown return value!\n");
    }

}

static void do_print_track(State state, char *line) {
    // we pass line for consistency, but don't need it.
    (void) line;

    print_track(not_null(state->track));
}

static void do_count_beats_left_in_track(State state, char *line) {
    // we pass line for consistency, but don't need it.
    (void) line;

    int count = count_beats_left_in_track(not_null(state->track));
    printf("There are %d beats left in the track.\n", count);

}


////////////////////////////////////////////////////////////////////////
//                         Stage 3 Functions                          //
////////////////////////////////////////////////////////////////////////

static void do_remove_beat(State state, char *line) {
    // we pass line for consistency, but don't need it.
    (void) line;

    int rm_beat_return = remove_selected_beat(not_null(state->track));

    if (rm_beat_return == TRACK_PLAYING) {
        printf("Track Still Playing.\n");
    } else if (rm_beat_return == TRACK_STOPPED) {
        printf("Track Stopped.\n");
    } else {
        printf("ERROR: Unknown return value!\n");
    }

}

////////////////////////////////////////////////////////////////////////
//                         Stage 4 Functions                          //
////////////////////////////////////////////////////////////////////////


static void do_add_musical_note_to_beat(State state, char *line) {
    if (line[1] != ' ') {
        printf("Command Invalid. The correct format is: %c <musical_note>\n", line[0]);
        return;
    }

    int add_note_return = add_musical_note_to_beat(not_null(state->building_beat), &line[2]);

    if (add_note_return == INVALID_MUSICAL_NOTE) {
        printf("Invalid musical note!\n");
    } else if (add_note_return == VALID_NOTE) {
        printf("Added musical note successfully!");
    } else {
        printf("ERROR: Unknown return value!\n");

    }

}

////////////////////////////////////////////////////////////////////////
//                         Stage 5 Functions                          //
////////////////////////////////////////////////////////////////////////


static void do_cut_range_to_end(State state, char *line) {
    int range_size;
    char command;

    int scanf_return = sscanf(line, "%c %d", &command, &range_size);

    if (scanf_return != 2) {
        printf("Command Invalid. The correct format is: %c <range_size>.\n", command);
        return;
    }

    cut_range_to_end(not_null(state->track), range_size);
}


static void do_reverse_range(State state, char *line) {
    int range_size;
    char command;

    int scanf_return = sscanf(line, "%c %d", &command, &range_size);

    if (scanf_return != 2) {
        printf("Command Invalid. The correct format is: %c <range_size>.\n", command);
        return;
    }

    int reverse_range_return = reverse_range(not_null(state->track), range_size);
    printf("Reversed %d beats.\n", reverse_range_return);
}

////////////////////////////////////////////////////////////////////////
//                           Other Functions                          //
////////////////////////////////////////////////////////////////////////

/*
 * In the words of an immortal CSE lecturer, John Lions:
 *
 *     You are not expected to understand this.
 *
 * The following code uses techniques that aren't covered in COMP1511 to
 * easily save your compositions --- we cover at least some of this in
 * COMP1521, Computer Systems Fundamentals, and COMP3231, Operating
 * Systems.
 *
 * Nonetheless, we've commented it to explain what's happening, but you
 * aren't expected to read it or understand it.  (If you do, of course,
 * and you find it interesting --- you'll enjoy COMP1521 and COMP3231!)
 */
static int has_suffix (const char *str, const char *suffix);

static void do_save_track (State state, char *line) {
    /*
     * The command syntax for track saving is:
     *
     *     S <speed> <file_name>
     *
     * where `speed' is the tempo, in beats-per-minute, to play back the
     * current track at; and where `file_name' is the name of the file
     * to write to.
     *
     * Parse that out using a `scanf' call, as usual --- unusual is the
     * use of the pattern `%n', which captures the current location in
     * the string, not anything from the string itself.
     */
    char command;
    int speed, start_of_filename = -1;
    int scanf_return =
        sscanf(line, "%c %d %n", &command, &speed, &start_of_filename);

    if (scanf_return != 2 || start_of_filename == -1) {
        printf(
            "Command Invalid. "
            "The correct format is: %c <speed> <file_name>.\n",
            command);
        return;
    }

    /*
     * We now have the name of the file.  We can only do something
     * useful with a filename ending in either `.beats' or `.wav'; so
     * check for both.
     */
    char *filename = &line[start_of_filename];
    int make_beats = has_suffix(filename, ".beats");
    int make_wav   = has_suffix(filename, ".wav");
    if (!make_beats && !make_wav) {
        printf(
            "Files created by this program "
            "should end in '.beats' or '.wav'.\n");
        return;
    }

    /*
     * Open the file to write to.  We use file descriptors (blessed
     * `int' values), not `FILE *' objects, as we'll need to do some
     * hocus-pocus that relies on file descriptors in a moment.
     */
    int out = -1;
    char temp_name[] = "/tmp/beats.XXXXXX";
    if (make_beats) {
        out = open(filename, O_RDWR | O_CREAT, 0600);
        if (out == -1) {
            printf("Could not open file %s.\n", filename);
            return;
        }
    } else if (make_wav) {
        out = mkstemp(temp_name);
        if (out == -1) {
            printf("Could not open temporary file!\n");
            return;
        }
    }

    /*
     * The `stdout' object is the default `FILE *' used for, e.g.,
     * `printf'.  The `fileno' function gets the underlying file
     * descriptor that a `FILE *' object refers to.  We get the file
     * descriptor that `stdout' is backed by, so we can preserve it by
     * making a copy of it with `dup'.
     */
    int stdout_fd = fileno(stdout);
    int save_out = dup(stdout_fd);

    /*
     * Now, we replace the `stdout' that our process was given when
     * started with the output file we opened above; `dup2' copies
     * a file descriptor to have a particular value.
     *
     * From this point, all calls to `printf', `putchar', etc., will be
     * directed into the file we opened above --- this means any calls
     * to `printf' in beats.c get redirected to where we want them.
     */
    if (dup2(out, stdout_fd) == -1) {
        printf("Could not change to writing to a file.\n");
    }

    /*
     * Print the speed for the synthesiser to play back at, and the
     * track description (as produced by the `print_track' function),
     * which is all the synthesiser needs.
     */
    printf("%d\n", speed);
    do_print_track(state, "");

    /*
     * Make sure everything was written out, and close the output file.
     */
    fflush(stdout);
    close(out);

    /*
     * Reconnect the original `stdout'; from here, `printf' etc. go back
     * to printing to the terminal as we expect.
     */
    dup2(save_out, stdout_fd);
    close(save_out);

    if (make_beats) {
        printf("Wrote to %s!\n", filename);
        return;
    }

    /*
     * If we're asked to synthesise the wav file as well, we need to
     * create a call to the synthesiser.  We do this by using `snprintf'
     * to compute the command we need to execute --- `snprintf' is like
     * `printf' but prints to a buffer of a particular size.
     */
    char sys_command[2048];
    snprintf(
        sys_command, 2048,
        "~cs1511/bin/cs_synth %s %s", temp_name, filename
    );

    /*
     * The `system' library function allows us to run a shell command.
     *
     * This function is usually forbidden in any code you want to submit
     * for CSE assignments.  You should almost never call this function.
     * We do so here so we can run the synthesiser.
     */
    int ret = system(sys_command);
    if (ret != 0) {
        printf("Sorry, making .wav files only works on CSE machines.\n");
    }
}

/*
 * Does `str' have the suffix `suffix'?
 *
 * has_suffix("music.wav", ".wav")    returns true.
 * has_suffix("music.wav", ".beats")  returns false.
 *
 * We use `strncmp', a length-bounded version of `strcmp', to check only
 * the length of the suffix provided.
 */
static int has_suffix (const char *str, const char *suffix) {
    if (str == NULL || suffix == NULL) {
        return 0;
    }

    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix > lenstr)
        return 0;

    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}
