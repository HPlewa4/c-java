package src.ui;

import src.ui.panels.*;
import src.ui.utils.UIStyles;
import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;
import javax.imageio.ImageIO;

public class MLAppUI extends JFrame {
    private DrawingPanel drawingCanvas;
    private StatusPanel statusPanel;
    private String currentMode = "MNIST";
    private JLabel canvasTitle;

    public MLAppUI() {
        setTitle("ML Image Processor - MNIST & CELEB-A");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(900, 700);
        setLocationRelativeTo(null);

        try {
            ImageIcon icon = new ImageIcon("icon.png");
            setIconImage(icon.getImage());
        } catch (Exception e) {
            System.err.println("Could not load icon: " + e.getMessage());
        }

        initComponents();
        setVisible(true);
    }

    private void initComponents() {
        setLayout(new BorderLayout(0, 0));
        getContentPane().setBackground(UIStyles.BG_DARK);

        // Header
        HeaderPanel headerPanel = new HeaderPanel(
            () -> switchMode("MNIST"),
            () -> switchMode("CELEB-A")
        );
        add(headerPanel, BorderLayout.NORTH);

        // Center layout
        JPanel centerPanel = new JPanel(new BorderLayout(0, 0));
        centerPanel.setBackground(UIStyles.BG_DARK);

        // Controls
        ControlPanel controlPanel = new ControlPanel(
            this::clearCanvas,
            this::selectFile,
            this::processImage
        );
        centerPanel.add(controlPanel, BorderLayout.WEST);

        // Canvas
        JPanel canvasPanel = new JPanel(new BorderLayout());
        canvasPanel.setBackground(UIStyles.BG_DARK);
        canvasPanel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

        canvasTitle = new JLabel("Drawing Canvas - " + currentMode);
        canvasTitle.setFont(new Font("Segoe UI", Font.BOLD, 14));
        canvasTitle.setForeground(UIStyles.TEXT_COLOR);
        canvasTitle.setBorder(BorderFactory.createEmptyBorder(0, 0, 10, 0));

        drawingCanvas = new DrawingPanel();
        drawingCanvas.setBackground(UIStyles.BG_DARKER);
        canvasPanel.add(canvasTitle, BorderLayout.NORTH);
        canvasPanel.add(drawingCanvas, BorderLayout.CENTER);

        centerPanel.add(canvasPanel, BorderLayout.CENTER);
        add(centerPanel, BorderLayout.CENTER);

        // Status bar
        statusPanel = new StatusPanel("Ready - Mode: " + currentMode);
        add(statusPanel, BorderLayout.SOUTH);
    }

    private void switchMode(String mode) {
        currentMode = mode;
        canvasTitle.setText("Drawing Canvas - " + currentMode);
        statusPanel.setStatus("Ready - Mode: " + currentMode);
        clearCanvas();
    }

    private void clearCanvas() {
        drawingCanvas.clearCanvas();
        statusPanel.setStatus("Canvas cleared - Mode: " + currentMode);
    }

    private void selectFile() {
        JFileChooser fileChooser = new JFileChooser();
        fileChooser.setFileFilter(new javax.swing.filechooser.FileNameExtensionFilter(
            "Image files", "jpg", "jpeg", "png", "bmp", "gif"));

        int result = fileChooser.showOpenDialog(this);
        if (result == JFileChooser.APPROVE_OPTION) {
            loadImage(fileChooser.getSelectedFile());
        }
    }

    private void loadImage(File file) {
        try {
            BufferedImage img = ImageIO.read(file);
            if (img != null) {
                drawingCanvas.loadImage(img);
                statusPanel.setStatus("Loaded: " + file.getName());
            }
        } catch (Exception e) {
            statusPanel.setStatus("Error loading image: " + e.getMessage());
            e.printStackTrace();
        }
    }

    private void processImage() {
        statusPanel.setStatus("Processing image with " + currentMode + " model...");
        JOptionPane.showMessageDialog(this,
            "Image processing with " + currentMode + " model will be implemented here.",
            "Process Image", JOptionPane.INFORMATION_MESSAGE);
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            try {
                UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
            } catch (Exception e) {
                e.printStackTrace();
            }
            new MLAppUI();
        });
    }
}
