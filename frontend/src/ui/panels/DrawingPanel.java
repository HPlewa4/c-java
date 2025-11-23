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
            int imgWidth = canvasImage.getWidth();
            int imgHeight = canvasImage.getHeight();

            int x = (getWidth() - imgWidth) / 2;
            int y = (getHeight() - imgHeight) / 2;

            g.drawImage(canvasImage, x, y, null);
        }
    }

    public void loadImage(BufferedImage img) {
        canvasImage = img;
        repaint();
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
