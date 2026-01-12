
import src.ui.panels.*;
import src.ui.utils.UIStyles;
import src.api.BackendClient;
import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;
import javax.imageio.ImageIO;

public class MLAppUI extends JFrame {
    private DrawingPanel drawingCanvas;
    private StatusPanel statusPanel;
    private JLabel canvasTitle;
    private BackendClient backendClient;

    public MLAppUI() {
        setTitle("Sigma's male detector");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(900, 700);
        setLocationRelativeTo(null);

        // Initialize backend client
        backendClient = new BackendClient("http://localhost:8080");

        try {
            ImageIcon icon = new ImageIcon("src/assets/icon.png");
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
        HeaderPanel headerPanel = new HeaderPanel();
        add(headerPanel, BorderLayout.NORTH);

        // Center layout
        JPanel centerPanel = new JPanel(new BorderLayout(0, 0));
        centerPanel.setBackground(UIStyles.BG_DARK);

        // Controls
        ControlPanel controlPanel = new ControlPanel(
                this::clearCanvas,
                this::selectFile,
                this::processImage);
        centerPanel.add(controlPanel, BorderLayout.WEST);

        // Canvas
        JPanel canvasPanel = new JPanel(new BorderLayout());
        canvasPanel.setBackground(UIStyles.BG_DARK);
        canvasPanel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

        canvasTitle = new JLabel("Selected file:");
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
        statusPanel = new StatusPanel("Ready");
        add(statusPanel, BorderLayout.SOUTH);
    }


    private void clearCanvas() {
        drawingCanvas.clearCanvas();
        statusPanel.setStatus("Canvas cleared");
    }

    private void selectFile() {
        JFileChooser fileChooser = new JFileChooser();

        // Set default directory to user's home
        fileChooser.setCurrentDirectory(new File(System.getProperty("user.home")));

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
        if (!drawingCanvas.hasImage()) {
            statusPanel.setStatus("No image uploaded. Please select a file first.");
            return;
        }

        statusPanel.setStatus("Analyzing image...");

        new Thread(() -> {
            try {
                // First check if backend is healthy
                BackendClient.HealthResponse health = backendClient.checkHealth();

                if (!health.modelLoaded) {
                    SwingUtilities.invokeLater(() -> {
                        statusPanel.setStatus("Error: Model not loaded on backend");
                    });
                    return;
                }

                // Get image from canvas
                BufferedImage image = drawingCanvas.getImage();

                // Classify image
                BackendClient.ClassificationResponse result = backendClient.classifyImage(image);

                // Update UI with result
                SwingUtilities.invokeLater(() -> {
                    String message = String.format(
                        "Result: %s (%.1f%% confidence)",
                        result.label,
                        result.confidence * 100
                    );
                    statusPanel.setStatus(message);

                    // Show dialog with detailed results
                    JOptionPane.showMessageDialog(
                        this,
                        "<html><body style='width: 300px; padding: 10px;'>" +
                        "<h2>Classification Result</h2>" +
                        "<p><b>Prediction:</b> " + result.label + "</p>" +
                        "<p><b>Confidence:</b> " + String.format("%.2f%%", result.confidence * 100) + "</p>" +
                        "</body></html>",
                        "Result",
                        JOptionPane.INFORMATION_MESSAGE
                    );
                });

            } catch (Exception e) {
                SwingUtilities.invokeLater(() -> {
                    statusPanel.setStatus("Error: " + e.getMessage());
                    JOptionPane.showMessageDialog(
                        this,
                        "Failed to classify image:\n" + e.getMessage(),
                        "Error",
                        JOptionPane.ERROR_MESSAGE
                    );
                });
                e.printStackTrace();
            }
        }).start();
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
