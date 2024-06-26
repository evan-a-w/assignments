/* make_wav.h
 * Fri Jun 18 17:06:02 PDT 2010 Kevin Karplus
 */

#ifndef MAKE_WAV_H
#define MAKE_WAV_H

/*
 * Open a file named `filename', write signed 16-bit little-endian
 * values as a monoaural WAV file at the specified sampling rate, and
 * close the file.
 */
void write_wav(
	char *filename, unsigned long num_samples, short int *data, int s_rate);

#endif
