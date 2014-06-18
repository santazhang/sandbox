package main

import (
	"fmt"
	"math"
)

func newton_sqrt(x float64) float64 {
	s := x / 2  // init value
	const loops = 100;
	const min_diff = 1e-8;
	round := 1
	for ; round <= loops ; round++ {
		last := s
		s = s - (s * s - x) / 2 / s
		if math.Abs(last - s) < min_diff {
			break
		}
	}
	fmt.Println("round", round, "actual:", s, "expect", math.Sqrt(x), "error", math.Abs(s - math.Sqrt(x)))
	return s
}

func main() {
	newton_sqrt(2)
	newton_sqrt(3)
	newton_sqrt(5)
	for i := 1 ; i < 10000; i++ {
		newton_sqrt(float64(i))
	}
}
