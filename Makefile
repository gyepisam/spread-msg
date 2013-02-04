.PHONY: clean all

all : test_recv_msg #print_events 

LDFLAGS += -lspread -ldl
CFLAGS += -Wall -g3

print_events : utils.o spread_events.o message.o spread_error.o print_events_main.o
	$(CC) -o $@  $(CFLAGS) $^ $(LDFLAGS)

test_recv_msg : utils.o spread_msg.o test_recv_msg.o print_msg.o spread_error.o
	$(CC) -o $@  $(CFLAGS) $^ $(LDFLAGS)

# Here, we want to grab all the error codes mentioned in the spread man
# pages, along with their descriptions, and generate, eventually, 
# a c file to print out an error handling code that prints out
# the specific error before aborting.
# all the machinery defines explicit and static dependencies
# to make it work

# unfortunately, we must tell make about the specific dependencies
# so the file gets built if anything changes in the chain.
spread_error.o : $(addprefix spread_error.,c h txt xml gsl pl)

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
