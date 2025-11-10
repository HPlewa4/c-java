package com.classifier.network;

import java.io.*;
import java.net.HttpURLConnection;

// Concrete implementation of NetworkClient - demonstrates inheritance
public class ImageClassifierClient extends NetworkClient {

    public ImageClassifierClient(String serverUrl, int timeout) {
        super(serverUrl, timeout);
    }

    @Override
    public String sendRequest(byte[] data) throws IOException {
        HttpURLConnection conn = createConnection("/classify");
        conn.setRequestMethod("POST");
        conn.setDoOutput(true);
        conn.setRequestProperty("Content-Type", "application/octet-stream");

        try (OutputStream os = conn.getOutputStream()) {
            os.write(data);
        }

        if (conn.getResponseCode() == HttpURLConnection.HTTP_OK) {
            return readResponse(conn.getInputStream());
        } else {
            throw new IOException("Server returned error: " + conn.getResponseCode());
        }
    }

    public boolean checkHealth() {
        try {
            HttpURLConnection conn = createConnection("/health");
            conn.setRequestMethod("GET");
            int code = conn.getResponseCode();
            return code == HttpURLConnection.HTTP_OK;
        } catch (IOException e) {
            System.err.println("Health check failed: " + e.getMessage());
            return false;
        }
    }
}
