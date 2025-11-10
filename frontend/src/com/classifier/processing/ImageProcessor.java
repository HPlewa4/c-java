package com.classifier.processing;

import java.awt.image.BufferedImage;

// Interface for polymorphism
public interface ImageProcessor {
    BufferedImage process(BufferedImage image);
    String getProcessorName();
}
