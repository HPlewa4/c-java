
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
    private JLabel canvasTitle;

    public MLAppUI() {
        setTitle("Widow's peak detector");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(900, 700);
        setLocationRelativeTo(null);

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

        // Set default directory to mounted volumes
        File homeDir = new File("/app/home");
        File uploadDir = new File("/app/uploads");

        if (homeDir.exists()) {
            fileChooser.setCurrentDirectory(homeDir);
        } else if (uploadDir.exists()) {
            fileChooser.setCurrentDirectory(uploadDir);
        }

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

        statusPanel.setStatus("Contacting backend...");

        new Thread(() -> {
            String response = callHealthEndpoint();

            SwingUtilities.invokeLater(() -> {
                statusPanel.setStatus("Backend Response: " + response);
            });
        }).start();
    }
    /*
    private void processImage() {
        if (!drawingCanvas.hasImage()) {
            statusPanel.setStatus("No image uploaded. Please select a file first.");
            return;
        }

        statusPanel.setStatus("Sending image to backend...");

        new Thread(() -> {
            BufferedImage img = drawingCanvas.getImage();  // You’ll need a getter in DrawingPanel
            byte[] imgBytes = bufferedImageToBytes(img, "png");

            if (imgBytes == null) {
                SwingUtilities.invokeLater(() ->
                    statusPanel.setStatus("Failed to convert image to bytes.")
                );
                return;
            }

            String response = sendImageToServer(imgBytes);

            SwingUtilities.invokeLater(() -> {
                statusPanel.setStatus("Backend Response: " + response);
            });
        }).start();
    }
    */
    private String sendImageToServer(byte[] imgBytes) {
        try {
            java.net.URL url = new java.net.URL("http://c-java-backend-1:8080/classify");
            java.net.HttpURLConnection conn = (java.net.HttpURLConnection) url.openConnection();
            conn.setRequestMethod("POST");
            conn.setDoOutput(true);
            conn.setRequestProperty("Content-Type", "application/octet-stream");
            conn.setConnectTimeout(5000);
            conn.setReadTimeout(5000);

            try (java.io.OutputStream os = conn.getOutputStream()) {
                os.write(imgBytes);
            }

            int status = conn.getResponseCode();
            java.io.InputStream stream =
                    (status >= 200 && status < 300)
                            ? conn.getInputStream()
                            : conn.getErrorStream();

            java.io.BufferedReader reader = new java.io.BufferedReader(
                    new java.io.InputStreamReader(stream)
            );
            StringBuilder response = new StringBuilder();
            String line;
            while ((line = reader.readLine()) != null) {
                response.append(line);
            }
            reader.close();
            conn.disconnect();
            return response.toString();

        } catch (Exception e) {
            e.printStackTrace();
            return "Error: " + e.getMessage();
        }
    }


    
    private String callHealthEndpoint() {
        try {
            java.net.URL url = new java.net.URL("http://c-java-backend-1:8080/health");
            java.net.HttpURLConnection conn = (java.net.HttpURLConnection) url.openConnection();

            conn.setRequestMethod("GET");
            conn.setConnectTimeout(3000);
            conn.setReadTimeout(3000);

            int status = conn.getResponseCode();

            java.io.InputStream stream =
                    (status >= 200 && status < 300)
                            ? conn.getInputStream()
                            : conn.getErrorStream();

            java.io.BufferedReader reader = new java.io.BufferedReader(
                    new java.io.InputStreamReader(stream)
            );

            StringBuilder response = new StringBuilder();
            String line;

            while ((line = reader.readLine()) != null) {
                response.append(line);
            }

            reader.close();
            conn.disconnect();

            return response.toString();

        } catch (Exception e) {
            return "Error: " + e.getMessage();
        }
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
