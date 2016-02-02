.PHONE: all clean

all: is_variable_name

clean:
	rm -f is_variable_name

is_variable_name: is_variable_name.c

is_variable_name.c: is_variable_name.rl
	ragel $^
