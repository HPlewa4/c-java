package com.classifier.processing;

import java.awt.image.BufferedImage;
import java.awt.Color;

// Concrete implementation - demonstrates inheritance and polymorphism
public class BrightnessProcessor implements ImageProcessor {
    private float factor;

    public BrightnessProcessor(float brightnessFactor) {
        this.factor = brightnessFactor;
    }

    @Override
    public BufferedImage process(BufferedImage image) {
        BufferedImage result = new BufferedImage(
            image.getWidth(),
            image.getHeight(),
            image.getType()
        );

        for (int y = 0; y < image.getHeight(); y++) {
            for (int x = 0; x < image.getWidth(); x++) {
                Color color = new Color(image.getRGB(x, y));

                int red = Math.min(255, (int)(color.getRed() * factor));
                int green = Math.min(255, (int)(color.getGreen() * factor));
                int blue = Math.min(255, (int)(color.getBlue() * factor));

                Color newColor = new Color(red, green, blue);
                result.setRGB(x, y, newColor.getRGB());
            }
        }

        return result;
    }

    @Override
    public String getProcessorName() {
        return "Brightness (" + factor + ")";
    }
}
