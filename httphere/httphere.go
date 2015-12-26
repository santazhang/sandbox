package main

import (
	"flag"
	"fmt"
	"log"
	"net/http"
	"os"
	"path/filepath"
	"regexp"
)

var (
	addr = flag.String("addr", "0.0.0.0", "Listening address")
	port = flag.Int("port", 8080, "Listening port")
)

type filteredFile struct {
	f            http.File
	ignoreRegexp *regexp.Regexp
}

type filteredFileSystem struct {
	sourceFs     http.FileSystem
	ignoreRegexp *regexp.Regexp
}

func makeFilteredFile(f http.File, ignoreRegexp *regexp.Regexp) filteredFile {
	return filteredFile{
		f,
		ignoreRegexp,
	}
}

func (f filteredFile) Close() error {
	return f.f.Close()
}

func (f filteredFile) Read(p []byte) (n int, err error) {
	return f.f.Read(p)
}

func (f filteredFile) Readdir(count int) (fi []os.FileInfo, err error) {
	var rfi []os.FileInfo
	rfi, err = f.f.Readdir(count)
	if err != nil {
		return nil, err
	}
	for _, i := range rfi {
		if i.IsDir() || !f.ignoreRegexp.MatchString(i.Name()) {
			fi = append(fi, i)
		}
	}
	return fi, nil
}

func (f filteredFile) Seek(offset int64, whence int) (int64, error) {
	return f.f.Seek(offset, whence)
}

func (f filteredFile) Stat() (os.FileInfo, error) {
	return f.f.Stat()
}

func makeFilteredFileSystem(sourceFs http.FileSystem) filteredFileSystem {
	ignoreRegexp, _ := regexp.Compile("^\\.DS_Store$|^\\._\\.")
	return filteredFileSystem{
		sourceFs:     sourceFs,
		ignoreRegexp: ignoreRegexp,
	}
}

func (fs filteredFileSystem) Open(name string) (http.File, error) {
	f, err := fs.sourceFs.Open(name)
	return makeFilteredFile(f, fs.ignoreRegexp), err
}

func main() {
	flag.Parse()
	listen := fmt.Sprintf("%s:%d", *addr, *port)
	root, err := filepath.Abs(flag.Arg(0))
	if err != nil {
		log.Fatal(err)
	}
	handler := http.FileServer(makeFilteredFileSystem(http.Dir(root)))
	log.Printf("Serving %s on %s...", root, listen)
	log.Fatal(http.ListenAndServe(listen, handler))
}
