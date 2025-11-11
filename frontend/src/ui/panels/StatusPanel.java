package src.ui.panels;

import src.ui.utils.UIStyles;
import javax.swing.*;
import java.awt.*;

public class StatusPanel extends JPanel {
    private JLabel statusLabel;

    public StatusPanel(String initialText) {
        setLayout(new BorderLayout());
        setBackground(UIStyles.BG_DARKER);
        setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createMatteBorder(2, 0, 0, 0, UIStyles.BORDER_COLOR),
            BorderFactory.createEmptyBorder(10, 20, 10, 20)
        ));

        statusLabel = new JLabel(initialText);
        statusLabel.setFont(new Font("Segoe UI", Font.PLAIN, 12));
        statusLabel.setForeground(UIStyles.TEXT_COLOR);
        add(statusLabel, BorderLayout.WEST);
    }

    public void setStatus(String text) {
        statusLabel.setText(text);
    }
}
