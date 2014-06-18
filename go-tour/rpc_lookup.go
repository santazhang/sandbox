package main

import (
	// rename package
	f "fmt"

	// ok to import again
	"fmt"
)

// like C++ typedef
type uhaha int;

type RpcRegistry struct {
	code int
	name string
}

func array_demo() {
	var arr [3]string;
	arr[0] = "first"
	arr[1] = "second"
	arr[2] = "third"
	fmt.Println(arr)
}

func main() {
	var func1 RpcRegistry = RpcRegistry {3, "d"};
	func1.code = 1987
	func1_ptr := &func1
	func1_ptr.name = "aloha"
	f.Println(func1)
	f.Println(func1_ptr)
	// another way to init struct
	f.Println(RpcRegistry{name: "no_name", code: 7})
	// partial init is ok
	f.Println(RpcRegistry{name: "dummy_func"})

	// and this is now we `new` an object
	newed_func := new(RpcRegistry)
	newed_func.code = 99
	newed_func.name = "newed"
	fmt.Println(newed_func)

	var u uhaha = 23;
	fmt.Println(u)

	array_demo()
}
