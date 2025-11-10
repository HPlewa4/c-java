package com.classifier.model;

import java.util.List;
import java.util.ArrayList;

// Class to encapsulate classification results
public class ClassificationResult {
    private List<Prediction> predictions;
    private ClassificationStatus status;
    private long timestamp;

    public ClassificationResult() {
        this.predictions = new ArrayList<>();
        this.status = ClassificationStatus.IDLE;
        this.timestamp = System.currentTimeMillis();
    }

    public void addPrediction(String className, double confidence, int classId) {
        predictions.add(new Prediction(className, confidence, classId));
    }

    public List<Prediction> getPredictions() {
        return predictions;
    }

    public void setStatus(ClassificationStatus status) {
        this.status = status;
    }

    public ClassificationStatus getStatus() {
        return status;
    }

    public long getTimestamp() {
        return timestamp;
    }

    // Inner class for individual predictions
    public static class Prediction {
        private String className;
        private double confidence;
        private int classId;

        public Prediction(String className, double confidence, int classId) {
            this.className = className;
            this.confidence = confidence;
            this.classId = classId;
        }

        public String getClassName() {
            return className;
        }

        public double getConfidence() {
            return confidence;
        }

        public int getClassId() {
            return classId;
        }

        @Override
        public String toString() {
            return String.format("%s: %.2f%%", className, confidence * 100);
        }
    }
}
