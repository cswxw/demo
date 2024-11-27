    #include <unistd.h>
    #include <stdlib.h>
    #include <stdio.h>
     
    int main(int argc, char *argv[])
    {
        int opt;
        char optstring[] = "a:b:c:d";
     
        while ((opt = getopt(argc, argv, optstring)) != -1)
        {
            printf("opt = %c\n", opt);
            printf("optarg = %s\n", optarg);
            printf("optind = %d\n", optind);
            printf("argv[optind - 1] = %s\n\n",  argv[optind - 1]);
        }
     
        return 0;
    }
