package main

import (
	"fmt"
	"net/http"
)

type Hello struct {}

func (h Hello) ServeHTTP (
	w http.ResponseWriter,
	r *http.Request) {
	fmt.Fprint(w, "hello!")
}

func main() {
	var h Hello
	http.ListenAndServe("0.0.0.0:4567", h)
}
