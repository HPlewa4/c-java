package com.classifier.network;

import java.io.*;
import java.net.HttpURLConnection;
import java.net.URL;

// Abstract base class for network operations
public abstract class NetworkClient {
    protected String serverUrl;
    protected int timeout;

    public NetworkClient(String serverUrl, int timeout) {
        this.serverUrl = serverUrl;
        this.timeout = timeout;
    }

    // Abstract method - must be overridden
    public abstract String sendRequest(byte[] data) throws IOException;

    // Template method pattern
    protected HttpURLConnection createConnection(String endpoint) throws IOException {
        URL url = new URL(serverUrl + endpoint);
        HttpURLConnection conn = (HttpURLConnection) url.openConnection();
        conn.setConnectTimeout(timeout);
        conn.setReadTimeout(timeout);
        return conn;
    }

    // Helper method for reading response
    protected String readResponse(InputStream inputStream) throws IOException {
        try (BufferedReader br = new BufferedReader(new InputStreamReader(inputStream))) {
            StringBuilder response = new StringBuilder();
            String line;
            while ((line = br.readLine()) != null) {
                response.append(line);
            }
            return response.toString();
        }
    }
}
