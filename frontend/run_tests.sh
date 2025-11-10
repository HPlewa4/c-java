#!/bin/bash
# Run Java unit tests

# Try to find homebrew-installed JDK if java is not in PATH
if ! command -v java &> /dev/null; then
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
fi

cd bin 2>/dev/null || cd src
echo "Running Java Unit Tests..."
java com.classifier.tests.TestRunner
