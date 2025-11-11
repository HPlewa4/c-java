import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.util.Base64;
import java.util.List;
import javax.imageio.ImageIO;
import java.awt.datatransfer.DataFlavor;

public class Main extends JPanel {

    private BufferedImage canvas;
    private Graphics2D g2;
    private int prevX, prevY;
    private boolean dragging;
    private BufferedImage droppedImage;

    public Main() {
        setPreferredSize(new Dimension(800, 600));
        setBackground(Color.DARK_GRAY);

        // Create a blank image to draw on
        canvas = new BufferedImage(800, 600, BufferedImage.TYPE_INT_ARGB);
        g2 = canvas.createGraphics();
        g2.setColor(Color.GREEN);
        g2.setStroke(new BasicStroke(5));
        g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

        // Mouse listener for drawing
        addMouseListener(new MouseAdapter() {
            public void mousePressed(MouseEvent e) {
                prevX = e.getX();
                prevY = e.getY();
                dragging = true;
            }

            public void mouseReleased(MouseEvent e) {
                dragging = false;
            }
        });

        addMouseMotionListener(new MouseMotionAdapter() {
            public void mouseDragged(MouseEvent e) {
                if (dragging) {
                    int x = e.getX();
                    int y = e.getY();
                    g2.drawLine(prevX, prevY, x, y);
                    prevX = x;
                    prevY = y;
                    repaint();
                }
            }
        });

        // ✅ Enable drag and drop of image files
        setTransferHandler(new TransferHandler() {
            @Override
            public boolean canImport(TransferSupport support) {
                return support.isDataFlavorSupported(DataFlavor.javaFileListFlavor);
            }

            @Override
            @SuppressWarnings("unchecked")
            public boolean importData(TransferSupport support) {
                try {
                    List<File> files = (List<File>) support.getTransferable()
                            .getTransferData(DataFlavor.javaFileListFlavor);
                    if (!files.isEmpty()) {
                        File file = files.get(0);
                        System.out.println("Dropped file: " + file.getAbsolutePath());
                        droppedImage = ImageIO.read(file);
                        repaint();
                        return true;
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                }
                return false;
            }
        });

        // Add button to export image
        JButton exportButton = new JButton("Export to Base64");
        exportButton.addActionListener(e -> {
            BufferedImage imgToExport = droppedImage != null ? droppedImage : canvas;
            String base64 = bufferedImageToBase64(imgToExport);

            try {
                // Save Base64 to a text file
                java.nio.file.Files.write(
                    java.nio.file.Paths.get("a.txt"),
                    base64.getBytes()
                );
                System.out.println("Base64 image saved to image_base64.txt");
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        });
        this.add(exportButton);
        // Button to choose image from file
        JButton chooseImageButton = new JButton("Choose Image");
        chooseImageButton.addActionListener(e -> {
            JFileChooser fileChooser = new JFileChooser();
            int result = fileChooser.showOpenDialog(this);
            if (result == JFileChooser.APPROVE_OPTION) {
                File selectedFile = fileChooser.getSelectedFile();
                try {
                    droppedImage = ImageIO.read(selectedFile);
                    repaint();
                    System.out.println("Loaded image: " + selectedFile.getAbsolutePath());
                } catch (Exception ex) {
                    ex.printStackTrace();
                    JOptionPane.showMessageDialog(this, "Failed to load image!", "Error", JOptionPane.ERROR_MESSAGE);
                }
            }
        });
        this.add(chooseImageButton);

    }

    // Convert BufferedImage to Base64 string
    private String bufferedImageToBase64(BufferedImage image) {
        try {
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            ImageIO.write(image, "PNG", baos);
            byte[] bytes = baos.toByteArray();
            return Base64.getEncoder().encodeToString(bytes);
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);

        // Ensure canvas matches current panel size
        if (canvas == null || canvas.getWidth() != getWidth() || canvas.getHeight() != getHeight()) {
            BufferedImage newCanvas = new BufferedImage(getWidth(), getHeight(), BufferedImage.TYPE_INT_ARGB);
            Graphics2D g2New = newCanvas.createGraphics();
            if (canvas != null) {
                g2New.drawImage(canvas, 0, 0, null); // copy old drawing
            }
            g2 = g2New;
            canvas = newCanvas;
            g2.setColor(Color.GREEN);
            g2.setStroke(new BasicStroke(5));
            g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        }

        g.drawImage(canvas, 0, 0, null);

        if (droppedImage != null) {
            g.drawImage(droppedImage, 100, 100, this);
        }
    // Draw centered string
        String text = "Draw with your mouse — drop an image here!";
        g.setFont(new Font("Arial", Font.BOLD, 24));
        g.setColor(Color.GREEN);

    // Get string width and height
        FontMetrics fm = g.getFontMetrics();
        int textWidth = fm.stringWidth(text);
        //int textHeight = fm.getHeight();

    // Calculate coordinates to center the text
        int x = (getWidth() - textWidth) / 2;
        //int y = (getHeight() - textHeight) / 2 + fm.getAscent(); // y is baseline

        g.drawString(text, x, 50);
    }


    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            JFrame frame = new JFrame("Drawing and Image Drop");
            frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            frame.setContentPane(new Main());
            frame.pack();
            frame.setLocationRelativeTo(null);
            frame.setVisible(true);
        });
    }
}
