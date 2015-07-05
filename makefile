.SUFFIXIES: .o .exe


# MAKE VARIABLES
WARN	    = -W -Wall -Wformat -Wnested-externs -Wparentheses -Wcast-align -Wno-unused 
COPT	    = -O3 -finline -finline-functions -foptimize-sibling-calls -fforce-addr -fthread-jumps -fexpensive-optimizations -freorder-blocks
DEFINES	    = -DMINGW32
CFLAGS	    = $(WARN) $(COPT) $(DEFINES)
LDFLAGS	    = -s -O3 -gc-sections -mwindows



EXECUTABLES = vimrun.exe

# TARGETS
all:	$(EXECUTABLES)

clean:
	rm *.o *.exe

%.o: %.c
	gcc -c $(CFLAGS) $<

%.exe: %.o
	gcc $(LDFLAGS) $^ -o $@
