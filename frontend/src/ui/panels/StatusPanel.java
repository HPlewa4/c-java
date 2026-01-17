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
            BorderFactory.createMatteBorder(2, 0, 0, 0, UIStyles.ColorPalette.BORDER.getColor()),
            BorderFactory.createEmptyBorder(10, 20, 10, 20)
        ));

        statusLabel = new JLabel(initialText);
        statusLabel.setFont(new Font("Segoe UI", Font.PLAIN, 12));
        statusLabel.setForeground(UIStyles.ColorPalette.TEXT.getColor());
        add(statusLabel, BorderLayout.WEST);
    }

    public void setStatus(String text) {
        statusLabel.setText(text);
    }

    @Override
    public void reset() {
        statusLabel.setText("Ready");
    }
}