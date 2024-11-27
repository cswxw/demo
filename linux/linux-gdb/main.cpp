
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <locale.h>

int main()
{
        setlocale(LC_ALL, "");
        const char* file_name = "/tmp/dywt.gdb.wprint";
        FILE* f = fopen(file_name, "rb");
        if(f == 0)
        {
                printf("Open '%s' error! ", file_name);
                return -1;
        }

        fseek(f, 0, SEEK_END);
        long file_size = ftell(f) + 1;
        fseek(f, 0, SEEK_SET);

        wchar_t* buf = (wchar_t*)malloc(file_size + sizeof(wchar_t));
        int r = fread(buf, 1, file_size, f);
        buf[file_size / sizeof(wchar_t)] = 0;
        printf("%ls\n", buf);
        free(buf);
}

