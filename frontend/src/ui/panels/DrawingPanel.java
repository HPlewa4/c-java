package src.ui.panels;

import src.ui.utils.UIComponent;
import src.ui.utils.UIStyles;
import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;

public class DrawingPanel extends JPanel implements UIComponent {
    private BufferedImage canvasImage;

    public DrawingPanel() {
        setBackground(UIStyles.ColorPalette.BG_DARKER.getColor());
        setBorder(BorderFactory.createLineBorder(UIStyles.ColorPalette.FIRE_YELLOW.getColor(), 3));
    }

    @Override
    public void reset() {
        clearCanvas();
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);

        Graphics2D g2d = (Graphics2D) g;
        g2d.setRenderingHint(RenderingHints.KEY_INTERPOLATION,
                RenderingHints.VALUE_INTERPOLATION_BILINEAR);
        g2d.setRenderingHint(RenderingHints.KEY_RENDERING,
                RenderingHints.VALUE_RENDER_QUALITY);
        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
                RenderingHints.VALUE_ANTIALIAS_ON);

        drawDecorativeBorder(g2d);

        if (canvasImage != null) {
            int imgWidth = canvasImage.getWidth();
            int imgHeight = canvasImage.getHeight();
            int panelWidth = getWidth();
            int panelHeight = getHeight();

            double scaleX = (double) (panelWidth - 20) / imgWidth;
            double scaleY = (double) (panelHeight - 20) / imgHeight;
            double scale = Math.min(scaleX, scaleY);

            int scaledWidth = (int) (imgWidth * scale);
            int scaledHeight = (int) (imgHeight * scale);

            int x = (panelWidth - scaledWidth) / 2;
            int y = (panelHeight - scaledHeight) / 2;

            g2d.setColor(new Color(0, 0, 0, 100));
            g2d.fillRect(x + 3, y + 3, scaledWidth, scaledHeight);

            g2d.drawImage(canvasImage, x, y, scaledWidth, scaledHeight, null);

            // Draw border around image
            g2d.setColor(UIStyles.ColorPalette.FIRE_YELLOW.getColor());
            g2d.setStroke(new BasicStroke(2f));
            g2d.drawRect(x - 2, y - 2, scaledWidth + 4, scaledHeight + 4);
        }
    }

    private void drawDecorativeBorder(Graphics2D g2d) {
        int w = getWidth();
        int h = getHeight();

        // Corner decorations
        Color deco1 = UIStyles.ColorPalette.FIRE_ORANGE.getColor();
        Color deco2 = UIStyles.ColorPalette.FIRE_RED.getColor();

        // Top-left corner
        int[] xPoints = { 0, 20, 0 };
        int[] yPoints = { 0, 0, 20 };
        g2d.setColor(deco1);
        g2d.fillPolygon(xPoints, yPoints, 3);

        // Top-right corner
        xPoints = new int[] { w, w - 20, w };
        yPoints = new int[] { 0, 0, 20 };
        g2d.setColor(deco2);
        g2d.fillPolygon(xPoints, yPoints, 3);

        // Bottom-left corner
        xPoints = new int[] { 0, 20, 0 };
        yPoints = new int[] { h, h, h - 20 };
        g2d.setColor(deco2);
        g2d.fillPolygon(xPoints, yPoints, 3);

        // Bottom-right corner
        xPoints = new int[] { w, w - 20, w };
        yPoints = new int[] { h, h, h - 20 };
        g2d.setColor(deco1);
        g2d.fillPolygon(xPoints, yPoints, 3);
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