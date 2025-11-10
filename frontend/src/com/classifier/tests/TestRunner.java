package com.classifier.tests;

import com.classifier.model.*;
import com.classifier.network.*;
import com.classifier.processing.*;
import com.classifier.utils.*;

import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.List;

// Simple test framework for Java
public class TestRunner {
    private static int testsPassed = 0;
    private static int testsFailed = 0;
    private static List<String> failures = new ArrayList<>();

    public static void main(String[] args) {
        System.out.println("==================================");
        System.out.println("  Java Unit Tests");
        System.out.println("==================================\n");

        // Run all tests
        testClassificationStatus();
        testCache();
        testCacheWithStrings();
        testClassificationResult();
        testImageProcessorInterface();
        testGrayscaleProcessor();
        testResizeProcessor();
        testBrightnessProcessor();
        testNetworkClientAbstractClass();
        testImageClassifierClient();
        testEnumValues();
        testGenericCache();

        // Print summary
        System.out.println("\n==================================");
        System.out.println("  Test Summary");
        System.out.println("==================================");
        System.out.println("Tests Passed: " + testsPassed);
        System.out.println("Tests Failed: " + testsFailed);
        System.out.println("Total Tests:  " + (testsPassed + testsFailed));

        if (testsFailed > 0) {
            System.out.println("\nFailed tests:");
            for (String failure : failures) {
                System.out.println("  - " + failure);
            }
        }

        if (testsFailed == 0) {
            System.out.println("\n✓ All tests passed!");
            System.exit(0);
        } else {
            System.err.println("\n✗ Some tests failed!");
            System.exit(1);
        }
    }

    private static void assertTrue(String testName, boolean condition) {
        if (condition) {
            testsPassed++;
            System.out.println("  ✓ PASS: " + testName);
        } else {
            testsFailed++;
            failures.add(testName);
            System.err.println("  ✗ FAIL: " + testName);
        }
    }

    private static void assertEquals(String testName, Object expected, Object actual) {
        if (expected == null && actual == null) {
            testsPassed++;
            System.out.println("  ✓ PASS: " + testName);
        } else if (expected != null && expected.equals(actual)) {
            testsPassed++;
            System.out.println("  ✓ PASS: " + testName);
        } else {
            testsFailed++;
            failures.add(testName);
            System.err.println("  ✗ FAIL: " + testName + " (expected: " + expected + ", actual: " + actual + ")");
        }
    }

    private static void assertNotNull(String testName, Object obj) {
        if (obj != null) {
            testsPassed++;
            System.out.println("  ✓ PASS: " + testName);
        } else {
            testsFailed++;
            failures.add(testName);
            System.err.println("  ✗ FAIL: " + testName + " (object was null)");
        }
    }

    // Test implementations
    private static void testClassificationStatus() {
        System.out.println("\nTesting ClassificationStatus enum...");
        assertEquals("IDLE status message", "Ready to classify", ClassificationStatus.IDLE.getMessage());
        assertEquals("SUCCESS status message", "Classification successful", ClassificationStatus.SUCCESS.getMessage());
        assertTrue("Different enum values are not equal", ClassificationStatus.IDLE != ClassificationStatus.SUCCESS);
    }

    private static void testCache() {
        System.out.println("\nTesting Cache<K,V> generic class...");
        Cache<Integer, String> cache = new Cache<>(3);
        cache.put(1, "one");
        cache.put(2, "two");
        assertEquals("Cache size after 2 inserts", 2, cache.size());
        assertEquals("Get value from cache", "one", cache.get(1));
        assertTrue("Cache contains key", cache.contains(2));
        assertTrue("Cache doesn't contain key", !cache.contains(999));
    }

    private static void testCacheWithStrings() {
        System.out.println("\nTesting Cache with different types...");
        Cache<String, Double> cache = new Cache<>(10);
        cache.put("pi", 3.14159);
        cache.put("e", 2.71828);
        assertTrue("Cache contains pi", cache.contains("pi"));
        assertEquals("Get pi value", 3.14159, cache.get("pi"));
    }

    private static void testGenericCache() {
        System.out.println("\nTesting Cache overflow...");
        Cache<Integer, String> cache = new Cache<>(2);
        cache.put(1, "one");
        cache.put(2, "two");
        cache.put(3, "three");
        assertEquals("Cache size after overflow", 2, cache.size());
    }

    private static void testClassificationResult() {
        System.out.println("\nTesting ClassificationResult...");
        ClassificationResult result = new ClassificationResult();
        result.addPrediction("cat", 0.95, 0);
        result.addPrediction("dog", 0.03, 1);
        assertEquals("Number of predictions", 2, result.getPredictions().size());
        ClassificationResult.Prediction pred = result.getPredictions().get(0);
        assertEquals("First prediction class", "cat", pred.getClassName());
        assertEquals("First prediction confidence", 0.95, pred.getConfidence());
        assertEquals("First prediction id", 0, pred.getClassId());
    }

    private static void testImageProcessorInterface() {
        System.out.println("\nTesting ImageProcessor interface...");
        ImageProcessor processor = new GrayscaleProcessor();
        assertNotNull("Processor created", processor);
        assertEquals("Processor name", "Grayscale", processor.getProcessorName());
    }

    private static void testGrayscaleProcessor() {
        System.out.println("\nTesting GrayscaleProcessor...");
        GrayscaleProcessor processor = new GrayscaleProcessor();
        BufferedImage testImage = new BufferedImage(10, 10, BufferedImage.TYPE_INT_RGB);
        for (int y = 0; y < 10; y++) {
            for (int x = 0; x < 10; x++) {
                testImage.setRGB(x, y, 0xFF0000);
            }
        }
        BufferedImage result = processor.process(testImage);
        assertNotNull("Processed image not null", result);
        assertEquals("Image width preserved", 10, result.getWidth());
        assertEquals("Image height preserved", 10, result.getHeight());
    }

    private static void testResizeProcessor() {
        System.out.println("\nTesting ResizeProcessor...");
        ResizeProcessor processor = new ResizeProcessor(32, 32);
        BufferedImage testImage = new BufferedImage(100, 100, BufferedImage.TYPE_INT_RGB);
        BufferedImage result = processor.process(testImage);
        assertNotNull("Resized image not null", result);
        assertEquals("Resized width", 32, result.getWidth());
        assertEquals("Resized height", 32, result.getHeight());
    }

    private static void testBrightnessProcessor() {
        System.out.println("\nTesting BrightnessProcessor...");
        BrightnessProcessor processor = new BrightnessProcessor(1.5f);
        BufferedImage testImage = new BufferedImage(5, 5, BufferedImage.TYPE_INT_RGB);
        BufferedImage result = processor.process(testImage);
        assertNotNull("Brightness processed image not null", result);
        assertTrue("Brightness processor name contains factor", processor.getProcessorName().contains("1.5"));
    }

    private static void testNetworkClientAbstractClass() {
        System.out.println("\nTesting NetworkClient abstract class...");
        NetworkClient client = new ImageClassifierClient("http://localhost:8080", 5000);
        assertNotNull("NetworkClient created", client);
    }

    private static void testImageClassifierClient() {
        System.out.println("\nTesting ImageClassifierClient inheritance...");
        ImageClassifierClient client = new ImageClassifierClient("http://localhost:8080", 5000);
        assertNotNull("Client created", client);
        assertTrue("Client is instance of NetworkClient", client instanceof NetworkClient);
    }

    private static void testEnumValues() {
        System.out.println("\nTesting Enum values...");
        ClassificationStatus[] values = ClassificationStatus.values();
        assertTrue("Enum has multiple values", values.length >= 5);
        assertEquals("Enum valueOf works", ClassificationStatus.SUCCESS, ClassificationStatus.valueOf("SUCCESS"));
    }
}
