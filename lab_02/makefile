CC = gcc

SRCPATH = ./src/
INCPATH = ./include/
OUTPATH = ./out/

CFLAGS = -I$(INCPATH) -Wall -Wpedantic -Wextra -Werror

SRCS = $(SRCPATH)io.c $(SRCPATH)main.c $(SRCPATH)interp.c 
OUTS = $(OUTPATH)io.o $(OUTPATH)main.o $(OUTPATH)interp.o 

.PHONY: run build clean

run: mult_interpolation.exe
	./$<

mult_interpolation.exe: $(OUTS)
	$(CC) -o $@ $^

$(OUTPATH)%.o: $(SRCPATH)%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTPATH)%.d: $(SRCPATH)%.c | $(OUTPATH)
	echo -n $(OUTPATH) > $@
	$(CC) -I$(INCPATH) -MM $< >> $@

$(OUTPATH):
	mkdir -p $(OUTPATH)

include $(SRCS:$(SRCPATH)%.c=$(OUTPATH)%.d) 

clean:
	rm -rf $(OUTPATH)*.d
	rm -rf $(OUTPATH)*.o
	rm -rf *.exe
