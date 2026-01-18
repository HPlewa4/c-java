package src.ui.utils;

import java.awt.*;

public interface UIStyles {
    public enum ColorPalette {
        BG_DARK(20, 12, 8),
        BG_DARKER(12, 7, 5),
        BG_LIGHTER(35, 18, 10),
        FIRE_RED(255, 64, 32),
        FIRE_ORANGE(255, 140, 0),
        FIRE_YELLOW(255, 213, 79),
        FIRE_GLOW(255, 170, 90);

        private final Color color;

        ColorPalette(int r, int g, int b) {
            this.color = new Color(r, g, b);
        }

        public Color getColor() {
            return color;
        }
    }
}