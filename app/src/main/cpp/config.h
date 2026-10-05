#ifndef CONFIG_H
#define CONFIG_H

#define HAVE_STDINT_H 1
#define HAVE_INTTYPES_H 1
#define FIXED_POINT 1
#define USE_KISS_FFT 1

/* Resolves legacy SWIG/NDK export definition symbols */
#ifndef EXPORT
#  define EXPORT __attribute__((visibility("default")))
#endif

#endif
