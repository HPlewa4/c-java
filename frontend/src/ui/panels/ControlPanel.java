package src.ui.panels;

import src.ui.utils.UIStyles;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.geom.*;

public class ControlPanel extends JPanel {

    private class Neon90sButton extends JButton {
        private Color baseColor;
        private Color glowColor;

        public Neon90sButton(String text, Color neonColor) {
            super(text);
            this.baseColor = neonColor;
            this.glowColor = new Color(
                    Math.min(255, neonColor.getRed() + 100),
                    Math.min(255, neonColor.getGreen() + 100),
                    Math.min(255, neonColor.getBlue() + 100));

            setFont(new Font("Impact", Font.BOLD, 14));
            setBackground(neonColor);
            setForeground(Color.BLACK);
            setFocusPainted(false);
            setBorderPainted(false);
            setContentAreaFilled(false);
            setAlignmentX(Component.CENTER_ALIGNMENT);
            setMaximumSize(new Dimension(190, 45));
            setCursor(new Cursor(Cursor.HAND_CURSOR));
            setOpaque(false);

            addMouseListener(new MouseAdapter() {
                @Override
                public void mouseEntered(MouseEvent e) {
                    repaint();
                }

                @Override
                public void mouseExited(MouseEvent e) {
                    repaint();
                }
            });
        }

        @Override
        protected void paintComponent(Graphics g) {
            Graphics2D g2d = (Graphics2D) g;
            g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

            boolean hovered = getModel().isArmed() || getModel().isPressed();

            if (hovered) {
                g2d.setColor(new Color(
                        glowColor.getRed(),
                        glowColor.getGreen(),
                        glowColor.getBlue(),
                        50));
                g2d.fillRect(-5, -5, getWidth() + 10, getHeight() + 10);
            }

            Paint gradient = new GradientPaint(
                    0, 0, hovered ? glowColor : baseColor,
                    0, getHeight(),
                    new Color(baseColor.getRed() / 2, baseColor.getGreen() / 2, baseColor.getBlue() / 2));
            g2d.setPaint(gradient);
            RoundRectangle2D.Float button = new RoundRectangle2D.Float(0, 0, getWidth() - 1, getHeight() - 1, 8, 8);
            g2d.fill(button);

            // Bright outline
            g2d.setColor(Color.WHITE);
            g2d.setStroke(new BasicStroke(2f));
            g2d.draw(button);

            // Dark shadow
            g2d.setColor(new Color(0, 0, 0, 100));
            g2d.setStroke(new BasicStroke(1f));
            g2d.drawLine(2, getHeight() - 2, getWidth() - 2, getHeight() - 2);

            // Text with shadow
            FontMetrics fm = g2d.getFontMetrics();
            String text = getText();
            int x = (getWidth() - fm.stringWidth(text)) / 2;
            int y = (getHeight() - fm.getHeight()) / 2 + fm.getAscent();

            // Shadow text
            g2d.setColor(new Color(0, 0, 0, 150));
            g2d.drawString(text, x + 2, y + 2);

            // Main text
            g2d.setColor(Color.BLACK);
            g2d.setFont(getFont());
            g2d.drawString(text, x, y);
        }
    }

    public ControlPanel(Runnable clearAction, Runnable selectFileAction, Runnable processAction) {
        setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
        setBackground(UIStyles.ColorPalette.BG_LIGHTER.getColor());
        setBorder(BorderFactory.createCompoundBorder(
                BorderFactory.createMatteBorder(0, 0, 0, 4, UIStyles.ColorPalette.FIRE_YELLOW.getColor()),
                BorderFactory.createEmptyBorder(20, 15, 20, 15)));
        setPreferredSize(new Dimension(220, 0));

        JLabel title = new JLabel("CONTROLS");
        title.setFont(new Font("Impact", Font.BOLD, 20));
        title.setForeground(UIStyles.ColorPalette.FIRE_YELLOW.getColor());
        title.setAlignmentX(Component.CENTER_ALIGNMENT);

        // Add shadow effect to title
        title.setBorder(BorderFactory.createEmptyBorder(2, 0, 0, 0));
        add(title);
        add(Box.createRigidArea(new Dimension(0, 25)));

        JButton btn1 = new Neon90sButton("Clear Canvas", UIStyles.ColorPalette.FIRE_RED.getColor());
        btn1.addActionListener(e -> clearAction.run());
        add(btn1);
        add(Box.createRigidArea(new Dimension(0, 12)));

        JButton btn2 = new Neon90sButton("Select File", UIStyles.ColorPalette.FIRE_ORANGE.getColor());
        btn2.addActionListener(e -> selectFileAction.run());
        add(btn2);
        add(Box.createRigidArea(new Dimension(0, 12)));

        JButton btn3 = new Neon90sButton("Process Image", UIStyles.ColorPalette.FIRE_YELLOW.getColor());
        btn3.addActionListener(e -> processAction.run());
        add(btn3);

        add(Box.createRigidArea(new Dimension(0, 12)));

        // Info area
        JPanel infoPanel = new JPanel();
        infoPanel.setLayout(new BoxLayout(infoPanel, BoxLayout.Y_AXIS));
        infoPanel.setBackground(UIStyles.ColorPalette.BG_DARK.getColor());
        infoPanel.setBorder(BorderFactory.createCompoundBorder(
                BorderFactory.createLineBorder(UIStyles.ColorPalette.FIRE_RED.getColor(), 3),
                BorderFactory.createEmptyBorder(15, 10, 15, 10)));
        infoPanel.setAlignmentX(Component.CENTER_ALIGNMENT);

        JLabel infoTitle = new JLabel(">>> INSTRUCTIONS <<<");
        infoTitle.setFont(new Font("Impact", Font.BOLD, 13));
        infoTitle.setForeground(UIStyles.ColorPalette.FIRE_YELLOW.getColor());
        infoTitle.setAlignmentX(Component.CENTER_ALIGNMENT);

        JTextArea infoText = new JTextArea(
                "[*] Select image\n" +
                        "[*] Process image\n" +
                        "[*] Read output\n");
        infoText.setEditable(false);
        infoText.setBackground(UIStyles.ColorPalette.BG_DARK.getColor());
        infoText.setForeground(UIStyles.ColorPalette.FIRE_YELLOW.getColor());
        infoText.setFont(new Font("Courier New", Font.BOLD, 11));
        infoText.setBorder(BorderFactory.createEmptyBorder(8, 0, 0, 0));

        infoPanel.add(infoTitle);
        infoPanel.add(infoText);
        add(infoPanel);
        add(Box.createVerticalGlue());
    }
}
