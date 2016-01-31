#!/bin/bash

rm -rf "$(dirname $0)/Crypto"
cd "$(dirname $0)/pycrypto-2.6.1"
python setup.py build
mv build/lib.*/Crypto ..
cd ..

echo ----

python -c "import spur"
