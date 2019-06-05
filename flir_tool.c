#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

/**
 * Convert raw radiometric data from FLIR E4/E8 thermal camera
 * into 16 bit grey where value = temperature_C*10;
 * Get raw radiomatric data like so:
 * exiftool $TIR_IMG -RawThermalImage -b | convert - -interlace none -depth 16 r:-  > radiometric.raw
 *
 * Reference: http://u88.n24.queensu.ca/exiftool/forum/index.php/topic,4898.msg23944.html#msg23944
 *
 * Dependencies: exiftools, ImageMagick
 *
 * Joe Desbonnet 4 May 2019.
 */


/**
 * Help on how to use this utility.
 */ 
static void usage (char *cmd) 
{
	printf ("%s [options] <spi-device>\n", cmd);
	printf ("Options:\n");
	printf ("  -d <debug_level>: debug level: 0=no debug (default), 9=most verbose\n");
}

static void version () 
{
}

int main (int argc, char **argv) {

	uint16_t v;
	uint32_t adc;


	// These constants come from the image EXIF metadata
	// eg exiftool -Pl* flir_image.jpg
	const double R1 = 14226.111;
	const double R2 = 0.027153991;
	const double O  = -7518;
	const double B  = 1387.2;
	const double F  = 2.5;


	double T,S,C;
	double multiplier = 1;
	int debug_level = 1;


	int fd;

	// Parse command line arguments. See usage() for details.
	int c;
	while ((c = getopt(argc, argv, "b:d:f:g:hio:qs:tv")) != -1) {
		switch(c) {


			case 'd':
				debug_level = atoi (optarg);
				break;

			case 'h':
				version();
				usage(argv[0]);
				exit(EXIT_SUCCESS);


			case 'm':
				multiplier = atof (optarg);
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





	fprintf (stdout, "P5 320 240 65535\n");

	while (!feof(stdin)) {

		fread (&v, sizeof v, 1, stdin);

		adc = ((v&0xff)<<8)  | (v>>8);

		S = (double)adc;


		// Temperature in K
		T = B / log(R1/(R2*(S+O))+1);

		// Temperature in Celsius
		C = T - 273.15;

		//fprintf(stderr,"%f ", T);
		//fputc ((v&0xff), stdout);
		//fputc (v>>8, stdout);

		v = (uint16_t)(C*100);
		fputc (v>>8,stdout);
		fputc (v&0xff,stdout);

	}
}
