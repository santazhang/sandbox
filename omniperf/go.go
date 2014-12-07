package main

import (
	"fmt"
	"time"
)

func main() {
	const loop_n = 1000 * 1000
	counter := 0
	start := time.Now()
	for counter < loop_n {
		counter++
	}
	nanosec := time.Since(start).Nanoseconds()
	qps := float64(counter) * 1000 * 1000 * 1000 / float64(nanosec);
	fmt.Printf("i++: %.0f / s\n", qps);
}
