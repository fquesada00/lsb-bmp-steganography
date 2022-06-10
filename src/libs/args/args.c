#include <args.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <steganography.h>
#include <stdio.h>
#include <stdlib.h>

extern char *optarg;

void parse_args(args_t *args, int argc, char *argv[])
{
  int index = 0;

  bool missing_embed = true, missing_in = true, missing_bitmap = true, missing_out = true, missing_steg = true;

  int option = getopt_long_only(argc, argv, "", long_options, &index);

  while (option != -1)
  {
    switch (option)
    {
    case EMBED:
      args->embed = true;
      missing_embed = false;
      break;
    case IN:
      strncpy(args->in, optarg, MAX_FILENAME_SIZE);
      missing_in = false;
      break;
    case BITMAP:
      strncpy(args->bitmap_file, optarg, MAX_FILENAME_SIZE);
      missing_bitmap = false;
      break;
    case OUT:
      strncpy(args->out, optarg, MAX_FILENAME_SIZE);
      missing_out = false;
      break;
    case STEGANOGRAPHY:
      missing_steg = false;
      if (strncmp(optarg, "LSB1", 4) == 0)
      {
        args->steganography_mode = LSB1;
      }
      else if (strncmp(optarg, "LSB4", 4) == 0)
      {
        args->steganography_mode = LSB4;
      }
      else if (strncmp(optarg, "LSBI", 4) == 0)
      {
        args->steganography_mode = LSBI;
      }
      else
      {
        fprintf(stderr, "Invalid steganography mode\n");
        exit(1);
      }
      break;
    default:
      break;
    }

    option = getopt_long_only(argc, argv, "", long_options, &index);
  }

  bool missing_args = false;

  if (missing_embed)
  {
    fprintf(stderr, "Missing embed argument\n");
    missing_args = true;
  }
  if (missing_in)
  {
    fprintf(stderr, "Missing in argument\n");
    missing_args = true;
  }
  if (missing_bitmap)
  {
    fprintf(stderr, "Missing bitmap argument\n");
    missing_args = true;
  }
  if (missing_out)
  {
    fprintf(stderr, "Missing out argument\n");
    missing_args = true;
  }
  if (missing_steg)
  {
    fprintf(stderr, "Missing steg argument\n");
    missing_args = true;
  }
  if (missing_args)
  {
    exit(1);
  }
}
