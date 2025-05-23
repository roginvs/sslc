# set -e

#
# This file runs sslc on test files in this folder
# and returns error code if git detects some changes
#
#


if [[ -z "${SSLC}" ]]; then
  echo "FAIL: No compiler env variable"
  exit 1
fi

sed -i 's/\r$//' "include/*.h" # To suppess warnings

ERRCODES=""

# echo "Debug: arg=$SSLC"
SSLC=$(realpath "$SSLC")
# echo "Debug: fullpath=$SSLC"

function run_tests() {
  DIR=$1
  OPTIMIZER_OPTS=$2
  cd $DIR
  for f in *.ssl; do
    echo "> $DIR/$f"
    sed -i 's/\r$//' "$f" # To suppess warnings
    $SSLC $OPTIMIZER_OPTS -I../include $f -o $(basename -s .ssl $f).int > $(basename -s .ssl $f).stdout
    RETURN_CODE=$?
    sed -i 's/\r$//' $(basename -s .ssl $f).stdout
    if [ $RETURN_CODE -eq 0 ]; then
      true # all ok
      # Debugging
      echo '===stdout=='
      cat $(basename -s .ssl $f).stdout      
      echo '==========='
    else
      ERRCODES="$ERRCODES $DIR/$f=$RETURN_CODE"
      echo "Return code is $RETURN_CODE for $DIR/$f"
      cat $(basename -s .ssl $f).stdout
    fi
  done
  cd ..
}

echo "=== Running tests using $SSLC ==="

run_tests with_optimizer "-q -p -l -O2 -d -s -n"


if [[ -z "${ERRCODES}" ]]; then
  true # No errors
else
  # echo "Error codes: $ERRCODES"
  exit 1
fi

echo "=== Checking for changes ==="
DIFFS=$(git -c core.autocrlf=false status --porcelain --untracked-files .)
if [[ -z "${DIFFS}" ]]; then
  true # No changes, no new files
else
  echo "=== GIT STATUS ==="
  git -c core.autocrlf=false status
  echo "=== GIT DIFF ==="
  git -c core.autocrlf=false diff .
  echo "=== exitting with error code due to changes ==="
  exit 1
fi

echo "=== All tests passed ==="