set -e

if [[ -z "${SSLC}" ]]; then
  echo "FAIL: No compiler env variable"
  exit 1
fi

echo "=== Running tests using $SSLC ==="


$SSLC --help

echo "=== All tests passed ==="