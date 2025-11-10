package com.classifier.processing;

import java.awt.image.BufferedImage;
import java.awt.Graphics2D;
import java.awt.RenderingHints;

// Concrete implementation - demonstrates inheritance and polymorphism
public class ResizeProcessor implements ImageProcessor {
    private int targetWidth;
    private int targetHeight;

    public ResizeProcessor(int width, int height) {
        this.targetWidth = width;
        this.targetHeight = height;
    }

    @Override
    public BufferedImage process(BufferedImage image) {
        BufferedImage resized = new BufferedImage(
            targetWidth,
            targetHeight,
            BufferedImage.TYPE_INT_RGB
        );

        Graphics2D g2d = resized.createGraphics();
        g2d.setRenderingHint(
            RenderingHints.KEY_INTERPOLATION,
            RenderingHints.VALUE_INTERPOLATION_BILINEAR
        );
        g2d.drawImage(image, 0, 0, targetWidth, targetHeight, null);
        g2d.dispose();

        return resized;
    }

    @Override
    public String getProcessorName() {
        return "Resize (" + targetWidth + "x" + targetHeight + ")";
    }
}
