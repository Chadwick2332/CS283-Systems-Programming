CC=gcc
CFLAGS= -lpthread
SRC=./src

test: replacer.o test_area.zip
	sh bin/quick_run.sh

filenames: replacer.o
	./replacer.o -d ./ -p $(PATTERN) -r $(REPLACE) -m 0 -f $(FNAMES)
	
sub_filenames: replacer.o
	./replacer.o -d $(DIR) -p $(PATTERN) -r $(REPLACE) -m 0 -f $(FNAMES)

filecontents: replacer.o
	./replacer.o -d ./ -p $(PATTER) -r $(BEFORE) -m 1 -f $(FNAMES)

sub_filecontents: replacer.o
	./replacer.o -d $(DIR) -p $(PATTER) -r $(BEFORE) -m 1 -f $(FNAMES)	

replacer.o: $(SRC)/replacer.c $(SRC)/stringly.c
	$(CC) $^ -o $@ $(CFLAGS)
	
clean:
	rm replacer.o