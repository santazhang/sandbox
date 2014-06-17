package main

import "fmt"

func add3(x, y, z int) int {
	return x + y + z;
}

func named_results(dont_care string) (x, y string) {
	var (
		aha string = "AHA*"
		unused_int int = 9
	)
	inline_var_def := "implicit type inference"
	x = aha + inline_var_def + string(unused_int)
	x = "this is x"
	y = "and this is y"
	return
}

func usage_of_for() {
	sum := 0
	for i := 1987; sum < 100000; i++ {
		sum += i
	}
	fmt.Printf("sum = %d\n", sum)


	// for without init and update, pretty much a while-loop
	sum = 1
	for sum < 1000 {
		sum *= 2
	}
	fmt.Printf("new sum = %d\n", sum)
}

func main() {
	const world = "*World*"
	fmt.Printf("hello, %s\n", world)
	fmt.Println("This is a totally new line")
	fmt.Println("And doing 1987 + 10 + 01 ->", add3(1987, 10, 01))
	fmt.Println(named_results(""))
	usage_of_for()
}
