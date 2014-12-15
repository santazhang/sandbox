package main

import _ "net/http/pprof"
import "log"
import "fmt"
import "net/http"

type Hello struct{}

func (h Hello) ServeHTTP(
	w http.ResponseWriter,
	r *http.Request) {
	fmt.Fprint(w, "Hello!")
}

func main() {
	go func() {
		log.Println(http.ListenAndServe("localhost:6060", nil))
	}()
	var h Hello
	http.ListenAndServe("0.0.0.0:4567", h)
}
