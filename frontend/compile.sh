#!/bin/bash
# Compile all Java files with proper package structure

echo "======================================"
echo "  Java Frontend Compile Script"
echo "======================================"
echo ""

# Try to find homebrew-installed JDK first
if [ -d "/opt/homebrew/opt/openjdk@17" ]; then
    export JAVA_HOME="/opt/homebrew/opt/openjdk@17"
    export PATH="$JAVA_HOME/bin:$PATH"
elif [ -d "/usr/local/opt/openjdk@17" ]; then
    export JAVA_HOME="/usr/local/opt/openjdk@17"
    export PATH="$JAVA_HOME/bin:$PATH"
elif [ -d "/opt/homebrew/opt/openjdk" ]; then
    export JAVA_HOME="/opt/homebrew/opt/openjdk"
    export PATH="$JAVA_HOME/bin:$PATH"
elif [ -d "/usr/local/opt/openjdk" ]; then
    export JAVA_HOME="/usr/local/opt/openjdk"
    export PATH="$JAVA_HOME/bin:$PATH"
fi

# Check for Java compiler
if ! command -v javac &> /dev/null; then
    echo "❌ ERROR: Java JDK is not installed!"
    echo ""
    echo "To install Java JDK:"
    echo "  macOS:   brew install openjdk@17"
    echo "           OR download from https://adoptium.net/"
    echo ""
    echo "  Ubuntu:  sudo apt-get install openjdk-17-jdk"
    echo ""
    echo "  Windows: Download from https://adoptium.net/"
    echo ""
    echo "After installing, you may need to set JAVA_HOME:"
    echo "  export JAVA_HOME=\$(/usr/libexec/java_home)  # macOS"
    echo ""
    exit 1
fi

# Check Java version
JAVA_VERSION=$(javac -version 2>&1 | awk '{print $2}' | cut -d'.' -f1)
if [ -n "$JAVA_VERSION" ] && [ "$JAVA_VERSION" -lt 11 ] 2>/dev/null; then
    echo "⚠️  WARNING: Java version $JAVA_VERSION detected"
    echo "   Java 11+ recommended for this project"
    echo ""
fi

echo "Java compiler found: $(javac -version 2>&1)"
echo "Compiling Java project..."
echo ""

# Create bin directory
mkdir -p bin

cd src

# Compile all files
javac -d ../bin com/classifier/*/*.java

if [ $? -eq 0 ]; then
    echo ""
    echo "======================================"
    echo "  ✅ Compilation Successful!"
    echo "======================================"
    echo ""
    echo "Compiled classes are in: frontend/bin/"
    echo ""
    echo "To run tests:  ./run_tests.sh"
    echo "To run GUI:    ./run_ui.sh"
    echo ""
else
    echo ""
    echo "❌ Compilation failed!"
    echo "   Check compiler errors above"
    exit 1
fi
