EXAMPLES = ex_ati9.exe ex_datamode.exe ex_edm_at.exe ex_edm_wifi.exe
OBJS = ucxh_mode.o ucxh_poll.o ucxh_wait.o ucxh_parser.o ucxh_wifi.o ucxh_edm_parser.o ucxh_send.o ucxh_edm.o ucxh_urc.o
INCLUDES =-I include -I examples

EXOBJS=$(EXAMPLES:.exe=.o)
LDOPTS=-mconsole
CGLAGS=
#VPATH = src:src/windows:examples

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)

.PHONY: all

all: $(EXAMPLES)

# link
#$(EXAMPLES): $(OBJS) ucxh_serial_win.o
#	gcc $(OBJS) $($*.exe=.o) -o $*.exe

%.exe: $(OBJS) ucxh_serial_win.o
	gcc $(LDOPTS) $(OBJS) ucxh_serial_win.o  -o $*.exe 


# compile
$(OBJS): src/$($*.o=.c)
	gcc -c $(CFLAGS) $(INCLUDES) src/$*.c -o $*.o

%.o: src/windows/ucxh_serial_win.c 
	gcc -c $(CFLAGS) $(INCLUDES) src/windows/$*.c -o $*.o

%.o: examples/%.c
	gcc -c $(CFLAGS) $(INCLUDES) examples/$*.c -o $*.o

# Make dependencies

ucxh_serial_win.d: src/windows/%.c $($*.d=.o)
	gcc -MM $(CFLAGS) $(INCLUDES) src/windows/$*.c > $*.d

%.d: examples/%.c $($*.d=.o)
	gcc -MM $(CFLAGS) $(INCLUDES) examples/$*.c > $*.d

%.d: $($*.d=.o)
	gcc -MM $(CFLAGS) $(INCLUDES) src/$*.c > $*.d

# remove compilation products
clean:
	rm -f *.exe *.o *.d
