import javax.swing.*;
import javax.swing.border.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;
import java.io.File;

public class MLAppUI extends JFrame {
    private DrawingPanel drawingCanvas;
    private JLabel statusLabel;
    private JToggleButton mnistButton;
    private JToggleButton celebAButton;
    private String currentMode = "MNIST";
    
    // Dark theme colors
    private static final Color BG_DARK = new Color(30, 30, 30);
    private static final Color BG_DARKER = new Color(20, 20, 20);
    private static final Color BG_LIGHTER = new Color(45, 45, 45);
    private static final Color ACCENT_GREEN = new Color(46, 204, 113);
    private static final Color TEXT_COLOR = new Color(220, 220, 220);
    private static final Color BORDER_COLOR = new Color(60, 60, 60);
    
    public MLAppUI() {
        setTitle("AI Recognizer");
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
        getContentPane().setBackground(BG_DARK);
        
        // Top Panel - Header
        JPanel headerPanel = createHeaderPanel();
        add(headerPanel, BorderLayout.NORTH);
        
        // Center Panel - Canvas and Controls
        JPanel centerPanel = new JPanel(new BorderLayout(0, 0));
        centerPanel.setBackground(BG_DARK);
        
        // Left Panel - Controls
        JPanel controlPanel = createControlPanel();
        centerPanel.add(controlPanel, BorderLayout.WEST);
        
        // Canvas Panel
        JPanel canvasPanel = createCanvasPanel();
        centerPanel.add(canvasPanel, BorderLayout.CENTER);
        
        add(centerPanel, BorderLayout.CENTER);
        
        // Bottom Panel - Status Bar
        JPanel statusPanel = createStatusPanel();
        add(statusPanel, BorderLayout.SOUTH);
    }
    
    private JPanel createHeaderPanel() {
        JPanel panel = new JPanel(new BorderLayout());
        panel.setBackground(BG_DARKER);
        panel.setBorder(BorderFactory.createEmptyBorder(15, 20, 15, 20));
        
        JLabel titleLabel = new JLabel("AI text and celebrity recognizer");
        titleLabel.setFont(new Font("Segoe UI", Font.BOLD, 24));
        titleLabel.setForeground(ACCENT_GREEN);
        
        // Mode Toggle Buttons
        JPanel modePanel = new JPanel(new FlowLayout(FlowLayout.RIGHT, 10, 0));
        modePanel.setOpaque(false);
        
        ButtonGroup modeGroup = new ButtonGroup();
        
        mnistButton = new JToggleButton("MNIST");
        celebAButton = new JToggleButton("CELEB-A");
        
        styleToggleButton(mnistButton, true);
        styleToggleButton(celebAButton, false);
        
        mnistButton.setSelected(true);
        modeGroup.add(mnistButton);
        modeGroup.add(celebAButton);
        
        mnistButton.addActionListener(e -> switchMode("MNIST"));
        celebAButton.addActionListener(e -> switchMode("CELEB-A"));
        
        modePanel.add(mnistButton);
        modePanel.add(celebAButton);
        
        panel.add(titleLabel, BorderLayout.WEST);
        panel.add(modePanel, BorderLayout.EAST);
        
        return panel;
    }
    
    private void styleToggleButton(JToggleButton btn, boolean selected) {
        btn.setFont(new Font("Segoe UI", Font.BOLD, 12));
        btn.setFocusPainted(false);
        btn.setBorderPainted(false);
        btn.setPreferredSize(new Dimension(100, 35));
        btn.setCursor(new Cursor(Cursor.HAND_CURSOR));
        
        if (selected) {
            btn.setBackground(ACCENT_GREEN);
            btn.setForeground(BG_DARKER);
        } else {
            btn.setBackground(BG_LIGHTER);
            btn.setForeground(TEXT_COLOR);
        }
        
        btn.addItemListener(e -> {
            if (e.getStateChange() == ItemEvent.SELECTED) {
                btn.setBackground(ACCENT_GREEN);
                btn.setForeground(BG_DARKER);
            } else {
                btn.setBackground(BG_LIGHTER);
                btn.setForeground(TEXT_COLOR);
            }
        });
    }
    
    private JPanel createCanvasPanel() {
        JPanel panel = new JPanel(new BorderLayout());
        panel.setBackground(BG_DARK);
        panel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
        
        JLabel canvasTitle = new JLabel("Drawing Canvas - " + currentMode);
        canvasTitle.setFont(new Font("Segoe UI", Font.BOLD, 14));
        canvasTitle.setForeground(TEXT_COLOR);
        canvasTitle.setBorder(BorderFactory.createEmptyBorder(0, 0, 10, 0));
        
        // Drawing Canvas with responsive sizing
        drawingCanvas = new DrawingPanel();
        drawingCanvas.setBackground(BG_DARKER);
        drawingCanvas.setBorder(BorderFactory.createLineBorder(BORDER_COLOR, 2));
        
        // Drag and drop support
        setupDragAndDrop();
        
        panel.add(canvasTitle, BorderLayout.NORTH);
        panel.add(drawingCanvas, BorderLayout.CENTER);
        
        return panel;
    }
    
    // Custom JPanel for drawing that handles resizing
    class DrawingPanel extends JPanel {
        private BufferedImage canvasImage;
        private Graphics2D g2d;
        private int lastX, lastY;
        
        public DrawingPanel() {
            setBackground(BG_DARKER);
            
            MouseAdapter mouseAdapter = new MouseAdapter() {
                @Override
                public void mousePressed(MouseEvent e) {
                    lastX = e.getX();
                    lastY = e.getY();
                }
                
                @Override
                public void mouseDragged(MouseEvent e) {
                    if (canvasImage != null && g2d != null) {
                        int x = e.getX();
                        int y = e.getY();
                        g2d.drawLine(lastX, lastY, x, y);
                        lastX = x;
                        lastY = y;
                        repaint();
                    }
                }
            };
            
            addMouseListener(mouseAdapter);
            addMouseMotionListener(mouseAdapter);
            
            // Handle resizing
            addComponentListener(new ComponentAdapter() {
                @Override
                public void componentResized(ComponentEvent e) {
                    initializeCanvas();
                }
            });
        }
        
        private void initializeCanvas() {
            int width = Math.max(getWidth(), 1);
            int height = Math.max(getHeight(), 1);
            
            BufferedImage oldImage = canvasImage;
            canvasImage = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
            g2d = canvasImage.createGraphics();
            g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
            
            // Fill with dark background
            g2d.setColor(BG_DARKER);
            g2d.fillRect(0, 0, width, height);
            
            // Copy old content if exists
            if (oldImage != null) {
                g2d.drawImage(oldImage, 0, 0, null);
            }
            
            // Set up drawing properties
            g2d.setColor(ACCENT_GREEN);
            g2d.setStroke(new BasicStroke(4, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
            
            repaint();
        }
        
        @Override
        protected void paintComponent(Graphics g) {
            super.paintComponent(g);
            if (canvasImage != null) {
                g.drawImage(canvasImage, 0, 0, null);
            }
        }
        
        public void clearCanvas() {
            if (g2d != null && canvasImage != null) {
                g2d.setColor(BG_DARKER);
                g2d.fillRect(0, 0, canvasImage.getWidth(), canvasImage.getHeight());
                g2d.setColor(ACCENT_GREEN);
                repaint();
            }
        }
        
        public void loadImage(BufferedImage img) {
            if (canvasImage != null && g2d != null) {
                g2d.setColor(BG_DARKER);
                g2d.fillRect(0, 0, canvasImage.getWidth(), canvasImage.getHeight());
                
                // Scale image to fit canvas
                int canvasWidth = canvasImage.getWidth();
                int canvasHeight = canvasImage.getHeight();
                double scale = Math.min((double)canvasWidth / img.getWidth(), 
                                       (double)canvasHeight / img.getHeight());
                int scaledWidth = (int)(img.getWidth() * scale);
                int scaledHeight = (int)(img.getHeight() * scale);
                int x = (canvasWidth - scaledWidth) / 2;
                int y = (canvasHeight - scaledHeight) / 2;
                
                g2d.drawImage(img, x, y, scaledWidth, scaledHeight, null);
                repaint();
            }
        }
        
        public BufferedImage getCanvasImage() {
            return canvasImage;
        }
    }
    
    @SuppressWarnings("unchecked")
    private void setupDragAndDrop() {
        drawingCanvas.setTransferHandler(new TransferHandler() {
            @Override
            public boolean canImport(TransferSupport support) {
                return support.isDataFlavorSupported(java.awt.datatransfer.DataFlavor.javaFileListFlavor);
            }
            
            @Override
            public boolean importData(TransferSupport support) {
                try {
                    java.util.List<File> files = (java.util.List<File>) support.getTransferable()
                        .getTransferData(java.awt.datatransfer.DataFlavor.javaFileListFlavor);
                    if (!files.isEmpty()) {
                        File file = files.get(0);
                        loadImage(file);
                        return true;
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                }
                return false;
            }
        });
    }
    
    private JPanel createControlPanel() {
        JPanel panel = new JPanel();
        panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));
        panel.setBackground(BG_LIGHTER);
        panel.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createMatteBorder(0, 0, 0, 2, BORDER_COLOR),
            BorderFactory.createEmptyBorder(20, 15, 20, 15)
        ));
        panel.setPreferredSize(new Dimension(220, 0));
        
        JLabel controlTitle = new JLabel("Controls");
        controlTitle.setFont(new Font("Segoe UI", Font.BOLD, 18));
        controlTitle.setForeground(ACCENT_GREEN);
        controlTitle.setAlignmentX(Component.CENTER_ALIGNMENT);
        
        panel.add(controlTitle);
        panel.add(Box.createRigidArea(new Dimension(0, 25)));
        
        // Clear Button
        JButton clearBtn = createStyledButton("Clear Canvas", new Color(231, 76, 60));
        clearBtn.addActionListener(e -> clearCanvas());
        panel.add(clearBtn);
        panel.add(Box.createRigidArea(new Dimension(0, 12)));
        
        // Select File Button
        JButton selectFileBtn = createStyledButton("Select File", new Color(52, 152, 219));
        selectFileBtn.addActionListener(e -> selectFile());
        panel.add(selectFileBtn);
        panel.add(Box.createRigidArea(new Dimension(0, 12)));
        
        // Process Button
        JButton processBtn = createStyledButton("Process Image", ACCENT_GREEN);
        processBtn.addActionListener(e -> processImage());
        panel.add(processBtn);
        panel.add(Box.createRigidArea(new Dimension(0, 30)));
        
        // Info Panel
        JPanel infoPanel = new JPanel();
        infoPanel.setLayout(new BoxLayout(infoPanel, BoxLayout.Y_AXIS));
        infoPanel.setBackground(BG_DARK);
        infoPanel.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(BORDER_COLOR, 1),
            BorderFactory.createEmptyBorder(15, 10, 15, 10)
        ));
        infoPanel.setAlignmentX(Component.CENTER_ALIGNMENT);
        
        JLabel infoTitle = new JLabel("Instructions");
        infoTitle.setFont(new Font("Segoe UI", Font.BOLD, 13));
        infoTitle.setForeground(ACCENT_GREEN);
        
        JTextArea infoText = new JTextArea(
            "• Draw on canvas with\n  green marker\n\n" +
            "• Drag & drop images\n\n" +
            "• Use file selector\n\n" +
            "• Switch between\n  MNIST/CELEB-A modes"
        );
        infoText.setEditable(false);
        infoText.setBackground(BG_DARK);
        infoText.setForeground(TEXT_COLOR);
        infoText.setFont(new Font("Segoe UI", Font.PLAIN, 11));
        infoText.setBorder(BorderFactory.createEmptyBorder(8, 0, 0, 0));
        
        infoPanel.add(infoTitle);
        infoPanel.add(infoText);
        
        panel.add(infoPanel);
        panel.add(Box.createVerticalGlue());
        
        return panel;
    }
    
    private JButton createStyledButton(String text, Color bgColor) {
        JButton btn = new JButton(text);
        btn.setFont(new Font("Segoe UI", Font.BOLD, 13));
        btn.setBackground(bgColor);
        btn.setForeground(Color.WHITE);
        btn.setFocusPainted(false);
        btn.setBorderPainted(false);
        btn.setAlignmentX(Component.CENTER_ALIGNMENT);
        btn.setMaximumSize(new Dimension(190, 42));
        btn.setCursor(new Cursor(Cursor.HAND_CURSOR));
        
        btn.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseEntered(MouseEvent e) {
                btn.setBackground(bgColor.brighter());
            }
            
            @Override
            public void mouseExited(MouseEvent e) {
                btn.setBackground(bgColor);
            }
        });
        
        return btn;
    }
    
    private JPanel createStatusPanel() {
        JPanel panel = new JPanel(new BorderLayout());
        panel.setBackground(BG_DARKER);
        panel.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createMatteBorder(2, 0, 0, 0, BORDER_COLOR),
            BorderFactory.createEmptyBorder(10, 20, 10, 20)
        ));
        
        statusLabel = new JLabel("Ready - Mode: " + currentMode);
        statusLabel.setFont(new Font("Segoe UI", Font.PLAIN, 12));
        statusLabel.setForeground(TEXT_COLOR);
        
        panel.add(statusLabel, BorderLayout.WEST);
        
        return panel;
    }
    
    private void switchMode(String mode) {
        currentMode = mode;
        statusLabel.setText("Ready - Mode: " + currentMode);
        clearCanvas();
    }
    
    private void clearCanvas() {
        drawingCanvas.clearCanvas();
        statusLabel.setText("Canvas cleared - Mode: " + currentMode);
    }
    
    private void selectFile() {
        JFileChooser fileChooser = new JFileChooser();
        fileChooser.setFileFilter(new javax.swing.filechooser.FileNameExtensionFilter(
            "Image files", "jpg", "jpeg", "png", "bmp", "gif"));
        
        int result = fileChooser.showOpenDialog(this);
        if (result == JFileChooser.APPROVE_OPTION) {
            File selectedFile = fileChooser.getSelectedFile();
            loadImage(selectedFile);
        }
    }
    
    private void loadImage(File file) {
        try {
            BufferedImage img = javax.imageio.ImageIO.read(file);
            if (img != null) {
                drawingCanvas.loadImage(img);
                statusLabel.setText("Loaded: " + file.getName());
            }
        } catch (Exception e) {
            statusLabel.setText("Error loading image: " + e.getMessage());
            e.printStackTrace();
        }
    }
    
    private void processImage() {
        // Placeholder for image processing logic
        statusLabel.setText("Processing image with " + currentMode + " model...");
        JOptionPane.showMessageDialog(this, 
            "Image processing with " + currentMode + " model will be implemented here.",
            "Process Image", 
            JOptionPane.INFORMATION_MESSAGE);
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