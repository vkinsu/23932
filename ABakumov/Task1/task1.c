#include <stdlib.h>
#include <stdio.h>
main(int argc, char *argv[])
{
     char options[ ] = "-:i:s:p:";  /* valid options */
     int c, invalid = 0, dflg = 0, fflg = 0, gflg = 0;
     char *f_ptr, *g_ptr;

     printf("argc equals %d\n", argc);
     while ((c = getopt(argc, argv, options)) != EOF) {
      switch (c) {
      case 'd':
          dflg++;
          break;
      case 'f':
          fflg++;
          f_ptr = optarg;
          break;
      case 'g':
          gflg++;
          g_ptr = optarg;
          break;
      case '?':
          printf("invalid option is %c\n", optopt);
          invalid++;
      }
     }
     printf("%lu\n", (unsigned long int)getuid());
     printf("dflg equals %d\n", dflg);
     if(fflg)
      printf("f_ptr points to %s\n", f_ptr);
     if(gflg)
      printf("g_ptr points to %s\n", g_ptr);
     printf("invalid equals %d\n", invalid);
     printf("optind equals %d\n", optind);
     if(optind < argc)
      printf("next parameter = %s\n", argv[optind]);
 }
