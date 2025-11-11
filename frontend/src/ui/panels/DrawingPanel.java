package src.ui.panels;

import src.ui.utils.UIStyles;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;

public class DrawingPanel extends JPanel {
    private BufferedImage canvasImage;
    private Graphics2D g2d;
    private int lastX, lastY;

    public DrawingPanel() {
        setBackground(UIStyles.BG_DARKER);

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

        addComponentListener(new ComponentAdapter() {
            @Override
            public void componentResized(ComponentEvent e) {
                initializeCanvas();
            }
        });

        // Enable drag-and-drop
        setTransferHandler(new TransferHandler() {
            @Override
            public boolean canImport(TransferSupport support) {
                return support.isDataFlavorSupported(java.awt.datatransfer.DataFlavor.javaFileListFlavor);
            }

            @Override
            @SuppressWarnings("unchecked")
            public boolean importData(TransferSupport support) {
                try {
                    java.util.List<java.io.File> files = (java.util.List<java.io.File>)
                        support.getTransferable().getTransferData(java.awt.datatransfer.DataFlavor.javaFileListFlavor);
                    if (!files.isEmpty()) {
                        java.io.File file = files.get(0);
                        BufferedImage img = javax.imageio.ImageIO.read(file);
                        loadImage(img);
                        return true;
                    }
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
                return false;
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

        g2d.setColor(UIStyles.BG_DARKER);
        g2d.fillRect(0, 0, width, height);

        if (oldImage != null) {
            g2d.drawImage(oldImage, 0, 0, null);
        }

        g2d.setColor(UIStyles.ACCENT_GREEN);
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
            g2d.setColor(UIStyles.BG_DARKER);
            g2d.fillRect(0, 0, canvasImage.getWidth(), canvasImage.getHeight());
            g2d.setColor(UIStyles.ACCENT_GREEN);
            repaint();
        }
    }

    public void loadImage(BufferedImage img) {
        if (canvasImage == null || g2d == null) return;
        g2d.setColor(UIStyles.BG_DARKER);
        g2d.fillRect(0, 0, canvasImage.getWidth(), canvasImage.getHeight());

        int cw = canvasImage.getWidth();
        int ch = canvasImage.getHeight();
        double scale = Math.min((double) cw / img.getWidth(), (double) ch / img.getHeight());
        int sw = (int) (img.getWidth() * scale);
        int sh = (int) (img.getHeight() * scale);
        int x = (cw - sw) / 2;
        int y = (ch - sh) / 2;

        g2d.drawImage(img, x, y, sw, sh, null);
        repaint();
    }
}
