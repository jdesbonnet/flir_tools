#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/**
 * Convert raw radiometric data from FLIR Ex/A320 thermal camera. Input is 
 * expected in stdin and in 16 bit PGM format. The tool also requires
 * radiometric constants R1, R2, B, O, F to be supplied.
 *
 *
 * Reference: http://u88.n24.queensu.ca/exiftool/forum/index.php/topic,4898.msg23944.html#msg23944
 *
 *
 * TODO: fix PGM header parsing
 *
 * Joe Desbonnet 2020-09-24.
 */


#define UNIT_C 1
#define UNIT_K 2

#define FORMAT_PGM 2
#define FORMAT_PGM_ASCII 5

/**
 * Help on how to use this utility.
 */ 
static void usage (char *cmd) 
{
	printf ("%s [options] <spi-device>\n", cmd);
	printf ("Options:\n");
	printf ("  -d <debug_level>: debug level: 0=no debug (default), 9=most verbose\n");
	printf ("  -C <key>=<value>: set constant, key can be one of R1,R2,B,O,F. Example -C R2=123\n");
	printf ("  -f <format>: output format, one of pgm or pgm_ascii. Default pgm. \n");
	printf ("  -u <unit>: output unit, one of C (Celsius) K (Kelvin). Default K.\n");
	printf ("  -m <n>: multiplier. Output is value (16 bit unsigned int) is value * multiplier.\n");
	printf ("          Example: with -u C -m 100 31.4C would be represented by value 3140\n");
}

static void version () 
{
}

int main (int argc, char **argv) {

	uint16_t v;
	uint32_t adc;


	// output unit
	int unit = UNIT_C;

	int format = FORMAT_PGM;


	// These constants come from the image EXIF metadata
	// eg exiftool -Pl* flir_image.jpg
	// Todo: allow to specify from command line

	// From Flir E4
	// double R1 = 14226.111;
	// double R2 = 0.027153991;
	// double O  = -7518;
	// double B  = 1387.2;
	// double F  = 2.5;

	// From Flir A320
        double R1 = 14809.436;
	double R2 = 0.010635848;
	double B = 1379.6;
	double O = -5846;
	double F = 1;


	double T,S,C;
	double multiplier = 100;
	int debug_level = 1;


	int fd;

	// Parse command line arguments. See usage() for details.
	int c;
	while ((c = getopt(argc, argv, "b:C:d:f:g:him:o:qs:tu:v")) != -1) {
		switch(c) {

			case 'C': {
				// constants in form var=val, eg R2=123.45. Locate the '=' character
				// and replace with string terminator (0). String to right is the
				// value which needs to be parsed as double. The string from optarg
				// to where the equal was is the key.
				char *s;
				s = strchr(optarg,'=');
				*(s++) = 0; // replace '=' with terminator, val is to the right of where '=' was
				double val = atof (s);
				char* key = optarg;
				//fprintf (stderr,"%s = %f\n", key, val);
				if (strcmp(key,"R1")==0) {
					R1 = val;
				} else if (strcmp(key,"R2")==0) {
					R2 = val;
				} else if (strcmp(key,"B")==0) {
					B = val;
				} else if (strcmp(key,"O")==0) {
					O = val;
				} else if (strcmp(key,"F")==0) {
					F = val;
				} else {
					fprintf (stderr, "unrecognized constant %s\n",key);
				}
				break;
				}
			case 'd':
				debug_level = atoi (optarg);
				break;

			case 'f':
				if (strcmp(optarg,"pgm")==0) {
					format = FORMAT_PGM;
				} else if (strcmp(optarg,"pgm_ascii")==0) {
					format = FORMAT_PGM_ASCII;
				} else {
					fprintf (stderr, "unrecognized format %s\n",optarg);
				}
				break;


			case 'h':
				version();
				usage(argv[0]);
				exit(EXIT_SUCCESS);


			case 'm':
				multiplier = atof (optarg);
				break;


			case 'u':
				if (strcmp(optarg,"C")==0) {
					unit = UNIT_C;
				} else if (strcmp(optarg,"K")==0) {
					unit = UNIT_K;
				}
				break;

			case 'v':
				version();
				exit(EXIT_SUCCESS);
			case '?':	// case when a command line switch argument is missing
				if (optopt == 'd') {
					fprintf (stderr,"ERROR: debug level 0 .. 9 must be specified with -d\n");
					exit(-1);
				}
				break;
		}
	}

	fprintf (stderr,"unit=%d multiplier=%f\n", unit, multiplier);


	// Read PGM header of raw radiometric file
	int pgm_width, pgm_height, pgm_maxval;


	// TODO problems with header parsing
	// hardcode for the moment
	pgm_width=320; pgm_height=240; pgm_maxval=65535;

/*
	while(getc(stdin) != '\n'); // skip file magic number ('P5')

	// ignore comment lines
	while (getc(stdin) == '#') {
		while (getc(stdin) != '\n');          
	}
	fseek(stdin, -1, SEEK_CUR);
	fscanf(stdin,"%d", &pgm_width);
	fscanf(stdin,"%d", &pgm_height);
	fscanf(stdin,"%d", &pgm_maxval);
*/

	// skip header
	int j = 0;
	while (j < 3) {
		if (getc(stdin)=='n') {
			j++;
		}
	}


	// Output file header
	if (format == FORMAT_PGM) {
		// binary PGM
		fprintf (stdout,"P5 %d %d\n%d\n", pgm_width, pgm_height, pgm_maxval);
	} else if (format == FORMAT_PGM_ASCII) {
		// ascii PGM
		fprintf (stdout,"P2 %d %d\n%d", pgm_width, pgm_height, pgm_maxval);
	}



	int i = 0;
	int voint;
	double vo;

	while (!feof(stdin)) {

		fread (&v, sizeof v, 1, stdin);

		// convert from intel/ARM little endian to big endian
		// TODO: make this code architecture neutral (assuming 
		// little endian arch)
		adc = ((v&0xff)<<8)  | (v>>8);


		S = (double)adc;

		// Temperature in K
		// Q: what about F?
		//T = B / log(R1/(R2*(S+O))+1);
		T = B / log(R1/(R2*(S+O))+F);

		//fprintf (stderr, "T=%f\n",T);

		switch (unit) {

			case UNIT_K:
			vo = T;
			break;

			case UNIT_C:
			vo = T - 273.15;
			break;

		}

		vo *= multiplier;

		// Limit to 16 bit unsigned integer
		if (vo > 65535) {
			vo = 65535;
		} else if (vo < 0) {
			vo = 0;
		}

		voint = (int)vo;

		// NetPBM format uses bigendian ordering
		// Ref https://en.wikipedia.org/wiki/Netpbm
		if (format == FORMAT_PGM_ASCII) {
			// LF at end of every line
			if (i%320==0) {
				fprintf(stdout,"\n");
			}
			fprintf (stdout, "%d ", voint);
		} else {
			// 16 bits big endian order
			fputc ((voint>>8) & 0xff, stdout);
			fputc (voint & 0xff, stdout);
		}
		i++;

	}
}
