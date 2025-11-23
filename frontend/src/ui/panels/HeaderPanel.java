package src.ui.panels;

import src.ui.utils.UIStyles;
import javax.swing.*;
import java.awt.*;

public class HeaderPanel extends JPanel {
    public HeaderPanel() {
        setLayout(new BorderLayout());
        setBackground(UIStyles.BG_DARKER);
        setBorder(BorderFactory.createEmptyBorder(15, 20, 15, 20));

        JLabel title = new JLabel("Widow's peak detector");
        title.setFont(new Font("Segoe UI", Font.BOLD, 24));
        title.setForeground(UIStyles.ACCENT_GREEN);
        add(title, BorderLayout.WEST);
    }
}
