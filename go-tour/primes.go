package main

import (
	"fmt"
	"math"
)

func is_prime(x int) bool {
	if x < 2 {
		return false;
	} else if x == 2 {
		return true;
	} else if x % 2 == 0 {
		return false;
	}
	s := int(math.Sqrt(float64(x)))
	for d := 3; d <= s; d++ {
		if x % d == 0 {
			return false
		}
	}
	return true;
}

func main() {
	const low int = -10
	const high int = 2000000;
	for x := low ; x <= high ; x++ {
		if is_prime(x) {
			fmt.Println(x)
		}
	}
}

