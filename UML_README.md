# UML Diagram - Image Classifier System

**Group Members:** [Add your names at the top of the UML diagram]

## Overview
This UML diagram represents the complete architecture of our Image Classifier system, which combines C++ backend with Java frontend.

## Requirements Met ✓

### UML Requirements (10% of grade)
- ✅ **10+ Classes**: We have 17 classes total
  - C++: NeuralNetwork, ActivationFunction, SigmoidActivation, ReLUActivation, TanhActivation, ActivationFactory, DataBuffer<T>, Matrix<T>
  - Java: ImageClassifierUI, ImageProcessor (interface), GrayscaleProcessor, ResizeProcessor, BrightnessProcessor, NetworkClient (abstract), ImageClassifierClient, Cache<K,V>, ClassificationResult, Prediction

- ✅ **Dependency Relationships**:
  - NeuralNetwork depends on ActivationType
  - ActivationFactory depends on ActivationFunction and ActivationType
  - ImageClassifierUI depends on ImageProcessor
  - Cache depends on ClassificationResult

- ✅ **Aggregation/Composition**:
  - **Composition** (◆): NeuralNetwork owns ActivationFunction, ImageClassifierUI owns Cache and ImageClassifierClient
  - **Aggregation** (◇): ImageClassifierUI uses ClassificationResult, ClassificationResult contains Predictions

- ✅ **Inheritance Relationships**:
  - C++: ActivationFunction → SigmoidActivation, ReLUActivation, TanhActivation
  - Java: NetworkClient → ImageClassifierClient
  - Java Interface: ImageProcessor implemented by GrayscaleProcessor, ResizeProcessor, BrightnessProcessor

- ✅ **4 Detailed Classes**: NeuralNetwork, DataBuffer<T>, Matrix<T>, ImageClassifierUI all have complete methods and variables

- ✅ **Access Modifiers**:
  - `+` (public): All public methods
  - `-` (private): All private fields
  - `#` (protected): Protected fields in NetworkClient

## How to View the UML Diagram

### Option 1: Online Viewer (Recommended)
1. Go to http://www.plantuml.com/plantuml/uml/
2. Copy the contents of `UML_Diagram.puml`
3. Paste into the text area
4. The diagram will render automatically

### Option 2: VS Code Extension
1. Install "PlantUML" extension in VS Code
2. Open `UML_Diagram.puml`
3. Press `Alt+D` to preview

### Option 3: Command Line
```bash
# Install PlantUML
brew install plantuml  # macOS
# or
sudo apt-get install plantuml  # Linux

# Generate PNG
plantuml UML_Diagram.puml

# This creates UML_Diagram.png
```

### Option 4: Export as PDF
1. Use online viewer to render
2. Take screenshot or export
3. Or use: `plantuml -tpdf UML_Diagram.puml`

## Class Descriptions

### C++ Backend Classes

#### 1. NeuralNetwork (Detailed ★)
Main neural network implementation with training and inference capabilities.
- Demonstrates: Composition, Polymorphism, Smart Pointers, Threading

#### 2. ActivationFunction (Abstract)
Abstract base class for activation functions.
- Demonstrates: Polymorphism, Virtual Methods, Inheritance

#### 3-5. Sigmoid/ReLU/Tanh Activation
Concrete implementations of activation functions.
- Demonstrates: Inheritance, Override

#### 6. ActivationFactory
Factory pattern for creating activation functions.
- Demonstrates: Factory Pattern, Smart Pointers

#### 7. DataBuffer<T> (Detailed ★)
Generic template class for buffered data storage.
- Demonstrates: Templates, Generic Programming, STL Algorithms

#### 8. Matrix<T> (Detailed ★)
Generic template class for matrix operations.
- Demonstrates: Templates, Operator Overloading, Generic Programming

### Java Frontend Classes

#### 9. ImageClassifierUI (Detailed ★)
Main GUI application for image classification.
- Demonstrates: Composition, Aggregation, Dependency Injection, Resource Management

#### 10. ImageProcessor (Interface)
Interface for image processing operations.
- Demonstrates: Interface, Polymorphism

#### 11-13. Image Processors
Concrete implementations (Grayscale, Resize, Brightness).
- Demonstrates: Interface Implementation, Polymorphism

#### 14. NetworkClient (Abstract)
Abstract base class for network operations.
- Demonstrates: Abstract Class, Template Method Pattern

#### 15. ImageClassifierClient
Concrete HTTP client implementation.
- Demonstrates: Inheritance, Override

#### 16. Cache<K,V>
Generic caching class.
- Demonstrates: Generics, Collections

#### 17. ClassificationResult
Data transfer object for results.
- Demonstrates: Encapsulation, Inner Classes

### Enums
- **ActivationType** (C++): enum class for activation types
- **ClassificationStatus** (Java): enum for UI states

## Relationships Explained

### Inheritance (is-a)
- `SigmoidActivation` **is an** `ActivationFunction`
- `ImageClassifierClient` **is a** `NetworkClient`
- Shows polymorphism and code reuse

### Composition (owns, strong)
- `NeuralNetwork` **owns** `ActivationFunction`
- `ImageClassifierUI` **owns** `Cache`
- Lifecycle dependency - owner controls lifetime

### Aggregation (has-a, weak)
- `ImageClassifierUI` **uses** `ClassificationResult`
- `ClassificationResult` **contains** `Prediction`
- Weaker relationship - can exist independently

### Dependency (uses)
- `ActivationFactory` **creates** `ActivationFunction`
- `ImageClassifierUI` **uses** `ImageProcessor`
- One class uses another temporarily

## Design Patterns Used

1. **Factory Pattern**: ActivationFactory creates concrete activation functions
2. **Template Method**: NetworkClient provides template for network operations
3. **Strategy Pattern**: ImageProcessor allows different processing strategies
4. **Singleton-like**: Static factory methods
5. **Resource Management**: RAII pattern in C++, cleanup in Java

## Notes for Presentation

- Point out the clear separation between C++ (backend/ML) and Java (frontend/UI)
- Highlight polymorphism in both languages
- Explain the use of generics/templates for reusability
- Show how the design supports extensibility (easy to add new activation functions or image processors)
- Demonstrate proper OOP principles: encapsulation, inheritance, polymorphism, abstraction

## File Location
- Main UML: `/Users/lukasz/Code/University/c-java/UML_Diagram.puml`
- This README: `/Users/lukasz/Code/University/c-java/UML_README.md`

---

**Submission Date**: November 30, 2024
**Project Type**: Neural Network Image Classifier (C++ + Java)
