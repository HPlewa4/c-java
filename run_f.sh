#!/bin/bash
cd frontend
set -e

mkdir -p out

echo "Compiling main class..."
javac -d out MLAppUI.java

echo "Compiling panels..."
javac -d out -cp out src/ui/panels/*.java

echo "Compiling utils..."
javac -d out -cp out src/ui/utils/*.java

sleep 1

echo "Running MLAppUI..."
java -cp out MLAppUI
