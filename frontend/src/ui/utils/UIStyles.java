package src.ui.utils;

import java.awt.*;

public interface UIStyles {
    public enum ColorPalette {
        // Fire-inspired palette to match logo.gif
        BG_DARK(20, 12, 8),
        BG_DARKER(12, 7, 5),
        BG_LIGHTER(35, 18, 10),
        FIRE_RED(255, 64, 32),
        FIRE_ORANGE(255, 140, 0),
        FIRE_YELLOW(255, 213, 79),
        FIRE_GLOW(255, 170, 90),
        EMBER(180, 70, 40),
        ASH(60, 45, 40),
        // Legacy names kept for compatibility
        ACCENT_GREEN(255, 213, 79),
        RED(255, 64, 32),
        BLUE(0, 180, 220),
        TEXT(255, 240, 220),
        BORDER(120, 70, 50);

        private final Color color;

        ColorPalette(int r, int g, int b) {
            this.color = new Color(r, g, b);
        }

        public Color getColor() {
            return color;
        }
    }
}