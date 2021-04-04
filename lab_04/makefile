CC = gcc

SRCPATH = ./src/
INCPATH = ./include/
OUTPATH = ./out/

CFLAGS = -I$(INCPATH) -Wall -Wpedantic -Wextra -Werror

SRCS = $(SRCPATH)io.c $(SRCPATH)main.c $(SRCPATH)approx.c 
OUTS = $(OUTPATH)io.o $(OUTPATH)main.o $(OUTPATH)approx.o 

.PHONY: run build clean new_test

run: min_square_approximation.exe
	./$<

new_test: gen_table.exe
	./$<

min_square_approximation.exe: $(OUTS)
	$(CC) -o $@ $^

gen_table.exe: $(OUTPATH)gen_table.o
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
