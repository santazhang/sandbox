#!/bin/bash

rm -rf "$(dirname $0)/Crypto" "$(dirname $0)/tornado"

cd "$(dirname $0)/pycrypto-2.6.1"
python setup.py build
mv build/lib.*/Crypto ..
cd ..

cd "$(dirname $0)/tornado-4.3.0"
python setup.py build
mv build/lib.*/tornado ..
cd ..

echo ----

python -c "import spur; import tornado"
