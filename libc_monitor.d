// example script which displays the count of libc calls
pid$target:libsystem_c.dylib:: {
    @calls[probefunc] = count();
}

END {
    printa(@calls);
}
