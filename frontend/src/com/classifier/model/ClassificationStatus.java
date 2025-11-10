package com.classifier.model;

// Enum for classification status
public enum ClassificationStatus {
    IDLE("Ready to classify"),
    PROCESSING("Processing image..."),
    SUCCESS("Classification successful"),
    ERROR("Classification failed"),
    NO_IMAGE("No image selected");

    private final String message;

    ClassificationStatus(String message) {
        this.message = message;
    }

    public String getMessage() {
        return message;
    }
}
