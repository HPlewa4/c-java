package src.ui.utils;

import java.awt.*;

public interface UIStyles {
    public enum ColorPalette {
        BG_DARK(30, 30, 30),
        BG_DARKER(20, 20, 20),
        BG_LIGHTER(45, 45, 45),
        ACCENT_GREEN(46, 204, 113),
        RED(231, 76, 60),
        BLUE(52, 152, 219),
        TEXT(220, 220, 220),
        BORDER(60, 60, 60);

        private final Color color;

        ColorPalette(int r, int g, int b) {
            this.color = new Color(r, g, b);
        }

        public Color getColor() {
            return color;
        }
    }
}