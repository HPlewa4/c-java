package src.ui.panels;

import src.ui.utils.UIStyles;
import javax.swing.*;
import java.awt.*;
import java.awt.geom.*;

public class HeaderPanel extends JPanel {
    private final Image logoImage;

    private class LogoPanel extends JPanel {
        @Override
        protected void paintComponent(Graphics g) {
            Graphics2D g2d = (Graphics2D) g;
            g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
            g2d.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BICUBIC);

            int width = getWidth();
            int height = getHeight();

            // Gradient background
            GradientPaint bg = new GradientPaint(0, 0,
                    UIStyles.ColorPalette.BG_DARKER.getColor(),
                    width,
                    height,
                    UIStyles.ColorPalette.BG_LIGHTER.getColor());
            g2d.setPaint(bg);
            g2d.fillRect(0, 0, width, height);

            g2d.setColor(new Color(255, 255, 255, 30));
            for (int x = -height; x < width + height; x += 22) {
                g2d.drawLine(x, 0, x + height, height);
            }

            // Display logo
            if (logoImage != null) {
                int imgWidth = logoImage.getWidth(null);
                int imgHeight = logoImage.getHeight(null);

                if (imgWidth > 0 && imgHeight > 0) {
                    // Calculate scaling
                    double scaleX = (double) (width - 40) / imgWidth;
                    double scaleY = (double) (height - 30) / imgHeight;
                    double scale = Math.min(scaleX, scaleY);

                    int scaledWidth = (int) (imgWidth * scale);
                    int scaledHeight = (int) (imgHeight * scale);

                    int x = (width - scaledWidth) / 2;
                    int y = (height - scaledHeight) / 2;

                    // Draw shadow effect
                    g2d.setColor(new Color(0, 0, 0, 150));
                    g2d.drawImage(logoImage, x + 3, y + 3, scaledWidth, scaledHeight, null);

                    // Draw main logo
                    g2d.drawImage(logoImage, x, y, scaledWidth, scaledHeight, null);
                }
            }
        }
    }

    public HeaderPanel() {
        Image tempLogo = null;
        try {
            tempLogo = new ImageIcon("src/assets/logo.gif").getImage();
        } catch (Exception gifError) {
            try {
                tempLogo = new ImageIcon("src/assets/logo.png").getImage();
            } catch (Exception pngError) {
                System.err.println("Could not load logo.gif or logo.png");
                System.err.println("GIF error: " + gifError.getMessage());
                System.err.println("PNG error: " + pngError.getMessage());
            }
        }
        logoImage = tempLogo;

        setLayout(new BorderLayout());
        setBackground(UIStyles.ColorPalette.BG_DARKER.getColor());
        setBorder(BorderFactory.createMatteBorder(0, 0, 6, 0, UIStyles.ColorPalette.FIRE_YELLOW.getColor()));
        setPreferredSize(new Dimension(0, 140));

        LogoPanel logoPanel = new LogoPanel();
        logoPanel.setBackground(UIStyles.ColorPalette.BG_DARKER.getColor());
        add(logoPanel, BorderLayout.CENTER);
    }
}
