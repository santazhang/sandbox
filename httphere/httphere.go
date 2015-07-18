package main

import (
	"flag"
	"fmt"
	"log"
	"net/http"
	"path/filepath"
)

var (
	addr = flag.String("addr", "0.0.0.0", "Listening address")
	port = flag.Int("port", 8080, "Listening port")
)

func main() {
	flag.Parse()
	listen := fmt.Sprintf("%s:%d", *addr, *port)
	root, err := filepath.Abs(flag.Arg(0))
	if err != nil {
		log.Fatal(err)
	}
	handler := http.FileServer(http.Dir(root))
	log.Printf("Serving %s on %s...", root, listen)
	log.Fatal(http.ListenAndServe(listen, handler))
}
