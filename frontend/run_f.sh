set -e

echo "=== Running Frontend Natively ==="
echo ""
JAVA_17_HOME=$(dirname $(dirname $(readlink -f $(which java))))


if [ -z "$JAVA_17_HOME" ]; then
    echo "Error: Java 17 not found. Please install it:"
    echo "  brew install openjdk@17"
    exit 1
fi

export JAVA_HOME="$JAVA_17_HOME"
export PATH="$JAVA_HOME/bin:$PATH"

echo "Using Java: $($JAVA_HOME/bin/java -version 2>&1 | head -1)"
echo ""

mkdir -p build/out

CP=".:lib/*:build/out"

echo "Compiling Java files..."
find . -name "*.java" -print0 | xargs -0 "$JAVA_HOME/bin/javac" -cp "$CP" -d build/out

echo "Starting ML App UI..."
echo "Backend should be running on http://localhost:8080"
echo ""

"$JAVA_HOME/bin/java" -cp "$CP" MLAppUI