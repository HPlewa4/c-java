package src.ui.panels;

import src.ui.utils.UIStyles;
import javax.swing.*;
import java.awt.*;

public class HeaderPanel extends JPanel {
    public HeaderPanel() {
        setLayout(new BorderLayout());
        setBackground(UIStyles.ColorPalette.BG_DARKER.getColor());
        setBorder(BorderFactory.createEmptyBorder(15, 20, 15, 20));

        JLabel title = new JLabel("Sigma's male detector");
        title.setFont(new Font("Segoe UI", Font.BOLD, 24));
        title.setForeground(UIStyles.ColorPalette.ACCENT_GREEN.getColor());
        add(title, BorderLayout.WEST);
    }
}
