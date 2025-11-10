package com.classifier.processing;

import java.awt.image.BufferedImage;
import java.awt.Color;

// Concrete implementation - demonstrates inheritance and polymorphism
public class GrayscaleProcessor implements ImageProcessor {

    @Override
    public BufferedImage process(BufferedImage image) {
        BufferedImage grayscale = new BufferedImage(
            image.getWidth(),
            image.getHeight(),
            BufferedImage.TYPE_BYTE_GRAY
        );

        for (int y = 0; y < image.getHeight(); y++) {
            for (int x = 0; x < image.getWidth(); x++) {
                Color color = new Color(image.getRGB(x, y));
                int gray = (int)(color.getRed() * 0.299 +
                               color.getGreen() * 0.587 +
                               color.getBlue() * 0.114);
                Color grayColor = new Color(gray, gray, gray);
                grayscale.setRGB(x, y, grayColor.getRGB());
            }
        }

        return grayscale;
    }

    @Override
    public String getProcessorName() {
        return "Grayscale";
    }
}
