package src.api;

import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.io.*;
import java.net.HttpURLConnection;
import java.net.URL;
import javax.imageio.ImageIO;

public class BackendClient {
    private final String baseUrl;

    public BackendClient(String baseUrl) {
        this.baseUrl = baseUrl;
    }

    public BackendClient() {
        this("http://localhost:8080");
    }

    public HealthResponse checkHealth() throws IOException {
        URL url = new URL(baseUrl + "/health");
        HttpURLConnection conn = (HttpURLConnection) url.openConnection();
        conn.setRequestMethod("GET");
        conn.setConnectTimeout(3000);
        conn.setReadTimeout(3000);

        int status = conn.getResponseCode();
        String response = readResponse(conn);

        conn.disconnect();

        if (status >= 200 && status < 300) {
            return HealthResponse.fromJson(response);
        } else {
            throw new IOException("Health check failed: " + response);
        }
    }

    public ClassificationResponse classifyImage(BufferedImage image) throws IOException {
        System.out.println("DEBUG: Image dimensions: " + image.getWidth() + "x" + image.getHeight());
        System.out.println("DEBUG: Image type: " + image.getType());

        BufferedImage rgbImage;
        if (image.getType() == BufferedImage.TYPE_INT_RGB) {
            rgbImage = image;
        } else {
            System.out.println("DEBUG: Converting image to RGB format");
            rgbImage = new BufferedImage(image.getWidth(), image.getHeight(), BufferedImage.TYPE_INT_RGB);
            Graphics2D g = rgbImage.createGraphics();
            g.drawImage(image, 0, 0, null);
            g.dispose();
        }

        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        boolean success = ImageIO.write(rgbImage, "jpg", baos);
        System.out.println("DEBUG: ImageIO.write returned: " + success);

        byte[] imageBytes = baos.toByteArray();
        System.out.println("DEBUG: Encoded image size: " + imageBytes.length + " bytes");

        if (imageBytes.length == 0) {
            throw new IOException("Failed to encode image to JPEG");
        }

        URL url = new URL(baseUrl + "/classify");
        HttpURLConnection conn = (HttpURLConnection) url.openConnection();
        conn.setRequestMethod("POST");
        conn.setDoOutput(true);
        conn.setDoInput(true);
        conn.setConnectTimeout(5000);
        conn.setReadTimeout(10000);
        conn.setRequestProperty("Content-Type", "application/octet-stream");
        conn.setRequestProperty("Content-Length", String.valueOf(imageBytes.length));

        System.out.println("DEBUG: Sending POST to " + url);
        System.out.println("DEBUG: Content-Length: " + imageBytes.length);

        try (OutputStream os = conn.getOutputStream()) {
            os.write(imageBytes);
            os.flush();
            System.out.println("DEBUG: Data written and flushed");
        }

        int status = conn.getResponseCode();
        String response = readResponse(conn);

        conn.disconnect();

        if (status >= 200 && status < 300) {
            return ClassificationResponse.fromJson(response);
        } else {
            throw new IOException("Classification failed: " + response);
        }
    }

    private String readResponse(HttpURLConnection conn) throws IOException {
        InputStream stream = (conn.getResponseCode() >= 200 && conn.getResponseCode() < 300)
                ? conn.getInputStream()
                : conn.getErrorStream();

        BufferedReader reader = new BufferedReader(new InputStreamReader(stream));
        StringBuilder response = new StringBuilder();
        String line;

        while ((line = reader.readLine()) != null) {
            response.append(line);
        }

        reader.close();
        return response.toString();
    }

    public static class HealthResponse {
        public String status;
        public boolean modelLoaded;

        public static HealthResponse fromJson(String json) {
            HealthResponse hr = new HealthResponse();
            hr.status = extractValue(json, "status");
            hr.modelLoaded = Boolean.parseBoolean(extractValue(json, "modelLoaded"));
            return hr;
        }
    }

    public static class ClassificationResponse {
        public int prediction;
        public double confidence;
        public String label;

        public static ClassificationResponse fromJson(String json) {
            ClassificationResponse cr = new ClassificationResponse();
            cr.prediction = Integer.parseInt(extractValue(json, "prediction"));
            cr.confidence = Double.parseDouble(extractValue(json, "confidence"));
            cr.label = extractValue(json, "label");
            return cr;
        }

        @Override
        public String toString() {
            return String.format("%s (%.1f%% confidence)", label, confidence * 100);
        }
    }

    private static String extractValue(String json, String key) {
        String searchKey = "\"" + key + "\":";
        int start = json.indexOf(searchKey);
        if (start == -1) return "";

        start += searchKey.length();
        if (json.charAt(start) == '"') {
            start++;
            int end = json.indexOf('"', start);
            return json.substring(start, end);
        } else {
            int end = start;
            while (end < json.length() && (Character.isDigit(json.charAt(end)) ||
                    json.charAt(end) == '.' || json.charAt(end) == '-' ||
                    json.charAt(end) == 'e' || json.charAt(end) == 'E' ||
                    json.charAt(end) == 't' || json.charAt(end) == 'r' ||
                    json.charAt(end) == 'u' || json.charAt(end) == 'f' ||
                    json.charAt(end) == 'a' || json.charAt(end) == 'l' ||
                    json.charAt(end) == 's')) {
                end++;
            }
            return json.substring(start, end).replaceAll("[,}]", "");
        }
    }
}