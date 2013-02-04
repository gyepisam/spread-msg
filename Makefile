.PHONY: clean all

all : test_recv_msg

LDFLAGS += -lspread -ldl
CFLAGS += -Wall -g3

test_recv_msg : utils.o spread_msg.o print_msg.o spread_error.o test_recv_msg.o 
	$(CC) -o $@  $(CFLAGS) $^ $(LDFLAGS)

# Here, we want to grab all the error codes mentioned in the spread man
# pages, along with their descriptions, and generate, eventually, 
# a c file to print out an error handling code that prints out
# the specific error and an explanation when a spread error occurs.
# all the machinery defines explicit and static dependencies to make it work

generated = $(addprefix spread_error.,c txt xml)

# force make to keep the files around so git doesn't
# miss them and complain 
.PRECIOUS : $(addprefix spread_error.,c txt xml)

# extract error codes and descriptions from man pages
%.txt :
	sh extract_$* > $@

# convert extracted text to xml
%.xml : %.pl %.txt
	perl $^  > $@

# generate c file from gsl and xml
%.c : %.xml %.gsl 
	gsl -q -script:$(*).gsl $< > $@

clean:
	$(RM) *~ *.o

realclean: clean
		$(RM) $(generated) 
