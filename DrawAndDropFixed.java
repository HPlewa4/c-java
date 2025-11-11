// Importing all the necessary classes for GUI, drawing, images, and file handling
import javax.swing.*;               // For JFrame, JPanel, Swing utilities, etc.
import java.awt.*;                  // For Graphics, Color, Font, etc.
import java.awt.event.*;            // For mouse events (clicks, drags)
import java.awt.image.BufferedImage; // For an image we can draw on
import java.io.File;                // For working with files (dragged images)
import java.util.List;              // For handling lists of dropped files
import javax.imageio.ImageIO;       // For reading image files (PNG, JPG, etc.)
import java.awt.datatransfer.DataFlavor; // For recognizing dragged file data type

// Main drawing panel class
public class DrawAndDropFixed extends JPanel {

    // This is the "canvas" where you draw with the mouse
    private BufferedImage canvas;

    // The 2D graphics object used for drawing lines on the canvas
    private Graphics2D g2;

    // Store previous mouse position to draw continuous lines
    private int prevX, prevY;

    // Whether the mouse is currently being dragged
    private boolean dragging;

    // The image dropped onto the panel
    private Image droppedImage;

    // Constructor — this is where everything gets set up
    public DrawAndDropFixed() {
        // Set the size of the drawing area
        setPreferredSize(new Dimension(800, 600));

        // Background color of the panel
        setBackground(Color.DARK_GRAY);

        // Create a blank transparent image to draw on
        canvas = new BufferedImage(800, 600, BufferedImage.TYPE_INT_ARGB);

        // Get a Graphics2D object from the canvas so we can draw lines on it
        g2 = canvas.createGraphics();
        g2.setColor(Color.GREEN); // Set the drawing color to green
        g2.setStroke(new BasicStroke(5)); // Set the line thickness
        g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON); // Smooth lines

        // ======== MOUSE INPUT HANDLING ========

        // Mouse listener for pressing and releasing the mouse
        addMouseListener(new MouseAdapter() {
            // When mouse button is pressed — start drawing
            public void mousePressed(MouseEvent e) {
                prevX = e.getX();  // Store starting x position
                prevY = e.getY();  // Store starting y position
                dragging = true;   // Mark that we’re now dragging
            }

            // When mouse button is released — stop drawing
            public void mouseReleased(MouseEvent e) {
                dragging = false;
            }
        });

        // Mouse motion listener — handles movement while mouse is pressed
        addMouseMotionListener(new MouseMotionAdapter() {
            // Called whenever the mouse moves while pressed down
            public void mouseDragged(MouseEvent e) {
                if (dragging) { // Only draw if mouse is pressed
                    int x = e.getX(); // Get current mouse x
                    int y = e.getY(); // Get current mouse y

                    // Draw a line from previous point to new point
                    g2.drawLine(prevX, prevY, x, y);

                    // Update previous coordinates for next segment
                    prevX = x;
                    prevY = y;

                    // Repaint panel so we see updated drawing
                    repaint();
                }
            }
        });

        // ======== DRAG AND DROP IMAGE HANDLER ========

        setTransferHandler(new TransferHandler() {
            // Only accept drops that contain a file (like an image file)
            @Override
            public boolean canImport(TransferSupport support) {
                return support.isDataFlavorSupported(DataFlavor.javaFileListFlavor);
            }

            // When a file is dropped, this is called
            @Override
            @SuppressWarnings("unchecked")
            public boolean importData(TransferSupport support) {
                try {
                    // Get list of files that were dropped
                    List<File> files = (List<File>) support.getTransferable()
                            .getTransferData(DataFlavor.javaFileListFlavor);

                    if (!files.isEmpty()) {
                        File file = files.get(0); // Take the first file only
                        System.out.println("Dropped file: " + file.getAbsolutePath());

                        // Read the image from file
                        droppedImage = ImageIO.read(file);

                        // Redraw panel to show dropped image
                        repaint();
                        return true;
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                }
                return false;
            }
        });
    }

    // ======== DRAWING ON SCREEN ========
    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g); // Clear the background first

        // Make sure the canvas always matches the panel size
        if (canvas == null || canvas.getWidth() != getWidth() || canvas.getHeight() != getHeight()) {
            // Create a new canvas if size changed
            BufferedImage newCanvas = new BufferedImage(getWidth(), getHeight(), BufferedImage.TYPE_INT_ARGB);
            Graphics2D g2New = newCanvas.createGraphics();

            // Copy the old drawing onto the new, resized canvas
            if (canvas != null) {
                g2New.drawImage(canvas, 0, 0, null);
            }

            // Replace the old graphics with the new one
            g2 = g2New;
            canvas = newCanvas;

            // Reset drawing style
            g2.setColor(Color.GREEN);
            g2.setStroke(new BasicStroke(5));
            g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        }

        // Draw the canvas (your doodles)
        g.drawImage(canvas, 0, 0, null);

        // If an image was dropped, show it at position (100, 100)
        if (droppedImage != null) {
            g.drawImage(droppedImage, 100, 100, this);
        }

        // Draw some instruction text at the top
        String text = "Draw with your mouse — drop an image here!";
        g.setFont(new Font("Arial", Font.BOLD, 24));
        g.setColor(Color.GREEN);

        // Get size of text so we can center it
        FontMetrics fm = g.getFontMetrics();
        int textWidth = fm.stringWidth(text);
        int textHeight = fm.getHeight();

        // Calculate position for centered text
        int x = (getWidth() - textWidth) / 2;
        int y = (getHeight() - textHeight) / 2 + fm.getAscent();

        // Draw the text near the top of the screen
        g.drawString(text, x, 50);
    }

    // ======== MAIN METHOD ========
    public static void main(String[] args) {
        // Run GUI code on the Event Dispatch Thread (best practice for Swing)
        SwingUtilities.invokeLater(() -> {
            // Create the main window
            JFrame frame = new JFrame("Drawing and Image Drop");
            frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE); // Close app when window closes
            frame.setContentPane(new DrawAndDropFixed()); // Add our custom drawing panel
            frame.pack(); // Adjust window size to fit the panel
            frame.setLocationRelativeTo(null); // Center on screen
            frame.setVisible(true); // Show the window
        });
    }
}
