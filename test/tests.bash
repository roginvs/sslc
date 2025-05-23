# set -e

if [[ -z "${SSLC}" ]]; then
  echo "FAIL: No compiler env variable"
  exit 1
fi

ERRCODES=""

echo "Debug: arg=$SSLC"
SSLC=$(realpath "$SSLC")
echo "Debug: fullpath=$SSLC"

function run_tests() {
  DIR=$1
  OPTIMIZER_OPTS=$2
  cd $DIR
  for f in *.ssl; do
    echo "Debug: running test for $DIR/$1"
    $SSLC $OPTIMIZER_OPTS $f -o $(basename -s .ssl $f).int > $(basename -s .ssl $f).stdout
    RETURN_CODE=$?
    if [ $RETURN_CODE -eq 0 ]; then
      true # all ok
    else
      ERRCODES="$ERRCODES $DIR/$f=$RETURN_CODE"
    fi
  done
  cd ..
}

echo "=== Running tests using $SSLC ==="

run_tests with_optimizer "-q -p -l -O2 -d -s -n"


if [[ -z "${ERRCODES}" ]]; then
  true # No errors
else
  echo "Error codes: $ERRCODES"
  exit 1
fi

echo "=== All tests passed ==="