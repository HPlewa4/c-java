package src.ui.panels;

import src.ui.utils.UIComponent;
import src.ui.utils.UIStyles;
import javax.swing.*;
import java.awt.*;

public class StatusPanel extends JPanel implements UIComponent {
    private JLabel statusLabel;

    public StatusPanel(String initialText) {
        setLayout(new BorderLayout());
        setBackground(UIStyles.ColorPalette.BG_DARKER.getColor());
        setBorder(BorderFactory.createCompoundBorder(
                BorderFactory.createMatteBorder(4, 0, 0, 0, UIStyles.ColorPalette.FIRE_RED.getColor()),
                BorderFactory.createEmptyBorder(10, 20, 10, 20)));

        statusLabel = new JLabel(initialText);
        statusLabel.setFont(new Font("Courier New", Font.BOLD, 13));
        statusLabel.setForeground(UIStyles.ColorPalette.FIRE_YELLOW.getColor());

        // Add a decorative prefix
        statusLabel.setText(">>> " + initialText + " <<<");
        add(statusLabel, BorderLayout.WEST);

        // Add animated decorations (static for now)
        JLabel deco = new JLabel("*");
        deco.setFont(new Font("Impact", Font.BOLD, 16));
        deco.setForeground(UIStyles.ColorPalette.FIRE_ORANGE.getColor());
        add(deco, BorderLayout.EAST);
    }

    public void setStatus(String text) {
        statusLabel.setText(">>> " + text + " <<<");
    }

    @Override
    public void reset() {
        statusLabel.setText(">>> Ready <<<");
    }
}