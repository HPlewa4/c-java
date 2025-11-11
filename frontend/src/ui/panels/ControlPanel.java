package src.ui.panels;

import src.ui.utils.UIStyles;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class ControlPanel extends JPanel {
    public ControlPanel(Runnable clearAction, Runnable selectFileAction, Runnable processAction) {
        setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
        setBackground(UIStyles.BG_LIGHTER);
        setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createMatteBorder(0, 0, 0, 2, UIStyles.BORDER_COLOR),
            BorderFactory.createEmptyBorder(20, 15, 20, 15)
        ));
        setPreferredSize(new Dimension(220, 0));

        JLabel title = new JLabel("Controls");
        title.setFont(new Font("Segoe UI", Font.BOLD, 18));
        title.setForeground(UIStyles.ACCENT_GREEN);
        title.setAlignmentX(Component.CENTER_ALIGNMENT);
        add(title);
        add(Box.createRigidArea(new Dimension(0, 25)));

        add(createButton("Clear Canvas", new Color(231, 76, 60), clearAction));
        add(Box.createRigidArea(new Dimension(0, 12)));
        add(createButton("Select File", new Color(52, 152, 219), selectFileAction));
        add(Box.createRigidArea(new Dimension(0, 12)));
        add(createButton("Process Image", UIStyles.ACCENT_GREEN, processAction));
        add(Box.createRigidArea(new Dimension(0, 30)));

        // Info area
        JPanel infoPanel = new JPanel();
        infoPanel.setLayout(new BoxLayout(infoPanel, BoxLayout.Y_AXIS));
        infoPanel.setBackground(UIStyles.BG_DARK);
        infoPanel.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(UIStyles.BORDER_COLOR, 1),
            BorderFactory.createEmptyBorder(15, 10, 15, 10)
        ));
        infoPanel.setAlignmentX(Component.CENTER_ALIGNMENT);

        JLabel infoTitle = new JLabel("Instructions");
        infoTitle.setFont(new Font("Segoe UI", Font.BOLD, 13));
        infoTitle.setForeground(UIStyles.ACCENT_GREEN);

        JTextArea infoText = new JTextArea(
            "• Draw on canvas\n" +
            "• Drag & drop images\n" +
            "• Use file selector\n" +
            "• Switch between modes"
        );
        infoText.setEditable(false);
        infoText.setBackground(UIStyles.BG_DARK);
        infoText.setForeground(UIStyles.TEXT_COLOR);
        infoText.setFont(new Font("Segoe UI", Font.PLAIN, 11));
        infoText.setBorder(BorderFactory.createEmptyBorder(8, 0, 0, 0));

        infoPanel.add(infoTitle);
        infoPanel.add(infoText);
        add(infoPanel);
        add(Box.createVerticalGlue());
    }

    private JButton createButton(String text, Color color, Runnable action) {
        JButton btn = new JButton(text);
        btn.setFont(new Font("Segoe UI", Font.BOLD, 13));
        btn.setBackground(color);
        btn.setForeground(Color.WHITE);
        btn.setFocusPainted(false);
        btn.setBorderPainted(false);
        btn.setAlignmentX(Component.CENTER_ALIGNMENT);
        btn.setMaximumSize(new Dimension(190, 42));
        btn.setCursor(new Cursor(Cursor.HAND_CURSOR));
        btn.addActionListener(e -> action.run());

        btn.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseEntered(MouseEvent e) { btn.setBackground(color.brighter()); }
            @Override
            public void mouseExited(MouseEvent e) { btn.setBackground(color); }
        });
        return btn;
    }
}
