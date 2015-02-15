#!/bin/bash

find . -iname \*.moon | xargs moonc

/Applications/love.app/Contents/MacOS/love .

