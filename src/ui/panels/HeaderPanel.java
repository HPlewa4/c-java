package src.ui.panels;

import src.ui.utils.UIStyles;
import javax.swing.*;
import java.awt.*;
import java.awt.event.ItemEvent;

public class HeaderPanel extends JPanel {
    public HeaderPanel(Runnable onMnist, Runnable onCelebA) {
        setLayout(new BorderLayout());
        setBackground(UIStyles.BG_DARKER);
        setBorder(BorderFactory.createEmptyBorder(15, 20, 15, 20));

        JLabel title = new JLabel("ML Image Processor");
        title.setFont(new Font("Segoe UI", Font.BOLD, 24));
        title.setForeground(UIStyles.ACCENT_GREEN);
        add(title, BorderLayout.WEST);

        JPanel modePanel = new JPanel(new FlowLayout(FlowLayout.RIGHT, 10, 0));
        modePanel.setOpaque(false);
        ButtonGroup group = new ButtonGroup();

        JToggleButton mnist = createToggle("MNIST");
        JToggleButton celebA = createToggle("CELEB-A");
        mnist.setSelected(true);

        group.add(mnist);
        group.add(celebA);

        mnist.addActionListener(e -> onMnist.run());
        celebA.addActionListener(e -> onCelebA.run());

        modePanel.add(mnist);
        modePanel.add(celebA);
        add(modePanel, BorderLayout.EAST);
    }

    private JToggleButton createToggle(String text) {
        JToggleButton btn = new JToggleButton(text);
        btn.setFont(new Font("Segoe UI", Font.BOLD, 12));
        btn.setFocusPainted(false);
        btn.setBorderPainted(false);
        btn.setPreferredSize(new Dimension(100, 35));
        btn.setBackground(UIStyles.BG_LIGHTER);
        btn.setForeground(UIStyles.TEXT_COLOR);

        btn.addItemListener(e -> {
            if (e.getStateChange() == ItemEvent.SELECTED) {
                btn.setBackground(UIStyles.ACCENT_GREEN);
                btn.setForeground(UIStyles.BG_DARKER);
            } else {
                btn.setBackground(UIStyles.BG_LIGHTER);
                btn.setForeground(UIStyles.TEXT_COLOR);
            }
        });
        return btn;
    }
}
