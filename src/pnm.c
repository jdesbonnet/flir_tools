//
// Functions to read PGM/PNM header
//
// From  https://stackoverflow.com/questions/52805454/read-ppm-header-information-and-output-to-console-and-output-file-in-c 
// 

#include <stdio.h>

static int  decimal_digit(const int c)
{
    switch (c) {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    default: return -1;
    }
}

int  pnm_magic(FILE *in)
{
    int  c;

    if (!in || ferror(in))
        return -2;  /* Invalid file handle. */

    c = getc(in);
    if (c != 'P')
        return -1;  /* Not a NetPBM file. */

    switch (getc(in)) {
    case '1': return 1;  /* Ascii PBM */
    case '2': return 2;  /* Ascii PGM */
    case '3': return 3;  /* Ascii PPM */
    case '4': return 4;  /* Binary PBM */
    case '5': return 5;  /* Binary PGM */
    case '6': return 6;  /* Binary PPM */
    /* case '7': return 7; for Portable Arbitrary map file */
    default: return -1;  /* Unknown format */ 
    }
}

int  pnm_endheader(FILE *in)
{
    int  c;

    if (!in || ferror(in))
        return -1;  /* Invalid file handle. */

    c = getc(in);

    /* Whitespace? */
    if (c == '\t' || c == '\n' || c == '\v' ||
        c == '\f' || c == '\r' || c == ' ')
        return 0;

    /* Nope, error. Don't consume the bad character. */
    if (c != EOF)
        ungetc(c, in);

    return -1;
}

int  pnm_value(FILE *in)
{
    unsigned int  val, old;
    int           c, digit;

    if (!in || ferror(in))
        return -1;  /* Invalid file handle. */

    /* Skip leading ASCII whitespace and comments. */
    c = getc(in);
    while (c == '\t' || c == '\n' || c == '\v' ||
           c == '\f' || c == '\r' || c == ' ' || c == '#')
        if (c == '#') {
            /* Skip the rest of the comment */
            while (c != EOF && c != '\n' && c != '\r')
                c = getc(in);
        } else
            c = getc(in);

    /* Parse initial decimal digit of value. */
    val = decimal_digit(c);
    if (val < 0)
        return -2; /* Invalid input. */

    while (1) {
        c = getc(in);

        /* Delimiter? End of input? */
        if (c == '\t' || c == '\n' || c == '\v' ||
            c == '\f' || c == '\r' || c == ' ' || c == '#') {
            /* Do not consume the character following the value. */
            ungetc(c, in);
            return val;
        } else
        if (c == EOF)
            return val;

        /* Is it a decimal digit? */
        digit = decimal_digit(c);
        if (digit < 0)
            return -2; /* Invalid input. */

        /* Convert, checking for overflow. */
        old = val;
        val = (val * 10) + digit;
        if (val / 10 != old)
            return -3; /* Overflow. */
    }
}


static float p5_gray(FILE *in, int maxval)
{
    if (maxval >= 256 && maxval < 65536) {
        int  hi, lo;
        hi = fgetc(in);
        lo = fgetc(in);
        if (lo == EOF)
            return -1.0f;
        return (float)(hi*256 + lo) / (float)maxval;
    } else
    if (maxval >= 1 && maxval < 256) {
        int  val;
        val = fgetc(in);
        if (val == EOF)
            return -1.0f;
        return (float)val / (float)maxval;
    } else
        return -2.0f;
}

static int p6_rgb(FILE *in, int maxval, float *red, float *green, float *blue)
{
    const float    max = (float)maxval;
    unsigned char  buf[6];

    if (maxval >= 256 && maxval < 65536) {
        if (fread(buf, 6, 1, in) != 1)
            return -1; /* Error! */
        if (red)
            *red = (float)(buf[0]*256 + buf[1]) / max;
        if (green)
            *green = (float)(buf[2]*256 + buf[1]) / max;
        if (blue)
            *blue = (float)(buf[4]*256 + buf[5]) / max;
        return 0;
    } else
    if (maxval >= 1 && maxval < 256) {
        if (fread(buf, 3, 1, in) != 1)
            return -1; /* Error! */
        if (red)
            *red = (float)buf[0] / max;
        if (green)
            *green = (float)buf[1] / max;
        if (blue)
            *blue = (float)buf[2] / max;
        return 0;
    } else
        return -2; /* Invalid maxval */
}




