package src.ui.panels;

import src.ui.utils.UIStyles;
import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;

public class DrawingPanel extends JPanel {
    private BufferedImage canvasImage;

    public DrawingPanel() {
        setBackground(UIStyles.BG_DARKER);
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        if (canvasImage != null) {
            Graphics2D g2d = (Graphics2D) g;
            g2d.setRenderingHint(RenderingHints.KEY_INTERPOLATION,
                                RenderingHints.VALUE_INTERPOLATION_BILINEAR);
            g2d.setRenderingHint(RenderingHints.KEY_RENDERING,
                                RenderingHints.VALUE_RENDER_QUALITY);
            g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
                                RenderingHints.VALUE_ANTIALIAS_ON);

            int imgWidth = canvasImage.getWidth();
            int imgHeight = canvasImage.getHeight();
            int panelWidth = getWidth();
            int panelHeight = getHeight();

            // Calculate scaling to fit image in panel while maintaining aspect ratio
            double scaleX = (double) panelWidth / imgWidth;
            double scaleY = (double) panelHeight / imgHeight;
            double scale = Math.min(scaleX, scaleY);

            // Calculate scaled dimensions
            int scaledWidth = (int) (imgWidth * scale);
            int scaledHeight = (int) (imgHeight * scale);

            // Center the image
            int x = (panelWidth - scaledWidth) / 2;
            int y = (panelHeight - scaledHeight) / 2;

            // Draw scaled image
            g2d.drawImage(canvasImage, x, y, scaledWidth, scaledHeight, null);
        }
    }

    public void loadImage(BufferedImage img) {
        this.canvasImage = img;
        this.revalidate();
        this.repaint();
    }

    public BufferedImage getImage() {
        return canvasImage;
    }

    public void clearCanvas() {
        canvasImage = null;
        repaint();
    }
    
    public boolean hasImage() {
        return canvasImage != null;
    }
}
