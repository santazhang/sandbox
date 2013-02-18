all: check-addrbook kill-mouse-accel

check-addrbook: check-addrbook.m
	clang -framework Foundation -framework AddressBook check-addrbook.m -o check-addrbook

kill-mouse-accel: kill-mouse-accel.m
	clang -framework Foundation -framework IOKit kill-mouse-accel.m -o kill-mouse-accel

clean:
	rm -f check-addrbook kill-mouse-accel
