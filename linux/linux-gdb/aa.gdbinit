define wprint
        set $fd = open("/tmp/dywt.gdb.wprint", 577)
        set $x = write($fd, $arg0, wcslen($arg0) * sizeof(wchar_t))
        set $y = close($fd)
        shell dywt.gdb.wprint
end

document wprint
        print the content of unicode text variable (wchar_t*)
        for example: wprint unicode_text_var_name
end

