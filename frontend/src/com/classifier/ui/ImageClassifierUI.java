package com.classifier.ui;

import com.classifier.model.*;
import com.classifier.network.*;
import com.classifier.processing.*;
import com.classifier.utils.*;

import javax.swing.*;
import javax.imageio.ImageIO;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.*;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.text.SimpleDateFormat;
import java.util.Date;

public class ImageClassifierUI extends JFrame {
    private JLabel imageLabel;
    private JTextArea resultsArea;
    private JLabel statusLabel;
    private File selectedFile;
    private BufferedImage currentImage;

    // Using new classes - demonstrates composition
    private ImageClassifierClient client;
    private Cache<String, ClassificationResult> resultsCache;
    private ExecutorService executorService;
    private ClassificationStatus currentStatus;

    // File logging
    private static final String LOG_FILE = "classification_history.txt";

    public ImageClassifierUI() {
        setTitle("ML Image Classifier");
        setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);  // Handle cleanup manually
        setSize(900, 700);
        setLocationRelativeTo(null);

        // Initialize components with proper resource management
        client = new ImageClassifierClient("http://localhost:8080", 10000);
        resultsCache = new Cache<>(50);  // Generic class usage
        executorService = Executors.newFixedThreadPool(4);  // Parallel programming
        currentStatus = ClassificationStatus.IDLE;

        // Add window listener for cleanup (destructor pattern)
        addWindowListener(new java.awt.event.WindowAdapter() {
            @Override
            public void windowClosing(java.awt.event.WindowEvent windowEvent) {
                int result = JOptionPane.showConfirmDialog(
                    ImageClassifierUI.this,
                    "Are you sure you want to exit?",
                    "Exit Confirmation",
                    JOptionPane.YES_NO_OPTION
                );
                if (result == JOptionPane.YES_OPTION) {
                    cleanup();
                    System.exit(0);
                }
            }
        });

        setupUI();
        checkServerHealth();
    }

    private void setupUI() {
        JPanel mainPanel = new JPanel(new BorderLayout(10, 10));
        mainPanel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

        JButton selectButton = new JButton("Select Image");
        selectButton.addActionListener(e -> selectImage());

        JButton classifyButton = new JButton("Classify");
        classifyButton.addActionListener(e -> classifyImage());

        JButton processButton = new JButton("Apply Grayscale");
        processButton.addActionListener(e -> applyImageProcessing());

        JButton saveButton = new JButton("Save Results");
        saveButton.addActionListener(e -> saveResultsToFile());

        JButton exitButton = new JButton("Exit");
        exitButton.addActionListener(e -> {
            cleanup();
            System.exit(0);
        });

        JPanel controlPanel = new JPanel();
        controlPanel.add(selectButton);
        controlPanel.add(classifyButton);
        controlPanel.add(processButton);
        controlPanel.add(saveButton);
        controlPanel.add(exitButton);

        imageLabel = new JLabel("No image", SwingConstants.CENTER);
        imageLabel.setPreferredSize(new Dimension(400, 400));
        imageLabel.setBorder(BorderFactory.createLineBorder(Color.GRAY));

        resultsArea = new JTextArea();
        resultsArea.setEditable(false);
        resultsArea.setFont(new Font("Monospaced", Font.PLAIN, 12));
        JScrollPane resultsScroll = new JScrollPane(resultsArea);

        statusLabel = new JLabel(ClassificationStatus.IDLE.getMessage());
        statusLabel.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));

        JSplitPane splitPane = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT,
                imageLabel, resultsScroll);
        splitPane.setResizeWeight(0.5);

        mainPanel.add(controlPanel, BorderLayout.NORTH);
        mainPanel.add(splitPane, BorderLayout.CENTER);
        mainPanel.add(statusLabel, BorderLayout.SOUTH);

        add(mainPanel);
    }

    private void checkServerHealth() {
        executorService.submit(() -> {
            boolean healthy = client.checkHealth();
            SwingUtilities.invokeLater(() -> {
                if (!healthy) {
                    statusLabel.setText("⚠ Server not responding. Please start the backend server.");
                    statusLabel.setForeground(Color.RED);
                } else {
                    statusLabel.setText("✓ Connected to server");
                    statusLabel.setForeground(Color.GREEN);
                }
            });
        });
    }

    private void selectImage() {
        JFileChooser chooser = new JFileChooser();
        if (chooser.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
            selectedFile = chooser.getSelectedFile();
            try {
                currentImage = ImageIO.read(selectedFile);
                displayImage(currentImage);
                updateStatus(ClassificationStatus.IDLE);
            } catch (IOException e) {
                System.err.println("Error loading image: " + e.getMessage());
                JOptionPane.showMessageDialog(this, "Error loading image: " + e.getMessage());
            }
        }
    }

    private void displayImage(BufferedImage img) {
        if (img != null) {
            ImageIcon icon = new ImageIcon(img.getScaledInstance(400, 400, Image.SCALE_SMOOTH));
            imageLabel.setIcon(icon);
            imageLabel.setText("");
        }
    }

    // Demonstrates polymorphism - using ImageProcessor interface
    private void applyImageProcessing() {
        if (currentImage == null) {
            JOptionPane.showMessageDialog(this, "Please select an image first");
            return;
        }

        // Polymorphism - treating different processors through same interface
        ImageProcessor processor = new GrayscaleProcessor();

        executorService.submit(() -> {
            try {
                BufferedImage processed = processor.process(currentImage);
                SwingUtilities.invokeLater(() -> {
                    currentImage = processed;
                    displayImage(processed);
                    resultsArea.append("\nApplied: " + processor.getProcessorName());
                });
            } catch (Exception e) {
                System.err.println("Processing error: " + e.getMessage());
            }
        });
    }

    private void classifyImage() {
        if (selectedFile == null) {
            updateStatus(ClassificationStatus.NO_IMAGE);
            return;
        }

        updateStatus(ClassificationStatus.PROCESSING);

        // Using ExecutorService for better parallel programming
        executorService.submit(() -> {
            try {
                // Check cache first
                String cacheKey = selectedFile.getAbsolutePath();
                if (resultsCache.contains(cacheKey)) {
                    ClassificationResult cached = resultsCache.get(cacheKey);
                    SwingUtilities.invokeLater(() -> {
                        displayResults(cached);
                        resultsArea.append("\n(from cache)");
                    });
                    return;
                }

                // Read file
                ByteArrayOutputStream baos = new ByteArrayOutputStream();
                try (FileInputStream fis = new FileInputStream(selectedFile)) {
                    byte[] buffer = new byte[4096];
                    int bytesRead;
                    while ((bytesRead = fis.read(buffer)) != -1) {
                        baos.write(buffer, 0, bytesRead);
                    }
                }

                // Send request using client
                String response = client.sendRequest(baos.toByteArray());

                // Parse and display results
                ClassificationResult result = parseResults(response);
                result.setStatus(ClassificationStatus.SUCCESS);

                // Cache results
                resultsCache.put(cacheKey, result);

                // Log to file
                logResultToFile(selectedFile.getName(), result);

                SwingUtilities.invokeLater(() -> {
                    displayResults(result);
                    updateStatus(ClassificationStatus.SUCCESS);
                });

            } catch (Exception e) {
                System.err.println("Classification error: " + e.getMessage());
                SwingUtilities.invokeLater(() -> {
                    resultsArea.setText("Error: " + e.getMessage());
                    updateStatus(ClassificationStatus.ERROR);
                });
            }
        });
    }

    private ClassificationResult parseResults(String json) {
        ClassificationResult result = new ClassificationResult();
        String[] parts = json.split("\"className\":\"");

        for (int i = 1; i < parts.length; i++) {
            try {
                String className = parts[i].substring(0, parts[i].indexOf("\""));
                String confStr = parts[i].split("\"confidence\":")[1].split(",")[0];
                String idStr = parts[i].split("\"classId\":")[1].split("}")[0];

                double confidence = Double.parseDouble(confStr);
                int classId = Integer.parseInt(idStr);

                result.addPrediction(className, confidence, classId);
            } catch (Exception e) {
                System.err.println("Error parsing prediction: " + e.getMessage());
            }
        }

        return result;
    }

    private void displayResults(ClassificationResult result) {
        StringBuilder sb = new StringBuilder("Classification Results:\n\n");
        int rank = 1;
        for (ClassificationResult.Prediction pred : result.getPredictions()) {
            sb.append(String.format("%d. %s\n", rank++, pred.toString()));
        }
        resultsArea.setText(sb.toString());
    }

    private void updateStatus(ClassificationStatus status) {
        currentStatus = status;
        statusLabel.setText(status.getMessage());

        switch (status) {
            case SUCCESS:
                statusLabel.setForeground(Color.GREEN);
                break;
            case ERROR:
            case NO_IMAGE:
                statusLabel.setForeground(Color.RED);
                break;
            case PROCESSING:
                statusLabel.setForeground(Color.BLUE);
                break;
            default:
                statusLabel.setForeground(Color.BLACK);
        }
    }

    // File write operations
    private void logResultToFile(String filename, ClassificationResult result) {
        try (FileWriter fw = new FileWriter(LOG_FILE, true);
             BufferedWriter bw = new BufferedWriter(fw);
             PrintWriter out = new PrintWriter(bw)) {

            SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
            out.println("=================================");
            out.println("Time: " + sdf.format(new Date(result.getTimestamp())));
            out.println("File: " + filename);
            out.println("Results:");
            for (ClassificationResult.Prediction pred : result.getPredictions()) {
                out.println("  " + pred.toString());
            }
            out.println();

        } catch (IOException e) {
            System.err.println("Failed to write log: " + e.getMessage());
        }
    }

    private void saveResultsToFile() {
        if (resultsArea.getText().isEmpty()) {
            JOptionPane.showMessageDialog(this, "No results to save");
            return;
        }

        JFileChooser chooser = new JFileChooser();
        chooser.setSelectedFile(new File("classification_results.txt"));

        if (chooser.showSaveDialog(this) == JFileChooser.APPROVE_OPTION) {
            try (PrintWriter writer = new PrintWriter(chooser.getSelectedFile())) {
                writer.println(resultsArea.getText());
                JOptionPane.showMessageDialog(this, "Results saved successfully");
            } catch (IOException e) {
                System.err.println("Failed to save results: " + e.getMessage());
                JOptionPane.showMessageDialog(this, "Failed to save: " + e.getMessage());
            }
        }
    }

    // Cleanup method (destructor pattern)
    private void cleanup() {
        try {
            if (executorService != null && !executorService.isShutdown()) {
                executorService.shutdown();
                System.out.println("Executor service shut down");
            }
            if (resultsCache != null) {
                resultsCache.clear();
                System.out.println("Cache cleared");
            }
        } catch (Exception e) {
            System.err.println("Error during cleanup: " + e.getMessage());
        }
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> new ImageClassifierUI().setVisible(true));
    }
}
